//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 写数据到文件
 * @param args [-a|-c|-w] 追加，覆写，写入
 * @return 1
 */
int my_write(char **args) {
    int i, j = 0, flag = 0, mode = 'w';
    char c, path[PATH_LENGTH], str[WRITE_SIZE];
    fcb *file;

    for (i = 1; args[i] != NULL; i++) {
        if (args[i][0] == '-') {
            if (!strcmp(args[i], "-w")) {
                mode = 'w';
            } else if (!strcmp(args[i], "-c")) {
                mode = 'c';
            } else if (!strcmp(args[i], "-a")) {
                mode = 'a';
            } else {
                printf("write: worng argument\n");
                return 1;
            }
        } else {
            // 保存文件路径位置
            flag = i;
        }
    }

    strcpy(path, args[flag]);
    if ((file = find_fcb(path)) == NULL) {
        printf("write: File not exists\n");
        return 1;
    }
    if (file->attribute == 0) {
        printf("write: cannot write a folder\n");
        return 1;
    }

    memset(str, '\0', WRITE_SIZE);
    for (int i = 0; i < MAX_OPENFILE; i++) {
        if (openfile_list[i].free == 0) {
            continue;
        }

        if (!strcmp(file->filename, openfile_list[i].open_fcb.filename) &&
            file->first == openfile_list[i].open_fcb.first) {
            if (mode == 'c') {
                printf("input location: ");
                scanf("%d", &openfile_list[i].count);
                getchar();
            }
            while (1) {
                for (; (str[j] = getchar()) != '\n'; j++);
                j++;
                if ((c = getchar()) == '\n') {
                    break;
                } else {
                    str[j] = c;
                    j++;
                }
            }
            if (mode == 'c') {
                do_write(i, str, j - 1, mode);
            } else {
                do_write(i, str, j + 1, mode);
            }
            return 1;
        }
    }
    printf("write: file is not open\n");
    return 1;
}

int do_write(int fd, char *content, size_t len, int wstyle) {
    fat *fat1 = (fat *) (myvhard + BLOCK_SIZE);
    fat *fat2 = (fat *) (myvhard + BLOCK_SIZE * 3);

    char text[WRITE_SIZE] = {0};

    int write = openfile_list[fd].count;
    openfile_list[fd].count = 0;

    // 将文件原来的内容读入 text
    do_read(fd, openfile_list[fd].open_fcb.length, text);

    openfile_list[fd].count = write;

    int i = openfile_list[fd].open_fcb.first;

    // 将用户输入的内容拷贝到 input 指定位置中
    char input[WRITE_SIZE] = {0};
    strncpy(input, content, len);

    // 截断写
    if (wstyle == 'w') {
        memset(text, 0, WRITE_SIZE);
        memcpy(text, input, len);
        // 覆盖写
    } else if (wstyle == 'c') {
        memcpy(text + openfile_list[fd].count, input, len);
        // 追加写
    } else if (wstyle == 'a') {
        memcpy(text + openfile_list[fd].count, input, len);
    }
    // 获取内容长度
    int length = strlen(text);
    int num = length / BLOCK_SIZE + 1;
    int static_num = num;

    while (num) {
        char buf[BLOCK_SIZE] = {0};
        memcpy(buf, &text[(static_num - num) * BLOCK_SIZE], BLOCK_SIZE);
        unsigned char *p = myvhard + BLOCK_SIZE * i;
        memcpy(p, buf, BLOCK_SIZE);
        num -= 1;
        if (num > 0) {
            fat *fat_cur = fat1 + i;
            if (fat_cur->id == END) {
                int next = get_free(1);
                fat_cur->id = next;
                fat_cur = fat1 + next;
                fat_cur->id = END;
            }
            i = (fat1 + i)->id;
        }
    }
    if (fat1[i].id != END) {
        int j = fat1[i].id;
        fat1[i].id = END;
        i = j;
        while (fat1[i].id != END) {
            int m = fat1[i].id;
            fat1[i].id = FREE;
            i = m;
        }
        fat1[i].id = FREE;
    }
    memcpy(fat2, fat1, BLOCK_SIZE * 2);
    openfile_list[fd].open_fcb.length = length;
    openfile_list[fd].fcb_state = 1;
    return strlen(input);
}
