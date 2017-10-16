#ifndef _LGS_PROTO_H
#define _LGS_PROTO_H

// Function prototypes. 

// main.c 
int main(int argc, char **argv);

// fat32.c 
int do_mount(message *m_ptr);
int do_unmount(message *m_ptr);
int do_pwd(message *m_ptr);
int do_cd(message *m_ptr);
int do_ls(message *m_ptr);
int do_cat(message *m_ptr);
int do_stat(message *m_ptr);
int sef_cb_init(int type, sef_init_info_t *info);
int get_txt(char **txt, message *m_ptr);
void reset(void);

// read.c 
int _pwd();
int _cd(char* path);
int _ls(char* path);
int _cat(char* path);
int _stat(char* path);

// mount.c 
int read_bpb(Bpb* bpb_ptr, int dev_fd);

// tree.c
int hop();
int find_child(char* query_name);
int find_child_by_cluster_id(char* full_name, int cluster_id);
void jump_to_sector(int sector);
int next_dir_entry();
int walk(char* path , int flags);
void stack_push();
int stack_pop();


// util.c 
unsigned int get_fat_sector(unsigned int cluster);
unsigned int get_fat_offset(unsigned int cluster);
unsigned int load_sector(unsigned int sector);
unsigned int store_sector(unsigned int sector);
unsigned int first_sector_of_cluster(unsigned int cluster);
unsigned int sector_to_cluster(unsigned int sector);
unsigned int fat_get(unsigned int cluster);
void fat_set(unsigned int cluster, unsigned int target_cluster);
int get_dir_type(unsigned int off);
int parse_dir(struct dir_entry* de, unsigned int off);
int is_directory(struct dir_entry* de);
int get_name(struct dir_entry* de, char* name);


#endif
