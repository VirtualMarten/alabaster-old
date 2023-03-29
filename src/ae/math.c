#include "ae/math.h"

double pi = 3.141592653589793238462643383279502884;
double epsilon = 1.19209290e-7;
double tau = 6.28318530717958647692528676655900576;

int choose(unsigned int n, ...) {
    va_list args;
    va_start(args, n);

    int x, rtn;
    unsigned int i, choice = rrange(0, n);
    for (i = 0; i < n; i++) {
        x = va_arg(args, int);
        if (i == choice) rtn = x;
    }

    va_end(args);

    return rtn;
}

float choosef(unsigned int n, ...) {
    va_list args;
    va_start(args, n);

    float x, rtn;
    unsigned int i, choice = rrange(0, n);
    for (i = 0; i < n; i++) {
        x = va_arg(args, double);
        if (i == choice) rtn = x;
    }

    va_end(args);

    return rtn;
}