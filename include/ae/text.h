#ifndef __TEXT_H__
#define __TEXT_H__

#include "texture.h"
#include "str.h"
#include "vector.h"
#include "color.h"
#include "surface.h"
#include <stdint.h>
#include "libs/stb_truetype.h"

typedef struct {
    unsigned int x, y, w, h;
} glyph_texrect_t;

typedef struct glyph_s {
    glyph_texrect_t texrect;
    int offset_x, offset_y;
    int width, height;
    int advance, lsb;
    float kern_advance;
    int32_t baseline;
    int32_t line_height;
} glyph_t;

typedef enum {
    FT_BITMAP,
    FT_SDF,
    FT_MSDF
} font_type_t;

typedef struct font_s {
    font_type_t type;
    str_t data;
    stbtt_fontinfo *info;
    int16_t glyph_width, glyph_height;
    texture_t *glyph_atlas;
    glyph_texrect_t *glyph_texrects;
    glyph_t _glyph;
    int32_t xm;
    int margin;
} font_t;

font_t* new_font(font_type_t type, uint16_t glyph_width, uint16_t glyph_height);
void delete_font(font_t **font);

glyph_t* get_glyph(font_t *font, float size, int codepoint, int next_codepoint);

int16_t load_font(font_t *f, const char *path, bool mipmap);
vec2_t get_text_size(font_t *font, const char *text, uint32_t len, float size, int16_t spacing);
vec2_t draw_text_ext(font_t *font, const char *text, uint32_t len, float size, int16_t spacing, vec2_t origin, vec2_t position, float depth, vec2_t scale, float rotation, color_t color);

static inline vec2_t draw_text(font_t *font, const char *text, uint32_t len, float size, int16_t spacing, vec2_t origin, vec2_t position, float depth, color_t color) {
    return draw_text_ext(font, text, len, size, spacing, origin, position, depth, vec2(1, 1), 0, color);
}

#endif