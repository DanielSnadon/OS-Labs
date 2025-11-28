#include "functions.h"
#include <math.h>

float sin_integral(float a, float b, float e)
{
    if (a > b) {
        return -sin_integral(b, a, e);
    }

    float res = 0.0;

    for (float x = a; x < b; x += e) {
        res += (sinf(x) + sinf(x + e)) * e / 2.0;
    }

    return res;
}