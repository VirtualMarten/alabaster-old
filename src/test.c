#include "ae/array.h"
#include "ae/str.h"
#include "ae/shader.h"
#include "ae/surface.h"
#include "ae/audio.h"
#include "ae/text.h"
#include "ae/utility.h"
#include "ae/vector.h"
#include "ae/matrix.h"
#include "ae/color.h"
#include "ae/image.h"
#include "ae/window.h"

#include <stdio.h>
#include <stdint.h>

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

static window_t *window;
static shader_t *shader;
static texture_t *texture;
static surface_t *surface;
static font_t *font;
static audio_channel_t *sfx;

void on_window_close(void *userdata, window_t *w) {
    close_window(w);
}

void on_key_press(void *userdata, int key, int scancode, int mods) {
    if (key == KEY_R && (mods & KEY_MOD_CONTROL)) {
        printf("Reloading Shader... ");
        if (!load_shader(shader, "shader.glsl")) {
            printf("Failed.\n");
        }
        printf("Done.\n");
    }
}

/*     ::::::::  .::::::.  :::::::.  .::::::.
 *        ::     ::    ::  ::    ::  ::    ::   
 *        ::     ::    ::  ::    ::  ::    ::
 *        ::     '::::::'  :::::::'  '::::::'
 * 
 * 
 *    ☑  Font Loading & Text Drawing
 *
 *    ☑  Audio
 * 
 *    ☑  Embed vertex_buffer_t into polygon rendering
 * 
 *    ☑  Removed reduntant mask from load_texture(...)
 * 
 *    ☑  Make window surface resize on window resize
 * 
 *    ☑  Optimize GL state changes on render
 * 
 */

static void on_window_resize(void *user_data, window_t *w, u32 width, u32 height) {
    (void)user_data;
    (void)w;
    surface->view.width = width;
    surface->view.height = height;
}

int main(void) {
    window = new_window(640, 480, "test");
    handle_event(window, EV_WINDOW_CLOSE, on_window_close, NULL);
    handle_event(window, EV_KEY_PRESS, on_key_press, NULL);
    handle_event(window, EV_WINDOW_RESIZE, on_window_resize, NULL);
    shader = new_shader();
    if (!load_shader(shader, "shader.glsl")) {
        fprintf(stderr, "Failed to compile shader.\n");
        return EXIT_FAILURE;
    }
    texture = new_texture(0, 0, NULL, 0, 0);
    load_texture(texture, "test.png");
    font = new_font(FT_SDF, 64, 0);
    load_font(font, "fonts/LiberationSans-Regular.ttf", false);
    vec2 position = vec2(0, 0);
    float rotation = 0;
    surface = new_window_surface(window, shader, C_GRAY);
    sfx = new_audio_channel();
    audio_sample_t *sample = new_audio_sample(NULL, 0, 0);
    load_audio_sample(sample, "laser.wav");
    audio_play(sfx, sample, 1, 1, 1);
    float text_size = 100;
    while (refresh_window(window)) {
        if (get_key(KEY_LEFT)) position.x -= 1;
        if (get_key(KEY_RIGHT)) position.x += 1;
        if (get_key(KEY_UP)) position.y -= 1;
        if (get_key(KEY_DOWN)) position.y += 1;
        if (get_key(KEY_O)) rotation -= 1;
        if (get_key(KEY_P)) rotation += 1;
        if (get_key(KEY_MINUS)) text_size -= 1.0f;
        if (get_key(KEY_EQUAL)) text_size += 1.0f;
        //draw_textured_polygon(quad_polygon, vec2(0, 0), vec2(-surface->view.width / 2, -surface->view.height / 2), 100, font->glyph_atlas->width, font->glyph_atlas->height, C_WHITE, font->glyph_atlas, texrect(0, 0, font->glyph_atlas->width, font->glyph_atlas->height, 0));
        draw_polygon_ext(quad_polygon, vec2(0.5f, 0.5f), vec2(position.x + 4, position.y + 4), 0, 128, 128, vec2(2, 2), 0, C_BLACK);
        draw_textured_polygon_ext(quad_polygon, vec2(0.5f, 0.5f), position, -10, 128, 128, vec2(2, 2), 0, C_WHITE, texture, texrect(0, 0, 16, 16, T_WRAP));
        if (get_key(KEY_A)) surface->view.position.x -= 1;
        if (get_key(KEY_D)) surface->view.position.x += 1;
        if (get_key(KEY_W)) surface->view.position.y -= 1;
        if (get_key(KEY_S)) surface->view.position.y += 1;
        if (get_key(KEY_Q)) surface->view.rotation -= 0.01f;
        if (get_key(KEY_E)) surface->view.rotation += 0.01f;
        draw_text_ext(font, "Hello, World!", 13, floorf(text_size), 0, vec2(0.5f, 0.5f), vec2(0, 0), -11, vec2(1, 1), rotation, C_BLUE);
        render_surface(surface);
    }
    delete_window(&window);
    return EXIT_SUCCESS;
}