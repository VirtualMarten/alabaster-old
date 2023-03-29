#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "polygon.h"
#include "shader.h"
#include "color.h"
#include "vector.h"
#include "matrix.h"
#include "texture.h"
#include "window.h"
#include "draw.h"
#include <stdint.h>

#ifndef PROJECTION_DEPTH_RANGE
#define PROJECTION_DEPTH_RANGE 1000
#endif

extern polygon_t *quad_polygon;

typedef struct {
    vec2_t position;
    float rotation;
    uint16_t width, height;
} view_t;

typedef struct {
    uint8_t texture_id;
    uint16_t width, height;
    uint16_t unit, mask;
    // // // // // // // //
    uint32_t framebuffer_id;
    uint8_t renderbuffer_id;
    shader_t *shader;
    color_t color;
    mat4_t projection;
    view_t view;
} surface_t;

surface_t* new_surface(shader_t *shader, uint16_t width, uint16_t height, color_t color, uint16_t mask);
surface_t* new_window_surface(window_t *window, shader_t *shader, color_t color);
void delete_surface(surface_t **surface);
void render_surface(surface_t *surface);

#endif