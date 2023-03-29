#include "game/character.h"
#include "ae/draw.h"
#include "game/level.h"

#define CH_BODY_W 16
#define CH_BODY_H 12
#define CH_BODY_X 0
#define CH_BODY_Y 0

#define CH_HEAD_W 16
#define CH_HEAD_H 16
#define CH_HEAD_X 0
#define CH_HEAD_Y 48

#define CH_HAND_W 4
#define CH_HAND_H 4
#define CH_HAND_X 92
#define CH_HAND_Y 0

#define CH_BODY_COLUMNS 5
#define CH_HEAD_COLUMNS 6

static i8 head_offset_y[] = {
    0, -1,  0, -1,  0,
    0,  0, -1,  0, -1,
    0,  0, -1,  0, -1,
    0,  0, -1,  0, -1
};

static struct { u8 frame; u8 mirror; } head_direction_info[] = {
    [2 ... 3] = { 1, 0 },
    [4]       = { 0, 0 },
    [5 ... 6] = { 1, 1 },
    [7]       = { 2, 1 },
    [0]       = { 3, 0 },
    [1]       = { 2, 0 },
};

static struct { u8 lx; u8 ly; u8 lf; u8 rx; u8 ry; u8 rf; } hand_frame_info[] = {
    { 2, 4, 0, /**/ 12, 4, 1 }, { 2, 2, 6, /**/ 11, 2, 1 }, { 2, 4, 0, /**/ 12, 4, 6 }, { 3, 2, 0, /**/ 12, 2, 6 }, { 2, 4, 6, /**/ 12, 4, 1 },
    { 2, 4, 6, /**/ 12, 4, 6 }, { 2, 4, 6, /**/ 12, 4, 6 }, { 2, 2, 6, /**/ 11, 2, 1 }, { 2, 4, 6, /**/ 12, 4, 6 }, { 3, 2, 0, /**/ 12, 2, 6 },
    { 2, 4, 0, /**/ 11, 4, 6 }, { 6, 3, 0, /**/  6, 2, 6 }, { 3, 4, 0, /**/  6, 2, 6 }, { 2, 4, 0, /**/ 12, 3, 6 }, { 4, 4, 0, /**/ 10, 4, 6 },
    { 2, 4, 6, /**/ 11, 4, 5 }, { 2, 4, 6, /**/ 12, 3, 5 }, { 6, 3, 6, /**/ 11, 4, 5 }, { 6, 3, 6, /**/  9, 4, 5 }, { 3, 4, 6, /**/ 11, 4, 5 },
};

static inline void draw_character_hand(character_t *ch, u8 x, u8 y, u8 frame, u8 mirror) {
    if (mirror && frame != 6) {
        if (frame % 2 == 0) frame += 1;
        else frame -= 1;
    }
    draw_texture_ext(
        vec2(0, 0),
        vec2(
            mirror ? (ch->position.x + (CH_BODY_W / 2 - 3) - x) : (ch->position.x - (CH_BODY_W / 2 + 1) + x),
            ch->position.y + y
        ),
        get_depth(ch->position.y + (frame == 6 ? -1 : 1)),
        CH_HAND_W, CH_HAND_H,
        vec2(1, 1),
        0, C_WHITE, sprites,
        texrect(CH_HAND_X, CH_HAND_H * frame + CH_HAND_Y, CH_HAND_W, CH_HAND_H, 0)
    );
}

void draw_character(character_t *ch) {
    // draw body
    draw_texture_ext(
        vec2(0.5f, 0),
        vec2(ch->draw_position.x, ch->draw_position.y),
        get_depth(ch->draw_position.y),
        CH_BODY_W, CH_BODY_H,
        vec2(ch->mirror_body ? -1 : 1, 1),
        0, C_WHITE, sprites,
        texrect(CH_BODY_W * (ch->body_frame % CH_BODY_COLUMNS) + CH_BODY_X, CH_BODY_H * (ch->body_frame / CH_BODY_COLUMNS) + CH_BODY_Y, CH_BODY_W, CH_BODY_H, 0)
    );

    if (ch->blink < 0 && ch->head_frame < 2)
        ch->head_frame += 4;

    draw_texture_ext(
        vec2(0.5f, 1),
        vec2(ch->draw_position.x, ch->draw_position.y + head_offset_y[ch->body_frame] + 4),
        get_depth(ch->draw_position.y),
        CH_HEAD_W, CH_HEAD_H,
        vec2(ch->mirror_head ? -1 : 1, 1),
        0, C_WHITE, sprites,
        texrect(CH_HEAD_W * (ch->head_frame % CH_HEAD_COLUMNS) + CH_HEAD_X, CH_HEAD_Y, CH_HEAD_W, CH_HEAD_H, 0)
    );

    if (ch->tool == NULL) {
        draw_character_hand(ch, hand_frame_info[ch->body_frame].lx, hand_frame_info[ch->body_frame].ly, hand_frame_info[ch->body_frame].lf, ch->mirror_body);
        draw_character_hand(ch, hand_frame_info[ch->body_frame].rx, hand_frame_info[ch->body_frame].ry, hand_frame_info[ch->body_frame].rf, ch->mirror_body);
    }
    else {
        vec2 pos = add2(ch->draw_position, vec2_ldd(ch->tool->distance, ch->aim_direction));
        u8 flip_x = ch->aim_direction > 90 && ch->aim_direction < 271;
        u8 flip_y = ch->aim_direction > 0 && ch->aim_direction < 180;
        float aim_q = fmodf(ch->aim_direction / 90, 1);
        if (flip_x) aim_q = 1 - aim_q;
        if (!flip_y) aim_q = 1 - aim_q;
        draw_texture_ext(
            vec2(0, 0), pos, get_depth(pos.y), ch->tool->texrect.width, ch->tool->texrect.height, vec2(1, flip_x ? -1 : 1), ch->aim_direction, C_WHITE, sprites,
            texrect(
                ch->tool->texrect.x,
                ch->tool->texrect.y + ((u8)roundf(lerpf(1, 4, aim_q)) * ch->tool->texrect.height),
                ch->tool->texrect.width,
                ch->tool->texrect.height,
                0
            )
        );
        mat3 m = mat3_from_translation(ch->draw_position);
        m = mat3_rotate(m, torad(ch->aim_direction));
        m = mat3_translate(m, vec2(ch->tool->distance, 0));
        mat3 m2;
        if (flip_x) m2 = mat3_translate(m, vec2(ch->tool->right_hand.x, -ch->tool->right_hand.y));
        else m2 = mat3_translate(m, ch->tool->right_hand);
        draw_texture_ext(
            vec2(0.5f, 0.5f),
            vec2(m2.m31, m2.m32),
            get_depth(pos.y - 1),
            CH_HAND_W, CH_HAND_H,
            vec2(1, 1),
            0, C_WHITE, sprites,
            texrect(CH_HAND_X, 0, CH_HAND_W, CH_HAND_H, 0)
        );
        if (flip_x) m2 = mat3_translate(m, vec2(ch->tool->left_hand.x, -ch->tool->left_hand.y));
        else m2 = mat3_translate(m, ch->tool->left_hand);
        draw_texture_ext(
            vec2(0.5f, 0.5f),
            vec2(m2.m31, m2.m32),
            get_depth(pos.y + 1),
            CH_HAND_W, CH_HAND_H,
            vec2(1, 1),
            0, C_WHITE, sprites,
            texrect(CH_HAND_X, 0, CH_HAND_W, CH_HAND_H, 0)
        );
    }
    
    // if (debug_options & SHOW_PLAYER) {
    //     vec2 p = vec2_scale(debug_player_tile_h, TILE_SIZE);
    //     draw_texture(vec2(0, 0), p, get_depth(p.y) - 1, TILE_SIZE, TILE_SIZE * 2, C_BLUE, sprites, texrect(80, 0, TILE_SIZE, TILE_SIZE * 2, 0));
    //     draw_texture(vec2(0, 0), vec2(p.x + 1, p.y + 10), get_depth(p.y) + 1, TILE_SIZE - 2, 1, C_BLUE, sprites, texrect(81, 0, TILE_SIZE - 2, 1, 0));
    //     p = vec2_scale(debug_player_tile_v, TILE_SIZE);
    //     draw_texture(vec2(0, 0), p, get_depth(p.y) - 1, TILE_SIZE, TILE_SIZE * 2, C_RED, sprites, texrect(80, 0, TILE_SIZE, TILE_SIZE * 2, 0));
    //     draw_texture(vec2(0, 0), vec2(p.x + 1, p.y + 10), get_depth(p.y) + 1, TILE_SIZE - 2, 1, C_RED, sprites, texrect(81, 0, TILE_SIZE - 2, 1, 0));
    // }
}

void update_character(character_t *ch) {
    //ch->look_direction_8 = (u16)roundf(ch->look_direction / 45.0f) % 8;

    ch->blink += 1;
    if (ch->blink >= CH_BLINK_TIME)
        ch->blink = -7;
}

static void draw_player_weapon_box(vec2 position, u8 width, u8 active, float scale, tool_t *tool) {
    draw_texture_ext(vec2(0, 1), vec2(position.x * scale, window->height - (position.y * scale)), -98, 6, 12, vec2(scale, scale), 0, C_WHITE, sprites, texrect(208, 12 * active + 64, 6, 12, 0));
    if (width) draw_texture_ext(vec2(0, 1), vec2((position.x + 6) * scale, window->height - (position.y * scale)), -98, width, 12, vec2(scale, scale), 0, C_WHITE, sprites, texrect(213, 12 * active + 64, 2, 12, 0));
    draw_texture_ext(vec2(0, 1), vec2((position.x + 6 + width) * scale, window->height - (position.y * scale)), -98, 6, 12, vec2(scale, scale), 0, C_WHITE, sprites, texrect(214, 12 * active + 64, 6, 12, 0));
    if (tool) {
        draw_texture_ext(
            vec2(0, 1),
            vec2(
                ((position.x + (12 + width) / 2.0f) - (tool->texrect.width / 2.0f)) * scale,
                window->height - ((position.y + 4) * scale)
            ),
            -99,
            tool->texrect.width, tool->texrect.height,
            vec2(scale, scale), 0,
            active ? color(0xc8c8c8ff) : color(0x1b1b1bff),
            sprites, tool->texrect
        );
    }
}

void draw_player_ui(player_t *player, float scale) {
    draw_player_weapon_box(vec2(1, 0), 6, player->selected_tool_slot == 1, scale, player->tools[0]);
    draw_player_weapon_box(vec2(20, 0), 12, player->selected_tool_slot == 2, scale, player->tools[1]);
    if (player->tool_name)
        draw_text_ext(
            small_font,
            player->tool_name, player->tool_name_len,
            6, 6 * scale,
            vec2(0, 1),
            vec2(2 * scale, window->height - (12 * scale) - player->tool_name_slide * scale),
            -99, vec2(scale, scale), 0,
            rgba(0, 0, 0, player->tool_name_slide / (float)CH_WEAPON_SLIDE)
        );
    if (player->previous_tool_name)
        draw_text_ext(
            small_font,
            player->previous_tool_name, player->previous_tool_name_len,
            6, 6 * scale,
            vec2(0, 1),
            vec2(2 * scale, window->height - (12 * scale) - (player->tool_name_slide + CH_WEAPON_SLIDE) * scale),
            -99, vec2(scale, scale), 0,
            rgba(0, 0, 0, 1 - player->tool_name_slide / (float)CH_WEAPON_SLIDE)
        );
}

static void select_weapon(player_t *player, u8 slot) {
    if (slot > CH_WEAPON_COUNT) return;
    player->previous_selected_tool_slot = player->selected_tool_slot;
    player->selected_tool_slot = slot;
    player->previous_tool_name = player->tool_name;
    player->previous_tool_name_len = player->tool_name_len;
    player->tool_name_slide = 0;
    if (slot > 0) {
        player->character->tool = player->tools[slot - 1];
        if (player->character->tool != NULL) {
            player->tool_name = (char*)player->character->tool->name;
            player->tool_name_len = strlen(player->character->tool->name);
        }
        else {
            player->tool_name = NULL;
            player->tool_name_len = 0;
        }
    }
    else {
        player->character->tool = 0;
        player->tool_name = NULL;
        player->tool_name_len = 0;
    }
}

static void on_player_key_press(void *userdata, int key, int scancode, int mods) {
    // TODO: These events are firing even while paused, which is not something that can easily be fixed.
    // So we'll have to move both of these into player_update(...)

    player_t *player = userdata;

    if (key >= KEY_1 && key < KEY_1 + CH_WEAPON_COUNT) {
        if (player->selected_tool_slot != (key - KEY_1) + 1) {
            select_weapon(player, (key - KEY_1) + 1);
        }
        else select_weapon(player, 0);
    }
    else if (key == KEY_Q) {
        select_weapon(player, player->previous_selected_tool_slot);
    }
}

static void on_player_mouse_scroll(void *userdata, double x, double y) {
    player_t *player = userdata;

    if (y > 0) {
        if (player->selected_tool_slot < CH_WEAPON_COUNT)
            select_weapon(player, player->selected_tool_slot + 1);
    }
    else if (y < 0) {
        if (player->selected_tool_slot > 0)
            select_weapon(player, player->selected_tool_slot - 1);
    }
}

player_t* new_player(character_t *c, window_t *window) {
    player_t *player = calloc(1, sizeof(player_t));
    player->character = c;
    player->key_press_event = handle_event(window, EV_KEY_PRESS, on_player_key_press, player);
    player->mouse_wheel_event = handle_event(window, EV_MOUSE_SCROLL, on_player_mouse_scroll, player);
    return player;
}

void delete_player(player_t **p) {
    player_t *player = *p;
    unhandle_event(window, player->key_press_event);
    unhandle_event(window, player->mouse_wheel_event);
    free(*p);
    *p = NULL;
}

static inline u8 get_body_frame_from_direction(u8 d) {
    return (((d > 1 && d < 4) || (d > 4 && d < 7)) ? 10 : ((d > 0 && d < 2) || d > 6) ? 15 : (d > 7 || d < 1) ? 5 : 0);
}

void update_player(player_t *player) {

    static bool moving;
    static float move_speed = 0;
    static vec2 velocity_normal;

    player->velocity = vec2(0, 0);
    character_t *ch = player->character;

    moving = false;

    if (player->tool_name_slide < CH_WEAPON_SLIDE)
        ++player->tool_name_slide;

    if (get_key(KEY_W)) {
        player->velocity.y = -1;
        move_speed = CH_MOVE_SPEED;
        moving = true;
    }

    if (get_key(KEY_S)) {
        player->velocity.y = 1;
        move_speed = CH_MOVE_SPEED;
        moving = true;
    }

    if (get_key(KEY_A)) {
        player->velocity.x = -1;
        move_speed = CH_MOVE_SPEED;
        moving = true;
    }

    if (get_key(KEY_D)) {
        player->velocity.x = 1;
        move_speed = CH_MOVE_SPEED;
        moving = true;
    }

    player->move_direction = todeg(vec2_angle(vec2(0, 0), player->velocity)) - 90;

    if (ch->tool == NULL) {
        if (moving) player->look_direction = player->move_direction;
    }
    else {
        player->look_direction = ch->aim_direction + 90;
    }

    player->look_direction_8 = (u16)roundf(player->look_direction / 45.0f) % 8;

    if (moving) {
        player->animation_index += move_speed / 6.0f;

        if (player->animation_index >= 4)
            player->animation_index -= 4;

        if (player->character->tool == NULL) {
            if (player->velocity.x) {
                if (player->velocity.y < 0) {
                    ch->body_frame = player->animation_index + 16;
                }
                else {
                    ch->body_frame = player->animation_index + 11;
                }
            }
            else {
                if (player->velocity.y < 0) {
                    ch->body_frame = player->animation_index + 6;
                }
                else {
                    ch->body_frame = player->animation_index + 1;
                }
            }

            ch->mirror_body = player->velocity.x < 0;
        }
        else {
            ch->body_frame = get_body_frame_from_direction(player->look_direction_8);
            u8 move_direction_8 = (u16)roundf(player->move_direction / 45.0f) % 8;
            i8 dd = abs(move_direction_8 - player->look_direction_8);
            if (player->look_direction_8 == 0 && move_direction_8 > 4)
                dd = 8 - dd;
            if (dd < 3) ch->body_frame += player->animation_index + 1;
            else ch->body_frame += (4 - player->animation_index) + 1;
            ch->mirror_body = head_direction_info[player->look_direction_8].mirror;
        }
    }
    else {
        move_speed = 0;
        ch->body_frame = get_body_frame_from_direction(player->look_direction_8);
        ch->mirror_body = head_direction_info[player->look_direction_8].mirror;
    }

    ch->head_frame = head_direction_info[player->look_direction_8].frame;
    ch->mirror_head = head_direction_info[player->look_direction_8].mirror;

    velocity_normal = vec2_normalize(player->velocity);
    player->velocity = vec2_scale(velocity_normal, move_speed);
    
    {
        player->tile_h = vec2(floorf((ch->position.x + (velocity_normal.x * 4) + player->velocity.x) / TILE_SIZE), floorf(ch->position.y / TILE_SIZE));
        player->tile_v = vec2(floorf(ch->position.x / TILE_SIZE), floorf((ch->position.y + (velocity_normal.y * 2) + player->velocity.y) / TILE_SIZE));
        tile_t *t = get_tile(current_level, player->tile_h.x, player->tile_h.y);
        if (t->id) player->velocity.x = 0;
        t = get_tile(current_level, player->tile_v.x, player->tile_v.y);
        if (t->id) player->velocity.y = 0;
    }

    if (player->character->tool != NULL) {
        player->character->aim_direction = todeg(vec2_angle(player->character->position, game_cursor)) + 180;
    }

    ch->position = add2(ch->position, player->velocity);
    ch->draw_position = interpolate_position(ch->position, player->velocity);
    game_surface->view.position = ch->draw_position;
}

#undef CH_BODY_W
#undef CH_BODY_H
#undef CH_HEAD_W
#undef CH_HEAD_H

#undef CH_BODY_COLUMNS
#undef CH_HEAD_COLUMNS