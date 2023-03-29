#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "texture.h"
#include "vector.h"
#include "color.h"
#include <stdint.h>

typedef struct {
    texture_t *texture;
    texrect_t texrect;
    uint16_t width, height;
    vec2_t scale;
    vec2_t origin;
    float rotation;
    color_t color;
} sprite_t;

sprite_t* new_sprite(texture_t *texture, uint16_t width, uint16_t height);
sprite_t* new_sprite_ext(texture_t *texture, uint16_t width, uint16_t height, vec2_t scale, vec2_t origin, float rotation, color_t color);
sprite_t* new_sprite_alt(texture_t *texture, uint16_t texture_x, uint16_t texture_y, uint16_t width, uint16_t height);
void draw_sprite(sprite_t *sprite, vec2_t position);

#endif