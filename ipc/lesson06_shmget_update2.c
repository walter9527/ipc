#include "common.h"
#include "my_semaphore.h"

#define SHM_FILE "./shmfile"
#define SHM_SIZE 4096

int shmid = -1;
void *shmaddr = NULL;

int semid = 0;

// 创建共享缓存
void creat_or_get_shm()
{
    key_t key = -1;
    int fd = -1;
   
    fd = open(SHM_FILE, O_RDWR | O_CREAT, 0644);
    print_err(fd, "open fail");

    key = ftok(SHM_FILE, 'b');
    print_err(key, "ftok fail");
    
    shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);
    print_err(shmid, "shmget fail");
}


void at_shm() 
{
    shmaddr = shmat(shmid, NULL, 0);
    print_err(shmaddr - ((void *)0), "shmat fail");

    memset(shmaddr, 0, SHM_SIZE);
}

void signal_fun(int signo)
{
    if (signo == SIGINT) { // 删除共享内存和管道
        // 取消映射
        shmdt(shmaddr);

        // 删除映射
        shmctl(shmid, IPC_RMID, NULL);
        remove(SHM_FILE);

        del_sem(semid);
        exit(-1);
    } else if (signo == SIGCHLD){ // 处理僵尸进程
        //printf("子进程已死亡\n");
        pid_t pid;
        while ((pid = waitpid(-1, NULL, WNOHANG) > 0)) {
            printf("SIGCHLD pid %d\n", pid);
        }
    } 
}

char buf[300] = {
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccbbbbbbbbbdddddddddddddddddddkfdsassse3213322"
};

int main(int argc, char *argv[])
{
    // 使用信号量来进行进程间的同步
    semid = creat_or_get_sem(2);
    
    init_sem(semid, 0, 1);
    init_sem(semid, 1, 0);
    
    int ret = -1;

    signal(SIGINT, signal_fun);
    signal(SIGCHLD, signal_fun);

    ret = fork();

    if (ret > 0) {

        // 创建共享内存
        creat_or_get_shm();
        // 建立映射
        at_shm();

        memset(shmaddr, 0, SHM_SIZE);

        sleep(1); // 等到子进程创建成功，确保第一次能够成功发送

        // 将应用缓存数据复制到共享内存中
        while (1) {
            p_sem_s(semid, 0);
            
            memcpy(shmaddr, buf, sizeof(buf));

            sleep(1);

            v_sem_s(semid, 1);
        }
    } else if (ret == 0) {
        
        creat_or_get_shm();
        at_shm();
            
        // 从共享内存获取数据
        while (1) {
            p_sem_s(semid, 1);

            printf("%s\n", (char *)shmaddr);
            memset(shmaddr, 0, SHM_SIZE);

            v_sem_s(semid, 0);
        }
    }

    return 0;
}
