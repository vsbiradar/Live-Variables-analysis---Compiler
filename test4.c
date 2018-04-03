#include <stdlib.h>

int main()
{
    int x, y, z, N, i;

    N = 100 + rand();

    y = 5;

    for (i=0; i<N%100; i++) {
        z = z + x + y;
    }

    return z;

}
