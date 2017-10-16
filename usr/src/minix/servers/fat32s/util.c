#include "inc.h"
#include <lib.h>
#include <unistd.h>
#include <fcntl.h>

// Utility functions for easily calculating memory offsets etc.

// Prints 32 bytes starting from buff[off]
void print_bytes(unsigned int off) {
    printf("Parsing dir_entry at offset: %d\n", off);
    printf("\tBytes at offset:");
    for (int i = 0; i < 32; ++i) {
        if(i % 8 == 0) printf("\n\t\t");
        printf("%02x ", fs_data.buff[off + i]);
    }
    printf("\n");
}

// Gets FAT table sector and offset for this cluster
unsigned int get_fat_sector(unsigned int cluster) {
    return bpb.num_reserved_sectors + (cluster * 4) / bpb.bytes_per_sector;
}
unsigned int get_fat_offset(unsigned int cluster) {
    return (cluster * 4) % bpb.bytes_per_sector;
}

// Loads one sector into memory
unsigned int load_sector(unsigned int sector) {
    if(fs_data.curr_sector != sector) {
        fs_data.curr_sector = sector;
        off_t seek_len = sector * bpb.bytes_per_sector;
        int seek_actual = lseek(device_fd, seek_len, SEEK_SET);
        unsigned int ret = read(device_fd, fs_data.buff, bpb.bytes_per_sector);
        return ret;
    }
    return bpb.bytes_per_sector; // Cache
}

// Stores currently active sector on disk
unsigned int store_sector(unsigned int sector) {
    lseek(device_fd, sector * bpb.bytes_per_sector, SEEK_SET);
    return write(device_fd, fs_data.buff, bpb.bytes_per_sector);
}

// For a cluster find its first sector
unsigned int first_sector_of_cluster(unsigned int cluster) {
    return fs_data.first_data_sector + ((cluster - 2) * bpb.sectors_per_cluster);
}

// For a sector finds the cluster it belongs to
unsigned int sector_to_cluster(unsigned int sector) {
    return (sector - fs_data.first_data_sector) / bpb.sectors_per_cluster + 2;
}

// Gets a value from FAT table: returns FAT[cluster]
unsigned int fat_get(unsigned int cluster) {
    load_sector(get_fat_sector(cluster));
    return (*((unsigned int *) &fs_data.buff[get_fat_offset(cluster)])) & FAT_LO_MASK;
}

// Sets a FAT table value: does FAT[cluster] = target_cluster
void fat_set(unsigned int cluster, unsigned int target_cluster) {
    unsigned int* entry = (unsigned int *) &fs_data.buff[get_fat_offset(cluster)];
    *entry = *entry & FAT_HI_MASK;
    *entry = *entry | (cluster & FAT_LO_MASK);
    store_sector(get_fat_sector(cluster));
}

// Returns directory type
int get_dir_type(unsigned int off) {
    if(fs_data.buff[off + 11] == (unsigned char)0x0F && \
     fs_data.buff[off + 12] == (unsigned char)0x00) {
        return DIR_TYPE_LONG;
    }
    return DIR_TYPE_SHORT;
}

// Parses 32B starting at buff[of] as a directory entry
int parse_dir(struct dir_entry* de, unsigned int off) {
    assert(off % 32 == 0);
    assert(sizeof(de->u) == 32);

    if(fs_data.buff[off] == (unsigned char)0) {
        return (de->type = DIR_TYPE_UNDEFINED);
    }
    de->type = get_dir_type(off);
    memcpy(&(de->u), fs_data.buff+off, sizeof(de->u));

    return de->type;
}

// Checks if a dir entry represents a directory
int is_directory(struct dir_entry* de) {
    return (de->u.reg.attributes & 0x10) > 0;
}

// Gets the name string from a dir entry
int get_name(struct dir_entry* de, char* name) {
    int idx = 0, i;
    switch(de->type) {
        case DIR_TYPE_UNDEFINED:
            return 0;
        case DIR_TYPE_SHORT:
            for(i = 0; i < 8 && de->u.reg.short_name[i] != ' '; ++i) {
                name[idx++] = de->u.reg.short_name[i];
            }
            if(name[0] == '.') {
                name[idx] = '\0';
                return 0;
            }
            name[idx++] = '.';
            for(i = 8; i < 11 && de->u.reg.short_name[i] != ' '; ++i) {
                name[idx++] = de->u.reg.short_name[i];
            }
            if(i == 8) {
                idx--;
            }
            name[idx] = '\0';
            return 1;
        case DIR_TYPE_LONG:
            for (i = 0; i < 10; i += 2) {
                name[idx++] = de->u.lng.name1[i];
            }
            for (i = 0; i < 12; i += 2) {
                name[idx++] = de->u.lng.name2[i];
            }
            for (i = 0; i < 4; i += 2) {
                name[idx++] = de->u.lng.name3[i];
            }
            name[idx] = '\0';
            return 1;
    }
    return 0;
}
