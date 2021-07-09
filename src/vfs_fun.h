#ifndef VFS_FUN_H
#define VFS_FUN_H

#include "common.h"
#include "fs.h"
#include "initrd.h"
#include "isr.h"
#include "monitor.h"

typedef enum COMM_TYPE {
    CREATE_FILE,
    CREATE_DIR,
    REMOVE_FILE,
    LS
} COMM_TYPE;

int current_inode;
int error_code;
extern char *tab_fs;
int file_exist[64];
COMM_TYPE command_fs;

int create_file (u8int type, u8int inode);
int remove_file (u8int inode);
void ls ();
void init_worker ();

#endif