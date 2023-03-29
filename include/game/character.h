#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "game/base.h"
#include "ae/vector.h"
#include "ae/texture.h"

#define CH_BLINK_TIME 240
#define CH_MOVE_SPEED 1
#define CH_WEAPON_COUNT 2
#define CH_WEAPON_SLIDE 4

typedef struct {
    const char *name;
    texrect_t texrect;
    u8 distance;
    vec2 left_hand, right_hand;
} tool_t;

typedef struct {
    vec2 position, draw_position;
    u8 head_frame, body_frame;
    u8 mirror_head, mirror_body;
    float aim_direction;
    i16 blink;
    tool_t *tool;
} character_t;

typedef struct {
    character_t *character;
    float look_direction;
    u8 look_direction_8;
    float move_direction;
    float animation_index;
    vec2 velocity;
    vec2 tile_h, tile_v;
    u8 selected_tool_slot, previous_selected_tool_slot;
    tool_t *tools[CH_WEAPON_COUNT];
    char *tool_name, *previous_tool_name;
    u8 tool_name_len, previous_tool_name_len;
    u8 tool_name_slide;
    int key_press_event;
    int mouse_wheel_event;
} player_t;

typedef struct {
    character_t *character;
    u8 id;
    i32 last_x, last_y;
    i32 next_x, next_y;
    str_t name;
} network_player_t;

void draw_character(character_t*);
void update_character(character_t*);
void draw_player_ui(player_t*, float scale);

player_t* new_player(character_t*, window_t*);
void delete_player(player_t **);
void update_player(player_t*);

#endif