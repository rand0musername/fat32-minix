#include "syslib.h"
#include <string.h>

int fat32_pwd() {
    message m;
    memset(&m, 0, sizeof(m));
    return (_syscall(FAT32S_PROC_NR, FAT32S_PWD, &m));
}

int fat32_mount(char* drive) {
    message m;
    memset(&m, 0, sizeof(m));
    m.m_fat32s.txt_loc = (vir_bytes)drive;
    m.m_fat32s.txt_len = strlen(drive);
    return (_syscall(FAT32S_PROC_NR, FAT32S_MOUNT, &m));
}

int fat32_unmount(char* drive) {
    message m;
    memset(&m, 0, sizeof(m));
    m.m_fat32s.txt_loc = (vir_bytes)drive;
    m.m_fat32s.txt_len = strlen(drive);
    return (_syscall(FAT32S_PROC_NR, FAT32S_UNMOUNT, &m));
}

int fat32_cd(char* path) {
    message m;
    memset(&m, 0, sizeof(m));
    m.m_fat32s.txt_loc = (vir_bytes)path;
    m.m_fat32s.txt_len = strlen(path);
    return (_syscall(FAT32S_PROC_NR, FAT32S_CD, &m));
}

int fat32_ls(char* path) {
    message m;
    memset(&m, 0, sizeof(m));
    m.m_fat32s.txt_loc = (vir_bytes)path;
    m.m_fat32s.txt_len = strlen(path);
    return (_syscall(FAT32S_PROC_NR, FAT32S_LS, &m));
}

int fat32_cat(char* path) {
    message m;
    memset(&m, 0, sizeof(m));
    m.m_fat32s.txt_loc = (vir_bytes)path;
    m.m_fat32s.txt_len = strlen(path);
    return (_syscall(FAT32S_PROC_NR, FAT32S_CAT, &m));
}

int fat32_stat(char* path) {
    message m;
    memset(&m, 0, sizeof(m));
    m.m_fat32s.txt_loc = (vir_bytes)path;
    m.m_fat32s.txt_len = strlen(path);
    return (_syscall(FAT32S_PROC_NR, FAT32S_STAT, &m));
}