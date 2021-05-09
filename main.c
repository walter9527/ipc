#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>


int main(int argc, char *argv[])
{
    int msqid; // 消息队列标识符
    struct msqid_ds info; // 消息队列数据结构
    struct msgbuf buf; // 消息模板
    
    int msqid = msgget(IPC_PRIVATE, 0664 | IPC_CREAT); // 创建消息队列，返回标识符

    buf.mtype = 1;
    buf.mtext[0] = 1;

    msgsnd(msqid, &buf, 1, 0);

    printf("%03o", info.msg_perm.mod & 0777);
    printf("%lu", info.msg_cbytes);
    printf("%lu", info.msg_qnum);
    printf("%lu", info.msg_qbytes);

    system("ipcs -q");

    msgctl(msqid,IPC_RMID, NULL);
    return 0
}
