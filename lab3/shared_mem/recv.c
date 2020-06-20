#include "common.h"

void *shmp;
int shmid;
sem_t *send_psx, *recv_psx, *final_psx;
void close_all();
void catch_ctrlc(int sig);

int main() {
    key_t key = KEY_NUM;
    char msg[SHM_SIZE];
    int must_stop = 0;

    // 捕捉 Ctrl+C 动作
    signal(SIGINT, catch_ctrlc);

    // 获取共享内存
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid < 0) {
        printf("get share memory error\n");
        exit(1);
    }
    // 启动共享内存访问
    shmp = shmat(shmid, NULL, 0);
    if (shmp < 0) {
        printf("get share memory pointer error\n");
        exit(1);
    }

    // 创建有名信号量
    send_psx = sem_open("sem_send", O_CREAT, 0666, 1);
    recv_psx = sem_open("sem_recv", O_CREAT, 0666, 0);
    final_psx = sem_open("sem_final", O_CREAT, 0666, 0);

    do {
        sem_wait(recv_psx);
        // 拷贝字符串
        strcpy(msg, (char *) shmp);
        if (strcmp(msg, "exit") == 0) {
            must_stop = 1;
        } else {
            // 添加字符串结尾标识
            memset((char *) shmp, '\0', SHM_SIZE);
            printf("receive: %s\n", msg);
        }
    }while (!must_stop);

    // 发送 over 到 sender
    strcpy((char *) shmp, "over");
    sem_post(final_psx);
    close_all();
    printf("receiver end\n");
    return 0;
}

void close_all()
{
    sem_close(send_psx);
    sem_close(recv_psx);
    sem_close(final_psx);
    shmdt(shmp);
}

void catch_ctrlc(int sig)
{
    close_all();
    exit(0);
}
