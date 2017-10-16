#include <string.h>
#include <lib.h>
#include <stdio.h>
#include <minix/syslib.h>

const char* kMount = "mount";
const char* kUmount = "umount";
const char* kPwd = "pwd";
const char* kLs = "ls";
const char* kCd = "cd";
const char* kStat = "stat";
const char* kCat = "cat";
const char* kHelp = "help";
const char* kQuit = "quit";

int get_args(char* input, const char* command) {
    int len = strlen(input);
    int clen = strlen(command);

    if(len >= clen && !strncmp(input, command, clen)) {
        return clen;
    }
    return 0;
}

char input[100];
void read_line() {
    memset(input, 0, sizeof(input));
    char c;
    for (int i = 0; (c = getchar()) != '\n'; ++i)
    {
        input[i] = c;
    }
}

// /dev/c0d1
int main() {
    while(1) {
        printf(">");
        read_line();
        int ret;
        if((ret = get_args(input, kQuit))) {
            printf("Bye.\n");
            break;
        }
        else if((ret = get_args(input, kMount)) && input[ret] == ' ') {
            fat32_mount(input + ret + 1);
        } else if((ret = get_args(input, kUmount)) && input[ret] == ' ') {
            fat32_unmount(input + ret + 1);
        } else if((ret = get_args(input, kPwd)) && input[ret] == '\0') {
            fat32_pwd();
        } else if((ret = get_args(input, kCd)) && input[ret] == ' ') {
            fat32_cd(input + ret + 1);
        } else if((ret = get_args(input, kLs))) {
            if(input[ret] == '\0') {
                fat32_ls(".");
            } else if(input[ret] == ' ') {
                fat32_ls(input + ret + 1);
            }
        } else if((ret = get_args(input, kCat)) && input[ret] == ' ') {
            fat32_cat(input + ret + 1);
        } else if((ret = get_args(input, kStat)) && input[ret] == ' ') {
            fat32_stat(input + ret + 1);
        } else if((ret = get_args(input, kHelp))) {
            printf("*************************\n");
            printf("Available commands:\n");
            printf("mount\numount\npwd\nls\ncd\n");
            printf("stat\ncat\nhelp\nquit\n");
            printf("*************************\n");
        }
        else {
            printf("Unknown command.\n");
        }
    }
    return 0;
}
