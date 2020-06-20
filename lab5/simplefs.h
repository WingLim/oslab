//
// Created by lewy on 2020/6/4.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#ifndef LAB5_SIMPLEFS_H
#define LAB5_SIMPLEFS_H
#define BLOCK_SIZE      1024                        // 磁盘块大小
#define BLOCK_COUNT     1000                        // 磁盘块数量
#define DISK_SIZE       (BLOCK_SIZE * BLOCK_COUNT)  // 磁盘大小
#define FILE_NAME       "sysfile"                   // 虚拟磁盘文件名
#define ROOT            "/"                         // 根目录
#define DELIM           "/"                         // 文件分割符
#define PATH_LENGTH     128                         // 路径最大长度
#define NAME_LENGTH     32                          // 文件名最大长度
#define END             65535
#define FREE            0
#define ROOT_BLOCK_NUM  2                           // 根目录初始所占盘块数
#define MAX_OPENFILE    10                          // 最大可打开文件数
#define WRITE_SIZE      20 * BLOCK_SIZE             // 文件可写大小
#define TOK_DELIM       " \t\r\n\a"
#define TOK_BUFSIZE     64
#define FOLDER_COLOR    "\e[5;32m"                  // 文件夹颜色，灰底绿字
#define DEFAULT_COLOR   "\e[0m"                     //  默认颜色

/**
 * @brief 文件控制块
 * 记录文件的描述和控制信息
 */
typedef struct FCB {
    char filename[8]; // 文件名
    char exname[3]; // 文件拓展名
    unsigned char attribute; // 文件属性，0为目录，1为文件
    unsigned short time; // 文件创建时间
    unsigned short date; // 文件创建日期
    unsigned short first; // 文件起始盘块号
    unsigned long length; // 文件长度
    char free; // 表示目录项是否为空，0为空，1为已分配
} fcb;

/**
 * @brief 文件分配表
 * 记录磁盘上每个文件所占据的磁盘块的块号
 * 记录磁盘上哪些块已经分配出去，哪些块时空闲的
 */
typedef struct FAT {
    unsigned short id;
} fat;

/**
 * @brief 引导块
 * 存放逻辑磁盘的相关描述信息
 */
typedef struct BLOCK {
    char magic_number[8];       //  魔法数
    char information[200];      //  描述信息
    unsigned short root;        //  根目录起始盘块号
    unsigned char *start_block; //  数据区开始位置
} block0;

/**
 * @brief 用户打开文件表
 * 记录当前用户打开的文件
 */
typedef struct USEROPEN {
    /** FCB */
    fcb open_fcb;
    /** 当前状态 */
    int dirno;                  //  打开文件的目录项在父目录文件中的盘块号
    int diroff;                 //  打开文件的目录项在父目录文件的 dirno 盘块中的目录项序号
    char dir[80]; //  打开文件所在的目录名
    int count;                  //  读写指针在文件中的位置
    char fcb_state;             //  是否修改了文件的 FCB 内容
    char free;                  //  当前表项是否被占用，0为空，1为被占用
} useropen;

typedef struct COMMAND {
    char *name;                 // 命令名
    int (*func)(char **);       // 实现该命令的函数指针
    char *desc;                 // 命令描述
} command;

/** 全局变量 */
unsigned char *myvhard;                 // 虚拟磁盘起始地址
useropen openfile_list[MAX_OPENFILE];   // 用户打开文件表数组
int currfd;                             // 当前文件指针
char current_dir[80];                   // 记录当前目录的目录名
unsigned char *start;                   // 记录虚拟磁盘上数据区开始位置
unsigned char buffer[DISK_SIZE];

/** 系统内置函数声明 */
int startsys(void);

void my_format(void);

int my_cd(char **args);

void do_cd(int fd);

int my_pwd(char **args);

int my_mkdir(char **args);

int do_mkdir(const char *parpath, const char *dirname);

int my_rmdir(char **args);

void do_rmdir(fcb *dir);

int my_ls(char **args);

void do_ls(int first, char mode);

int my_create(char **args);

int do_create(const char *parpath, const char *filename);

int my_rm(char **args);

void do_rm(fcb *file);

int my_open(char **args);

int do_open(char *path);

int my_close(char **args);

void do_close(int fd);

int my_write(char **args);

int do_write(int fd, char *content, size_t len, int wstyle);

int my_read(char **args);

int do_read(int fd, int len, char *text);

int my_exitsys();

int help(char **args);

fcb *fcb_copy(fcb *dest, fcb *src);

int get_free(int count);

int set_free(unsigned short first, unsigned short length, int mode);

int set_fcb(fcb *f, const char *filename, const char *exname, unsigned char attr, unsigned short first,
            unsigned long length, char ffree);

unsigned short get_time(struct tm *timeinfo);

unsigned short get_date(struct tm *timeinfo);

char *trans_date(char *sdate, unsigned short date);

char *trans_time(char *stime, unsigned short time);

char *get_abspath(char *abspath, const char *relpath);

fcb *find_fcb(const char *path);

fcb *find_fcb_r(char *token, int first);

void get_fullname(char *fullname, fcb *fcb1);

int get_useropen();

void init_folder(int first, int second);

int check_args(char **args, char *func);

int check_open(fcb *file, char mode);

#endif //LAB5_SIMPLEFS_H
