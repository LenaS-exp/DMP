dd if=/dev/random of=/dev/mapper/dmp1 bs=4k count=1
dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=1
cat /sys/module/dmp/stat/volumes
