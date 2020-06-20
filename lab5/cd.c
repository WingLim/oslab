//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 打开文件夹
 * @param args
 * @return 1
 */
int my_cd(char **args) {
    int i, fd;
    char abspath[PATH_LENGTH];
    fcb *dir;

    if (check_args(args, "cd")) {
        return 1;
    }

    memset(abspath, '\0', PATH_LENGTH);

    // 获取绝对路径
    get_abspath(abspath, args[1]);
    // 寻找 fcb
    dir = find_fcb(abspath);
    // 如果文件夹不存在或者为文件，则返回
    if (dir == NULL || dir->attribute == 1) {
        printf("cd: No such folder\n");
        return 1;
    }

    // 在 openfile_list 中查找文件夹，存在则进入文件夹
    for (i = 0; i < MAX_OPENFILE; i++) {
        if (openfile_list[i].free == 0) {
            continue;
        }

        if (!strcmp(dir->filename, openfile_list[i].open_fcb.filename) &&
            dir->first == openfile_list[i].open_fcb.first) {
            do_cd(i);
            return 1;
        }
    }

    // 不在 openfile_list 中则打开再进入文件夹
    if ((fd = do_open(abspath)) > 0) {
        do_cd(fd);
    }
    return 1;
}

/**
 * 打开文件夹的实际操作
 * @param fd
 */
void do_cd(int fd) {
    // 设置当前文件下标
    currfd = fd;
    // 设置字符结束符
    memset(current_dir, '\0', sizeof(current_dir));
    // 将文件夹名拷贝到当前文件夹
    strcpy(current_dir, openfile_list[currfd].dir);
}
