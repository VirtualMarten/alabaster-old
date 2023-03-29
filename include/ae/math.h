#ifndef __MATH_H__
#define __MATH_H__

#include <stdarg.h>
#include <math.h>
#include <stdlib.h>

#ifndef typeof
#define typeof __typeof__
#endif

#ifndef MIN
#define MIN(a, b) (a < b ? a : b)
#endif
#ifndef MAX
#define MAX(a, b) (a > b ? a : b)
#endif

#define ABS(n) (n > 0 ? n : -n)
#define LERP(a, b, t) ((1.0 - t) * a + t * b)

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define min(a, b) ({ typeof(a) _a = a; typeof(b) _b = b; _a < _b ? _a : _b; })
#define max(a, b) ({ typeof(a) _a = a; typeof(b) _b = b; _a > _b ? _a : _b; })
#define abs(n) ({ typeof(n) _n = n; _n > 0 ? _n : -_n; })
#define sign(n) (n >= 0 ? 1 : -1)

static inline double lerp(double a, double b, double t) { return (1.0 - t) * a + t * b; }
static inline float lerpf(float a, float b, float t) { return (1.0f - t) * a + t * b; }

static inline float cubic_bezier(float x0, float x2, float x1, float t) {
    return powf(1.0f - t, 3) * x0 + 3.0f * t * powf(1.0f - t, 2.0f) * x1 + 3.0f * powf(t, 2.0f) * (1.0f - t) * x2 + powf(t, 3.0f) * x2;
}

extern double pi;
extern double epsilon;
extern double tau;

#define torad(deg) (deg * (pi / 180.0))
#define todeg(rad) (rad * (180.0 / pi))
#define ldx(length, rad) (length * cosf(rad))
#define ldy(length, rad) (length * sinf(rad))
#define lddx(length, deg) (length * cosf(torad(deg)))
#define lddy(length, deg) (length * sinf(torad(deg)))

static inline float randf() { return rand() / (float)RAND_MAX; }

static inline int rrange(int min, int max) { return rand() % (max + 1 - min) + min; }
static inline float rrangef(float min, float max) { return fmaf(max - min, rand() / (float)RAND_MAX, min); }

int choose(unsigned int n, ...);

static inline int choose2(int a, int b) {
    return rrange(0, 1) ? b : a;
}

static inline int choose3(int a, int b, int c) {
    const char choice = rrange(0, 2);
    return choice == 0 ? a : choice == 1 ? b : c;
}

static inline int choose4(int a, int b, int c, int d) {
    const char choice = rrange(0, 3);
    return choice == 0 ? a : choice == 1 ? b : choice == 2 ? c : d;
}

float choosef(unsigned int n, ...);

static inline float choose2f(float a, float b) {
    return rrange(0, 1) ? b : a;
}

static inline float choose3f(float a, float b, float c) {
    const char choice = rrange(0, 2);
    return choice == 0 ? a : choice == 1 ? b : c;
}

static inline float choose4f(float a, float b, float c, float d) {
    const char choice = rrange(0, 3);
    return choice == 0 ? a : choice == 1 ? b : choice == 2 ? c : d;
}

#endif