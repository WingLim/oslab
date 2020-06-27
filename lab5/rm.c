//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 删除文件入口
 * @param args [-r] 删除文件夹
 * @return 1
 */
int my_rm(char **args) {
    fcb *file;
    char mode = 'n';
    if (check_args(args, "rm")) {
        return 1;
    }

    // 判断参数中是否含有 -r
    for (int i = 1; args[i] != NULL; i++) {
        if (args[i][0] == '-') {
            if (!strcmp(args[i], "-r")) {
                mode = 'r';
                break;
            } else {
                printf("rm: worng argument\n");
                return 1;
            }
        }
    }

    for (int i = 1; args[i] != NULL; i++) {
        // 如果 -r 跟在 rm 命令后，则跳过当前循环
        if (!strcmp(args[i], "-r")) {
            continue;
        }
        file = find_fcb(args[i]);
        if (file == NULL) {
            printf("rm: cannot remove '%s': No such file\n", args[i]);
            return 1;
        }

        // 正常模式，删除文件
        if (mode == 'n') {
            if (file->attribute == 0) {
                printf("rm: cannot remove '%s': Is a directory\n", args[i]);
                return 1;
            }

            check_open(file, 2);
            do_rm(file);
            // 递归模式，删除文件夹及其子文件
        } else if (mode == 'r') {
            // 如果为文件则直接删除
            if (file->attribute == 1) {
                check_open(file, 2);
                do_rm(file);
                // 如果为文件夹，调用 my_rmdir 删除
            } else if (file->attribute == 0) {
                my_rmdir(args);
            }
        }
    }
    return 1;
}

/**
 * 删除文件实际操作
 * @param file 要删除的文件的 fcb 指针
 */
void do_rm(fcb *file) {
    int first = file->first;
    file->free = 0;
    set_free(first, 0, 1);
}

/**
 * 删除文件夹入口
 * @param args
 * @return
 */
int my_rmdir(char **args) {
    fcb *dir;
    if (check_args(args, "rmdir")) {
        return 1;
    }

    for (int i = 1; args[i] != NULL; i++) {
        // 处理 rm -r 时传递 -r 参数，跳过循环
        if (!strcmp(args[i], "-r")) {
            continue;
        }
        // 判断是否要删除 . 和 ..
        if (!strcmp(args[i], ".") || !strcmp(args[i], "..")) {
            printf("rmdir: cannot remove '%s': '.' or '..' is read only\n", args[i]);
            return 1;
        }
        // 禁止删除根目录
        if (!strcmp(args[i], "/")) {
            printf("rmdir: cannot remove root dir");
            return 1;
        }

        dir = find_fcb(args[i]);
        if (dir == NULL) {
            printf("rmdir: cannot remove '%s': No such folder\n", args[i]);
            return 1;
        }

        if (dir->attribute == 1) {
            printf("rmdir: cannot remove '%s': Is a directory\n", args[i]);
        }

        check_open(dir, 2);
        do_rmdir(dir);
    }
    return 1;
}

/**
 * 删除文件夹实际操作
 * @param dir 要删除的文件的 fcb 指针
 */
void do_rmdir(fcb *dir) {
    int first = dir->first;

    dir->free = 0;
    dir = (fcb *) (myvhard + BLOCK_SIZE * first);
    dir->free = 0;
    dir++;
    dir->free = 0;

    set_free(first, 1, 1);
}