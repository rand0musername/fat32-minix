#include "inc.h"
#include <lib.h>
#include <unistd.h>
#include <fcntl.h>

// Global variables
Bpb bpb;                                // BIOS parameter block
FsData fs_data;                         // Holds the current buffer and some computed values
int device_fd;                          // File descriptor for currently "mounted" device
char device_label[MAX_DEVICE_LABEL];    // Label of currently "mounted" device
DirBuff dir_buff;                       // Currently active directory entry
int stack[STACK_SIZE], sp;              // Stack and stack pointer, holds sectors
int last_hop_sector, last_hop_offset;   // Last (sector, offset) pair we visited

// For copying the message contents
char* txt;

// Important invariant: after each command we are at the beggining of some cluster
// Mounts a filesystem
int do_mount(message *m_ptr) {
    if(device_fd != -1) {
        printf("FAT32S: A filesystem is already mounted.\n");
        return (EINVAL);
    }
    if(get_txt(&txt, m_ptr) != OK) {
        free(txt);
        printf("FAT32S: Memcpy failed.\n");
        return (EINVAL);
    }
    strcpy(device_label, txt);
    device_fd = open(device_label, O_RDWR); // Actually opens the device file
    if(device_fd == -1) {
        printf("FAT32S: Can't open the device file.\n");
        return (EINVAL);
    }
    if(read_bpb(&bpb, device_fd) != OK) {
        printf("FAT32S: Invalid BPB.\n");
        return (EINVAL);
    }
    print_welcome_stats();
    free(txt);

    // Move to root sector with offset 0 and load that area
    dir_buff.sector = first_sector_of_cluster(bpb.root_cluster);
    dir_buff.offset = 0;
    load_sector(dir_buff.sector);
    parse_dir(&dir_buff.de, dir_buff.offset);

    sp = 0;
    return (OK);
}

// Umnounts a filesystem
int do_unmount(message *m_ptr) {
    if(device_fd == -1) {
        printf("FAT32S: Nothing is mounted.\n");
        return (EINVAL);
    }
    if(get_txt(&txt, m_ptr) != OK) {
        free(txt);
        printf("FAT32S: Memcpy failed.\n");
        return (EINVAL);
    }
    if(strcmp(device_label, txt) != OK) {
        free(txt);
        printf("FAT32S: The device with this label not mounted.\n");
        return (EINVAL);
    }
    if(close(device_fd) != OK) {
        free(txt);
        printf("FAT32S: Closing failed.\n");
        return (EINVAL);
    }
    reset();
    free(txt);
    return (OK);
}

// Prints working directory
int do_pwd(message *m_ptr) {
    if(device_fd == -1) {
        printf("FAT32S: Nothing is mounted.\n");
        return (EINVAL);
    }
    _pwd();
    return (OK);
}

// Changes directory
int do_cd(message *m_ptr) {
    if(device_fd == -1) {
        printf("FAT32S: Nothing is mounted.\n");
        return (EINVAL);
    }
    if(get_txt(&txt, m_ptr) != OK) {
        free(txt);
        printf("FAT32S: Memcpy failed.\n");
        return (EFAULT);
    }
    int status = _cd(txt);
    if(status == 1) {
        printf("FAT32S: Can't find the requested file.\n");
        return (ENOENT);
    } else if(status == 2) {
        printf("FAT32S: Can't cd into a file.\n");
        return (EINVAL);
    }
    free(txt);
    return (OK);
}

// Lists the contents of the directory
int do_ls(message *m_ptr) {
    if(device_fd == -1) {
        printf("FAT32S: Nothing is mounted.\n");
        return (EINVAL);
    }
    if(get_txt(&txt, m_ptr) != OK) {
        free(txt);
        printf("FAT32S: Memcpy failed.\n");
        return (EINVAL);
    }
    int status = _ls(txt);
    if(status != 0) {
        printf("FAT32S: You can't ls like that.");
    }
    return status;
}

// Writes files to stdout
int do_cat(message *m_ptr) {
    if(device_fd == -1) {
        printf("FAT32S: Nothing is mounted.\n");
        return (EINVAL);
    }
    if(get_txt(&txt, m_ptr) != OK) {
        free(txt);
        printf("FAT32S: Memcpy failed.\n");
        return (EINVAL);
    }
    int status = _cat(txt);
    if(status != 0) {
        printf("FAT32S: You can't cat the requested file.");
    }
    return status;
}

// Prints information about a file
int do_stat(message *m_ptr) {
    if(device_fd == -1) {
        printf("FAT32S: Nothing is mounted.\n");
        return (EINVAL);
    }
    if(get_txt(&txt, m_ptr) != OK) {
        free(txt);
        printf("FAT32S: Memcpy failed.\n");
        return (EINVAL);
    }
    int status = _stat(txt);
    if(status != OK) {
        printf("FAT32S: Stat error.\n");
    }
    return status;
}

// Initializes SEF
int sef_cb_init(int type, sef_init_info_t *info) {
    printf("sef_cb_init\n");
    assert(sizeof(Bpb) == 90);
    assert(sizeof(FsInfo) == 512);
    assert(sizeof(RegDirEntry) == 32);
    assert(sizeof(LongDirEntry) == 32);
    assert(sizeof(DirEntry) == 36);
    reset();
    return (OK);
}

// Resets the memory state
void reset() {
    memset(&bpb, 0, sizeof(bpb));
    memset(&fs_data, 0, sizeof(fs_data));
    device_fd = -1;
    memset(device_label, 0, sizeof(device_label));
    memset(&dir_buff, 0, sizeof(dir_buff));
    memset(stack, 0, sizeof(stack));
    sp = 0;
    last_hop_sector = last_hop_offset = 0;
}

// Extracts text from pointed location
int get_txt(char **txt, message *m_ptr) {
    int len = m_ptr->m_fat32s.txt_len;
    (*txt) = (char*) malloc(len+1);
    int status = sys_datacopy(m_ptr->m_source, m_ptr->m_fat32s.txt_loc, SELF, 
                              (vir_bytes) (*txt), m_ptr->m_fat32s.txt_len);
    (*txt)[len] = '\0';
    return status;
}