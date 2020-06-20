//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 关闭并保存文件
 * @param args [-a] 关闭所有文件
 * @return 1
 */
int my_close(char **args) {
    fcb *file;

    if (check_args(args, "close")) {
        return 1;
    }

    if (args[1][0] == '-') {
        if (!strcmp(args[1], "-a")) {
            // 遍历 openfile_list
            for (int i = 0; i < MAX_OPENFILE; i++) {
                // 不关闭当前文件夹
                if (i == currfd) {
                    continue;
                }
                openfile_list[i].free = 0;
            }
            return 1;
        } else {
            printf("close: worng arguemnt\n");
            return 1;
        }
    }

    for (int i = 1; args[i] != NULL; i++) {
        file = find_fcb(args[i]);
        if (file == NULL) {
            printf("close: cannot close %s: No such file or folder\n", args[i]);
            return 1;
        }
        // 检查 openfile_list 中是否存在该文件夹，存在则关闭
        check_open(file, 2);
    }
    return 1;
}

/**
 * 关闭文件的实际操作
 * @param fd 文件在 openfile_list 中的下标
 */
void do_close(int fd) {
    // 如果文件被修改，则保存到父文件夹中再关闭
    if (openfile_list[fd].fcb_state == 1) {
        fcb_copy(find_fcb(openfile_list[fd].dir), &openfile_list[fd].open_fcb);
    }
    openfile_list[fd].free = 0;
}
