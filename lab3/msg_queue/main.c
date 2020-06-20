#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

sem_t send_psx, recv_psx, final_recv;
pthread_t send_pid, recv_pid;

// 消息结构体
struct msgbuf {
    long mtype; // 消息类型 1为正常消息 2为结束消息
    char mtext[256]; // 消息内容
};

int msgid;

void *send_thread(void *arg);
void *recv_thread(void *arg);

int main() {
    // 创建发送无名信号量
    sem_init(&send_psx, 0, 1);
    // 创建接收无名信号量
    sem_init(&recv_psx, 0, 0);
    // 创建结束无名信号量
    sem_init(&final_recv, 0, 0);
    // 创建一个消息队列
    msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    // 判断消息队列是否创建成功
    if (msgid < 0) {
        printf("msg queue create error\n");
        exit(1);
    }
    // 创建发送线程
    pthread_create(&send_pid, NULL, send_thread, NULL);
    // 创建接收线程
    pthread_create(&recv_pid, NULL, recv_thread, NULL);
    // 等待发送线程结束
    pthread_join(send_pid, NULL);
    // 等待接收线程结束
    pthread_join(recv_pid, NULL);
    return 0;
}

void *send_thread(void *arg) {
    int count = 0;
    char info[256];
    struct msgbuf s_msg;
    // 设置消息类型
    s_msg.mtype = 1;
    while (1) {
        sem_wait(&send_psx);
        printf("send_thread: ");
        scanf("%s", info);
        // 如果用户输入 exit 则发送 end 到接收线程
        if (strcmp(info, "exit") == 0) {
            strcpy(s_msg.mtext, "end");
            msgsnd(msgid, &s_msg, sizeof(struct msgbuf), 0);
            sem_post(&recv_psx);
            break;
        }
        strcpy(s_msg.mtext, info);
        // 计数 +1
        count++;
        printf("%d\n", count);
        // 发送消息
        msgsnd(msgid, &s_msg, sizeof(struct msgbuf), 0);
        sem_post(&send_psx);
        sem_post(&recv_psx);
    }
    sem_wait(&final_recv);
    // 接收接收线程的 over 消息
    msgrcv(msgid, &s_msg, sizeof(struct msgbuf), 2, 0);
    printf("send_thread recv %s\n done. \n", s_msg.mtext);
    // 删除消息队列
    msgctl(msgid, IPC_RMID, 0);
    // 退出线程
    pthread_exit(NULL);
}

void *recv_thread(void *arg) {
    struct msgbuf r_msg;
    // 设置消息类型
    r_msg.mtype = 1;
    while (1) {
        sem_wait(&recv_psx);
        // 等待两秒
        sleep(2);
        // 接收消息
        msgrcv(msgid, &r_msg, sizeof(struct msgbuf), 1 ,0);
        // 如果接收到 end 则发送 over 到发送线程
        if (strcmp(r_msg.mtext, "end") == 0) {
            strcpy(r_msg.mtext, "over");
            // 设置消息类型
            r_msg.mtype = 2;
            // 发送 over 信息
            msgsnd(msgid, &r_msg, sizeof(struct msgbuf), 0);
            printf("recv_thread recv 'end', return 'over'\n");
            sem_post(&final_recv);
            break;
        }
        printf("recv_thread: %s\n", r_msg.mtext);
    }
    // 退出线程
    pthread_exit(NULL);
}