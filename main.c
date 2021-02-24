#include <stdio.h>


int main(int argc, char *argv[])
{
    int a = 10, b = 20, c = 30;

    if (c > b > a) {
        printf("TRUE");
    } else {
        printf("FALSE");
    }
    getchar();
    return 0;
}
