#include "common.h"


void print_err(char *msg) {
    printf(msg);
}

int main(int argc, char *argv[]) {
    int ret1 = 0;
    int ret2 = 0;

    int fildes1[2] = {0};
    int fildes2[2] = {0};
    
    ret1 = pipe(fildes1);
    ret2 = pipe(fildes2);

    if (ret1 == -1 || ret2 == -1) {
        print_err("pipe fail\n");
        return 0;
    }
    
    printf("get pipe successfully\n");

    ret1 = fork();


    if (ret1 > 0) {

        close(fildes1[1]);
        close(fildes2[0]);

        char buf[128] = "";

        while (1) {
            memset(buf, 0, sizeof(buf));
            
            read(fildes1[0], buf, sizeof(buf));
            sleep(1);
            
            printf("parent: recv data - %s\n", buf);

            write(fildes2[1], "world", 5);
        }
    } else if (ret1 == 0) {

        close(fildes2[1]);
        close(fildes1[0]);

        char buf[128] = "";

        while (1) {
            memset(buf, 0, sizeof(buf));

            write(fildes1[1], "hello", 5);
            sleep(1);
            
            read(fildes2[0], buf, sizeof(buf));
            printf("child: recv data - %s\n", buf);
        }

    }

    return 0;
}
