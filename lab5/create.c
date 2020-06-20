//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 创建文件
 * @param args 文件名
 * @return 1
 */
int my_create(char **args) {
    char path[PATH_LENGTH], parpath[PATH_LENGTH], filename[PATH_LENGTH];
    char *end;

    if (check_args(args, "create")) {
        return 1;
    }

    memset(parpath, '\0', PATH_LENGTH);
    memset(path, '\0', PATH_LENGTH);

    for (int i = 1; args[i] != NULL; i++) {
        // 获取绝对路径
        get_abspath(path, args[i]);
        // 获取要创建的文件名
        end = strrchr(path, '/');
        // 获取父文件夹地址和文件名
        if (end == path) {
            strcpy(parpath, "/");
            strcpy(filename, path + 1);
        } else {
            strncpy(parpath, path, end - path);
            strcpy(filename, end + 1);
        }

        // 寻找父文件夹
        if (find_fcb(parpath) == NULL) {
            printf("create: cannot create '%s': Parent folder not exists\n", parpath);
            continue;
        }
        // 寻找文件
        if (find_fcb(path) != NULL) {
            printf("create: cannot create '%s': Folder or file exists\n", args[i]);
            continue;
        }

        do_create(parpath, filename);
    }
    return 1;
}

/**
 * 创建文件的实际操作
 * @param parpath 父文件夹路径
 * @param filename 文件名
 * @return 失败返回 -1，成功返回 0
 */
int do_create(const char *parpath, const char *filename) {
    char fullname[NAME_LENGTH], fname[16], exname[8];
    char *token;
    int flag = 0;

    // 在 FAT 表中获取 1 个空闲块
    int first = get_free(1);
    
    fcb *dir = (fcb *) (myvhard + BLOCK_SIZE * find_fcb(parpath)->first);

    // 检查父文件夹中是否有空闲的空间
    for (int i = 0; i < BLOCK_SIZE / sizeof(fcb); i++, dir++) {
        if (dir->free == 0) {
            flag = 1;
            break;
        }
    }
    // 父文件夹中没有足够的空闲空间
    if (!flag) {
        printf("create: Cannot create more file in %s\n", parpath);
        return -1;
    }
    // FAT 表中没有空闲块
    if (first == -1) {
        printf("create: No more space\n");
        return -1;
    }
    // 分配 FAT 表
    set_free(first, 1, 0);

    memset(fullname, '\0', NAME_LENGTH);
    memset(fname, '\0', 8);
    memset(exname, '\0', 3);

    // 分割文件名和拓展名
    strcpy(fullname, filename);
    token = strtok(fullname, ".");
    strncpy(fname, token, 8);
    token = strtok(NULL, ".");
    if (token != NULL) {
        strncpy(exname, token, 3);
    } else {
        strncpy(exname, "d", 2);
    }

    set_fcb(dir, fname, exname, 1, first, 0, 1);
    return 0;
}

