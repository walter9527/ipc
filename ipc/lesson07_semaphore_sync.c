#include "common.h"
#include "my_semaphore.h"

#define NSEMS 3

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

    semid = creat_or_get_sem(NSEMS);

    init_sem(semid, 0, 1);
    init_sem(semid, 1, 0);
    init_sem(semid, 2, 0);

    int ret = fork();

    if (ret > 0) {
        ret = fork();
        if (ret > 0) { // 父进程 编号 2 唤醒 0 打印 33333

            signal(SIGINT, signal_fun);

            int semnum_buf[1] = {0};
            while (1) {
                semnum_buf[0] = 2;
                p_sem(semid, semnum_buf, 1);
                printf("33333\n");
                sleep(1);
                semnum_buf[0] = 0;
                v_sem(semid, semnum_buf, 1);
            }
        } else if (ret == 0) { // 子进程 2 编号 1 唤醒 2 打印 22222
            int semnum_buf[1] = {0};
            while (1) {
                semnum_buf[0] = 1;
                p_sem(semid, semnum_buf, 1);
                printf("22222\n");
                sleep(1);
                semnum_buf[0] = 2;
                v_sem(semid, semnum_buf, 1);
            }
        }
    } else if (ret == 0) { // 子进程 1 编号 0 唤醒 1 打印 11111
        int semnum_buf[1] = {0};
        while (1) {
            semnum_buf[0] = 0;
            p_sem(semid, semnum_buf, 1);
            printf("11111\n");
            sleep(1);
            semnum_buf[0] = 1;
            v_sem(semid, semnum_buf, 1);
        }
    }
    return 0;
}
