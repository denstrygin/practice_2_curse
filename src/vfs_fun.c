#include "vfs_fun.h"

char *tab_fs = "  ";

void create_file (u8int type) {
    if (type == FS_FILE) {
        monitor_write("ogga\n");
        file_exist[1] = 1;
    } else {
        //monitor_write("ogga\n");
        file_exist[0] = 1;
    }
    u8int i = 0;
    // Create a new file node.
    //strcpy(root_nodes[i].name, filename);
    // root_nodes[i].mask = 110;
    // root_nodes[i].uid = 0;
    // root_nodes[i].gid = 0;
    // root_nodes[i].length = 0;
    // root_nodes[i].flags = type;
    // root_nodes[i].read = type == FS_FILE ? &initrd_read : 0;
    // root_nodes[i].write = 0;
    // root_nodes[i].readdir = type == FS_FILE ? &initrd_readdir : 0;
    // root_nodes[i].finddir = type == FS_FILE ? &initrd_finddir : 0;
    // root_nodes[i].open = 0;
    // root_nodes[i].close = 0;
    // root_nodes[i].impl = 0;
}

void remove_file (u8int type) {
    if (type == FS_FILE) {
        monitor_write("bogga\n");
    } else {
        //monitor_write("bogga\n");
    }
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
            monitor_write("\n"); monitor_write(tab_fs); monitor_write(tab_fs);
            monitor_write("contents: \"");
            char buf[256];
            u32int sz = read_fs(fsnode, 0, 256, buf);
            int j;
            for (j = 0; j < sz; j++)
                monitor_put(buf[j]);
            
            monitor_write("\"\n");
        }
        i++;
    }
}

static void worker (registers_t regs) {
    switch (command_fs) {
        case CREATE_FILE:
            create_file(FS_FILE);
            break;
        case CREATE_DIR:
            create_file(FS_DIRECTORY);
            break;
        case REMOVE_FILE:
            remove_file(FS_FILE);
            break;
        case REMOVE_DIR:
            remove_file(FS_DIRECTORY);
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
    register_interrupt_handler(IRQ1, &worker);
}