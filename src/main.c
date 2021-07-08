// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
//           Made for JamesM's tutorials <www.jamesmolloy.co.uk>

#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "paging.h"
#include "multiboot.h"
#include "fs.h"
#include "initrd.h"
#include "vfs_fun.h"

extern u32int placement_address;

int main(struct multiboot *mboot_ptr) {
    // Initialise all the ISRs and segmentation
    init_descriptor_tables();
    // Initialise the screen (by clearing it)
    monitor_clear();

    //asm volatile("sti");
    // Find the location of our initial ramdisk.
    ASSERT(mboot_ptr->mods_count > 0);
    monitor_write("ASSERT SUCCESS!!!\n");
    u32int initrd_location = *((u32int*)mboot_ptr->mods_addr);
    u32int initrd_end = *(u32int*)(mboot_ptr->mods_addr+4);
    // Don't trample our module with placement accesses, please!
    placement_address = initrd_end;

    // Start paging.
    //initialise_paging();

    // Initialise the initial ramdisk, and set it as the filesystem root.
    fs_root = initialise_initrd(initrd_location);
    init_worker();
    command_fs = CREATE_FILE;
    asm volatile ("int $0x21");
    command_fs = CREATE_DIR;
    asm volatile ("int $0x21");
    command_fs = REMOVE_FILE;
    asm volatile ("int $0x21");
    command_fs = LS;
    asm volatile ("int $0x21");
    //asm volatile("int %0" : "a=" (IRQ1));

    return 0;
}
