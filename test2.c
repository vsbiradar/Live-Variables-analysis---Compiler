#include <stdlib.h>

int main(int argc, char *argv[])
{
    int x, y, z;

    if (argc >= 2) {
        y = atoi(argv[1]);
    }else return 1;

    if (y >= 3) x = 2;
    else x = 5;

    if (y >= 5) z = 3;

    x = x + z;

    return x;
}

