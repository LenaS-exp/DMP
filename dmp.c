#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bio.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/device-mapper.h>

#define DMP_NAME "dmp"

struct my_dm_target {
	struct dm_dev *dev;
	sector_t start;
};

struct stats {
	unsigned long long read_reqs, read_size , write_reqs, write_size;
};

static struct stats stat = { 0, 0, 0, 0 };

static struct kobject* kob;


/*This function will be called when we read the sysfs file*/
static ssize_t stat_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	unsigned long long int avg_read = 0, avg_write = 0;

	if (stat.read_reqs) {
		//average block size per read
		avg_read = stat.read_size / stat.read_reqs;
	}

	if (stat.write_reqs) {
		//average block size per write
		avg_write = stat.write_size / stat.write_reqs;
	}

	return sprintf(buf,"\n\
		read_reqs: %llu \n\
		average read_size: %llu\n\
		write_reqs: %llu \n\
		average write_size: %llu \n\
		total_reqs: %llu \n\
		average block size: %llu \n\n",
		stat.read_reqs, avg_read,
		stat.write_reqs, avg_write,
		stat.read_reqs + stat.write_reqs, (avg_read + avg_write) / 2);
}


/*This function will be called when we write the sysfsfs file*/
static ssize_t stat_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return count;
}


static struct kobj_attribute stat_attr = __ATTR(volumes, 0440, stat_show, stat_store);


/*Constructor function*/
static int dmp_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
    	struct my_dm_target *mdt;
	unsigned long long start;

	if (argc != 2) {
		printk(KERN_CRIT "\n Invalid no.of arguments.\n");
		ti->error = "Invalid argument count";
		return -EINVAL;
	}

	mdt = kmalloc(sizeof(struct my_dm_target), GFP_KERNEL);

	if(mdt==NULL) {
		printk(KERN_CRIT "\n Mdt is null\n");
		ti->error = "dm-basic_target: Cannot allocate linear context";
		return -ENOMEM;
	}       

	if(sscanf(argv[1], "%llu", &start)!=1) {
		ti->error = "dm-basic_target: Invalid device sector";
		goto bad;
	}

	mdt->start=(sector_t)start;

	if (dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &mdt->dev)) {
		ti->error = "dm-basic_target: Device lookup failed";
		goto bad;
	}

	ti->private = mdt;
                 
	return 0;

bad:
	kfree(mdt);
	return -EINVAL;
   
}


/*Destruction function*/
static void dmp_dtr(struct dm_target *ti)
{
	struct my_dm_target *mdt = (struct my_dm_target *) ti->private;
	dm_put_device(ti, mdt->dev);
	kfree(mdt);
}


/*Map function. This function gets called whenever you get a new bio.*/
static int dmp_map(struct dm_target *ti, struct bio *bio)
{
    
	struct my_dm_target *mdt = (struct my_dm_target *) ti->private;
	
	bio_set_dev(bio, mdt->dev->bdev);
	
	/*bio_data_dir(bio) returns the data direction, READ or WRITE.*/
	
	if (bio_data_dir(bio) == READ) {
		stat.read_reqs++; //read request count
		stat.read_size += bio->bi_iter.bi_size; //read block size count
	} else {
		stat.write_reqs++; //write request count
		stat.write_size += bio->bi_iter.bi_size; //write block size count
	}
	
	submit_bio(bio);
	
	return DM_MAPIO_SUBMITTED;

}


static struct target_type dmp_target = {
	.name = DMP_NAME,
	.version = {1, 0, 0},
	.module = THIS_MODULE,
	.ctr = dmp_ctr,
	.dtr = dmp_dtr,
	.map = dmp_map,
};


static int dmp_init(void)
{
	int r;
	
	r = dm_register_target(&dmp_target);
	if (r < 0) {
		printk(KERN_ERR "Failed to register device mapper target\n");
		return r;
	}
	
	/*Creating a directory in /sys/kernel/ */
	kob = kobject_create_and_add("stat", &THIS_MODULE->mkobj.kobj);
	if (!kob) {
		return -ENOMEM;
	}
	
	r = sysfs_create_file(kob, &stat_attr.attr);
	if (r) {
		printk(KERN_ERR "Failed to create sysfs file for statistics\n");
	} else {
		printk(KERN_INFO "Device Mapper Proxy initialized\n");
	}
	
	return 0;
}


static void dmp_exit(void)
{  
	/*Freeing Kobj*/
	kobject_put(kob);
	
	dm_unregister_target(&dmp_target);
	sysfs_remove_file(kob, &stat_attr.attr);
	printk(KERN_INFO "Device Mapper Proxy unloaded\n");
}


module_init(dmp_init);
module_exit(dmp_exit);

MODULE_AUTHOR("Elena Salomatkina <nuclear12explosion@gmail.com>");
MODULE_DESCRIPTION("Device Mapper Proxy");
MODULE_LICENSE("GPL");
