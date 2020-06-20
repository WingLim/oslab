//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 显示当前目录
 * @param args 无参数
 * @return 1
 */
int my_pwd(char **args) {
    printf("%s\n", current_dir);
    return 1;
}

/**
 * 复制 FCB
 * @param dest 目标 FCB
 * @param src 源 FCB
 * @return 目标 FCB 的指针
 */
fcb *fcb_copy(fcb *dest, fcb *src) {
    strcpy(dest->filename, src->filename);
    strcpy(dest->exname, src->exname);
    dest->attribute = src->attribute;
    dest->time = src->time;
    dest->date = src->date;
    dest->first = src->first;
    dest->length = src->length;
    dest->free = src->free;

    return dest;
}

/**
 * 寻找 FAT 表中指定数量的空闲块
 * @param count 所需的空闲块的数量
 * @return -1 为没有足够的空闲块数量或者返回第一个空闲块的块号
 */
int get_free(int count) {
    unsigned char *ptr = myvhard;
    fat *fat1 = (fat *) (ptr + BLOCK_SIZE);
    int i, j, flag = 0;
    int fat[BLOCK_SIZE];

    // 将 fat1 中内容复制到 fat 数组中
    for (i = 0; i < BLOCK_SIZE; i++, fat1++) {
        fat[i] = fat1->id;
    }

    // 寻找 fat 中 count 数量的连续空间
    for (i = 0; i < BLOCK_SIZE - count; i++) {
        for (j = i; j < i + count; j++) {
            if (fat[j] > 0) {
                flag = 1;
                break;
            }
        }
        if (flag) {
            flag = 0;
            i = j;
        } else {
            return i;
        }
    }
    return -1;
}

/**
 * 设置 FAT 表中指定起始块及其后续数量块的值
 * @param first 起始块号
 * @param length 起始块后的块数量
 * @param mode 0为分配，1为重设，2为格式化
 * @return
 */
int set_free(unsigned short first, unsigned short length, int mode) {
    int i, offset;
    fat *flag = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat1 = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat2 = (fat *) (myvhard + BLOCK_SIZE * 3);

    for (i = 0; i < first; i++, fat1++, fat2++);

    if (mode == 1) {
        while (fat1->id != END) {
            offset = fat1->id - (fat1 - flag) / sizeof(fat);
            fat1->id = FREE;
            fat2->id = FREE;
            fat1 += offset;
            fat2 += offset;
        }
        fat1->id = FREE;
        fat2->id = FREE;
    } else if (mode == 2) {
        /** 格式化 FAT */
        for (i = 0; i < BLOCK_SIZE; i++, fat1++, fat2++) {
            fat1->id = FREE;
            fat2->id = FREE;
        }
    } else {
        for (; i < first + length - 1; i++, fat1++, fat2++) {
            fat1->id = first + 1;
            fat2->id = first + 1;
        }
        fat1->id = END;
        fat2->id = END;
    }
    return 0;
}

/**
 * 设置 FCB 属性
 * @param f FCB 指针
 * @param filename  FCB 文件名
 * @param exname    FCB 拓展名
 * @param attr      FCB 属性
 * @param first     FCB 起始块
 * @param length    FCB 文件长度
 * @param ffree     文件状态，1为占用，0为空闲
 */
int set_fcb(fcb *f, const char *filename, const char *exname, unsigned char attr, unsigned short first,
            unsigned long length, char ffree) {
    time_t *now = (time_t *) malloc(sizeof(time_t));
    struct tm *timeinfo;
    time(now);
    timeinfo = localtime(now);

    memset(f->filename, 0, 8);
    memset(f->exname, 0, 3);
    strncpy(f->filename, filename, 7);
    strncpy(f->exname, exname, 2);
    f->attribute = attr;
    f->time = get_time(timeinfo);
    f->date = get_date(timeinfo);
    f->first = first;
    f->length = length;
    f->free = ffree;

    free(now);
    return 0;
}

/**
 * 将时间戳转换为 "小时分钟秒"数字 格式
 * @param timeinfo 当前时间结构体
 * @return "小时分钟秒"
 */
unsigned short get_time(struct tm *timeinfo) {
    int hour, min, sec;
    unsigned short result;

    hour = timeinfo->tm_hour;
    min = timeinfo->tm_min;
    sec = timeinfo->tm_sec;
    result = (hour << 11) + (min << 5) + (sec >> 1);

    return result;
}

/**
 * 将时间戳转换为 "年月日"数字 格式
 * @param timeinfo 当前时间结构体
 * @return "年月日"
 */
unsigned short get_date(struct tm *timeinfo) {
    int year, mon, day;
    unsigned short result;

    year = timeinfo->tm_year;
    mon = timeinfo->tm_mon;
    day = timeinfo->tm_mday;
    result = (year << 9) + (mon << 5) + day;

    return result;
}

/**
 * 将 "年月日"数字 转换为日期字符串
 * @param sdate 日期字符串
 * @param date 表示日期的数字
 * @return sdate
 */
char *trans_date(char *sdate, unsigned short date) {
    int year, month, day;
    memset(sdate, '\0', 16);

    year = date & 0xfe00;
    month = date & 0x01e0;
    day = date & 0x001f;
    sprintf(sdate, "%04d-%02d-%02d", (year >> 9) + 1900, (month >> 5) + 1, day);
    return sdate;
}

/**
 * 将 "小时分钟秒"数字 转换为日期字符串
 * @param stime 时间字符串
 * @param time 表示时间的数字
 * @return stime
 */
char *trans_time(char *stime, unsigned short time) {
    int hour, min, sec;
    memset(stime, '\0', 16);

    hour = time & 0xfc1f;
    min = time & 0x03e0;
    sec = time & 0x001f;
    sprintf(stime, "%02d:%02d:%02d", hour >> 11, min >> 5, sec << 1);
    return stime;
}

/**
 * 将相对路径转换为绝对路径
 * @param abspath 绝对路径
 * @param relpath 相对路径
 * @return 绝对路径
 */
char *get_abspath(char *abspath, const char *relpath) {
    // 相对路径等于绝对路径
    if (!strcmp(relpath, DELIM) || relpath[0] == '/') {
        strcpy(abspath, relpath);
        return 0;
    }

    char str[PATH_LENGTH];
    char *token, *end;

    // 设置字符串结束符
    memset(abspath, '\0', PATH_LENGTH);
    abspath[0] = '/';
    strcpy(abspath, current_dir);

    strcpy(str, relpath);
    token = strtok(str, DELIM);

    do {
        if (!strcmp(token, ".")) {
            continue;
        }
        if (!strcmp(token, "..")) {
            if (!strcmp(abspath, ROOT)) {
                continue;
            } else {
                end = strrchr(abspath, '/');
                if (end == abspath) {
                    strcpy(abspath, ROOT);
                    continue;
                }
                memset(end, '\0', 1);
                continue;
            }
        }
        if (strcmp(abspath, "/")) {
            strcat(abspath, DELIM);
        }
        strcat(abspath, token);
    } while ((token = strtok(NULL, DELIM)) != NULL);

    return abspath;
}

/**
 * 根据路径寻找 fcb
 * @param path 文件路径
 * @return fcb 指针
 */
fcb *find_fcb(const char *path) {
    char abspath[PATH_LENGTH];
    // 获取绝对路径
    get_abspath(abspath, path);
    char *token = strtok(abspath, DELIM);
    // 返回根目录
    if (token == NULL) {
        return (fcb *) (myvhard + BLOCK_SIZE * 5);
    }
    return find_fcb_r(token, 5);
}

/**
 * 递归查找 fcb
 * @param token 文件名
 * @param first 文件起始盘块号
 * @return 文件 fcb 指针
 */
fcb *find_fcb_r(char *token, int first) {
    int i, length = BLOCK_SIZE;
    char fullname[NAME_LENGTH] = "\0";
    fcb *root = (fcb *) (myvhard + BLOCK_SIZE * first);
    fcb *dir;
    block0 *boot = (block0 *) myvhard;
    if (first == boot->root) {
        length = ROOT_BLOCK_NUM * BLOCK_SIZE;
    }

    // 从根目录开始遍历
    for (i = 0, dir = root; i < length / sizeof(fcb); i++, dir++) {
        // 目录项为空，跳过
        if (dir->free == 0) {
            continue;
        }
        get_fullname(fullname, dir);
        if (!strcmp(token, fullname)) {
            token = strtok(NULL, DELIM);
            // 如果已经是路径中的最后一项，则返回
            if (token == NULL) {
                return dir;
            }
            // 否则继续往下查找
            return find_fcb_r(token, dir->first);
        }
    }
    return NULL;
}

/**
 * 获取文件全名
 * @param fullname 一个字符数组，长度为 NAME_LENGTH
 * @param the_fcb 想要获取文件名的 fcb 指针
 */
void get_fullname(char *fullname, fcb *the_fcb) {
    memset(fullname, '\0', NAME_LENGTH);

    strcat(fullname, the_fcb->filename);
    // 如果为文件，则添加拓展名
    if (the_fcb->attribute == 1) {
        strncat(fullname, ".", 2);
        strncat(fullname, the_fcb->exname, 3);
    }
}

/**
 * 获取一个文件打开列表中的空位置
 * @return 返回位置，如果已满返回 -1
 */
int get_useropen() {
    for (int i = 0; i < MAX_OPENFILE; i++) {
        if (openfile_list[i].free == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * 检查是否缺少参数
 * @param args 参数
 * @param func 函数名
 * @return 缺少参数返回1，不缺少返回0；
 */
int check_args(char **args, char *func) {
    if (args[1] == NULL) {
        printf("%s: missing argument\n", func);
        return 1;
    }
    return 0;
}

/**
 * 检查文件是否在打开列表中
 * @param file 要检查的文件的指针
 * @param mode 1 打开文件, 2 关闭文件, 3 删除文件, 4 删除文件夹
 * @return
 */
int check_open(fcb *file, char mode) {
    for (int j = 0; j < MAX_OPENFILE; j++) {
        if (openfile_list[j].free == 0) {
            continue;
        }

        // 同时满足文件名相同和起始块号相同
        if (!strcmp(file->filename, openfile_list[j].open_fcb.filename) &&
            file->first == openfile_list[j].open_fcb.first) {
            switch (mode) {
                case 1: // 打开文件
                    printf("open: cannot open '%s': File or folder is opened\n", file->filename);
                    continue;
                case 2: // 关闭文件
                    do_close(j);
                    break;
                case 3: // 删除文件
                    printf("rm: cannot remove '%s': File is opened\n", file->filename);
                    return 1;
                case 4: // 删除文件夹
                    printf("rmdir: cannot remove '%s': File is opened\n", file->filename);
                    return 1;
            }

        }
    }
}
