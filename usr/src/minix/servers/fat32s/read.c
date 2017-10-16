#include "inc.h"
#include <lib.h>

// Implementations of several FS functions: pwd, cd, ls, stat, cat

// Print working directory
int _pwd() {
    stack_push(dir_buff.sector);

    char name[MAX_NAME_LEN];
    char full_path[1024];

    full_path[0] = '\0';

    int cluster_id = sector_to_cluster(dir_buff.sector);

    // Follow the parent chain
    while(find_child("..") == 0) {
        hop();
        stack_push(dir_buff.sector);

        int success = find_child_by_cluster_id(name, cluster_id);
        if(success != 0) {
            printf("This should never happen..");
            return -1;
        }
        // Append names to form a full path
        strcat(name, full_path);
        strcpy(full_path, "/");
        strcat(full_path, name); 
        
        jump_to_sector(stack_pop());

        cluster_id = sector_to_cluster(dir_buff.sector);
    }
    if(strlen(full_path) == 0) {
        printf("/\n");
    } else {
        printf("%s\n", full_path);
    } 
    jump_to_sector(stack_pop());
    return (OK);
}

// Change directory
int _cd(char* path) {
    // Just walk, and be careful not to hit files
    stack_push(dir_buff.sector);
    int status = walk(path, WALK_BAN_FILES);
    if(status != 0) {
        jump_to_sector(stack_pop());
        return status;
    }
    stack_pop();
    return 0;
}

// List current dir
int _ls(char* path) {
    stack_push(dir_buff.sector);
    if(strcmp(path, ".") != 0) {
        int status = _cd(path);
        if(status != 0) {
            jump_to_sector(stack_pop());
            return status;
        }
    }
    char part[MAX_NAME_LEN];
    char full_name[MAX_NAME_LEN];
    full_name[0] = '\0';

    while(dir_buff.de.type != DIR_TYPE_UNDEFINED) {
        if(dir_buff.de.type == DIR_TYPE_LONG) {
            // When you encounter a long dir concat its name to get one full name
            get_name(&dir_buff.de, part);
            strcat(part, full_name);
            strcpy(full_name, part);
        } else if(dir_buff.de.type == DIR_TYPE_SHORT) {
            // When you hit a short dir print the full name you created
            if(strlen(full_name) == 0) {
                get_name(&dir_buff.de, part);
                strcat(full_name, part);
            }
            if(is_directory(&dir_buff.de)) {
                printf("[*]");
            }
            printf("%s\n", full_name);
            full_name[0] = '\0';
        } else {
            printf("OOPS");
        }
        // Move to the next dir entry
        next_dir_entry();
    }
    jump_to_sector(stack_pop());
    return (OK);
}

// Print file cntents into file
int _cat(char* path) {
    // Walk further
    stack_push(dir_buff.sector);
    int status = walk(path, WALK_BAN_DIRS);
    if(status != 0) {
        jump_to_sector(stack_pop());
        return status;
    }
    
    // Find file size in parent
    stack_push(dir_buff.sector);
    dir_buff.sector = last_hop_sector;
    dir_buff.offset = last_hop_offset;
    load_sector(dir_buff.sector);
    parse_dir(&dir_buff.de, dir_buff.offset);
    unsigned int sz = dir_buff.de.u.reg.file_size;
    jump_to_sector(stack_pop());

    // Print all bytes in a file
    for(int i = 0; i < sz; i++) {
        printf("%c", fs_data.buff[dir_buff.offset]);
        dir_buff.offset++;
        if(dir_buff.offset == bpb.bytes_per_sector) {
            if((dir_buff.sector + 1 - fs_data.first_data_sector) % bpb.sectors_per_cluster == 0) {
                // You left this cluster, consult the FAT table to find the next one
                int cluster = sector_to_cluster(dir_buff.sector);
                int next_cluster = fat_get(cluster);
                dir_buff.sector = first_sector_of_cluster(next_cluster);
            } else {
                dir_buff.sector++;
            }
            // Load next sector
            dir_buff.offset = 0;
            load_sector(dir_buff.sector);
        }
    }
    printf("^EOF\n");
    jump_to_sector(stack_pop());
    return (OK);
}

// Print statistics
int _stat(char* path) {
    // Remove last path component
    int status;
    char* last_name = NULL;
    int len = strlen(path);
    int i = len - 1;
    for(int i = len - 2; i >= 0; i--) {
        if(path[i] == '/') {
            last_name = path+i+1;
            path[i] = '\0';
            break;
        }
    }

    // Walk to one level above
    stack_push(dir_buff.sector);
    if(!last_name) {
        last_name = path;
    } else {
        status = walk(path, WALK_BAN_FILES);
        if(status != 0) {
            jump_to_sector(stack_pop());
            return status;
        }
    }

    // Use the find_child function to find the full name
    status = find_child(last_name);
    if(status != 0) {
        jump_to_sector(stack_pop());
        return status;
    }
    // Print entry info and long name
    print_regular_dir_entry(&dir_buff.de.u.reg);
    print_long_name_rde(last_name);
    jump_to_sector(stack_pop());
    return OK;
}