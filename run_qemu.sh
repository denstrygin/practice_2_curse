#!/bin/bash

#запускаем в эмуляторе qemu

# ./make_initrd test.txt test.txt test2.txt test2.txt
./make_initrd
sudo make update_image
sudo qemu-system-i386 -initrd initrd.img -kernel src/MiniOS.bin