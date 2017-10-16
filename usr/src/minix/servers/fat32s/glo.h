#ifndef FAT32_GLO_H
#define FAT32_GLO_H
#include <assert.h>

// Size constaints
#define MAX_BYTES_PER_SECTOR 4096
#define BPB_SIZE 512
#define DIRENTRY_SIZE 32
#define STACK_SIZE 128
#define MAX_NAME_LEN 512
#define MAX_DEVICE_LABEL 16

// Masks for FAT entries
#define FAT_LO_MASK 0x0FFFFFFF
#define FAT_HI_MASK 0xF0000000

// FAT magic number
#define MAGIC 0xAA55

// Types of FAT directories
#define DIR_TYPE_UNDEFINED 0
#define DIR_TYPE_SHORT 1
#define DIR_TYPE_LONG 2

// Flags for Walk function
#define WALK_BAN_FILES 1
#define WALK_BAN_DIRS 2

#define P __attribute__((packed))

// BIOS parameter block
typedef struct P bpb {
    unsigned char       jump_boot[3];
    char                oem_name[8];
    unsigned short      bytes_per_sector;
    unsigned char       sectors_per_cluster;
    unsigned short      num_reserved_sectors;
    unsigned char       num_fats;
    unsigned short      num_root_entries;   // 0 for FAT32
    unsigned short      total_sectors_16;   // 0 for fat32
    unsigned char       media;              // 0xF8
    unsigned short      fat_size_16;        // 0 for FAT32
    unsigned short      sectors_per_track;  
    unsigned short      num_heads;
    unsigned int        hidden_sectors;
    unsigned int        total_sectors_32;
    unsigned int        fat_size_32;
    unsigned short      ext_flags;  
    unsigned short      fs_version;
    unsigned int        root_cluster;
    unsigned short      fs_info_sector;
    unsigned short      backup_boot_sector;
    unsigned char       reserved[12];
    
    unsigned char       drive_number;
    unsigned char       bs_reserved;
    unsigned char       boot_signature;
    unsigned int        volume_id;
    char                volume_label[11];
    char                fs_type[8];
} Bpb; // 90

// FSInfo block
typedef struct P fs_info {
    unsigned int        lead_sig;
    unsigned char       reserved[480];
    unsigned int        struct_sig;
    unsigned int        free_count;
    unsigned int        next_free;
    unsigned char       reserved2[12];
    unsigned char       trail_sig[4];

} FsInfo; // 512

// Short name directory
typedef struct P regular_dir_entry {
    char                short_name[11];
    unsigned char       attributes;
    unsigned char       nt_res;
    unsigned char       time_created_10ms;
    unsigned short      time_created;
    unsigned short      date_created;
    unsigned short      date_last_access;
    unsigned short      first_cluster_hi;
    unsigned short      time_last_write; // extract these (pg29)
    unsigned short      date_last_write;
    unsigned short      first_cluster_lo;
    unsigned int        file_size;
} RegDirEntry; // 32

// Long name directory
typedef struct P long_dir_entry {
    unsigned char       order;
    char                name1[10];
    unsigned char       attributes;
    unsigned char       type;
    unsigned char       checksum;
    char                name2[12];
    unsigned short      first_cluster_lo; // always 0
    char                name3[4];
} LongDirEntry; // 32

// A directory
typedef struct dir_entry {
    int type;
    union {
        RegDirEntry reg;
        LongDirEntry lng;
    } u;
} DirEntry; // 36

// Contains buffer and some computed values
typedef struct P fs_data {
    unsigned char       magic[2];           // 519 511
    unsigned int        first_data_sector;
    unsigned int        num_data_sectors;
    unsigned int        num_data_clusters;


    unsigned char       buff[MAX_BYTES_PER_SECTOR];
    unsigned int        curr_sector;
} FsData;

// Currently active dir entry
typedef struct P dir_buff {
    DirEntry de;
    int     sector;
    int     offset;
} DirBuff;

// Explained in fat32.c
extern DirBuff              dir_buff;
extern int                  stack[STACK_SIZE];
extern int                  sp;
extern Bpb                  bpb;
extern FsData               fs_data;
extern int                  device_fd;
extern char                 device_label[16];
extern int                  last_hop_sector;
extern int                  last_hop_offset;

#define fat32_debug printf

#endif /* FAT32_GLO_H */
