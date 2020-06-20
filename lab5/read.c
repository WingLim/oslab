//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 读取文件命令
 * @param args
 * @return
 */
int my_read(char **args) {
    int i, length, flag = 0, mode = 'a';
    char path[PATH_LENGTH], str[WRITE_SIZE];
    fcb *file;

    for (i = 1; args[i] != NULL; i++) {
        if (args[i][0] == '-') {
            if (!strcmp(args[i], "-s")) {
                mode = 's';
            } else if (!strcmp(args[i], "-a")) {
                mode = 'a';
            } else {
                printf("read: wrong argument\n");
                return 1;
            }
        } else {
            // 保存文件路径位置
            flag = i;
        }
    }

    strcpy(path, args[flag]);
    if ((file = find_fcb(path)) == NULL) {
        printf("read: File not exists\n");
        return 1;
    }
    if (file->attribute == 0) {
        printf("read: cannot read a folder\n");
        return 1;
    }

    memset(str, '\0', WRITE_SIZE);
    for (i = 0; i < MAX_OPENFILE; i++) {
        if (openfile_list[i].free == 0) {
            continue;
        }

        if (!strcmp(file->filename, openfile_list[i].open_fcb.filename) &&
            file->first == openfile_list[i].open_fcb.first) {
            if (mode == 'a') {
                openfile_list[i].count = 0;
                length = UINT16_MAX;
            } else if (mode == 's') {
                printf("input location: ");
                scanf("%d", &openfile_list[i].count);
                printf("input length: ");
                scanf("%d", &length);
            }
            do_read(i, length, str);
            fputs(str, stdout);
            return 1;
        }
    }
    printf("read: file is not open\n");
    return 1;
}

/**
 * 读取文件命令实际操作
 * @param fd
 * @param len
 * @param text
 * @return
 */
int do_read(int fd, int len, char *text) {
    int len_tmp = len;
    char *textptr = text;
    memset(text, '\0', WRITE_SIZE);
    char *buf = (char *) malloc(BLOCK_SIZE);
    // 分配内存失败
    if (buf == NULL) {
        printf("do_read: malloc failure\n");
        return -1;
    }
    int off = openfile_list[fd].count;
    int block_num = openfile_list[fd].open_fcb.first;
    fat *fatptr = (fat *) (myvhard + BLOCK_SIZE) + block_num;

    while (off >= BLOCK_SIZE) {
        off -= BLOCK_SIZE;
        block_num = fatptr->id;
        if (block_num == END) {
            printf("do_read: block does not exist\n");
            return -1;
        }
        fatptr = (fat *) (myvhard + BLOCK_SIZE) + block_num;
    }

    char *blockptr = myvhard + BLOCK_SIZE * block_num;
    memcpy(buf, blockptr, BLOCK_SIZE);
    // 如果可读字节数小于要读字节数，则读取最多可读字节
    if ((openfile_list[fd].open_fcb.length - off) < len) {
        len = openfile_list[fd].open_fcb.length - off;
    }
    while (len) {
        if ((off + len) <= BLOCK_SIZE) {
            memcpy(textptr, &buf[off], len);
            openfile_list[fd].count = openfile_list[fd].count + len;
            textptr += len;
            len = 0;
        } else {
            memcpy(textptr, &buf[off], BLOCK_SIZE - off);
            openfile_list[fd].count += BLOCK_SIZE - off;
            textptr += BLOCK_SIZE - off;
            len = len - BLOCK_SIZE - off; //还剩下的想要读取的字节数
        }
    }
    free(buf);
    return len_tmp - len;
}
