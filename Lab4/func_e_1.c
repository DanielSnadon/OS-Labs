#include "functions.h"
#include <math.h>

float e(int x)
{
    if (x <= 0) {
        return 1.0;
    }

    return powf(1.0 + (1.0 / (float)x), (float)x);
}

