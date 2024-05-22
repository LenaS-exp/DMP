make
insmod dmp.ko
echo "Input device size: "
read size
dmsetup create zero1 --table "0 $size zero"
ls -al /dev/mapper/*
dmsetup create dmp1 --table "0 $size dmp /dev/mapper/zero1 0"
ls -al /dev/mapper/*
./test.sh
