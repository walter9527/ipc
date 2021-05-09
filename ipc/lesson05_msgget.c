#include "common.h"
#define MSG_FILE "./msgfile"

#define MSG_SIZE 1024

struct msgbuf {
    long   mtype;       /* Message type. */
    char   mtext[MSG_SIZE];    /* Message text. */
};


void printf_err(char *msg) {
    perror(msg);
    exit(-1); 
}

void err_handler(int flag, char *msg)
{
    if (flag == -1) {
        printf_err(msg);
    }
}

int creat_or_get_msgqueue()
{
    int msqid = -1;
    key_t key = -1;
    int fd = 0;

    /* 创建一个消息队列的专用文件，ftok 会用到这个文件的路径名 */
    fd = open(MSG_FILE, O_RDWR | O_CREAT, 0664);
    err_handler(fd, "open fail");

    /* 利用存在的文件路径名和8位的整型数，计算出 key 值 */
    key = ftok(MSG_FILE, 'a');
    err_handler(key, "ftok fail");

    /* 利用 key 值创建、或者获取消息队列 */
    msqid = msgget(key, 0664 | IPC_CREAT);
    err_handler(msqid, "msgget fail");

    printf("msqid = %d\n", msqid);
    return msqid;
}

int msqid = -1; // 消息队列标识符

void signal_fun(int signo) 
{
    msgctl(msqid, IPC_RMID, NULL);

    exit(-1);
}


int main(int argc, char *argv[])
{

    long recv_msgtype = 0; // 从命令行获取消息类型 

    if (argc != 2) {
        printf("param num error\n");
        exit(-1);
    }

    recv_msgtype = atol(argv[1]);

    msqid = creat_or_get_msgqueue();

    struct msgbuf msgp = {0};

    int ret = fork();

    if (ret > 0) { // 父进程(客户端)发送消息

        signal(SIGINT, signal_fun); // 按下 C_c 删除消息队列

        int ret = 0;
        while (1) {
            memset(&msgp, 0, sizeof(msgp));

            /* 封装消息包 */
            scanf("%s", msgp.mtext);
            printf("input snd_msgtype\n"); // 发送端自己指定端口
            scanf("%ld", &msgp.mtype);

            /* 发送消息包 */
            ret = msgsnd(msqid, &msgp, sizeof(msgp.mtext), 0);
            err_handler(ret, "msgsnd fail");
        }

    } else if (ret == 0) { // 子进程(服务端)接收消息
        ssize_t ret = 0;
        while (1) {
            memset(&msgp, 0, sizeof(msgp));
            
            // 服务端有固定的端口
            ret = msgrcv(msqid, &msgp, MSG_SIZE, recv_msgtype, 0);
            err_handler(ret, "msgrcv fail");

            if (ret > 0) {
                printf("rcv: %s\n", msgp.mtext);
            }
        } 
    }
    return 0;
}
