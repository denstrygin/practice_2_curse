# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

SOURCES=boot.o monitor.o main.o common.o descriptor_tables.o isr.o interrupt.o gdt.o timer.o \
        kheap.o paging.o ordered_array.o fs.o initrd.o vfs_fun.o

CFLAGS=-Wall -fno-builtin -fno-stack-protector -nostdinc -nostdlib -ggdb3 -m32
LDFLAGS=-T ./link.ld -m elf_i386
ASFLAGS=-felf

all: $(SOURCES) link

clean:
	-rm *.o kernel

link:
	ld $(LDFLAGS) -o MiniOS.bin $(SOURCES)
	cp MiniOS.bin MiniOS.bin.dbg 
	strip MiniOS.bin

.s.o:
	nasm $(ASFLAGS) $<
