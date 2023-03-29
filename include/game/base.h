#ifndef __BASE_H__
#define __BASE_H__

#include "ae/vector.h"
#include "ae/matrix.h"
#include "ae/str.h"
#include "ae/window.h"
#include "ae/draw.h"
#include "ae/surface.h"
#include "ae/text.h"

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint32_t  u32;
typedef int32_t   i32;
typedef uint64_t  u64;
typedef int64_t   i64;
typedef vec2_t    vec2;
typedef vec3_t    vec3;
typedef vec4_t    vec4;
typedef mat3_t    mat3;
typedef mat4_t    mat4;

extern window_t *window;
extern shader_t *shader;
extern surface_t *window_surface;
extern surface_t *game_surface;
extern float game_scale;
extern u32 game_visible_width;
extern u32 game_visible_height;
extern texture_t *sprites;
extern font_t *debug_font;
extern font_t *small_font;
extern vec2 game_surface_cutoff;
extern vec2 cursor, game_cursor;

extern str_t debug_log[128];
extern float debug_log_offset;

void print(const char *format, ...);

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

bool check_version(unsigned char major, unsigned char minor);

static inline render_instance_t* draw_texture(vec2 origin, vec2 position, float depth, u16 width, u16 height, color_t color, texture_t *texture, texrect_t texrect) {
    return draw_textured_polygon(quad_polygon, origin, position, depth, width, height, color, texture, texrect);
}

static inline render_instance_t* draw_texture_ext(vec2 origin, vec2 position, float depth, u16 width, u16 height, vec2 scale, float rotation, color_t color, texture_t *texture, texrect_t texrect) {
    return draw_textured_polygon_ext(quad_polygon, origin, position, depth, width, height, scale, rotation, color, texture, texrect);
}

vec2 to_game_coord(vec2 window_coord);
vec2 to_window_coord(vec2 game_coord);

float get_depth(float y);

static inline bool vec2_contains(vec2 a, vec2 b, u16 width, u16 height) {
    return (a.x >= b.x && a.y >= b.y && a.x < b.x + width && a.y < b.y + height);
}

vec2 interpolate_position(vec2 position, vec2 velocity);

#endif