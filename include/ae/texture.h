#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/glew.h>
#include <stdint.h>
#include <stdbool.h>
#include "image.h"

#include "texture_defs.h"

typedef struct {
    uint8_t id;
    uint16_t width, height;
    uint16_t unit, mask;
} texture_t;

typedef struct {
    uint16_t x, y, width, height;
    uint8_t wrap;
} texrect_t;

#define texrect(x, y, w, h, wrap) (texrect_t){x, y, w, h, wrap}

texture_t* new_texture(uint16_t width, uint16_t height, uint8_t *data, uint16_t unit_n, uint16_t mask);
texture_t* load_texture(texture_t *t, const char *path);
texture_t* update_texture(texture_t *t, uint16_t width, uint16_t height, uint8_t *data, uint16_t mask);
texture_t* update_texture_area(texture_t *t, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, uint16_t mask);
texture_t* set_texture_smooth(texture_t *t, bool smooth);
texture_t* set_texture_wrap(texture_t *t, bool wrap_x, bool wrap_y);
texture_t* generate_texture_mipmap(texture_t *t);

static inline void delete_texture(texture_t **t) {
    glDeleteTextures(1, (GLuint*)&(*t)->id);
    free(*t);
    *t = NULL;
}

#endif