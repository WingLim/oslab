//
// Created by lewy on 2020/6/4.
//
#include "simplefs.h"

int startsys(void) {
    myvhard = (char *) malloc(DISK_SIZE);
    FILE *fp = fopen(FILE_NAME, "r");
    if (fp != NULL) {
        // 将文件读入内存
        fread(buffer, DISK_SIZE, 1, fp);
        fclose(fp);
        // 检测魔法数是否正确
        if (memcmp(buffer, "10101010", 8) == 0) {
            memcpy(myvhard, buffer, DISK_SIZE);
            printf("%s read successed\n", FILE_NAME);
        } else {
            printf("invalid magic num\n");
            my_format();
            memcpy(buffer, myvhard, DISK_SIZE);
        }
    } else {
        my_format();
        memcpy(buffer, myvhard, DISK_SIZE);
    }
    // 将第 6 块盘块设为根目录区
    fcb *root = (fcb *) (myvhard + BLOCK_SIZE * 5);
    fcb_copy(&openfile_list[0].open_fcb, root);

    strcpy(openfile_list[0].dir, ROOT);
    openfile_list[0].count = 0;
    openfile_list[0].fcb_state = 0;
    openfile_list[0].free = 1;
    
    currfd = 0;
    strcpy(current_dir, openfile_list[currfd].dir);
    return 0;
}

int my_exitsys() {
    FILE *fp;
    for (int i = 0; i < MAX_OPENFILE; i++) {
        do_close(i);
    }
    fp = fopen(FILE_NAME, "w");
    fwrite(myvhard, DISK_SIZE, 1, fp);
    free(myvhard);
    fclose(fp);
    return 0;
}
