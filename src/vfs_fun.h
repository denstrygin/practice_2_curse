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
    REMOVE_DIR,
    LS
} COMM_TYPE;

char filename[128];
extern char *tab_fs;
int file_exist[64] = {0};
COMM_TYPE command_fs;

void create_file (u8int type);
void remove_file (u8int type);
void ls ();
void init_worker ();

#endif