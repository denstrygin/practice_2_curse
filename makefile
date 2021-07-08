#!/bin/bash
#обновление ядра в образе
update_image: compile
	@losetup /dev/loop1 ./hdd.img
	@echo "Mounting partition to /dev/loop2..."
	@losetup /dev/loop2 ./hdd.img \
	--offset	`echo \`fdisk -lu /dev/loop1 | sed -n 9p | awk '{print $$3}'\`*512 | bc` \
	--sizelimit `echo \`fdisk -lu /dev/loop1 | sed -n 9p | awk '{print $$4}'\`*512 | bc`
	@losetup -d /dev/loop1

	@echo "Write new MiniOS.bin to hdd.img..."
	@mkdir -p tempdir
	@mount /dev/loop2 tempdir
	@cp src/MiniOS.bin tempdir/
	@umount /dev/loop2
	@rm -r tempdir
	@losetup -d /dev/loop2

#компиляция исходников. они в каталоге src, запускаем там свой Makefile
compile:
	make --directory=src

#создание образа
image:
	@echo "Creating hdd.img..."
	@dd if=/dev/zero of=./hdd.img bs=512 count=16065 1>/dev/null 2>&1

	@echo "Creating bootable first FAT32 partition..."
	@losetup /dev/loop1 ./hdd.img
	@(echo c; echo u; echo n; echo p; echo 1; echo ;  echo ; echo a; echo 1; echo t; echo c; echo w;) | fdisk /dev/loop1 1>/dev/null 2>&1 || true

	@echo "Mounting partition to /dev/loop2..."
	@losetup /dev/loop2 ./hdd.img \
	--offset	`echo \`fdisk -lu /dev/loop1 | sed -n 9p | awk '{print $$3}'\`*512 | bc` \
	--sizelimit `echo \`fdisk -lu /dev/loop1 | sed -n 9p | awk '{print $$4}'\`*512 | bc`
	@losetup -d /dev/loop1

	@echo "Format partition..."
	@mkdosfs -I -n 'MiniOS' /dev/loop2

	@echo "Copy kernel and grub files on partition..."
	@mkdir -p tempdir
	@mount /dev/loop2 tempdir
	@mkdir tempdir/boot
	@cp -r grub tempdir/boot/
	@cp ./src/MiniOS.bin tempdir/
	@sleep 1
	@umount /dev/loop2
	@rm -r tempdir
	@losetup -d /dev/loop2

	@echo "Installing GRUB..."
	#@echo "device (hd0) hdd.img \n \
	#	   root (hd0,0)		 \n \
	#	   setup (hd0)		  \n \
	#	   quit\n" | grub-install >/dev/null

#	@mount -o loop,rw,offset=1048576 ./hdd.img ./hdd/
#	@losetup --partscan /dev/loop1 ./hdd.img
#	@mount /dev/loop1p1 ./hdd
#	grub-install --recheck --boot-directory=./hdd/ /dev/loop1
#	@losetup --detach /dev/loop1

	# @kpartx -v -a /dev/loop0
	# @losetup /dev/loop1 /dev/mapper/control
	# @mount /dev/loop1 /mnt

	# @mkdir -p /mnt/boot/grub
	# @cat > /mnt/boot/grub/device.map <<EOF
	# (hd0)   /dev/loop0
	# (hd0,1) /dev/loop1
	# EOF

	# @mount --bind /dev /mnt/dev
	# @chroot /mnt grub-mkconfig -o /boot/grub/grub.cfg

	# grub-install --no-floppy --grub-mkdevicemap=/mnt/boot/grub/device.map --root-directory=/mnt /dev/loop0

	# @umount /mnt/dev
	# @umount /mnt
	# @losetup -d /dev/loop1
	# @kpartx -v -d /dev/loop0

	# @echo "Done!"

#очистка скомпилированных данных
clean:
	make clean --directory=src

#освобождение занятых ресурсов (если аварийно заверешены команды компиляции)
umount:
	@umount /dev/loop2
	@losetup -d /dev/loop1
	@losetup -d /dev/loop2
