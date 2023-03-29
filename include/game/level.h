#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "game/base.h"
#include "ae/vector.h"

#define TILE_SIZE 16
#define CHUNK_SIZE 32

typedef struct {
    u8 id, state;
} tile_t;

typedef struct {
    i16 x, y;
    tile_t tiles[CHUNK_SIZE * 2];
} chunk_t;

//static chunk_t *chunks = NULL;

typedef struct {
    tile_t *tiles;
    u16 width, height;
    vec2 position;
    surface_t *surface;
} level_t;

extern level_t *current_level;

level_t* new_level(u16 width, u16 height, vec2 position);
void render_level(level_t *l);
void draw_level(level_t *l);
void init_tile_states();
tile_t* get_tile(level_t *l, i32 x, i32 y);
void set_tile(level_t *l, u16 x, u16 y, u8 id);

#endif