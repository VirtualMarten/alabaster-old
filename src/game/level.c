#include "game/level.h"
#include "ae/surface.h"
#include "ae/draw.h"

level_t *current_level = NULL;

level_t* new_level(u16 width, u16 height, vec2 position) {
    level_t *l = malloc(sizeof(level_t));
    l->tiles = calloc(width * height, sizeof(tile_t));
    l->width = width;
    l->height = height;
    l->position = position;
    l->surface = new_surface(shader, width * TILE_SIZE, height * TILE_SIZE, color(0xd700ffff), T_RGB);
    l->surface->view.position = vec2(l->surface->width / 2, l->surface->height / 2);
    return l;
}

void render_level(level_t *l) {
    static u8 tx;
    static tile_t t;
    for (u16 y = 0; y < l->height; y++) {
        for (u16 x = 0; x < l->width; x++) {
            t = l->tiles[y * l->width + x];
            if (t.id && t.state < 8) {
                tx = t.state / 2;
                if (tx) tx = (tx + 1) % 3;
                tx *= TILE_SIZE;
                draw_texture(vec2(0, 0), vec2(x * TILE_SIZE, y * TILE_SIZE), 0, TILE_SIZE, TILE_SIZE, C_WHITE, sprites, texrect(
                    96 + (TILE_SIZE * 4) * (t.id - 1) + tx,
                    TILE_SIZE * 4,
                    TILE_SIZE, TILE_SIZE, 0
                ));
            }
        }
    }
    render_surface(l->surface);
}

static u8 tile_state_table[] = {
    0, 12, 3, 15,
    1, 13, 2, 14,
    4,  8, 7, 11,
    5,  9, 6, 10
};

static u16 tile_state_tx[16];
static u16 tile_state_ty[16];

void init_tile_states() {
    for (u8 i = 0; i < 16; i++)
        tile_state_tx[i] = (tile_state_table[i] % 4) * TILE_SIZE;
    for (u8 i = 0; i < 16; i++)
        tile_state_ty[i] = (tile_state_table[i] / 4) * TILE_SIZE;
}

void draw_level(level_t *l) {
    static float ty;
    static i8 tx;
    static tile_t t;
    for (u16 y = 0; y < l->height; y++) {
        for (u16 x = 0; x < l->width; x++) {
            ty = l->position.y + (y * TILE_SIZE);
            t = l->tiles[y * l->width + x];
            if (!t.id) continue;
            draw_texture(vec2(0, 0), vec2(l->position.x + x * TILE_SIZE, ty), get_depth(ty), TILE_SIZE, TILE_SIZE, C_WHITE, sprites, texrect(
               112 + (TILE_SIZE * 4) * (t.id - 1) + tile_state_tx[t.state],
               tile_state_ty[t.state],
               TILE_SIZE, TILE_SIZE, 0
            ));
            if (t.state < 8) {
                tx = t.state / 2;
                if (tx == 1) tx = 3;
                else if (tx > 1) tx -= 1;
                tx *= TILE_SIZE;
                draw_texture(vec2(0, -1), vec2(x * TILE_SIZE, y * TILE_SIZE), 999, TILE_SIZE, TILE_SIZE, C_WHITE, sprites, texrect(
                    112 + (TILE_SIZE * 4) * (t.id - 1) + tx,
                    TILE_SIZE * 4,
                    TILE_SIZE, TILE_SIZE, 0
                ));
            }
        }
    }
    // if ((debug_options & SHOW_TILE) && vec2_contains(game_cursor, l->position, l->width * TILE_SIZE, l->height * TILE_SIZE)) {
    //     debug_tile_position = vec2_floor(div2(game_cursor, vec2(TILE_SIZE, TILE_SIZE)));
    //     debug_tile = l->tiles[(u16)debug_tile_position.y * l->width + (u16)debug_tile_position.x];
    //     vec2 p = vec2_scale(debug_tile_position, TILE_SIZE);
    //     draw_texture(vec2(0, 0), p, get_depth(p.y) - 1, TILE_SIZE, TILE_SIZE * 2, C_LIME, sprites, texrect(80, 0, TILE_SIZE, TILE_SIZE * 2, 0));
    //     draw_texture(vec2(0, 0), vec2(p.x + 1, p.y + 10), get_depth(p.y) + 1, TILE_SIZE - 2, 1, C_LIME, sprites, texrect(81, 0, TILE_SIZE - 2, 1, 0));
    // }
    // else debug_tile = (tile_t){0};
}

#define TILE_N (1 << 0)
#define TILE_W (1 << 1)
#define TILE_E (1 << 2)
#define TILE_S (1 << 3)

tile_t* get_tile(level_t *l, i32 x, i32 y) {
    static tile_t nt = {0};
    if (x >= 0 && y >= 0 && x < l->width && y < l->height)
        return &l->tiles[y * l->width + x];
    return &nt;
}

static void update_tile_state(level_t *l, u16 x, u16 y, bool rec) {
    u8 mask = 0;
    if (get_tile(l, x, y - 1)->id) {
        mask |= TILE_N;
        if (rec) update_tile_state(l, x, y - 1, false);
    }
    if (get_tile(l, x, y + 1)->id) {
        mask |= TILE_S;
        if (rec) update_tile_state(l, x, y + 1, false);
    }
    if (get_tile(l, x - 1, y)->id) {
        mask |= TILE_W;
        if (rec) update_tile_state(l, x - 1, y, false);
    }
    if (get_tile(l, x + 1, y)->id) {
        mask |= TILE_E;
        if (rec) update_tile_state(l, x + 1, y, false);
    }
    get_tile(l, x, y)->state = mask;
}

void set_tile(level_t *l, u16 x, u16 y, u8 id) {
    l->tiles[y * l->width + x].id = id;
    update_tile_state(l, x, y, true);
}