// Formatted printing of various fields

#include "inc.h"
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <minix/com.h>
#include <sys/stat.h>
#include <minix/vfsif.h>
#include <minix/bdev.h>

// Individual BPB fields
char tmp[20];
void print_jump_boot() {
    strncpy(tmp, bpb.jump_boot, 3);
    tmp[3] = '\0';
    fat32_debug("           jump_boot: %s\n", tmp); 
}
void print_oem_name() {
    strncpy(tmp, bpb.oem_name, 8);
    tmp[8] = '\0';
    fat32_debug("            oem_name: %s\n", tmp); 
}
void print_bytes_per_sector() {      fat32_debug("    bytes_per_sector: %d\n", bpb.bytes_per_sector); }
void print_sectors_per_cluster() {   fat32_debug(" sectors_per_cluster: %d\n", bpb.sectors_per_cluster); }
void print_num_reserved_sectors() {  fat32_debug("num_reserved_sectors: %d\n", bpb.num_reserved_sectors); }
void print_num_fats() {              fat32_debug("            num_fats: %d\n", bpb.num_fats); }
void print_num_root_entries() {      fat32_debug("    num_root_entries: %d\n", bpb.num_root_entries); }   
void print_total_sectors_16() {      fat32_debug("    total_sectors_16: %d\n", bpb.total_sectors_16); }   
void print_media() {                 fat32_debug("               media: %c\n", bpb.media); }              
void print_fat_size_16() {           fat32_debug("         fat_size_16: %d\n", bpb.fat_size_16); }        
void print_sectors_per_track() {     fat32_debug("   sectors_per_track: %d\n", bpb.sectors_per_track); }  
void print_num_heads() {             fat32_debug("           num_heads: %d\n", bpb.num_heads); }
void print_hidden_sectors() {        fat32_debug("      hidden_sectors: %d\n", bpb.hidden_sectors); }
void print_total_sectors_32() {      fat32_debug("    total_sectors_32: %d\n", bpb.total_sectors_32); }
void print_fat_size_32() {           fat32_debug("         fat_size_32: %d\n", bpb.fat_size_32); }
void print_ext_flags() {             fat32_debug("           ext_flags: %d\n", bpb.ext_flags); }  
void print_fs_version() {            fat32_debug("          fs_version: %d\n", bpb.fs_version); }
void print_root_cluster() {          fat32_debug("        root_cluster: %d\n", bpb.root_cluster); }
void print_fs_info_sector() {        fat32_debug("      fs_info_sector: %d\n", bpb.fs_info_sector); }
void print_backup_boot_sector() {    fat32_debug("  backup_boot_sector: %d\n", bpb.backup_boot_sector); }
void print_reserved() {              fat32_debug("            reserved: %s\n", bpb.reserved); }
void print_drive_number() {          fat32_debug("        drive_number: %d\n", bpb.drive_number); }
void print_bs_reserved() {           fat32_debug("         bs_reserved: %c\n", bpb.bs_reserved); }
void print_boot_signature() {        fat32_debug("      boot_signature: %c\n", bpb.boot_signature); }
void print_volume_id() {             fat32_debug("           volume_id: %d\n", bpb.volume_id); }
void print_volume_label() {          fat32_debug("        volume_label: %s\n", bpb.volume_label); }
void print_fs_type() {
    strncpy(tmp, bpb.fs_type, 8);
    tmp[8] = '\0';
    fat32_debug("             fs_type: %s\n", tmp); 
}

// Prints whole BPB
void print_bpb() {
	fat32_debug("Printing bpb:\n");
	print_jump_boot();
	print_oem_name();
	print_bytes_per_sector();
	print_sectors_per_cluster();
	print_num_reserved_sectors();
	print_num_fats();
	print_num_root_entries();
	print_total_sectors_16();
	print_media();
	print_fat_size_16();
	print_sectors_per_track();
	print_num_heads();
	print_hidden_sectors();
	print_total_sectors_32();
	print_fat_size_32();
	print_ext_flags();
	print_fs_version();
	print_root_cluster();
	print_fs_info_sector();
	print_backup_boot_sector();
	print_reserved();
	print_drive_number();
	print_bs_reserved();
	print_boot_signature();
	print_volume_id();
	print_volume_label();
	print_fs_type();
}

// RDE fields
void print_short_name_rde(struct regular_dir_entry* rde)        {
    char name[32];
    DirEntry d;
    d.type = DIR_TYPE_SHORT;
    d.u.reg = *rde;
    get_name(&d, name); 
    fat32_debug("       short_name: %s \n", name); 
}
void print_attributes_rde(struct regular_dir_entry* rde)        { fat32_debug("       attributes: %d \n", rde->attributes); }
void print_nt_res_rde(struct regular_dir_entry* rde)            { fat32_debug("           nt_res: %d \n", rde->nt_res); }
void print_time_created_10ms_rde(struct regular_dir_entry* rde) { fat32_debug("time_created_10ms: %d \n", rde->time_created_10ms); }
void print_first_cluster_hi_rde(struct regular_dir_entry* rde)  { fat32_debug(" first_cluster_hi: %d \n", rde->first_cluster_hi); }
void print_first_cluster_lo_rde(struct regular_dir_entry* rde)  { fat32_debug(" first_cluster_lo: %d \n", rde->first_cluster_lo); }
void print_file_size_rde(struct regular_dir_entry* rde)         { fat32_debug("        file_size: %d \n", rde->file_size); }
void print_long_name_rde(char* name)                            { fat32_debug("        long_name: %s \n", name); }

// RDE date/time fields
void print_time_last_write_rde(struct regular_dir_entry* rde)   { 
    printf("  time_last_write: %02d:%02d:%02d\n", ((rde->time_last_write & 0xf800) >> 11), ((rde->time_last_write & 0x07e0) >> 5), (rde->time_last_write & 0x001f)*2);  
}
void print_time_created_rde(struct regular_dir_entry* rde)      { 
    printf("     time_created: %02d:%02d:%02d\n", ((rde->time_created & 0xf800) >> 11), ((rde->time_created & 0x07e0) >> 5), (rde->time_created & 0x001f)*2); 
}
void print_date_created_rde(struct regular_dir_entry* rde)   { 
    printf("     date_created: %d/%d/%d\n", ((rde->date_created & 0xfe00) >> 9) + 1980, ((rde->date_created & 0x1e0) >> 5), rde->date_created & 0x1f); 
}
void print_date_last_access_rde(struct regular_dir_entry* rde)  { 
    printf(" date_last_access: %d/%d/%d\n", ((rde->date_last_access & 0xfe00) >> 9) + 1980, ((rde->date_last_access & 0x1e0) >> 5), rde->date_last_access & 0x1f); 
}
void print_date_last_write_rde(struct regular_dir_entry* rde)   { 
    printf("  date_last_write: %d/%d/%d\n", ((rde->date_last_write & 0xfe00) >> 9) + 1980, ((rde->date_last_write & 0x1e0) >> 5), rde->date_last_write & 0x1f); 
}

// Print whole short name dir
void print_regular_dir_entry(struct regular_dir_entry* rde) {
	fat32_debug("Printing regular_dir_entry:\n");
	print_short_name_rde(rde);
	print_attributes_rde(rde);
	print_nt_res_rde(rde);
	print_time_created_10ms_rde(rde);
	print_time_created_rde(rde);
	print_date_created_rde(rde);
	print_date_last_access_rde(rde);
	print_first_cluster_hi_rde(rde);
	print_time_last_write_rde(rde);
	print_date_last_write_rde(rde);
	print_first_cluster_lo_rde(rde);
	print_file_size_rde(rde);
}

// LDE fields
void print_order_lde(struct long_dir_entry* lde)                { fat32_debug("           order_lde: %d\n", lde->order); }
void print_name1_lde(struct long_dir_entry* lde)                { fat32_debug("           name1_lde: %s\n", lde->name1); }
void print_attributes_lde(struct long_dir_entry* lde)           { fat32_debug("      attributes_lde: %d\n", lde->attributes); }
void print_type_lde(struct long_dir_entry* lde)                 { fat32_debug("            type_lde: %d\n", lde->type); }
void print_checksum_lde(struct long_dir_entry* lde)             { fat32_debug("        checksum_lde: %d\n", lde->checksum); }
void print_name2_lde(struct long_dir_entry* lde)                { fat32_debug("           name2_lde: %s\n", lde->name2); }
void print_first_cluster_lo_lde(struct long_dir_entry* lde)     { fat32_debug("first_cluster_lo_lde: %d\n", lde->first_cluster_lo); }
void print_name3_lde(struct long_dir_entry* lde)                { fat32_debug("           name3_lde: %s\n", lde->name3); }

// Print whole long name dir
void print_long_dir_entry(struct long_dir_entry* lde) {
	fat32_debug("Printing long_dir_entry:\n");
	print_order_lde(lde);
	print_name1_lde(lde);
	print_attributes_lde(lde);
	print_type_lde(lde);
	print_checksum_lde(lde);
	print_name2_lde(lde);
	print_first_cluster_lo_lde(lde);
	print_name3_lde(lde);
}

// Print some valuable info from FS data
void print_magic() {                fat32_debug("               magic: %X%X\n", fs_data.magic[0], fs_data.magic[1]); }     
void print_first_data_sector() {    fat32_debug("   first_data_sector: %d\n", fs_data.first_data_sector); }
void print_num_data_sectors() {     fat32_debug("    num_data_sectors: %d\n", fs_data.num_data_sectors); }
void print_num_data_clusters() {    fat32_debug("   num_data_clusters: %d\n", fs_data.num_data_clusters); }
void print_curr_sector() {          fat32_debug("      curr_sector: %d\n", fs_data.curr_sector); }

// Print whole FS data
void print_fs_data() {
	fat32_debug("Printing fs_data:\n");
	print_magic();
	print_first_data_sector();
	print_num_data_sectors();
	print_num_data_clusters();

	print_curr_sector();
}

// Print a welcome message with selected info about the drive
void print_welcome_stats() {
    fat32_debug("***** FAT32 BPB loaded ******\n");
    fat32_debug("Drive info:\n");
    print_magic();
    print_jump_boot();
    print_oem_name();
    print_bytes_per_sector();
    print_sectors_per_cluster();
    print_num_reserved_sectors();
    print_num_fats();
    print_total_sectors_32();
    print_fat_size_32();
    print_root_cluster();
    print_fs_type();
    print_first_data_sector();
    print_num_data_sectors();
    print_num_data_clusters();
    fat32_debug("*****************************\n");
}
