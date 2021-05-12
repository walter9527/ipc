#include "common.h"

#define SHM_FILE "./shmfile"
#define SHM_SIZE 4096

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
   
    fd = open(SHM_FILE, O_RDWR | O_CREAT, 0664);
    print_err(fd, "open fail");

    key = ftok(SHM_FILE, 'b');
    print_err(key, "ftok fail");
    
    shmid = shmget(key, SHM_SIZE, 0664 | IPC_CREAT);
    print_err(shmid, "shmget fail");
}

void at_shm() 
{
    shmaddr = shmat(shmid, NULL, 0);
    print_err(shmaddr - ((void *)0), "shmat fail");

    memset(shmaddr, 0, SHM_SIZE);
}

void signal_fun(int sigflg)
{
    // 取消映射
    shmdt(shmaddr);
    // 删除映射
    shmctl(shmid, IPC_RMID, NULL);
    remove(SHM_FILE);
    printf("remove success\n");
    exit(-1);
}

char buf[300] = {
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccbbbbbbbbbdddddddddddddddddddkfdsassse3213322"
};

int main(int argc, char *argv[])
{

    signal(SIGINT, signal_fun);

    int ret = fork();

    if (ret > 0) {

        // 创建共享内存
        creat_or_get_shm();

        // 建立映射
        at_shm();

        // 将应用缓存数据复制到共享内存中
        while (1) {
            memcpy(shmaddr, buf, sizeof(buf));
            sleep(1);
        }
    } else if (ret == 0) {
        
        // 创建共享内存
        creat_or_get_shm();

        // 建立映射
        at_shm();

        // 从共享内存获取数据
        while (1) {
            if (strlen((char *)shmaddr) > 0) {
                printf("%s\n", (char *)shmaddr);
                memset(shmaddr, 0, SHM_SIZE);
            }
        }
    }

    // raise(SIGINT);
    
    return 0;
}
