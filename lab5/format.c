//
// Created by lewy on 2020/6/5.
//
#include "simplefs.h"

/**
 * 格式化虚拟磁盘
 */
void my_format(void) {
    unsigned char *ptr = myvhard;
    // 引导块
    block0 *boot = (block0 *) ptr;
    int first;
    FILE *fp;

    // 设置魔法数
    strcpy(boot->magic_number, "10101010");
    strcpy(boot->information,
           "Disk Size = 1MB, Block Size = 1KB, Block0 in 0, FAT0/1 in 1/3, Root Directory in 5");
    boot->root = 5;
    boot->start_block = (unsigned char *) (boot + BLOCK_SIZE * 7);
    ptr += BLOCK_SIZE;

    // 初始化 fat1 和 fat2 表
    set_free(0, 0, 2);

    // 分配引导块，fat1 表 和 fat2 表
    set_free(get_free(1), 1, 0);
    set_free(get_free(2), 2, 0);
    set_free(get_free(2), 2, 0);

    ptr += BLOCK_SIZE * 4;

    fcb *root = (fcb *) ptr;
    first = get_free(ROOT_BLOCK_NUM);
    set_free(first, ROOT_BLOCK_NUM, 0);
    set_fcb(root, ".", "di", 0, first, BLOCK_SIZE * 2, 1);
    root++;
    set_fcb(root, "..", "di", 0, first, BLOCK_SIZE * 2, 1);
    root++;

    // 设置除了 . 和 .. 以外的目录属性为空
    for (int i = 2; i < BLOCK_SIZE * 2 / sizeof(fcb); i++, root++) {
        root->free = 0;
    }
    
    // 将上面格式化之后的内容写入到文件
    fp = fopen(FILE_NAME, "w");
    fwrite(myvhard, DISK_SIZE, 1, fp);
    fclose(fp);
}

