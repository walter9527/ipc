#include "common.h"

#define FIFONAME1 "./fifo1"

void print_err(char *msg) {
    perror(msg);
    exit(-1);
}

int create_open_fifo(char *fifoname, int open_mode) {

    int ret = -1;
    int fd = -1;

    ret = mkfifo(fifoname, 0664);
    // 忽略管道文件已存在的错误
    if (ret == -1 && errno != EEXIST) {
        print_err("mkfifo fail\n");
    }

    printf("mkfifo success\n");

    // 如果没有其他读进程，则只写 open 会一直阻塞
    fd = open(fifoname, open_mode);
    if (fd == -1) {
        print_err("open fail\n");
    }

    printf("open success\n");
    return fd;
}

void signal_fun() {
    //unlink(FIFONAME1);
    remove(FIFONAME1);
    exit(-1);
}

int main(int argc, char *argv[]) {


    char buf[128] = "";

    // 这个 signal 设置要放在 open 之前，避免 open 阻塞
    // 当使用 ctrl+c 时，删除管道文件
    signal(SIGINT, signal_fun);

    int fd = create_open_fifo(FIFONAME1, O_RDONLY);

    while (1) {
        memset(buf, 0, sizeof(buf));
        read(fd, buf, sizeof(buf));
        printf("recv: %s\n", buf);
    }

   return 0;
}
