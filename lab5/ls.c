//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * ls 命令入口
 * @param args [-l] 显示详细信息
 * @return  1
 */
int my_ls(char **args) {
    int first = openfile_list[currfd].open_fcb.first;
    int i, mode = 'n';
    int flag[3];
    fcb *dir;

    for (i = 0; args[i] != NULL; i++) {
        flag[i] = 0;
    }
    if (i > 3) {
        printf("ls: too many arguments\n");
        return 1;
    }
    flag[0] = 1;
    // 遍历检测是否含 '-l' 参数
    for (i = 1; args[i] != NULL; i++) {
        if (args[i][0] == '-') {
            flag[i] = 1;
            if (!strcmp(args[i], "-l")) {
                mode = 'l';
                break;
            } else {
                printf("ls: worng argument\n");
                return 1;
            }
        }
    }

    // 列出指定文件夹信息
    for (i = 1; args[i] != NULL; i++) {
        if (flag[i] == 0) {
            dir = find_fcb(args[i]);
            if (dir != NULL && dir->attribute == 0) {
                first = dir->first;
            } else {
                printf("ls: cannot access %s: No such file or dir\n", args[i]);
                return 1;
            }
            break;
        }
    }

    do_ls(first, mode);
    return 1;
}

/**
 * 实际显示文件夹信息操作
 * @param first 文件夹的第一个块号
 * @param mode  'n' 正常模式， 'l' 详细模式
 */
void do_ls(int first, char mode) {
    int i, count, length = BLOCK_SIZE;
    char fullname[NAME_LENGTH], date[16], time[16];
    fcb *root = (fcb *) (myvhard + BLOCK_SIZE * first);
    block0 *boot = (block0 *) myvhard;

    if (first == boot->root) {
        length = ROOT_BLOCK_NUM * BLOCK_SIZE;
    }

    if (mode == 'n') {
        for (i = 0, count = 1; i < length / sizeof(fcb); i++, root++) {
            if (root->free == 0) {
                continue;
            }
            if (root->attribute == 0) {
                printf("%s", FOLDER_COLOR);
                printf("%s\t", root->filename);
                printf("%s", DEFAULT_COLOR);
            } else {
                get_fullname(fullname, root);
                printf("%s\t", fullname);
            }
            if (count % 5 == 0) {
                printf("\n");
            }
            count++;
        }
    } else if (mode == 'l') {
        for (i = 0, count = 1; i < length / sizeof(fcb); i++, root++) {
            if (root->free == 0) {
                continue;
            }
            trans_date(date, root->date);
            trans_time(time, root->time);
            get_fullname(fullname, root);
            printf("%d\t%6d\t%6ld\t%s\t%s\t", root->attribute, root->first, root->length, date, time);
            if (root->attribute == 0) {
                printf("%s", FOLDER_COLOR);
                printf("%s\n", fullname);
                printf("%s", DEFAULT_COLOR);
            } else {
                printf("%s\n", fullname);
            }
            count++;
        }
    }
    printf("\n");
}
