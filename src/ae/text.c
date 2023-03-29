#include "ae/text.h"
#include "ae/array.h"
#include "ae/edtaa3func.h"
#include "ae/utility.h"
#include <float.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include "ae/libs/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "ae/libs/stb_truetype.h"

font_t* new_font(font_type_t type, uint16_t glyph_width, uint16_t glyph_height) {
    font_t *font = calloc(1, sizeof(font_t));
    font->type = type;
    font->glyph_width = glyph_width;
    font->glyph_height = glyph_height ? glyph_height : glyph_width;
    return font;
}

void delete_font(font_t **font) {
    if ((*font)->info) free((*font)->info);
    if ((*font)->glyph_atlas)
        delete_texture(&(*font)->glyph_atlas);
    if ((*font)->glyph_texrects)
        arr_delete((*font)->glyph_texrects);
    if ((*font)->data)
        str_del((*font)->data);
    free(*font);
    *font = NULL;
}

static glyph_t* get_glyph_metrics(font_t *font, float size, int codepoint, int next_codepoint) {
    if (!size) size = font->glyph_height;
    if (!font->info) {
        float scale = (size / font->glyph_height);
        font->_glyph.width = scale * font->glyph_width;
        font->_glyph.height = size;
        font->_glyph.advance = scale * font->glyph_width;
        if (codepoint == '\t')
            font->_glyph.advance *= 4;
        return &font->_glyph;
    }
    float scale = stbtt_ScaleForPixelHeight(font->info, size);
    int x, y;
    stbtt_GetCodepointHMetrics(font->info, codepoint == '\t' ? ' ' : codepoint, &font->_glyph.advance, &font->_glyph.lsb);
    font->_glyph.advance *= scale;
    if (codepoint == '\n') font->_glyph.advance = 0;
    else if (codepoint == '\t') font->_glyph.advance *= 4;
    font->_glyph.lsb *= scale;
    stbtt_GetCodepointBitmapBox(font->info, codepoint, scale, scale, &font->_glyph.offset_x, &font->_glyph.offset_y, &x, &y);
    font->_glyph.width = x - font->_glyph.offset_x;
    font->_glyph.height = y - font->_glyph.offset_y;
    int32_t ascent, line_gap;
    stbtt_GetFontVMetrics(font->info, &ascent, NULL, &line_gap);
    font->_glyph.baseline = (int)(ascent * scale);
    font->_glyph.line_height = font->_glyph.height + font->_glyph.baseline + (line_gap * scale);
    if (next_codepoint)
        font->_glyph.kern_advance = scale * stbtt_GetCodepointKernAdvance(font->info, codepoint, next_codepoint);
    else font->_glyph.kern_advance = 0;
    return &font->_glyph;
}

int prerender_bitmap_ttf(font_t *font, float size, bool smooth, bool mipmap) {
    const uint16_t m = 95;
    int32_t i, atlas_size = 512;
    stbrp_node *packing_nodes = malloc(sizeof(stbrp_node) * m);
    stbrp_rect *packing_rects = malloc(sizeof(stbrp_rect) * m);
    float scale = stbtt_ScaleForPixelHeight(font->info, size);

    int x0, y0, x1, y1;
    for (i = 0; i < m; i++) {
        stbtt_GetCodepointBitmapBox(font->info, i + 32, scale, scale, &x0, &y0, &x1, &y1);
        glyph_texrect_t texrect = {
            .w = packing_rects[i].w = x1 - x0,
            .h = packing_rects[i].h = y1 - y0
        };
        arr_set(font->glyph_texrects, i, texrect);
    }

    GLint max_texsize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texsize);

    while (1) {
        stbrp_context rpc;
        stbrp_init_target(&rpc, atlas_size, atlas_size, packing_nodes, m);
        stbrp_setup_allow_out_of_mem(&rpc, 1);
        if (!stbrp_pack_rects(&rpc, packing_rects, m)) {
            atlas_size += 128;
            if (atlas_size - max_texsize > 0) {
                atlas_size -= max_texsize;
                putlog("UI", "Warning: Reached system maximum texture size for font atlas.\n", 0);
                return 0;
            }
        }
        else break;
    }

    for (i = 0; i < m; i++) {
        font->glyph_texrects[i].x = packing_rects[i].x;
        font->glyph_texrects[i].y = packing_rects[i].y;
    }

    free(packing_rects);
    free(packing_nodes);

    font->glyph_atlas = new_texture(atlas_size, atlas_size, NULL, 1, (smooth ? (T_R | T_SMOOTH) : T_R));

    glyph_texrect_t *texrect;
    for (i = 1; i < m; i++) {
        texrect = &font->glyph_texrects[i];
        int w, h;
        unsigned char *bitmap = stbtt_GetCodepointBitmap(font->info, scale, scale, i + 32, &w, &h, NULL, NULL);
        if ((int)texrect->w != w) putlog("UI", "Warning: Inconsistancy with rendered width and stored width. ('%c' %d: S %d R %d)\n", 0, i + 32, i + 32, texrect->w, w);
        if ((int)texrect->h != h) putlog("UI", "Warning: Inconsistancy with rendered height and stored height. (S %d R %d)\n", 0, texrect->h, h);
        update_texture_area(font->glyph_atlas, texrect->x, texrect->y, texrect->w, texrect->h, bitmap, T_R);
        stbtt_FreeBitmap(bitmap, NULL);
    }

    if (mipmap) generate_texture_mipmap(font->glyph_atlas);

    return 1;
}

int prerender_sdf_bitmap_ttf(font_t *font, float size, int margin, bool mipmap) {
    const uint16_t m = 95;
    int32_t i, atlas_size = 256;
    stbrp_node *packing_nodes = malloc(sizeof(stbrp_node) * m);
    stbrp_rect *packing_rects = malloc(sizeof(stbrp_rect) * m);
    float scale = stbtt_ScaleForPixelHeight(font->info, size);
    font->margin = margin;

    int x0, y0, x1, y1;
    for (i = 0; i < m; i++) {
        stbtt_GetCodepointBitmapBox(font->info, i + 32, scale, scale, &x0, &y0, &x1, &y1);
        glyph_texrect_t texrect = {
            .w = packing_rects[i].w = (x1 - x0) + (margin * 2),
            .h = packing_rects[i].h = (y1 - y0) + (margin * 2)
        };
        arr_set(font->glyph_texrects, i, texrect);
    }

    GLint max_texsize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texsize);

    while (1) {
        stbrp_context rpc;
        stbrp_init_target(&rpc, atlas_size, atlas_size, packing_nodes, m);
        stbrp_setup_allow_out_of_mem(&rpc, 1);
        if (!stbrp_pack_rects(&rpc, packing_rects, m)) {
            atlas_size += 128;
            if (atlas_size - max_texsize > 0) {
                atlas_size -= max_texsize;
                putlog("UI", "Warning: Reached system maximum texture size for font atlas.\n", 0);
                return 0;
            }
        }
        else break;
    }

    for (i = 0; i < m; i++) {
        font->glyph_texrects[i].x = packing_rects[i].x;
        font->glyph_texrects[i].y = packing_rects[i].y;
    }

    free(packing_rects);
    free(packing_nodes);

    font->glyph_atlas = new_texture(atlas_size, atlas_size, NULL, 1, T_R | T_SMOOTH | (font->type == FT_SDF ? T_SDF : font->type == FT_MSDF ? T_SDF : 0));

    int w = 0, h = 0;
    glyph_texrect_t *texrect;
    for (i = 1; i < m; i++) {
        texrect = &font->glyph_texrects[i];

        unsigned char *bitmap = stbtt_GetCodepointBitmap(font->info, scale, scale, i + 32, &w, &h, NULL, NULL);
        uint32_t mx = w + margin * 2, my = h + margin * 2;
        double *img = malloc(sizeof(double) * mx * my);

        {
            uint32_t x,  y;

            for (x = 0; x < mx; x++)
                for (y = 0; y < margin; y++)
                    img[((my - 1) - y) * mx + x] = img[y * mx + x] = 0;

            for (y = 0; y < my; y++)
                for (x = 0; x < margin; x++)
                    img[y * mx + ((mx - 1) - x)] = img[y * mx + x] = 0;

            for (y = 0; y < h; y++)
                for (x = 0; x < w; x++)
                    img[(y + margin) * mx + (x + margin)] = bitmap[y * w + x] / 255.0;
        }

        stbtt_FreeBitmap(bitmap, NULL);

        int16_t *xdist = malloc(mx * my * sizeof(int16_t));
        int16_t *ydist = malloc(mx * my * sizeof(int16_t));
        double *gx = calloc(mx * my, sizeof(double));
        double *gy = calloc(mx * my, sizeof(double));
        double *outside = calloc(mx * my, sizeof(double));
        double *inside = calloc(mx * my, sizeof(double));

        double vmin = DBL_MAX;
        uint32_t j;

        computegradient(img, mx, my, gx, gy);
        edtaa3(img, gx, gy, mx, my, xdist, ydist, outside);
        for (j = 0; j < mx * my; ++j)
            if (outside[j] < 0.0)
                outside[j] = 0.0;

        memset(gx, 0, sizeof(double) * mx * my);
        memset(gy, 0, sizeof(double) * mx * my);

        for (j = 0; j < mx * my; ++j)
            img[j] = 1 - img[j];

        computegradient(img, mx, my, gx, gy);
        edtaa3(img, gx, gy, mx, my, xdist, ydist, inside);
        for (j = 0; j < mx * my; ++j)
            if (inside[j] < 0)
                inside[j] = 0.0;

        for (j = 0; j < mx * my; ++j) {
            outside[j] -= inside[j];
            if (outside[j] < vmin)
                vmin = outside[j];
        }

        vmin = fabs(vmin);

        for (j = 0; j < mx * my; ++j) {
            double v = outside[j];
            if (v < -vmin) outside[j] = -vmin;
            else if (v > +vmin) outside[j] = +vmin;
            img[j] = (outside[j] + vmin) / (2 * vmin);
        }

        free(xdist);
        free(ydist);
        free(gx);
        free(gy);
        free(outside);
        free(inside);

        unsigned char sdf[mx * my];
        for (j = 0; j < mx * my; ++j)
            sdf[j] = (unsigned char)(255 * (1 - img[j]));

        free(img);

        // unsigned char downscaled_sdf[texrect->w * texrect->h];
        // float sx = mx / (float)texrect->w;
        // float sy = my / (float)texrect->h;
        // for (int32_t y = 0; y < texrect->h; y++) {
        //     for (int32_t x = 0; x < texrect->w; x++) {
        //         downscaled_sdf[y * texrect->w + x] = sdf[(int)roundf(y * sy) * mx + (int)roundf(x * sx)];
        //     }
        // }
        update_texture_area(font->glyph_atlas, texrect->x, texrect->y, texrect->w, texrect->h, sdf, T_R);
        
    }

    if (mipmap) generate_texture_mipmap(font->glyph_atlas);

    return 1;
}

int16_t load_ttf(font_t *font, const char *path, bool mipmap) {
    font->data = read_file(path);
    font->glyph_texrects = arr_new(sizeof(glyph_texrect_t), 96, 8);
    font->info = malloc(sizeof(stbtt_fontinfo));
    stbtt_InitFont(font->info, (const unsigned char*)font->data, stbtt_GetFontOffsetForIndex((const unsigned char*)font->data, 0));
    switch (font->type) {
        case FT_BITMAP:
            if (!prerender_bitmap_ttf(font, font->glyph_height, true, mipmap))
                return 0;
            break;
        case FT_SDF:
           if (!prerender_sdf_bitmap_ttf(font, font->glyph_height, 2, mipmap))
               return 0;
          break;
        default: return 0;
    }
    return 1;
}

int16_t load_image_font(font_t *font, const char *path, bool mipmap) {
    font->glyph_atlas = new_texture(0, 0, NULL, 1, mipmap ? T_MIPMAP : 0);
    load_texture(font->glyph_atlas, path);
    font->xm = floorf(font->glyph_atlas->width / (float)font->glyph_width);
    font->_glyph.offset_x = 0;
    font->_glyph.offset_y = 0;
    font->_glyph.texrect.w = font->glyph_width;
    font->_glyph.texrect.h = font->glyph_height;
    return 1;
}

int16_t load_font(font_t *font, const char *path, bool mipmap) {
    char *dot = strchr(path, '.') + 1;

    if (!strcmp(dot, "ttf"))
        return load_ttf(font, path, mipmap);
    else if (!strcmp(dot, "png") || !strcmp(dot, "jpg") || !strcmp(dot, "jpeg"))
        return load_image_font(font, path, mipmap);

    return 0;
}

glyph_t* get_glyph(font_t *font, float size, int codepoint, int next_codepoint) {
    if (!font) return NULL;
    if (font->type == FT_BITMAP || font->type == FT_SDF) {
        get_glyph_metrics(font, size, codepoint, next_codepoint);
        if (codepoint < 32 || codepoint > 126) {
            font->_glyph.texrect.x
                = font->_glyph.texrect.y
                = font->_glyph.texrect.w
                = font->_glyph.texrect.h
                = 0;
            return &font->_glyph;
        }
        if (!font->glyph_texrects) {
            if (codepoint > ' ') {
                font->_glyph.texrect.x = ((codepoint - 33) % font->xm) * font->glyph_width + font->margin;
                font->_glyph.texrect.y = ((codepoint - 33) / font->xm) * font->glyph_height + font->margin;
            }
            return &font->_glyph;
        }
        // dynamic caching here
        font->_glyph.texrect = arr_get_as(font->glyph_texrects, codepoint - 32, glyph_texrect_t);
        font->_glyph.texrect.x += font->margin;
        font->_glyph.texrect.y += font->margin;
        font->_glyph.texrect.w -= font->margin;
        font->_glyph.texrect.h -= font->margin;
        return &font->_glyph;
    }
    return NULL;
}

vec2_t get_text_size(font_t *font, const char *text, uint32_t len, float size, int16_t spacing) {
    unsigned int i;
    vec2_t sz = vec2(0, 0);
    float h, w = 0, y = 0;
    if (font->glyph_texrects) {
        glyph_t *g;
        for (i = 0; i < len; i++) {
            g = get_glyph(font, size, text[i], (i + 1 < len) ? text[i + 1] : 0);
            w += g->advance + g->kern_advance + spacing;
            h = g->height + g->baseline + g->offset_y;
            if (text[i] == '\n') {
                y += g->line_height;
                w = 0;
            }
            if (sz.y < h + y) sz.y = MAX(h, size) + y;
            if (sz.x < w) sz.x = w;
        }
    }
    else {
        sz.x = len * (font->glyph_width + spacing);
        sz.y = font->glyph_height;
    }
    return sz;
}

vec2_t draw_text_ext(font_t *font, const char *text, uint32_t len, float size, int16_t spacing, vec2_t origin, vec2_t position, float depth, vec2_t scale, float rotation, color_t color) {
    float x = 0, y = 0;
    glyph_t *g;
    vec2_t offset = {0};
    vec2_t sz = get_text_size(font, text, len, size, spacing);

    if (origin.x || origin.y) {
        offset.x = sz.x * origin.x;
        offset.y = sz.y * origin.y;
    }

    for (uint32_t i = 0; i < len; i++) {
        g = get_glyph(font, size, text[i], (i + 1 < len) ? text[i + 1] : 0);

        if (text[i] == '\n') {
            y += g->line_height;
            x = 0;
        }

        if ((unsigned char)text[i] > ' ') {
            mat3_t m = mat3_from_translation(position);
            if (rotation) m = mat3_rotate(m, torad(rotation));
            m = mat3_translate(m, vec2((g->lsb + x) - offset.x, g->baseline + g->offset_y + y - offset.y));
            m = mat3_scale(m, vec2(g->width * scale.x, g->height * scale.y));
            draw_2d(quad_polygon, m, depth, g->width, g->height, font->glyph_atlas, texrect(g->texrect.x, g->texrect.y, g->texrect.w, g->texrect.h, 0), color);
        }
        
        x += g->advance + g->kern_advance + spacing;
    }

    return sz;
}