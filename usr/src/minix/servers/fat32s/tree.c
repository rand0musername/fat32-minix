#include "inc.h"
#include <lib.h>
#include <unistd.h>
#include <fcntl.h>

// Helper functions for navigating the file tree

// There is always one currently active directory entry: look sat the file it represents
// and jumps there
int hop() {
    if(dir_buff.de.type != DIR_TYPE_SHORT) {
        return EINVAL;
    }
    last_hop_sector = dir_buff.sector;
    last_hop_offset = dir_buff.offset;

    // Compute cluster number
    RegDirEntry reg_dir = dir_buff.de.u.reg;
    int cluster = reg_dir.first_cluster_lo + (reg_dir.first_cluster_hi << 16);
    if(cluster == 0) {
        cluster = bpb.root_cluster;
    }

    // Find first sector in a cluster and hop there
    dir_buff.sector = first_sector_of_cluster(cluster); 
    dir_buff.offset = 0;
    load_sector(dir_buff.sector);
    parse_dir(&dir_buff.de, dir_buff.offset);
    return OK;
}


// Finds a child with the given name, dir_buff points to the start of the cluster
int find_child(char* query_name) {
    char part[MAX_NAME_LEN];
    char full_name[MAX_NAME_LEN];
    full_name[0] = '\0';

    while(dir_buff.de.type != DIR_TYPE_UNDEFINED) {
        if(dir_buff.de.type == DIR_TYPE_LONG) {
            // Concat pieces of long name
            get_name(&dir_buff.de, part);
            strcat(part, full_name);
            strcpy(full_name, part);
        } else if(dir_buff.de.type == DIR_TYPE_SHORT) {
            // Compare full name to the query name
            if(strlen(full_name) == 0) {
                get_name(&dir_buff.de, part);
                strcat(full_name, part);
            }
            if(strcmp(full_name, query_name) == 0) {
                // Found it
                return 0;
            }
            full_name[0] = '\0';
        }
        // Move to the next dir entry
        next_dir_entry();
    }
    return 1; // Not found
}


// Starts from dir_buff and looks for a child node with cluster id 
// cluster_id and in process populates the file name
int find_child_by_cluster_id(char* full_name, int cluster_id) {
    
    char part[MAX_NAME_LEN];
    full_name[0] = '\0';

    while(dir_buff.de.type != DIR_TYPE_UNDEFINED) {
        if(dir_buff.de.type == DIR_TYPE_LONG) {
            // Concat pieces of long name
            get_name(&dir_buff.de, part);
            strcat(part, full_name);
            strcpy(full_name, part);
        } else if(dir_buff.de.type == DIR_TYPE_SHORT) {
            // Compare cluster number to query cluster number and finalize name
            if(strlen(full_name) == 0) {
                get_name(&dir_buff.de, part);
                strcat(full_name, part);
            }
            RegDirEntry reg = dir_buff.de.u.reg;
            int curr_cluster_id = (reg.first_cluster_hi << 16) + reg.first_cluster_lo;
            if(curr_cluster_id == 0) {
                // Root cluster
                curr_cluster_id = 2;
            }
            if(cluster_id == curr_cluster_id) {
                // Found it 
                return 0;
            }
            full_name[0] = '\0';
        }
        // Move to the next dir entry
        next_dir_entry();
    }
    return 1; // Not found
}

// Jumps straight to this sector
void jump_to_sector(int sector) {
    dir_buff.sector = sector;
    dir_buff.offset = 0;
    load_sector(dir_buff.sector);
    parse_dir(&dir_buff.de, dir_buff.offset);
}

// Moves to the next directory entry and returns its type
int next_dir_entry() {
    if(dir_buff.offset+DIRENTRY_SIZE == bpb.bytes_per_sector) {
        if((dir_buff.sector + 1 - fs_data.first_data_sector) % bpb.sectors_per_cluster == 0) {
            // Leaving this cluster, consult the FAT table to see where to go next
            int cluster = sector_to_cluster(dir_buff.sector);
            int next_cluster = fat_get(cluster);
            dir_buff.sector = first_sector_of_cluster(next_cluster);
        } else {
            dir_buff.sector++;
        }
        // Left the sector, load another sector
        dir_buff.offset = 0;
        load_sector(dir_buff.sector);
    } else {
        dir_buff.offset += DIRENTRY_SIZE;
    }
    return parse_dir(&dir_buff.de, dir_buff.offset);
}

// Performs a walk from current position (dir_buff)
// following the path specified by the / delimited string path

// Flags: set WALK_BAN_FILES to ban files from being the last node on the walk (ex. cd),
//        set WALK_BAN_DIRS to ban dirs from being the last node on the walk (ex cat).
int walk(char* path, int flags) {
    // Parse flags
    int ban_files = (flags & WALK_BAN_FILES) > 0;
    int ban_dirs = (flags & WALK_BAN_DIRS) > 0;

    stack_push(dir_buff.sector);

    int left = 0;
    int right = 0;
    int len = strlen(path);
    while(right < len) {
        // Split the string into path sesgments
        while(right < len && path[right] != '/') {
            right++;
        }
        path[right] = '\0';

        // For each path segment find the child and jump there
        if(find_child(path + left) != 0) {
            jump_to_sector(stack_pop());
            return 1;
        }
        
        // If this is not the end we hit a file, abort
        if(right < len && !is_directory(&dir_buff.de)) {
            jump_to_sector(stack_pop());
            return 2;
        }

        // If this is the end look at the flags and potentially abort
        if(right == len) {
            int is_dir = is_directory(&dir_buff.de);
            if((ban_dirs && is_dir) || (ban_files && !is_dir)) {
                jump_to_sector(stack_pop());
                return 2;
            }
        }

        // Make one hop
        hop();
        right++;
        left = right;
    }
    stack_pop(); // Roll back the current DE pointer
    return 0;
}

// Stack functions, used for saving sector numbers and rolling back
void stack_push(int sector) {
    stack[sp++] = sector;
}
int stack_pop() {
    return stack[--sp];
}