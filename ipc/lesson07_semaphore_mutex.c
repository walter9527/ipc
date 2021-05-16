#include "common.h"
#include "my_semaphore.h"

#define NSEMS 1

int semid = 0;

void signal_fun(int signo)
{
    if (signo == SIGINT) {
        del_sem(semid);
        //remove("./file");
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    int fd = open("./file", O_RDWR | O_CREAT | O_TRUNC, 0644);
    print_err(fd, "open fail");

    semid = creat_or_get_sem(NSEMS);

    for (int i = 0; i < NSEMS; i++) {
        init_sem(semid, i, 1); // 初始化只有由一个进程来做，这里由父进程来做
    }

    int ret = fork();

    if (ret > 0) {
        int semnum_buf[1] = {0};
        signal(SIGINT, signal_fun);
        while (1) {
            semnum_buf[0] = 0; // 设置要操作的信号量的编号
            p_sem(semid, semnum_buf, 1); // p 操作
            write(fd, "hello ", 6);
            write(fd, "world\n", 6);
            semnum_buf[0] = 0; // 设置要操作的信号量的编号
            v_sem(semid, semnum_buf, 1); // v 操作解锁
        }
    } else if (ret == 0) {
        int semnum_buf[1] = {0};
        while (1) {
            semnum_buf[0] = 0; // 设置要操作的信号量的编号
            p_sem(semid, semnum_buf, 1); // p 操作
            write(fd, "hhhhh ", 6);
            write(fd, "wwwww\n", 6);
            semnum_buf[0] = 0; // 设置要操作的信号量的编号
            v_sem(semid, semnum_buf, 1); // v 操作解锁
        }
    }

    return 0;
}
