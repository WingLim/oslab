//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 打开文件入口
 * @param args [-l] 显示所有已打开的文件
 * @return 1
 */
int my_open(char **args) {
    fcb *file;
    char path[PATH_LENGTH];

    if (check_args(args, "open")) {
        return 1;
    }
    // 检测参数 -l 并输出
    if (args[1][0] == '-') {
        if (!strcmp(args[1], "-l")) {
            // 表头
            printf("fd filename exname state path\n");
            for (int i = 0; i < MAX_OPENFILE; i++) {
                if (openfile_list[i].free == 0) {
                    continue;
                }

                // 每一列内容
                printf("%2d %8s %-6s %-5d %s\n", i, openfile_list[i].open_fcb.filename,
                       openfile_list[i].open_fcb.exname,
                       openfile_list[i].fcb_state, openfile_list[i].dir);
            }
            return 1;
        } else {
            printf("open: wrong argumeng\n");
            return 1;
        }
    }
    for (int i = 1; args[i] != NULL; i++) {
        file = find_fcb(args[i]);
        if (file == NULL) {
            printf("open: cannot open '%s': No such file or folder\n", args[i]);
            return 1;
        }
        // 检测文件是否已经打开
        check_open(file, 1);
        do_open(get_abspath(path, args[i]));
    }
    return 1;
}

/**
 * 打开文件的实际操作
 * @param path 文件的绝对路径
 * @return -1 无法打开，fd 在文件打开列表中的位置
 */
int do_open(char *path) {
    int fd = get_useropen();
    fcb *file = find_fcb(path);

    if (fd == -1) {
        printf("open: cannot open file, no more useropen entry\n");
        return -1;
    }
    fcb_copy(&openfile_list[fd].open_fcb, file);
    openfile_list[fd].free = 1;
    openfile_list[fd].count = 0;
    memset(openfile_list[fd].dir, '\0', 80);
    strcpy(openfile_list[fd].dir, path);

    return fd;
}
