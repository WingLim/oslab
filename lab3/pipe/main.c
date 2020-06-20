#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define CHILD_NUM 3
void write_to_pipe(int fd[2], int sid);
void read_from_pipe(int fd[2]);
// 销毁信号量
void destory_sem() {
    sem_unlink("sem_write");
    sem_unlink("sem_read");
}

int main() {
    int fd[2];
    pid_t pid;
    int sid = 0;
    sem_t *write_psx, *read_psx;
    // 创建写信号量
    write_psx = sem_open("sem_write", O_CREAT, 0666, 1);
    // 创建读信号量
    read_psx = sem_open("sem_read", O_CREAT, 0666, 0);
    // 创建一个无名管道
    if (pipe(fd) < 0) {
        printf("pipe error\n");
        exit(1);
    }
    // 创建子进程
    for (int i = 0; i < CHILD_NUM; i++) {
        pid = fork();
        sid++;
        // 判断是否 fork 进程成功
        if (pid < 0) {
            printf("fork error\n");
            destory_sem();
            exit(0);
        // 如果在子进程中，则跳出循环
        } else if (pid == 0) {
            break;
        }
    }
    if (pid == 0) {
        sem_wait(write_psx);
        write_to_pipe(fd, sid);

        sem_post(write_psx);
        sem_post(read_psx);
        exit(0);
    } else {
        for (int i = 0; i < CHILD_NUM; i++) {
            sem_wait(read_psx);
        }
        read_from_pipe(fd);
        destory_sem();
    }
    return 0;
}

// 写数据到管道
void write_to_pipe(int fd[2], int sid) {
    char buf[100];
    // 关闭管道的读取端
    close(fd[0]);
    memset(buf, sid + '0', 100);
    printf("child proc %d write %d bytes data\n", sid, (int) sizeof(buf));
    // 向管道中写入数据
    write(fd[1], buf, sizeof(buf));
}
// 从管道中读取数据
void read_from_pipe(int fd[2]) {
    int ret;
    char buf[101];
    // 关闭管道的写入端
    close(fd[1]);
    memset(buf, '\0', 101);
    ret = read(fd[0], buf, 101);
    while (ret > 0) {
        printf("father read %d bytes data: %s\n", ret, buf);
        memset(buf, '\0', 101);
        ret = read(fd[0], buf, 101);
    }
}