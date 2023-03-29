#include "game/base.h"
#include <stdio.h>

window_t *window;
shader_t *shader;
surface_t *window_surface;
surface_t *game_surface;
float game_scale = 0;
u32 game_visible_width;
u32 game_visible_height;
texture_t *sprites;
font_t *debug_font;
font_t *small_font;
vec2 game_surface_cutoff;
vec2 cursor, game_cursor;

str_t debug_log[128] = {0};
float debug_log_offset = 0;

static void pushlog(const char *text) {
    if (debug_log[127]) str_del(debug_log[127]);
    memmove(&debug_log[1], debug_log, 127 * sizeof(str_t));
    debug_log[0] = str_new(text);
}

void print(const char *format, ...) {
    static char str[128];
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
    puts(str);
    pushlog(str);
}

bool check_version(unsigned char major, unsigned char minor) {
    if (major != VERSION_MAJOR) return false;
    switch (major) {
        case 0: return minor == VERSION_MINOR;
        default: return false;
    }
    return true;
}

vec2 to_game_coord(vec2 window_coord) {
    window_coord.x /= window_surface->width;
    window_coord.y /= window_surface->height;
    return vec2(
        game_surface->view.position.x + ((window_coord.x * game_visible_width) - game_visible_width / 2),
        game_surface->view.position.y + ((window_coord.y * game_visible_height) - game_visible_height / 2)
    );
}

vec2 to_window_coord(vec2 game_coord) {
    game_coord.x = (game_coord.x - game_surface->view.position.x) + game_visible_width / 2;
    game_coord.y = (game_coord.y - game_surface->view.position.y) + game_visible_height / 2;
    return vec2(
        ((game_coord.x / game_visible_width) * window_surface->width),
        ((game_coord.y / game_visible_height) * window_surface->height)
    );
}

float get_depth(float y) {
    float bottom = game_surface->view.position.y + (game_surface->view.height / 2);
    float top = game_surface->view.position.y - bottom;
    if (y >= top && y <= bottom)
        return bottom - y;
    return 9999;
}

vec2 interpolate_position(vec2 position, vec2 velocity) {
    if (velocity.x || velocity.y) {
        vec2 p;
        if (abs(velocity.x) > abs(velocity.y)) {
            p.x = roundf(position.x);
            p.y = roundf(position.y + (p.x - position.x) * velocity.y / velocity.x);
        } else {
            p.y = roundf(position.y);
            p.x = roundf(position.x + (p.y - position.y) * velocity.x / velocity.y);
        }
        return p;
    }
    return vec2(roundf(position.x), roundf(position.y));
}