//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 创建文件夹命令
 * @param args
 * @return
 */
int my_mkdir(char **args) {
    int i;
    char path[PATH_LENGTH], parpath[PATH_LENGTH], dirname[PATH_LENGTH];
    char *end;

    if (check_args(args, "mkdir")) {
        return 1;
    }

    for (i = 1; args[i] != NULL; i++) {
        get_abspath(path, args[i]);
        // 获取要创建的文件夹名
        end = strrchr(path, '/');
        // 获取父文件夹地址和文件夹名
        if (end == path) {
            strcpy(parpath, "/");
            strcpy(dirname, path + 1);
        } else {
            strncpy(parpath, path, end - path);
            strcpy(dirname, end + 1);
        }

        // 寻找父文件夹
        if (find_fcb(parpath) == NULL) {
            printf("mkdir: cannot create %s: parent folder not exists\n", parpath);
            continue;
        }
        // 寻找文件夹
        if (find_fcb(path) != NULL) {
            printf("mkdir: cannot create %s: folder or file exists\n", args[i]);
            continue;
        }
        do_mkdir(parpath, dirname);
    }
    return 1;
}

/**
 * 创建文件夹命令实际操作
 * @param parpath 父文件夹路径
 * @param dirname 文件夹名
 * @return 失败返回 -1，成功返回 0
 */
int do_mkdir(const char *parpath, const char *dirname) {
    int sencond = get_free(1);
    
    int i, flag = 0, first = find_fcb(parpath)->first;
    fcb *dir = (fcb *) (myvhard + BLOCK_SIZE * first);

    // 检查父文件夹中是否有空闲的空间
    for (i = 0; i < BLOCK_SIZE / sizeof(fcb); i++, dir++) {
        if (dir->free == 0) {
            flag = 1;
            break;
        }
    }
    if (!flag) {
        printf("mkdir: cannot create more file in %s\n", parpath);
        return -1;
    }
    // FAT表中没有空闲块
    if (sencond == -1) {
        printf("mkdir: no more spaces\n");
        return -1;
    }
    set_free(sencond, 1, 0);

    set_fcb(dir, dirname, "di", 0, sencond, BLOCK_SIZE, 1);
    init_folder(first, sencond);
    return 0;
}

/**
 * 初始化文件夹
 * @param first 父文件夹起始块号
 * @param second 当前文件夹起始块号
 */
void init_folder(int first, int second) {
    fcb *par = (fcb *) (myvhard + BLOCK_SIZE * first);
    fcb *cur = (fcb *) (myvhard + BLOCK_SIZE * second);

    // 创建 .
    set_fcb(cur, ".", "di", 0, second, BLOCK_SIZE, 1);
    cur++;
    // 创建 ..
    set_fcb(cur, "..", "di", 0, first, par->length, 1);
    cur++;
    for (int i = 2; i < BLOCK_SIZE / sizeof(fcb); i++, cur++) {
        cur->free = 0;
    }
}
