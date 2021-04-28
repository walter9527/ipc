#include "common.h"


void print_err(char *msg) {
    printf(msg);
}

int main(int argc, char *argv[]) {
    int ret = 0;

    /************************
      用于存放管道的读写文件描述符
      
      [0]: 读文件描述符 
      [1]: 写文件描述符 
     **********************/
    int fildes[2] = {0};
    
    ret = pipe(fildes);

    if (ret == -1) {
        print_err("pipe fail\n");
        return 0;
    }
    
    printf("get pipe successfully\n");

    ret = fork();


    if (ret > 0) {

        // 关闭没用到的写文件描述符，一样可以运行
        close(fildes[1]);
        close(fildes[0]); // 关闭所有的读信号，写进程将被关闭

        // 父进程，读管道
        char buf[128] = "";
        while (1) {
            memset(buf, 0, sizeof(buf));
            
            read(fildes[0], buf, sizeof(buf));
            
            printf(buf);
        }
    } else if (ret == 0){

        // 关闭没用到的写文件描述符，一样可以运行
        close(fildes[0]);
        close(fildes[1]);

        signal(SIGPIPE, SIG_IGN); // 忽略 SIGPIPE 信号，写进程不会被关闭

        // 子进程，写管道
        while (1) {
            write(fildes[1], "pipe learning\n", 14);
            sleep(1);
        }
    }

    return 0;
}
