umount /mnt
mount -t vbfs -o share=fat32 none /mnt
cp -R /mnt/test /home
cd /mnt/test
clang test.c -o test -lsys -ltimers
./test