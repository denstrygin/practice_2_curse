#!/bin/bash

#запускаем в эмуляторе qemu

./make_initrd test.txt test.txt test2.txt test2.txt #форматируем диск и добавляем файлы test.txt и text2.txt
sudo make update_image #компилим исходники в папке src в ядро и обновляем ядро в нашем образе
sudo qemu-system-i386 -initrd initrd.img -kernel src/MiniOS.bin #запускаем qemu