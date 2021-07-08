#include "vfs_fun.h"

char *tab_fs = "  ";

void create_file (u8int type, u8int inode) {
    u8int i = inode - 1;
    if (file_exist[i]) {
        monitor_write("File/directiry already exist\n\n");
        return;
    } else {
        file_exist[i] = 1;
    }
    static char *filename = "file00.txt";
    static char *dir = "dir00";
    if (type == FS_FILE) {
        filename[5]++;
        if (filename[5] > '9') {
            filename[4]++;
            filename[5] = '0';
        }
        strcpy(root_nodes[i].name, filename);
    } else if (type == FS_DIRECTORY) {
        dir[4]++;
        if (dir[4] > '9') {
            dir[3]++;
            dir[4] = '0';
        }
        strcpy(root_nodes[i].name, dir);
    }
    root_nodes[i].mask = 6;
    root_nodes[i].uid = 0;
    root_nodes[i].gid = 0;
    root_nodes[i].length = 0;
    root_nodes[i].flags = type;
    root_nodes[i].read = type == FS_FILE ? &initrd_read : 0;
    root_nodes[i].write = 0;
    root_nodes[i].readdir = type == FS_FILE ? &initrd_readdir : 0;
    root_nodes[i].finddir = type == FS_FILE ? &initrd_finddir : 0;
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].impl = 0;
    monitor_write("Created file ");
    monitor_write(root_nodes[i].name);
    monitor_write("!\n\n");
}

void remove_file (u8int inode) {
    u8int i = inode - 1;
    if (i == 0) {
        monitor_write("Can't delete dev\n\n");
        return;
    }
    if (!file_exist[i]) {
        monitor_write("File/directiry doesn't exist\n\n");
        return;
    } else {
        file_exist[i] = 0;
    }
    monitor_write("Deleted file ");
    monitor_write(root_nodes[i].name);
    monitor_write("!\n\n");
}

void ls () {
    int i = 0;
    struct dirent *node = 0;
    monitor_write("/ (root)\n");
    while ( (node = readdir_fs(fs_root, i)) != 0)
    {
        if (!file_exist[i]) {
            ++i;
            continue;
        }
        monitor_write(tab_fs);
        monitor_write(node->name);
        fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags&0x7) == FS_DIRECTORY)
        {
            monitor_write(" (directory)\n");
        }
        else
        {
            monitor_write(" (file)\n");
        }
        i++;
    }
    monitor_write("\n");
}

static void worker (registers_t regs) {
    switch (command_fs) {
        case CREATE_FILE:
            create_file(FS_FILE, current_inode);
            break;
        case CREATE_DIR:
            create_file(FS_DIRECTORY, current_inode);
            break;
        case REMOVE_FILE:
            remove_file(current_inode);
            break;
        case LS:
            ls();
            break;
        default:
            monitor_write("Incorrect command, try again\n");
            break;
    }
}

void init_worker () {
    for (int i = 0; i < 64; ++i) {
        file_exist[i] = 0;
    }
    file_exist[0] = 1;
    register_interrupt_handler(IRQ1, &worker);
}