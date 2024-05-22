# DMP
Linux kernel module that creates virtual block devices on top of an existing device mapper and monitors the statistics of performed operations on the device. Statistics are available through the sysfs module.

Supported statistics:
• Number of write requests - *write_reqs*
• Number of read requests - *read_reqs*
• Average block size per write - *average write_size*
• Average block size per read - *average read_size*
• Total number of requests - *total_reqs*
• Average block size - *average block size*

Tested on Ubuntu 22.04.4

| Filename                        | Description                                                                                                                                                             |
|--------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dmp.c           | source code                                                                                                        
| Makefile        | build script                    
| start.sh        | module connecting and creating test devices
| test.sh         | module testing  
| end.sh          | remove module and test devices

## Usage
Start:
```bash 
git clone https://github.com/LenaS-exp/DMP.git
cd DMP/
chmod +x start.sh test.sh end.sh
sudo ./start.sh
```
Testing:
```bash
sudo ./test.sh
```
Finish:
```bash
sudo ./end.sh
```
## Useful links
1. [Writing your own Device Mapper Target](https://medium.com/@gauravmmh1/writing-your-own-device-mapper-target-539689d19a89)
2. [Linux bio functions](https://github.com/torvalds/linux/blob/master/include/linux/bio.h)
3. [Sysfs in Linux Kernel](https://embetronicx.com/tutorials/linux/device-drivers/sysfs-in-linux-kernel/)
