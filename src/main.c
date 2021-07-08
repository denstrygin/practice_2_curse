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

void send_interrupt(COMM_TYPE type, u8int inode) {
    command_fs = type;
    current_inode = inode;
    asm volatile ("int $0x21");
}

int main(struct multiboot *mboot_ptr) {
    // Initialise all the ISRs and segmentation
    init_descriptor_tables();
    // Initialise the screen (by clearing it)
    monitor_clear();

    //asm volatile("sti");
    // Find the location of our initial ramdisk.
    ASSERT(mboot_ptr->mods_count > 0);
    monitor_write("VFS DEMO\n\n");
    u32int initrd_location = *((u32int*)mboot_ptr->mods_addr);
    u32int initrd_end = *(u32int*)(mboot_ptr->mods_addr+4);
    // Don't trample our module with placement accesses, please!
    placement_address = initrd_end;

    // Initialise the initial ramdisk, and set it as the filesystem root.
    fs_root = initialise_initrd(initrd_location);
    init_worker();

    //creating file with inode 2 and dir with inode 7
    send_interrupt(CREATE_FILE, 2);
    send_interrupt(CREATE_DIR, 7);
    send_interrupt(LS, 0);
    //removing file with inode 5 (doesn't exist) and 7
    send_interrupt(REMOVE_FILE, 5);
    send_interrupt(REMOVE_FILE, 7);
    send_interrupt(LS, 0);

    return 0;
}
