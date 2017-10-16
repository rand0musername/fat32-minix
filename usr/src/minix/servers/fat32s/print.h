#ifndef FAT32_PRINT_H
#define FAT32_PRINT_H

// Headers for print.c

void print_magic_number(void);

void print_jump_boot(void);
void print_oem_name(void);
void print_bytes_per_sector(void);
void print_sectors_per_cluster(void);
void print_num_reserved_sectors(void);
void print_num_fats(void);
void print_num_root_entries(void);
void print_total_sectors_16(void);
void print_media(void);
void print_fat_size_16(void);
void print_sectors_per_track(void);
void print_num_heads(void);
void print_hidden_sectors(void);
void print_total_sectors_32(void);
void print_fat_size_32(void);
void print_ext_flags(void);
void print_fs_version(void);
void print_root_cluster(void);
void print_fs_info_sector(void);
void print_backup_boot_sector(void);
void print_reserved(void);
void print_drive_number(void);
void print_bs_reserved(void);
void print_boot_signature(void);
void print_volume_id(void);
void print_volume_label(void);
void print_fs_type(void);

void print_bpb(void);

void print_short_name_rde(struct regular_dir_entry* rde);
void print_attributes_rde(struct regular_dir_entry* rde);
void print_nt_res_rde(struct regular_dir_entry* rde);
void print_time_created_10ms_rde(struct regular_dir_entry* rde);
void print_time_created_rde(struct regular_dir_entry* rde);
void print_date_created_rde(struct regular_dir_entry* rde);
void print_date_last_access_rde(struct regular_dir_entry* rde);
void print_first_cluster_hi_rde(struct regular_dir_entry* rde);
void print_time_last_write_rde(struct regular_dir_entry* rde);
void print_date_last_write_rde(struct regular_dir_entry* rde);
void print_first_cluster_lo_rde(struct regular_dir_entry* rde);
void print_file_size_rde(struct regular_dir_entry* rde);
void print_long_name_rde(char* name);

void print_regular_dir_entry(struct regular_dir_entry* rde);

void print_order_lde(struct long_dir_entry* lde);
void print_name1_lde(struct long_dir_entry* lde);
void print_attributes_lde(struct long_dir_entry* lde);
void print_type_lde(struct long_dir_entry* lde);
void print_checksum_lde(struct long_dir_entry* lde);
void print_name2_lde(struct long_dir_entry* lde);
void print_first_cluster_lo_lde(struct long_dir_entry* lde);
void print_name3_lde(struct long_dir_entry* lde);

void print_long_dir_entry(struct long_dir_entry* lde);

void print_magic(void);
void print_first_data_sector(void);
void print_num_data_sectors(void);
void print_num_data_clusters(void);
void print_curr_sector(void);

void print_fs_data(void);

void print_welcome_stats(void);

#endif /* FAT32_PRINT_H */
