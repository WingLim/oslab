#include "common.h"
void *shmp;
int shmid;

void destroy();
void catch_ctrlc(int sig);

int main() {
    key_t key = KEY_NUM;
    sem_t *send_psx, *recv_psx, *final_psx;

    // 捕捉 Ctrl+C 动作
    signal(SIGINT, catch_ctrlc);

    // 获取共享内存
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    // 是否成功创建共享内存
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

    // 添加字符串结尾标识
    memset((char *)shmp, '\0', SHM_SIZE);

    do {
        printf("sender> ");
        sem_wait(send_psx);
        // 输入信息到共享内存中
        scanf("%s", (char *)shmp);
        sem_post(recv_psx);
        sem_post(send_psx);
    } while (strncmp((char *)shmp, "exit", strlen("exit")));

    sem_wait(final_psx);
    printf("receive: %s\n", (char *)shmp);
    destroy();
    printf("sender end\n");
    return 0;
}

void destroy() {
    // 分离共享内存
    shmdt(shmp);
    // 删除共享内存
    shmctl(shmid, IPC_RMID, NULL);
    sem_unlink("sem_send");
    sem_unlink("sem_recv");
    sem_unlink("sem_final");
}

void catch_ctrlc(int sig)
{
    destroy();
    exit(0);
}