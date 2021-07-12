#include "vfs_fun.h"

char *tab_fs = "  ";
extern int current_inode;
extern int error_code;
extern COMM_TYPE command_fs;

int create_file (u8int type, u8int inode) {
    u8int i = inode;
    if (i >= max_count_of_file) {
        monitor_write("Incorrect inode!\n\n");
        return 1;
    }
    if (root_nodes[i].flags) {
        monitor_write("File/directiry ");
        monitor_write(root_nodes[i].name);
        monitor_write(" already exist\n\n");
        return 2;
    }
    static char *filename = "file00.txt";
    static char *dir = "dir00";
    ++nroot_nodes;
    if (type == FS_FILE) {
        filename[5]++;
        if (filename[5] > '9') {
            filename[4]++;
            filename[5] = '0';
        }
        strcpy(root_nodes[i].name, filename);
        root_nodes[i].name[strlen(root_nodes[i].name)] = 0;
    } else if (type == FS_DIRECTORY) {
        dir[4]++;
        if (dir[4] > '9') {
            dir[3]++;
            dir[4] = '0';
        }
        strcpy(root_nodes[i].name, dir);
        root_nodes[i].name[strlen(root_nodes[i].name)] = 0;
    }
    root_nodes[i].mask = 6;
    root_nodes[i].uid = 0;
    root_nodes[i].gid = 0;
    root_nodes[i].length = 0;
    root_nodes[i].flags = type;
    root_nodes[i].read = type == FS_FILE ? &initrd_read : 0;
    root_nodes[i].write = 0;
    root_nodes[i].readdir = type == FS_DIRECTORY ? &initrd_readdir : 0;
    root_nodes[i].finddir = type == FS_DIRECTORY ? &initrd_finddir : 0;
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].impl = 0;
    monitor_write("Created ");
    if (type == FS_FILE) {
        monitor_write("file ");
    } else {
        monitor_write("directory ");
    }
    monitor_write(root_nodes[i].name);
    monitor_write("!\n\n");
    return 0;
}

int remove_file (u8int inode) {
    u8int i = inode - 1;
    if (i > 64) {
        monitor_write("Incorrect inode!\n\n");
        return 1;
    }
    if (i == 0) {
        monitor_write("Can't delete dev\n\n");
        return 2;
    }
    u32int type = root_nodes[i].flags;
    if (!type) {
        monitor_write("File/directiry ");
        monitor_write(root_nodes[i].name);
        monitor_write(" doesn't exist\n\n");
        return 3;
    } else {
        root_nodes[i].flags = 0;
    }
    monitor_write("Deleted ");
    if (type == FS_FILE) {
        monitor_write("file ");
    } else {
        monitor_write("directory ");
    }
    monitor_write(root_nodes[i].name);
    monitor_write("!\n\n");
    return 0;
}

void ls (fs_node_t *fs_node, int step) {
    int i = 0;
    struct dirent *node = 0;
    if (fs_node == fs_root) {
        monitor_write(fs_node->name);
        monitor_write(" (root)\n");
    }
    while ( (node = readdir_fs(fs_node, i)) != 0)
    {
        fs_node_t *fsnode = finddir_fs(fs_node, node->name);
        if (fsnode->flags == 0) {
            ++i;
            continue;
        }
        for (int j = 0; j < step + 1; ++j) {
            monitor_write(tab_fs);
        }
        //monitor_write_dec(fsnode->flags);
        if ((fsnode->flags&0x7) == FS_DIRECTORY)
        {
            monitor_write(node->name);
            monitor_write(" (directory)\n");
            //ls(fsnode, step + 1);
        }
        else if ((fsnode->flags&0x7) == FS_FILE)
        {
            monitor_write(node->name);
            monitor_write(" (file)\n");
            for (int j = 0; j < step + 2; ++j) {
                monitor_write(tab_fs);
            }
            monitor_write("contents: \"");
            char buf[256];
            u32int sz = read_fs(fsnode, 0, 256, buf);
            for (int j = 0; j < sz; j++) {
                monitor_put(buf[j]);
            }
            monitor_write("\"\n");
        }
        i++;
    }
    if (fs_node == fs_root) {
        monitor_write("\n");
    }
}

static void worker (registers_t regs) {
    switch (command_fs) {
        case CREATE_FILE:
            error_code = create_file(FS_FILE, current_inode);
            break;
        case CREATE_DIR:
            error_code = create_file(FS_DIRECTORY, current_inode);
            break;
        case REMOVE_FILE:
            error_code = remove_file(current_inode);
            break;
        case LS:
            ls(fs_root, 0);
            break;
        default:
            monitor_write("Incorrect command, try again\n");
            break;
    }
}

void init_worker () {
    register_interrupt_handler(IRQ1, &worker);
}