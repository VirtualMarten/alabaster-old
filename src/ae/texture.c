#include "ae/texture.h"
#include "ae/libs/stb_image.h"

static inline GLenum texmask_to_gl_pixel_format(uint16_t mask) {
    return ((mask & T_RGBA) ? GL_RGBA : (mask & T_RGB) ? GL_RGB : (mask & T_RG) ? GL_RG : (mask & T_R) ? GL_RED : 0);
}

static inline uint8_t texmask_to_color_components(uint16_t mask) {
    return ((mask & T_RGBA) ? 4 : (mask & T_RGB) ? 3 : (mask & T_RG) ? 2 : (mask & T_R) ? 1 : 0);
}

static inline uint16_t texmask_from_color_components(uint8_t components) {
    switch (components) {
        case 1: return T_R;
        case 2: return T_RG;
        case 3: return T_RGB;
        case 4: return T_RGBA;
        default: return 0;
    }
    return 0;
}

static inline bool is_opaque(uint8_t *rgba_data, uint16_t width, uint16_t height) {
    unsigned int i, s = width * height;
    for (i = 0; i < s; i++)
        if (rgba_data[i * 4 + 3] < 255 && rgba_data[i * 4 + 3] > 0)
            return false;
    return true;
}

texture_t* new_texture(uint16_t width, uint16_t height, uint8_t *data, uint16_t unit_n, uint16_t mask) {
    GLint unit_max;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &unit_max);
    if (unit_n >= unit_max) return NULL;
    texture_t *t = calloc(1, sizeof(texture_t));
    t->mask = mask;
    if (mask & T_MSDF) t->mask &= ~T_SDF;
    glGenTextures(1, (GLuint*)&t->id);
    glBindTexture(GL_TEXTURE_2D, t->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (mask & T_SMOOTH) ?  GL_LINEAR : GL_NEAREST);
    if (mask & T_MIPMAP)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (mask & T_SMOOTH) ?  GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
    else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (mask & T_SMOOTH) ?  GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (mask & T_WRAP_X) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (mask & T_WRAP_Y) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    if (width && height && ((mask & T_R) + (mask & T_RG) + (mask & T_RGB) + (mask & T_RGBA) > 0))
        update_texture(t, width, height, data, mask);
    return t;
}

texture_t* load_texture(texture_t *t, const char *path) {
    if (!(t && path)) return NULL;
    int width, height, components_in_file;
    uint8_t components = texmask_to_color_components(t->mask);
    stbi_uc *data = stbi_load(path, &width, &height, &components_in_file, components);
    if (!data) return NULL;
    if (!components) t->mask |= texmask_from_color_components(components_in_file);
    update_texture(t, width, height, data, t->mask);
    stbi_image_free(data);
    return t;
}

texture_t* update_texture(texture_t *t, uint16_t width, uint16_t height, uint8_t *data, uint16_t mask) {
    if (!t) return NULL;
    if ((t->mask & T_R) + (t->mask & T_RG) + (t->mask & T_RGB) + (t->mask & T_RGBA) == 0) {
        // If the texture has no existing color format, then we need to add the input color format to the mask.
        if ((mask & T_RGBA)) t->mask |= T_RGBA;
        else if ((mask & T_RGB)) t->mask |= T_RGB;
        else if ((mask & T_RG)) t->mask |= T_RG;
        else if ((mask & T_R)) t->mask |= T_R;
    }
    t->width = width;
    t->height = height;
    glBindTexture(GL_TEXTURE_2D, t->id);
    glPixelStorei(GL_PACK_ALIGNMENT, (t->mask & T_PADDED) ? 4 : 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, (mask & T_PADDED) ? 4 : 1);
    glTexImage2D(GL_TEXTURE_2D, 0, texmask_to_gl_pixel_format(t->mask), t->width, t->height, 0, texmask_to_gl_pixel_format(mask), GL_UNSIGNED_BYTE, data);
    t->mask |= T_OPAQUE;
    if ((t->mask & T_RGBA) && data && !is_opaque(data, width, height))
        t->mask &= ~T_OPAQUE;
    return t;
}

texture_t* update_texture_area(texture_t *t, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, uint16_t mask) {
    if (!(t && data)) return NULL;

    // if (w < 1 && t->width) w = t->width - w;
    // if (h < 1 && t->height) h = t->height - h;
    // if (!(w && h)) return false;

    // if (x < t->width) x += t->width;
    // if (y < t->height) y += t->height;
    // x %= t->width;
    // y %= t->height;

    glBindTexture(GL_TEXTURE_2D, t->id);
    glPixelStorei(GL_PACK_ALIGNMENT, (t->mask & T_PADDED) ? 4 : 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, (mask & T_PADDED) ? 4 : 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, texmask_to_gl_pixel_format(mask), GL_UNSIGNED_BYTE, data);

    t->mask |= T_OPAQUE;
    if ((t->mask & T_RGBA) && !is_opaque(data, t->width, t->height))
        t->mask &= ~T_OPAQUE;

    return t;
}

texture_t* set_texture_smooth(texture_t *t, bool smooth) {
    if (!t) return NULL;
    if (smooth) t->mask |= T_SMOOTH;
    else t->mask &= ~T_SMOOTH;
    glBindTexture(GL_TEXTURE_2D, t->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (t->mask & T_SMOOTH) ? GL_NEAREST : GL_LINEAR);
    if (t->mask & T_MIPMAP)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (t->mask & T_SMOOTH) ?  GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (t->mask & T_SMOOTH) ?  GL_LINEAR : GL_NEAREST);
    return t;
}

texture_t* set_texture_wrap(texture_t *t, bool wrap_x, bool wrap_y) {
    if (!t) return NULL;
    if (wrap_x) t->mask |= T_WRAP_X;
    else t->mask &= ~T_WRAP_X;
    if (wrap_y) t->mask |= T_WRAP_Y;
    else t->mask &= ~T_WRAP_Y;
    glBindTexture(GL_TEXTURE_2D, t->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (t->mask & T_WRAP_X) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (t->mask & T_WRAP_Y) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    return t;
}

texture_t* generate_texture_mipmap(texture_t *t) {
    if (!(t->mask & T_MIPMAP)) {
        t->mask |= T_MIPMAP;
        set_texture_smooth(t, !(t->mask & T_SMOOTH));
    }
    glEnable(GL_TEXTURE_2D);
    glGenerateTextureMipmap(t->id);
    return t;
}