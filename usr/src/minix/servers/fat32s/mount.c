#include "inc.h"
#include <lib.h>
#include <unistd.h>
#include <fcntl.h>

// Reads the BIOS parameter block using dev_fd file descriptor and puts it in bpb_ptr
int read_bpb(Bpb* bpb_ptr, int dev_fd)
{
    int status;
    char buff[BPB_SIZE];

    // Load sector 0 in buffer
    status = read(dev_fd, buff, BPB_SIZE);
    if (status != BPB_SIZE) {
        return(EINVAL);
    }

    // Copy to bpb_ptr
    memcpy(bpb_ptr, buff, sizeof(Bpb));

    // Check magic values
    fs_data.magic[0] = buff[510];
    fs_data.magic[1] = buff[511];
    if(*((unsigned short*)fs_data.magic) != MAGIC) {
        return (EINVAL);
    }

    // Calculate some fs useful values
    fs_data.first_data_sector = bpb_ptr->num_reserved_sectors + bpb_ptr->num_fats * bpb_ptr->fat_size_32;
    fs_data.num_data_sectors = bpb_ptr->total_sectors_32 - bpb_ptr->num_reserved_sectors - bpb_ptr->num_fats * bpb_ptr->fat_size_32;
    fs_data.num_data_clusters = fs_data.num_data_sectors / bpb_ptr->sectors_per_cluster;

    return (OK);
}