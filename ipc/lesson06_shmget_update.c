#include "common.h"

#define SHM_FILE "./shmfile"
#define SHM_SIZE 4096

#define FIFO_FILE "./fifofile"

int shmid = -1;
void *shmaddr = NULL;

void print_err(int flg, char *err_msg)
{
    if (flg == -1) {
        perror(err_msg);
        exit(-1);
    }
}

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

// 创建管道，目的是传递进程 ID
int creat_open_fifo(int open_mode)
{
    int ret = mkfifo(FIFO_FILE, 0644);
    if (ret == -1 && errno != EEXIST) {
        perror("mkfifo fail");
        exit(-1);
    }

    int fd = open(FIFO_FILE, open_mode);
    print_err(fd, "open fail");

    return fd;
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

        // 删除管道
        remove(FIFO_FILE);
        exit(-1);
    } else if (signo == SIGCHLD){ // 处理僵尸进程
        //printf("子进程已死亡\n");
        pid_t pid;
        while ((pid = waitpid(-1, NULL, WNOHANG) > 0)) {
            printf("SIGCHLD pid %d\n", pid);
        }
    } else if (signo == SIGUSR1){ // 唤醒子进程

    }
}

char buf[300] = {
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccbbbbbbbbbdddddddddddddddddddkfdsassse3213322"
};

int main(int argc, char *argv[])
{
    int ret = -1;

    signal(SIGINT, signal_fun);
    signal(SIGCHLD, signal_fun);
    signal(SIGUSR1, signal_fun);

    ret = fork();

    if (ret > 0) {

        int fd = creat_open_fifo(O_RDONLY);
        int pid = -1;
        read(fd, &pid, sizeof(pid));

        // 创建共享内存
        creat_or_get_shm();
        // 建立映射
        at_shm();


        // 将应用缓存数据复制到共享内存中
        while (1) {
            memcpy(shmaddr, buf, sizeof(buf));
            
            // 唤醒读进程
            kill(pid, SIGUSR1);

            sleep(1);
        }
    } else if (ret == 0) {
        
        // 传递自己的 pid
        int fd = creat_open_fifo(O_WRONLY);
        pid_t pid = getpid();
        write(fd, &pid, sizeof(pid_t));
	

        creat_or_get_shm();
        at_shm();
        
            
        // 从共享内存获取数据
        while (1) {
            pause(); // 进入休眠，等待写进程发送信号
            printf("%s\n", (char *)shmaddr);
            memset(shmaddr, 0, SHM_SIZE);
        }
    }

    return 0;
}
