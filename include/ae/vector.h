#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "math.h"

typedef union { struct { float x, y; }; float data[2]; } vec2_t;
typedef union { struct { float x, y, z; }; vec2_t xy; float data[3]; } vec3_t;
typedef union { struct { float x, y, z, w; }; vec2_t xy, zw; vec3_t xyz; float data[4]; } vec4_t;

#define _GET_VEC(_1, _2, _3, _4, NAME, ...) NAME
#define vec(...) _GET_VEC(__VA_ARGS__, vec4, vec3, vec2, vec2)(__VA_ARGS__)

#define const_vec2(x, y) {{ x, y }}
#define const_vec3(x, y, z) {{ x, y, z }}
#define const_vec4(x, y, z, w) {{ x, y, z, w }}

#define vec2(x, y) ((vec2_t){{ x, y }})
#define vec3(x, y, z) ((vec3_t){{ x, y, z }})
#define vec4(x, y, z, w) ((vec4_t){{ x, y, z, w }})

#define vec2_cast(v) (*(vec2_t*)&v)
#define vec3_cast(v) (*(vec3_t*)&v)
#define vec4_cast(v) (*(vec4_t*)&v)

static inline vec2_t add2(vec2_t a, vec2_t b) { return vec2(a.x + b.x, a.y + b.y); }
static inline vec3_t add3(vec3_t a, vec3_t b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline vec4_t add4(vec4_t a, vec4_t b) { return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }

static inline vec2_t sub2(vec2_t a, vec2_t b) { return vec2(a.x - b.x, a.y - b.y); }
static inline vec3_t sub3(vec3_t a, vec3_t b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline vec4_t sub4(vec4_t a, vec4_t b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }

static inline vec2_t mul2(vec2_t a, vec2_t b) { return vec2(a.x * b.x, a.y * b.y); }
static inline vec3_t mul3(vec3_t a, vec3_t b) { return vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
static inline vec4_t mul4(vec4_t a, vec4_t b) { return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }

static inline vec2_t div2(vec2_t a, vec2_t b) { return vec2(a.x / b.x, a.y / b.y); }
static inline vec3_t div3(vec3_t a, vec3_t b) { return vec3(a.x / b.x, a.y / b.y, a.z / b.z); }
static inline vec4_t div4(vec4_t a, vec4_t b) { return vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }

static inline vec2_t vec2_scale(vec2_t a, float scale) { return vec2(a.x * scale, a.y * scale); }
static inline vec3_t vec3_scale(vec3_t a, float scale) { return vec3(a.x * scale, a.y * scale, a.z * scale); }
static inline vec4_t vec4_scale(vec4_t a, float scale) { return vec4(a.x * scale, a.y * scale, a.z * scale, a.w * scale); }

static inline vec2_t vec2_invert(vec2_t v) { return vec2(1.0f / v.x, 1.0f / v.y); }
static inline vec3_t vec3_invert(vec3_t v) { return vec3(1.0f / v.x, 1.0f / v.y, 1.0f / v.z); }
static inline vec4_t vec4_invert(vec4_t v) { return vec4(1.0f / v.x, 1.0f / v.y, 1.0f / v.z, 1.0f / v.w); }

static inline vec2_t vec2_abs(vec2_t v) { return vec2(ABS(v.x), ABS(v.y)); }
static inline vec3_t vec3_abs(vec3_t v) { return vec3(ABS(v.x), ABS(v.y), ABS(v.z)); }
static inline vec4_t vec4_abs(vec4_t v) { return vec4(ABS(v.x), ABS(v.y), ABS(v.z), ABS(v.w)); }

static inline vec2_t vec2_min(vec2_t a, vec2_t b) { return vec2(MIN(a.x, b.x), MIN(a.y, b.y)); }
static inline vec3_t vec3_min(vec3_t a, vec3_t b) { return vec3(MIN(a.x, b.x), MIN(a.y, b.y), MIN(a.z, b.z)); }
static inline vec4_t vec4_min(vec4_t a, vec4_t b) { return vec4(MIN(a.x, b.x), MIN(a.y, b.y), MIN(a.z, b.z), MIN(a.w, b.w)); }

static inline vec2_t vec2_max(vec2_t a, vec2_t b) { return vec2(MAX(a.x, b.x), MAX(a.y, b.y)); }
static inline vec3_t vec3_max(vec3_t a, vec3_t b) { return vec3(MAX(a.x, b.x), MAX(a.y, b.y), MAX(a.z, b.z)); }
static inline vec4_t vec4_max(vec4_t a, vec4_t b) { return vec4(MAX(a.x, b.x), MAX(a.y, b.y), MAX(a.z, b.z), MAX(a.w, b.w)); }

static inline vec2_t vec2_floor(vec2_t v) { return vec2(floorf(v.x), floorf(v.y)); }
static inline vec3_t vec3_floor(vec3_t v) { return vec3(floorf(v.x), floorf(v.y), floorf(v.z)); }
static inline vec4_t vec4_floor(vec4_t v) { return vec4(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w)); }

static inline vec2_t vec2_round(vec2_t v) { return vec2(roundf(v.x), roundf(v.y)); }
static inline vec3_t vec3_round(vec3_t v) { return vec3(roundf(v.x), roundf(v.y), roundf(v.z)); }
static inline vec4_t vec4_round(vec4_t v) { return vec4(roundf(v.x), roundf(v.y), roundf(v.z), roundf(v.w)); }

static inline vec2_t vec2_ceil(vec2_t v) { return vec2(ceilf(v.x), ceilf(v.y)); }
static inline vec3_t vec3_ceil(vec3_t v) { return vec3(ceilf(v.x), ceilf(v.y), ceilf(v.z)); }
static inline vec4_t vec4_ceil(vec4_t v) { return vec4(ceilf(v.x), ceilf(v.y), ceilf(v.z), ceilf(v.w)); }

static inline vec2_t lerp_vec2(vec2_t a, vec2_t b, float t) { return vec2(LERP(a.x, b.x, t), LERP(a.y, b.y, t)); }
static inline vec3_t lerp_vec3(vec3_t a, vec3_t b, float t) { return vec3(LERP(a.x, b.x, t), LERP(a.y, b.y, t), LERP(a.z, b.z, t)); }
static inline vec4_t lerp_vec4(vec4_t a, vec4_t b, float t) { return vec4(LERP(a.x, b.x, t), LERP(a.y, b.y, t), LERP(a.z, b.z, t), LERP(a.w, b.w, t)); }

static inline vec2_t vec2_ld(float length, float radians) { return vec2(ldx(length, radians), ldy(length, radians)); }
static inline vec2_t vec2_ldd(float length, float degrees) { return vec2(lddx(length, degrees), lddy(length, degrees)); }

static inline float vec2_length(vec2_t v) { return sqrtf(v.x * v.x + v.y * v.y); }
static inline float vec3_length(vec3_t v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }
static inline float vec4_length(vec4_t v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w); }

static inline float vec2_dot(vec2_t a, vec2_t b) { return (a.x * b.x) + (a.y * b.y); }
static inline float vec3_dot(vec3_t a, vec3_t b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }
static inline float vec4_dot(vec4_t a, vec4_t b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w); }

static inline vec3_t cross_vec2(vec2_t a, vec2_t b) { return vec3(0, 0, (a.x * b.y) - (a.y * b.x)); }
static inline vec3_t cross_vec3(vec3_t a, vec3_t b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

static inline vec2_t vec2_normalize(vec2_t v) {
    float len = vec2_length(v);
    return (len > 0) ? vec2(v.x * (len = 1 / len), v.y * len) : v;
}

static inline vec3_t vec3_normalize(vec3_t v) {
    float len = vec3_length(v);
    return (len > 0) ? vec3(v.x * (len = 1 / len), v.y * len, v.z * len) : v;
}

static inline vec4_t vec4_normalize(vec4_t v) {
    float len = vec4_length(v);
    return (len > 0) ? vec4(v.x * (len = 1 / len), v.y * len, v.z * len, v.w * len) : v;
}

static inline float vec2_angle(vec2_t a, vec2_t b) {
    const vec2_t c = vec2_normalize(sub2(a, b));
    return atan2f(c.y, c.x);
}

static inline float vec2_distance(vec2_t a, vec2_t b) {
    const float xd = b.x - a.x;
    const float yd = b.y - a.y;
    return sqrtf(xd * xd + yd * yd);
}

static inline float vec3_distance(vec3_t a, vec3_t b) {
    float xd = b.x - a.x;
    float yd = b.y - a.y;
    float zd = b.z - a.z;
    return sqrtf(xd * xd + yd * yd + zd * zd);
}

static inline float vec4_distance(vec4_t a, vec4_t b) {
    float xd = b.x - a.x;
    float yd = b.y - a.y;
    float zd = b.z - a.z;
    float wd = b.w - a.w;
    return sqrtf(xd * xd + yd * yd + zd * zd + wd * wd);
}

#endif