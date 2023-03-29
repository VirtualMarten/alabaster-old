#ifndef __DRAW_H__
#define __DRAW_H__

#include "texture.h"
#include "color.h"
#include "polygon.h"
#include "matrix.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define INST_DRAW_2D 1
#define INST_DRAW_3D 2

typedef enum {
    UNIFORM_VEC2,
    UNIFORM_VEC3,
    UNIFORM_VEC4,
    UNIFORM_MAT3,
    UNIFORM_MAT4,
    UNIFORM_1F,
    UNIFORM_2F,
    UNIFORM_3F,
    UNIFORM_4F,
    UNIFORM_1I,
    UNIFORM_2I,
    UNIFORM_3I,
    UNIFORM_4I,
    UNIFORM_1FV,
    UNIFORM_2FV,
    UNIFORM_3FV,
    UNIFORM_4FV,
    UNIFORM_1IV,
    UNIFORM_2IV,
    UNIFORM_3IV,
    UNIFORM_4IV,
    UNIFORM_MATRIX_2F,
    UNIFORM_MATRIX_3F,
    UNIFORM_MATRIX_4F,
    UNIFORM_MATRIX_2FV,
    UNIFORM_MATRIX_3FV,
    UNIFORM_MATRIX_4FV
} uniform_type_t;

typedef struct {
    const char *name;
    uniform_type_t type;
    bool post;
    void *data;
} uniform_update_t;

typedef struct {
    uint8_t type;
    texture_t *texture;
    color_t color;
    uniform_update_t *uniform_updates;
    union {
        struct {
            polygon_t *polygon;
            uint16_t width, height;
            mat3_t transform;
            float depth;
            texrect_t texrect;
        } draw_2d;
        struct {
            mat4_t transform;
        } draw_3d;
    };
} render_instance_t;

extern render_instance_t *opaque_render_instances;
extern render_instance_t *transparent_render_instances;

render_instance_t* push_render_instance(render_instance_t inst);
void set_uniform(render_instance_t *inst, const char *name, bool post, uniform_type_t type, ...);
void set_uniform_array(render_instance_t *inst, const char *name, bool post, uniform_type_t type, uint32_t count, ...);
render_instance_t* draw_2d(polygon_t *polygon, mat3_t transform, float depth, uint16_t width, uint16_t height, texture_t *texture, texrect_t texrect, color_t color);
render_instance_t* draw_polygon(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, color_t color);
render_instance_t* draw_polygon_ext(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, vec2_t scale, float rotation, color_t color);
render_instance_t* draw_textured_polygon(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, color_t color, texture_t *texture, texrect_t texrect);
render_instance_t* draw_textured_polygon_ext(polygon_t *polygon, vec2_t origin, vec2_t position, float depth, uint16_t width, uint16_t height, vec2_t scale, float rotation, color_t color, texture_t *texture, texrect_t texrect);

#endif