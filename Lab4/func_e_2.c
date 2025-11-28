#include "functions.h"
#include <math.h>

long long fact(int n)
{
    if (n < 0) {
        return 0;
    }

    if (n == 0 || n == 1) {
        return 1;
    }

    long long res = 1;
    for (int i = 2; i < n; i++) {
        res *= i;
    }

    return res;
}

float e(int x)
{
    if (x < 0) {
        return 0.0;
    }
    
    float res = 0.0;

    for (int n = 0; n <= x; n++) {
        res += 1.0 / (float)fact(n);
    }

    return res;
}