#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "vector.h"
#include "math.h"

typedef union {
    struct {
        float m11, m12, m13;
        float m21, m22, m23;
        float m31, m32, m33;   
    };
    struct {
        float xx, xy, xz;
        float yx, yy, yz;
        float zx, zy, zz;
    };
    float data[9];
} mat3_t;

#define identity_mat3 ((mat3_t){{\
    1, 0, 0,\
    0, 1, 0,\
    0, 0, 1\
}})

typedef union {
    struct {
        float m11, m12, m13, m14;
        float m21, m22, m23, m24;
        float m31, m32, m33, m34;   
        float m41, m42, m43, m44;
    };
    struct {
        float xx, xy, xz, xw;
        float yx, yy, yz, yw;
        float zx, zy, zz, zw;
        float wx, wy, wz, ww;
    };
    float data[16];
} mat4_t;

#define identity_mat4 ((mat4_t){{\
    1, 0, 0, 0,\
    0, 1, 0, 0,\
    0, 0, 1, 0,\
    0, 0, 0, 1\
}})

#define const_mat3(...) {{__VA_ARGS__}}
#define mat3(...) ((mat3_t){{__VA_ARGS__}})

#define const_mat4(...) {{__VA_ARGS__}}
#define mat4(...) ((mat4_t){{__VA_ARGS__}})

static inline mat3_t mat3_add(mat3_t a, mat3_t b) {
    return mat3(
        a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13,
        a.m21 + b.m21, b.m22 + b.m22, a.m23 + b.m23,
        a.m31 + b.m31, b.m32 + b.m32, a.m33 + b.m33
    );
}

static inline mat4_t mat4_add(mat4_t a, mat4_t b) {
    return mat4(
        a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13, a.m14 + b.m14,
        a.m21 + b.m21, b.m22 + b.m22, a.m23 + b.m23, a.m24 + b.m24,
        a.m31 + b.m31, b.m32 + b.m32, a.m33 + b.m33, a.m34 + b.m34,
        a.m41 + b.m41, b.m42 + b.m42, a.m43 + b.m43, a.m44 + b.m44
    );
}

static inline mat3_t mat3_subtract(mat3_t a, mat3_t b) {
    return mat3(
        a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13,
        a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23,
        a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33
    );
}

static inline mat4_t mat4_subtract(mat4_t a, mat4_t b) {
    return mat4(
        a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13, a.m14 - b.m14,
        a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23, a.m24 - b.m24,
        a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33, a.m34 - b.m34,
        a.m41 - b.m41, a.m42 - b.m42, a.m43 - b.m43, a.m44 - b.m44
    );
}

static inline mat3_t mat3_multiply(mat3_t a, mat3_t b) {
    return mat3(
        (a.m11 * b.m11 + a.m21 * b.m12 + a.m31 * b.m13),
        (a.m12 * b.m11 + a.m22 * b.m12 + a.m32 * b.m13),
        (a.m13 * b.m11 + a.m23 * b.m12 + a.m33 * b.m13),
                                                                    
        (a.m11 * b.m21 + a.m21 * b.m22 + a.m31 * b.m23),
        (a.m12 * b.m21 + a.m22 * b.m22 + a.m32 * b.m23),
        (a.m13 * b.m21 + a.m23 * b.m22 + a.m33 * b.m23),
                                                                    
        (a.m11 * b.m31 + a.m21 * b.m32 + a.m31 * b.m33),
        (a.m12 * b.m31 + a.m22 * b.m32 + a.m32 * b.m33),
        (a.m13 * b.m31 + a.m23 * b.m32 + a.m33 * b.m33)
    );
}

static inline mat4_t mat4_multiply(mat4_t a, mat4_t b) {
    return mat4(
        (a.m11 * b.m11 + a.m21 * b.m12 + a.m31 * b.m13 + a.m41 * b.m14),
        (a.m12 * b.m11 + a.m22 * b.m12 + a.m32 * b.m13 + a.m42 * b.m14),
        (a.m13 * b.m11 + a.m23 * b.m12 + a.m33 * b.m13 + a.m43 * b.m14),
        (a.m14 * b.m11 + a.m23 * b.m12 + a.m34 * b.m13 + a.m44 * b.m14),

        (a.m11 * b.m21 + a.m21 * b.m22 + a.m31 * b.m23 + a.m41 * b.m24),
        (a.m12 * b.m21 + a.m22 * b.m22 + a.m32 * b.m23 + a.m42 * b.m24),
        (a.m13 * b.m21 + a.m23 * b.m22 + a.m33 * b.m23 + a.m43 * b.m24),
        (a.m14 * b.m21 + a.m24 * b.m22 + a.m34 * b.m23 + a.m44 * b.m24),

        (a.m11 * b.m31 + a.m21 * b.m32 + a.m31 * b.m33 + a.m41 * b.m34),
        (a.m12 * b.m31 + a.m22 * b.m32 + a.m32 * b.m33 + a.m42 * b.m34),
        (a.m13 * b.m31 + a.m23 * b.m32 + a.m33 * b.m33 + a.m43 * b.m34),
        (a.m14 * b.m31 + a.m24 * b.m32 + a.m34 * b.m33 + a.m44 * b.m34),

        (a.m11 * b.m41 + a.m21 * b.m42 + a.m31 * b.m43 + a.m41 * b.m44),
        (a.m12 * b.m41 + a.m22 * b.m42 + a.m32 * b.m43 + a.m42 * b.m44),
        (a.m13 * b.m41 + a.m23 * b.m42 + a.m33 * b.m43 + a.m43 * b.m44),
        (a.m14 * b.m41 + a.m24 * b.m42 + a.m34 * b.m43 + a.m44 * b.m44)
    );
}

static inline mat3_t mat3_transpose(mat3_t m) {
    return mat3(
        m.m11, m.m21, m.m31,
        m.m12, m.m22, m.m32,
        m.m13, m.m23, m.m33
    );
}

static inline mat4_t mat4_transpose(mat4_t m) {
    return mat4(
        m.m11, m.m21, m.m31, m.m41,
        m.m12, m.m22, m.m32, m.m42,
        m.m13, m.m23, m.m33, m.m43,
        m.m14, m.m24, m.m34, m.m44 
    );
}

static inline mat3_t mat3_scale(mat3_t m, vec2_t scale) {
    return mat3(
        m.m11 * scale.x, m.m12 * scale.x, m.m13 * scale.x,
        m.m21 * scale.y, m.m22 * scale.y, m.m23 * scale.y,
        m.m31,           m.m32,           m.m33
    );
}

static inline mat4_t mat4_scale(mat4_t m, vec3_t scale) {
    return mat4(
        m.m11 * scale.x, m.m12 * scale.x, m.m13 * scale.x, m.m14 * scale.x,
        m.m21 * scale.y, m.m22 * scale.y, m.m23 * scale.y, m.m24 * scale.y,
        m.m31 * scale.z, m.m32 * scale.z, m.m33 * scale.z, m.m34 * scale.z,
        m.m41,           m.m42,           m.m43,           m.m44
    );
}

static inline mat3_t mat3_translate(mat3_t m, vec2_t v) {
    return mat3(
        m.m11, m.m12, m.m13,
        m.m21, m.m22, m.m23,
        (v.x * m.m11 + v.y * m.m21 + m.m31),
        (v.x * m.m12 + v.y * m.m22 + m.m32),
        (v.x * m.m13 + v.y * m.m23 + m.m33)
    );
}

static inline mat4_t mat4_translate(mat4_t m, vec3_t v) {
    return mat4(
        m.m11, m.m12, m.m13, m.m14,
        m.m21, m.m22, m.m23, m.m24,
        m.m31, m.m32, m.m33, m.m34,
        (v.x * m.m11 + v.y * m.m21 + v.z * m.m31 + m.m41),
        (v.x * m.m12 + v.y * m.m22 + v.z * m.m32 + m.m42),
        (v.x * m.m13 + v.y * m.m23 + v.z * m.m33 + m.m43),
        (v.x * m.m14 + v.y * m.m24 + v.z * m.m34 + m.m44)
    );
}

static inline mat3_t mat3_from_scale(vec2_t v) {
    return mat3(
        v.x, 0.0, 0.0,
        0.0, v.y, 0.0,
        0.0, 0.0, 1.0
    );
}

static inline mat4_t mat4_from_scale(vec3_t v) {
    return mat4(
        v.x, 0.0, 0.0, 0.0,
        0.0, v.y, 0.0, 0.0,
        0.0, 0.0, v.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

static inline mat3_t mat3_from_translation(vec2_t v) {
    return mat3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        v.x, v.y, 1.0
    );
}

static inline mat4_t mat4_from_translation(vec3_t v) {
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        v.x, v.y, v.z, 1.0
    );
}

static inline mat3_t mat3_from_mat4(mat4_t m) {
    return mat3(
        m.m11, m.m12, m.m14,
        m.m21, m.m22, m.m24,
        m.m41, m.m42, m.m44
    );
}

static inline mat4_t mat4_from_mat3(mat3_t m) {
    return mat4(
        m.m11, m.m12, 0, m.m13,
        m.m21, m.m22, 0, m.m23,
        0,     0,     1, 0,
        m.m31, m.m32, 0, m.m33,
    );
}

static inline mat3_t mat3_from_quaternion(vec4_t q) {
    const float
        x2 = q.x + q.x,
        y2 = q.y + q.y,
        z2 = q.z + q.z,
        xx = q.x * x2,
        yx = q.y * x2,
        yy = q.y * y2,
        zx = q.z * x2,
        zy = q.z * y2,
        zz = q.z * z2,
        wx = q.w * x2,
        wy = q.w * y2,
        wz = q.w * z2;

    return mat3(
        (1 - yy - zz),
        (yx + wz),
        (zx - wy),
        (yx - wz),
        (1 - xx - zz),
        (zy + wx),
        (zx + wy),
        (zy - wx),
        (1 - xx - yy)
    );
}

static inline mat4_t mat4_from_quaternion(vec4_t q) {
    const double
        x2 = q.x + q.x,
        y2 = q.y + q.y,
        z2 = q.z + q.z,
        xx = q.x * x2,
        yx = q.y * x2,
        yy = q.y * y2,
        zx = q.z * x2,
        zy = q.z * y2,
        zz = q.z * z2,
        wx = q.w * x2,
        wy = q.w * y2,
        wz = q.w * z2;
                                
    return mat4(
        .m11 = 1 - yy - zz,
        .m12 = yx + wz,
        .m13 = zx - wy,
        .m14 = 0,
        .m21 = yx - wz,
        .m22 = 1 - xx - zz,
        .m23 = zy + wx,
        .m24 = 0,
        .m31 = zx + wy,
        .m32 = zy - wx,
        .m33 = 1 - xx - yy,
        .m34 = 0,
        .m41 = 0,
        .m42 = 0,
        .m43 = 0,
        .m44 = 1
    );
}

static inline mat3_t mat3_adjoint(mat3_t m) {
    return mat3(
        .m11 = (m.m22 * m.m33 - m.m23 * m.m32),
        .m12 = (m.m13 * m.m32 - m.m12 * m.m33),
        .m13 = (m.m12 * m.m23 - m.m13 * m.m22),
        .m21 = (m.m23 * m.m31 - m.m21 * m.m33),
        .m22 = (m.m11 * m.m33 - m.m13 * m.m31),
        .m23 = (m.m13 * m.m21 - m.m11 * m.m23),
        .m31 = (m.m21 * m.m32 - m.m22 * m.m31),
        .m32 = (m.m12 * m.m31 - m.m11 * m.m32),
        .m33 = (m.m11 * m.m22 - m.m12 * m.m21)
    );
}

static inline mat4_t mat4_adjoint(mat4_t m) {                                                         
    const double
        a00 = m.m11, a01 = m.m12, a02 = m.m13, a03 = m.m14,
        a10 = m.m21, a11 = m.m22, a12 = m.m23, a13 = m.m24,
        a20 = m.m31, a21 = m.m32, a22 = m.m33, a23 = m.m34,
        a30 = m.m41, a31 = m.m42, a32 = m.m43, a33 = m.m44;
                                                                        
    return mat4(
        .m11 = (a11 * (a22 * a33 - a23 * a32)
                - a21 * (a12 * a33 - a13 * a32)
                + a31 * (a12 * a23 - a13 * a22)),                                                
        .m12 = - (a01 * (a22 * a33 - a23 * a32)
                - a21 * (a02 * a33 - a03 * a32)
                + a31 * (a02 * a23 - a03 * a22)),                                                    
        .m13 =   (a01 * (a12 * a33 - a13 * a32)
                - a11 * (a02 * a33 - a03 * a32)
                + a31 * (a02 * a13 - a03 * a12)),                                                      
        .m14 = - (a01 * (a12 * a23 - a13 * a22)
                - a11 * (a02 * a23 - a03 * a22)
                + a21 * (a02 * a13 - a03 * a12)),                                                   
        .m21 = - (a10 * (a22 * a33 - a23 * a32)
                - a20 * (a12 * a33 - a13 * a32)
                + a30 * (a12 * a23 - a13 * a22)),                                                
        .m22 =  (a00 * (a22 * a33 - a23 * a32)
                - a20 * (a02 * a33 - a03 * a32)
                + a30 * (a02 * a23 - a03 * a22)),                                               
        .m23 = - (a00 * (a12 * a33 - a13 * a32)
                - a10 * (a02 * a33 - a03 * a32)
                + a30 * (a02 * a13 - a03 * a12)),                                             
        .m24 =  (a00 * (a12 * a23 - a13 * a22)
                - a10 * (a02 * a23 - a03 * a22)
                + a20 * (a02 * a13 - a03 * a12)),                                            
        .m31 =  (a10 * (a21 * a33 - a23 * a31)
                - a20 * (a11 * a33 - a13 * a31)
                + a30 * (a11 * a23 - a13 * a21)),                                            
        .m32 = - (a00 * (a21 * a33 - a23 * a31)
                - a20 * (a01 * a33 - a03 * a31)
                + a30 * (a01 * a23 - a03 * a21)),                                        
        .m33 =  (a00 * (a11 * a33 - a13 * a31)
                - a10 * (a01 * a33 - a03 * a31)
                + a30 * (a01 * a13 - a03 * a11)),                                      
        .m34 = - (a00 * (a11 * a23 - a13 * a21)
                - a10 * (a01 * a23 - a03 * a21)
                + a20 * (a01 * a13 - a03 * a11)),                                     
        .m41 = - (a10 * (a21 * a32 - a22 * a31)
                - a20 * (a11 * a32 - a12 * a31)
                + a30 * (a11 * a22 - a12 * a21)),                                     
        .m42 =  (a00 * (a21 * a32 - a22 * a31)
                - a20 * (a01 * a32 - a02 * a31)
                + a30 * (a01 * a22 - a02 * a21)),                                   
        .m43 = - (a00 * (a11 * a32 - a12 * a31)
                - a10 * (a01 * a32 - a02 * a31)
                + a30 * (a01 * a12 - a02 * a11)),                                   
        .m44 =  (a00 * (a11 * a22 - a12 * a21)
                - a10 * (a01 * a22 - a02 * a21)
                + a20 * (a01 * a12 - a02 * a11))
    );
}

static inline mat3_t mat3_invert(mat3_t m) {
    const float
        a11 = m.m11, a12 = m.m12, a13 = m.m13,
        a21 = m.m21, a22 = m.m22, a23 = m.m23,
        a31 = m.m31, a32 = m.m32, a33 = m.m33,
        b11 = a33 * a22 - a23 * a32,
        b21 = -a33 * a21 + a23 * a31,
        b31 = a32 * a21 - a22 * a31;
    float det = a11 * b11 + a12 * b21 + a13 * b31;
    
    return det ? mat3(
        .m11 = ((det = 1.0f / det) * b11),
        .m12 = (det * (-a33 * a12 + a13 * a32)),
        .m13 = (det * (a23 * a12 - a13 * a22)),
        .m21 = (det * b21),
        .m22 = (det * (a33 * a11 - a13 * a31)),
        .m23 = (det * (-a23 * a11 + a13 * a21)),
        .m31 = (det * b31),
        .m32 = (det * (-a32 * a11 + a12 * a31)),
        .m33 = (det * (a22 * a11 - a12 * a21))
    ) : m;
}

static inline mat4_t mat4_invert(mat4_t m) {
    mat4_t b = {0};
                                                                        
    double a00 = m.m11, a01 = m.m12, a02 = m.m13, a03 = m.m14;
    double a10 = m.m21, a11 = m.m22, a12 = m.m23, a13 = m.m24;
    double a20 = m.m31, a21 = m.m32, a22 = m.m33, a23 = m.m34;
    double a30 = m.m41, a31 = m.m42, a32 = m.m43, a33 = m.m44;
                                                                        
    double b00 = a00 * a11 - a01 * a10;
    double b01 = a00 * a12 - a02 * a10;
    double b02 = a00 * a13 - a03 * a10;
    double b03 = a01 * a12 - a02 * a11;
    double b04 = a01 * a13 - a03 * a11;
    double b05 = a02 * a13 - a03 * a12;
    double b06 = a20 * a31 - a21 * a30;
    double b07 = a20 * a32 - a22 * a30;
    double b08 = a20 * a33 - a23 * a30;
    double b09 = a21 * a32 - a22 * a31;
    double b10 = a21 * a33 - a23 * a31;
    double b11 = a22 * a33 - a23 * a32;
                                                                        
    double det =
        b00 * b11 - b01 * b10 +
        b02 * b09 + b03 * b08 -
        b04 * b07 + b05 * b06;
                                                                        
    if (det) {
        det = 1.0f / det;
                                                                        
        b.m11 = (a11 * b11 - a12 * b10 + a13 * b09) * det;
        b.m12 = (a02 * b10 - a01 * b11 - a03 * b09) * det;
        b.m13 = (a31 * b05 - a32 * b04 + a33 * b03) * det;
        b.m14 = (a22 * b04 - a21 * b05 - a23 * b03) * det;
                                                                        
        b.m21 = (a12 * b08 - a10 * b11 - a13 * b07) * det;
        b.m22 = (a00 * b11 - a02 * b08 + a03 * b07) * det;
        b.m23 = (a32 * b02 - a30 * b05 - a33 * b01) * det;
        b.m24 = (a20 * b05 - a22 * b02 + a23 * b01) * det;
                                                                        
        b.m31 = (a10 * b10 - a11 * b08 + a13 * b06) * det;
        b.m32 = (a01 * b08 - a00 * b10 - a03 * b06) * det;
        b.m33 = (a30 * b04 - a31 * b02 + a33 * b00) * det;
        b.m34 = (a21 * b02 - a20 * b04 - a23 * b00) * det;
                                                                        
        b.m41 = (a11 * b07 - a10 * b09 - a12 * b06) * det;
        b.m42 = (a00 * b09 - a01 * b07 + a02 * b06) * det;
        b.m43 = (a31 * b01 - a30 * b03 - a32 * b00) * det;
        b.m44 = (a20 * b03 - a21 * b01 + a22 * b00) * det;
    }

    return b;
}

static inline mat3_t mat3_rotate(mat3_t m, float rad) {
    const float c = cosf(rad), s = sinf(rad);
    return mat3(
        (c * m.m11 + s * m.m21),
        (c * m.m12 + s * m.m22),
        (c * m.m13 + s * m.m23),
        (c * m.m21 - s * m.m11),
        (c * m.m22 - s * m.m12),
        (c * m.m23 - s * m.m13),
        m.m31, m.m32, m.m33
    );
}

static inline mat4_t mat4_rotate(mat4_t m, float rad, vec3_t v) {
    const double d = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    const double c = cosf(rad), s = sinf(rad), t = 1 - c;
                                                    
    v.x /= d;
    v.y /= d;
    v.z /= d;
                                                    
    m.m11 = v.x * v.x * t + c;
    m.m12 = v.x * v.y * t - v.z * s;
    m.m13 = v.x * v.z * t + v.y * s;
    m.m14 = 0;
                                                    
    m.m21 = v.y * v.x * t + v.z * s;
    m.m22 = v.y * v.y * t + c;
    m.m23 = v.y * v.z * t - v.x * s;
    m.m24 = 0;
                                                    
    m.m31 = v.z * v.x * t - v.y * s;
    m.m32 = v.z * v.y * t + v.x * s;
    m.m33 = v.z * v.z * t + c;
    m.m34 = 0;
                                                    
    m.m41 = 0;
    m.m42 = 0;
    m.m43 = 0;
    m.m44 = 1;
                                                    
    return m;
}

static inline double mat4_determinant(mat4_t m) {
    const double a00 = m.m11, a01 = m.m12, a02 = m.m13, a03 = m.m14;
    const double a10 = m.m21, a11 = m.m22, a12 = m.m23, a13 = m.m24;
    const double a20 = m.m31, a21 = m.m32, a22 = m.m33, a23 = m.m34;
    const double a30 = m.m41, a31 = m.m42, a32 = m.m43, a33 = m.m44;
                                                                        
    const double b00 = a00 * a11 - a01 * a10;
    const double b01 = a00 * a12 - a02 * a10;
    const double b02 = a00 * a13 - a03 * a10;
    const double b03 = a01 * a12 - a02 * a11;
    const double b04 = a01 * a13 - a03 * a11;
    const double b05 = a02 * a13 - a03 * a12;
    const double b06 = a20 * a31 - a21 * a30;
    const double b07 = a20 * a32 - a22 * a30;
    const double b08 = a20 * a33 - a23 * a30;
    const double b09 = a21 * a32 - a22 * a31;
    const double b10 = a21 * a33 - a23 * a31;
    const double b11 = a22 * a33 - a23 * a32;
                                                                        
    const double det =
          b00 * b11
        - b01 * b10
        + b02 * b09
        + b03 * b08
        - b04 * b07
        + b05 * b06;

    return det;
}

static inline mat4_t mat4_rotate_x(mat4_t m, float rad) {
    const double s = sinl(rad);
    const double c = cosl(rad);
    const double a10 = m.m21;
    const double a11 = m.m22;
    const double a12 = m.m23;
    const double a13 = m.m24;
    const double a20 = m.m31;
    const double a21 = m.m32;
    const double a22 = m.m33;
    const double a23 = m.m34;
    m.m21 = a10 * c + a20 * s;
    m.m22 = a11 * c + a21 * s;
    m.m23 = a12 * c + a22 * s;
    m.m24 = a13 * c + a23 * s;
    m.m31 = a20 * c - a10 * s;
    m.m32 = a21 * c - a11 * s;
    m.m33 = a22 * c - a12 * s;
    m.m34 = a23 * c - a13 * s;
    return m;
}

static inline mat4_t mat4_rotate_y(mat4_t m, float rad) {
    const double s = sinl(rad);
    const double c = cosl(rad);
    const double a00 = m.m11;
    const double a01 = m.m12;
    const double a02 = m.m13;
    const double a03 = m.m14;
    const double a20 = m.m31;
    const double a21 = m.m32;
    const double a22 = m.m33;
    const double a23 = m.m34;
    m.m11 = a00 * c - a20 * s;
    m.m12 = a01 * c - a21 * s;
    m.m13 = a02 * c - a22 * s;
    m.m14 = a03 * c - a23 * s;
    m.m31 = a00 * s + a20 * c;
    m.m32 = a01 * s + a21 * c;
    m.m33 = a02 * s + a22 * c;
    m.m34 = a03 * s + a23 * c;
    return m;
}

static inline mat4_t mat4_rotate_z(mat4_t m, float rad) {
    const double s = sinl(rad);
    const double c = cosl(rad);
    const double a00 = m.m11;
    const double a01 = m.m12;
    const double a02 = m.m13;
    const double a03 = m.m14;
    const double a10 = m.m21;
    const double a11 = m.m22;
    const double a12 = m.m23;
    const double a13 = m.m24;
    m.m11 = a00 * c + a10 * s;
    m.m12 = a01 * c + a11 * s;
    m.m13 = a02 * c + a12 * s;
    m.m14 = a03 * c + a13 * s;
    m.m21 = a10 * c - a00 * s;
    m.m22 = a11 * c - a01 * s;
    m.m23 = a12 * c - a02 * s;
    m.m24 = a13 * c - a03 * s;
    return m;
}

static inline mat4_t mat4_frustum(float top, float left, float bottom, float right, float near, float far) {
    const double rl = 1.0 / (right - left);
    const double tb = 1.0 / (top - bottom);
    const double nf = 1.0 / (near - far);
                                                              
    return mat4(
        .m11 = rl * (2.0f * near),
        .m12 = 0,
        .m13 = 0,
        .m14 = 0,                                                         
        .m21 = 0,
        .m22 = tb * (2.0f * near),
        .m23 = 0,
        .m24 = 0,                                   
        .m31 = rl * (right + left),
        .m32 = rl * (top + bottom),
        .m33 = nf * (far + near),
        .m34 = -1,                                                  
        .m41 = 0,
        .m42 = 0,
        .m43 = nf * (2.0f * far * near),
        .m44 = 0
    );
}

static inline mat4_t mat4_perspective(float fov, float aspect, float near, float far) {
    double f = 1.0 / tanf(fov / 2);
    int nf = 1 / (near - far);
                                                     
    return mat4(
        .m11 = f / aspect,
        .m12 = 0,
        .m13 = 0,
        .m14 = 0,
        .m21 = 0,
        .m22 = f,
        .m23 = 0,
        .m24 = 0,
        .m31 = 0,
        .m32 = 0,
        .m33 = (near + far) * nf,
        .m34 = -1,
        .m41 = 0,
        .m42 = 0,
        .m43 = (2.0f * far * near) * nf,
        .m44 = 0
    );
}

static inline mat4_t mat4_ortho(float left, float right, float bottom, float top, float near, float far) {
    const double lr = 1.0 / (left - right);
    const double bt = 1.0 / (bottom - top);
    const double nf = 1.0 / (near - far);
                                                            
    return mat4(
        .m11 = -2.0 * lr,
        .m12 = 0,
        .m13 = 0,
        .m14 = 0,                                              
        .m21 = 0,
        .m22 = -2.0 * bt,
        .m23 = 0,
        .m24 = 0,                                     
        .m31 = 0,
        .m32 = 0,
        .m33 = 2.0 * nf,
        .m34 = 0,                                    
        .m41 = lr * (left + right),
        .m42 = bt * (top + bottom),
        .m43 = nf * (far + near),
        .m44 = 1
    );
}

static inline mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up) {
    double x0, x1, x2, y0, y1, y2, z0, z1, z2, len;

    mat4_t m = {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};

    if (!(fabs(eye.x - center.x) < epsilon &&
            fabs(eye.y - center.y) < epsilon &&
            fabs(eye.z - center.z) < epsilon)) {
        z0 = eye.x - center.x;
        z1 = eye.y - center.y;
        z2 = eye.z - center.z;
        len = 1.0 / sqrt(z0 * z0 + z1 * z1 + z2 * z2);
        z0 *= len;
        z1 *= len;
        z2 *= len;
        x0 = up.y * z2 - up.z * z1;
        x1 = up.z * z0 - up.x * z2;
        x2 = up.x * z1 - up.y * z0;
        len = sqrt(x0 * x0 + x1 * x1 + x2 * x2);

        if (len) {
            len = 1.0 / len;
            x0 *= len;
            x1 *= len;
            x2 *= len;
        }
        else x0 = x1 = x2 = 0;

        y0 = z1 * x2 - z2 * x1;
        y1 = z2 * x0 - z0 * x2;
        y2 = z0 * x1 - z1 * x0;
        len = sqrt(y0 * y0 + y1 * y1 + y2 * y2);

        if (len) {
            len = 1.0 / len;
            y0 *= len;
            y1 *= len;
            y2 *= len;
        }
        else x0 = x1 = x2 = 0;

        m.m11 = x0;
        m.m12 = y0;
        m.m13 = z0;
        m.m14 = 0;
        m.m21 = x1;
        m.m22 = y1;
        m.m23 = z1;
        m.m24 = 0;
        m.m31 = x2;
        m.m32 = y2;
        m.m33 = z2;
        m.m34 = 0;
        m.m41 = -(x0 * eye.x + x1 * eye.y + x2 * eye.z);
        m.m42 = -(y0 * eye.x + y1 * eye.y + y2 * eye.z);
        m.m43 = -(z0 * eye.x + z1 * eye.y + z2 * eye.z);
        m.m44 = 1;
    }

    return m;
}

#endif