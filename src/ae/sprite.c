#include "ae/sprite.h"
#include "ae/surface.h"

sprite_t* new_sprite(texture_t *texture, uint16_t width, uint16_t height) {
    sprite_t *sprite = malloc(sizeof(sprite_t));
    sprite->texture = texture;
    sprite->texrect = texrect(0, 0, texture->width, texture->height, 0);
    sprite->width = width;
    sprite->height = height;
    sprite->scale = vec2(1, 1);
    sprite->origin = vec2(0, 0);
    sprite->rotation = 0;
    sprite->color = rgba(0, 0, 0, 1);
    return sprite;
}

sprite_t* new_sprite_ext(texture_t *texture, uint16_t width, uint16_t height, vec2_t scale, vec2_t origin, float rotation, color_t color) {
    sprite_t *sprite = new_sprite(texture, width, height);
    sprite->rotation = rotation;
    sprite->scale = scale;
    sprite->origin = origin;
    sprite->color = color;
    return sprite;
}

sprite_t* new_sprite_alt(texture_t *texture, uint16_t texture_x, uint16_t texture_y, uint16_t width, uint16_t height) {
    sprite_t *sprite = new_sprite(texture, width, height);
    sprite->texrect = texrect(texture_x, texture_y, width, height, 0);
    return sprite;
}

void draw_sprite(sprite_t *sprite, vec2_t position) {
    // TODO
}