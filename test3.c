#include <stdlib.h>

int main()
{
    int x, y, z, i, M;

    M = rand();

    y = 5;

    for (i=0; i<M; i++) {
        if (i == 0) z = 0;
        else z += x*y;
    }

    return z;
}
