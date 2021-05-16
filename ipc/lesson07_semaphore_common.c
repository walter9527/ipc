#include "common.h"

#define SEM_FILE "./semfile"

union semun {
     int val;
     struct semid_ds *buf;
     unsigned short int *array;
     struct seminfo *__buf;
} sem_un;

void print_err(int flg, char *msg)
{
    if (flg == -1) {
        perror(msg);
        exit(-1);
    }
}

int creat_or_get_sem(int nsems)
{
    int fd = open(SEM_FILE, O_RDWR | O_CREAT, 0644);
    print_err(fd, "open fail");

    int key = ftok(SEM_FILE, 'a');
    print_err(key, "ftok fail");

    int semid = semget(key, nsems, 0644 | IPC_CREAT);
    print_err(semid, "semget fail");

    return semid;
}


void init_sem(int semid, int semnum, int val)
{
	sem_un.val = val;
    int ret = semctl(semid, semnum, SETVAL, sem_un);
    // int ret = semctl(semid, semnum, SETVAL, val); // 也可以不定义联合体
	print_err(ret, "semctl init fail");
}

void del_sem(int semid)
{
    int ret = semctl(semid, 0, IPC_RMID);
    print_err(ret, "semctl del fail");

    remove(SEM_FILE);
}

void p_sem(int semid, int semnum_buf[], size_t nsops)
{
    struct sembuf sops[nsops];
    for (int i = 0; i < nsops; i++) {
        sops[i].sem_num = semnum_buf[i]; // 信号量编号
        sops[i].sem_op = -1; // -1 表示 p 操作 
        sops[i].sem_flg = SEM_UNDO; // 防止死锁
    }
    int ret = semop(semid, sops, nsops);
    print_err(ret, "semop fail");
}

void v_sem(int semid, int semnum_buf[], size_t nsops)
{
    struct sembuf sops[nsops];
    for (int i = 0; i < nsops; i++) {
        sops[i].sem_num = semnum_buf[i]; // 信号量编号
        sops[i].sem_op = 1; // -1 表示 v 操作 
        sops[i].sem_flg = SEM_UNDO; // 防止死锁
    }
    int ret = semop(semid, sops, nsops);
    print_err(ret, "semop fail");
}

void p_sem_s(int semid, int semnum)
{
    struct sembuf sops = {semnum, -1, SEM_UNDO};

    int ret = semop(semid, &sops, 1);
    print_err(ret, "semop fail");
}

void v_sem_s(int semid, int semnum)
{
    struct sembuf sops = {semnum, 1, SEM_UNDO};

    int ret = semop(semid, &sops, 1);
    print_err(ret, "semop fail");
}
