#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>


int main(int argc, char *argv[])
{
    char *p = NULL;
    char *q = ((char *)-1);

    printf("%ld\n", q - p);
    return 0;
}
