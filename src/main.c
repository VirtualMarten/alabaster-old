#include "game/base.h"
#include "game/asset.h"
#include "game/debug.h"
#include "game/console.h"
#include "game/level.h"
#include "game/character.h"

#include "ae/array.h"
#include "ae/str.h"
#include "ae/shader.h"
#include "ae/surface.h"
#include "ae/audio.h"
#include "ae/text.h"
#include "ae/utility.h"
#include "ae/color.h"
#include "ae/image.h"
#include "ae/window.h"
#include "ae/net.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#define TPS 60
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "test"
#define GAME_WIDTH  256
#define GAME_HEIGHT 224
#define DEFAULT_PORT 12434
#define LEVEL_WIDTH 32
#define LEVEL_HEIGHT 32

static asset_type_t shader_asset, texture_asset;

static player_t *player;
static console_t *console;

static tile_t debug_tile;
static vec2 debug_tile_position;

#define TOP_LEFT     0
#define TOP_RIGHT    1
#define BOTTOM_LEFT  2
#define BOTTOM_RIGHT 3

static vec4 draw_debug_text(u8 corner, i32 offset, u32 backmax, const char *format, u32 len, ...) {
    static char str[128];
    static vec2 sz, origin, pos;
    va_list args;
    va_start(args, len);
    vsprintf(str, format, args);
    va_end(args);
    switch (corner) {
        case TOP_LEFT:
            origin = vec2(0, 0);
            pos = vec2(2, 2 + offset);
            break;
        case TOP_RIGHT:
            origin = vec2(1, 0);
            pos = vec2(window->width - 2, 2 + offset);
            break;
        case BOTTOM_LEFT:
            origin = vec2(0, 1);
            pos = vec2(2, window->height - offset);
            break;
        case BOTTOM_RIGHT:
            origin = vec2(1, 1);
            pos = vec2(window->width - 2, window->height - offset);
            break;
    }
    sz = draw_text(debug_font, str, len ?: strlen(str), 16, 0, origin, pos, -99, rgba(1, 1, 1, 0.75f));
    if (corner > TOP_RIGHT) pos.x -= 2;
    else {
        pos.x += 2;
        pos.y -= 2;
    }
    draw_polygon(quad_polygon, origin, pos, -98, MAX(sz.x, backmax) + 4, sz.y + 2, rgba(0, 0, 0, 0.75f));
    return vec4(0, sz.y + 4, sz.x, sz.y);
}











// static struct host_character {
//     socket_wrapper_t *socket;
//     character_t character;
// } *host_characters = NULL;

// u8 get_new_id() {
//     static u8 next_id = 0;
//     if (next_id < 255)
//         return next_id++;
//     else {
//         next_id = 0;
//         for (u8 i = 0; i < arr_len(host_characters); i++)
//             if (host_characters[i].character.id == next_id)
//                 ++next_id;
//         return next_id;
//     }
// }

static character_t *characters = NULL;
static level_t *test_level;

// static server_t server;
// bool hosting = false;
// static client_t client = {0};
// bool connected = false;
// static clock_t ping = 0, ping_time;
// static u8 pong = true;

// typedef enum {
//     PKT_PING = 0,
//     PKT_PONG,
//     PKT_MESSAGE,
//     PKT_JOIN,
//     PKT_JOIN_ACCEPT,
//     PKT_KICK,
//     PKT_CHARACTER_DELETE,
//     PKT_CHARACTER_CREATE,
//     PKT_CHARACTER_UPDATE,
//     PKT_PLAYER_NAME,
//     PKT_LEVEL_CREATE,
//     PKT_LEVEL_UPDATE,
//     PKT_END
// } packet_type_t;

// static const char *pkt_names[] = {
//     "PKT_PING",
//     "PKT_PONG",
//     "PKT_MESSAGE",
//     "PKT_JOIN",
//     "PKT_JOIN_ACCEPT",
//     "PKT_KICK",
//     "PKT_CHARACTER_DELETE",
//     "PKT_CHARACTER_CREATE",
//     "PKT_CHARACTER_UPDATE",
//     "PKT_PLAYER_NAME",
//     "PKT_LEVEL_CREATE",
//     "PKT_LEVEL_UPDATE",
//     "PKT_END"
// };

// typedef struct {
//     i16 id;
//     u8 message_length;
//     char message[];
// } pkt_message_t;

// typedef struct {
//     unsigned char version_major, version_minor;
//     u8 name_length;
//     char name[];
// } pkt_join_t;

// typedef struct {
//     u8 your_id;
// } pkt_join_accept_t;

// typedef struct {
//     u8 reason_length;
//     char reason[];
// } pkt_kick_t;

// typedef struct {
//     u8 id;
// } pkt_character_delete_t;

// typedef struct {
//     u8 id;
//     u8 name_length;
//     char name[];
// } pkt_character_create_t;

// typedef struct {
//     u8 id;
//     int x, y;
//     u8 legs_frame;
//     u8 flip_legs;
//     u16 look_direction;
// } pkt_character_update_t;

// typedef struct {
//     u8 id;
//     u8 name_length;
//     char name[];
// } pkt_player_name_t;

// typedef struct {
//     u16 width, height;
//     u8 tile_ids[];
// } pkt_level_create_t;

// typedef struct {
//     u16 x, y;
//     u8 tile_id;
// } pkt_level_update_t;

// int pkt_nsend(socket_wrapper_t *s, packet_type_t type, void *pkt, u16 len) {
//     char data[len + 1];
//     data[0] = type;
//     if (pkt) memcpy(&data[1], pkt, len);
//     return packet_send(s, data, len + 1);
// }

// int pkt_msg_send(socket_wrapper_t *s, i16 id, const char *format, ...) {
//     va_list args;
//     va_start(args, format);
//     char buffer[128];
//     int l = vsnprintf(buffer, 128, format, args);
//     va_end(args);
//     pkt_message_t *pkt = malloc(sizeof(pkt_message_t) + l);
//     pkt->id = id;
//     pkt->message_length = l;
//     strncpy(pkt->message, buffer, l);
//     int _s = pkt_nsend(s, PKT_MESSAGE, pkt, sizeof(pkt_message_t) + l);
//     free(pkt);
//     return _s;
// }

// int pkt_send(socket_wrapper_t *s, packet_type_t type, void *pkt) {
//     char len;
//     switch (type) {
//         case PKT_PING:
//         case PKT_PONG:             len = 0;                              break;
//         case PKT_MESSAGE:          len = sizeof(pkt_message_t);          break;
//         case PKT_JOIN:             len = sizeof(pkt_join_t);             break;
//         case PKT_JOIN_ACCEPT:      len = sizeof(pkt_join_accept_t);      break;
//         case PKT_KICK:             len = sizeof(pkt_kick_t);             break;
//         case PKT_CHARACTER_DELETE: len = sizeof(pkt_character_delete_t); break;
//         case PKT_CHARACTER_CREATE: len = sizeof(pkt_character_create_t); break;
//         case PKT_CHARACTER_UPDATE: len = sizeof(pkt_character_update_t); break;
//         case PKT_LEVEL_CREATE:     len = sizeof(pkt_level_create_t);     break;
//         case PKT_LEVEL_UPDATE:     len = sizeof(pkt_level_update_t);     break;
//         default: return -3;
//     }
//     return pkt_nsend(s, type, pkt, len);
// }

// static inline int client_pkt_send(client_t *client, packet_type_t type, void *pkt) {
//     return pkt_send((socket_wrapper_t*)client->connect.handle, type, pkt);
// }

// static inline int client_pkt_nsend(client_t *client, packet_type_t type, void *pkt, u16 len) {
//     return pkt_nsend((socket_wrapper_t*)client->connect.handle, type, pkt, len);
// }

// #define client_pkt_msg_send(client, id, format, ...) pkt_msg_send((socket_wrapper_t*)(client)->connect.handle, id, format, __VA_ARGS__);

// void server_receive_packet_cb(socket_wrapper_t *s, void *data, int len) {
//     if (len < 1) return;
//     switch (*(u8*)data) {
//         case PKT_PING:
//             pkt_send(s, PKT_PONG, NULL);
//             break;
//         case PKT_MESSAGE:
//             for (u8 i = 0; i < arr_len(server.client_sockets); i++)
//                 packet_send(server.client_sockets[i], data, len);
//             break;
//         case PKT_JOIN: {
//                 pkt_join_t *p = data + 1;
//                 if (!check_version(p->version_major, p->version_minor)) {
//                     const char str[] = "Version Mismatch";
//                     const u8 l = strlen(str);
//                     pkt_kick_t *pkt = malloc(l + 1);
//                     pkt->reason_length = l;
//                     strncpy(pkt->reason, str, l);
//                     pkt_nsend(s, PKT_KICK, pkt, l + 1);
//                     break;
//                 }
//                 u8 id = get_new_id();
//                 str_t name_str = str_nnew(p->name, p->name_length);
//                 arr_add(host_characters, (struct host_character){
//                     s, { .id = id, .name = name_str }
//                 });
//                 {
//                     pkt_join_accept_t pkt = {
//                         .your_id = id
//                     };
//                     pkt_send(s, PKT_JOIN_ACCEPT, &pkt);
//                 }
//                 for (u8 i = 0; i < arr_len(host_characters); i++) { // function for broadcasting a packet
//                     if (host_characters[i].character.id == id) continue;
//                     const u8 l = str_len(host_characters[i].character.name);
//                     pkt_character_create_t *pkt = malloc(sizeof(pkt_character_create_t) + l);
//                     pkt->id = host_characters[i].character.id;
//                     pkt->name_length = l;
//                     strncpy(pkt->name, host_characters[i].character.name, l);
//                     pkt_nsend(s, PKT_CHARACTER_CREATE, pkt, sizeof(pkt_character_create_t) + l);
//                     free(pkt);
//                 }
//                 {
//                     pkt_character_create_t *pkt = malloc(sizeof(pkt_character_create_t) + p->name_length);
//                     pkt->id = id;
//                     pkt->name_length = p->name_length;
//                     strncpy(pkt->name, p->name, p->name_length);
//                     for (u8 i = 0; i < arr_len(host_characters); i++) {
//                         if (host_characters[i].socket == s) continue;
//                         pkt_nsend(host_characters[i].socket, PKT_CHARACTER_CREATE, pkt, sizeof(pkt_character_create_t) + p->name_length);
//                         pkt_msg_send(host_characters[i].socket, -1, "%s joined the server.", name_str);
//                     }
//                     free(pkt);
//                 }
//                 const u32 size = current_level->width * current_level->height;
//                 pkt_level_create_t *pkt = malloc(sizeof(pkt_level_create_t) + size);
//                 pkt->width = current_level->width;
//                 pkt->height = current_level->height;
//                 for (u16 y = 0; y < current_level->height; y++) {
//                     for (u16 x = 0; x < current_level->width; x++) {
//                         pkt->tile_ids[y * pkt->width + x] = current_level->tiles[y * pkt->width + x].id;
//                     }
//                 }
//                 pkt_nsend(s, PKT_LEVEL_CREATE, pkt, sizeof(pkt_level_create_t) + size);
//                 free(pkt);
//                 printf("[SERVER] Client %s joined. (ID %u)\n", name_str, id);
//             } break;
//         case PKT_CHARACTER_UPDATE: {
//                 pkt_character_update_t *p = data + 1;
//                 for (u8 i = 0; i < arr_len(host_characters); i++) {
//                     if (host_characters[i].socket == s) {
//                         host_characters[i].character.legs_frame = p->legs_frame;
//                         host_characters[i].character.flip_legs = p->flip_legs;
//                         host_characters[i].character.look_direction = p->look_direction;
//                         host_characters[i].character.position = vec2(p->x, p->y);
//                         host_characters[i].character.has_updated = true;
//                         break;
//                     }
//                 }
//             } break;
//         case PKT_PLAYER_NAME: {
//                 pkt_player_name_t *p = data + 1;
//                 for (u8 i = 0; i < arr_len(host_characters); i++) // function for finding a host_character
//                     if (host_characters[i].socket == s)
//                         str_nset(host_characters[i].character.name, p->name, p->name_length);
//                     else packet_send(host_characters[i].socket, data, len);
//             } break;
//         case PKT_LEVEL_UPDATE:
//             for (u8 i = 0; i < arr_len(host_characters); i++)
//                 if (host_characters[i].socket != s)
//                     packet_send(host_characters[i].socket, data, len);
//             break;
//         default:
//             if (*(u8*)data < PKT_END)
//                 printf("[SERVER] Warning: Recieved unhandled packet. (%s)\n", pkt_names[*(u8*)data]);
//             else printf("[SERVER] Warning: Recieved unknown packet. (%u)\n", *(u8*)data);
//     }
// }

// void client_receive_packet_cb(socket_wrapper_t *s, void *data, int len) {
//     if (len < 1) return;
//     switch (*(u8*)data) {
//         case PKT_PONG:
//             pong = true;
//             ping = (clock() - ping_time);
//             break;
//         case PKT_MESSAGE: {
//                 pkt_message_t *p = data + 1;
//                 char msg[p->message_length + 1];
//                 strncpy(msg, p->message, p->message_length);
//                 msg[p->message_length] = '\0';
//                 if (p->id < 0) print(msg);
//                 else for (u8 i = 0; i < arr_len(characters); i++) {
//                     if (characters[i].id == p->id)
//                         print("%s: %s", characters[i].name, msg);
//                 }
//             } break;
//         case PKT_JOIN_ACCEPT: {
//                 pkt_join_accept_t *p = data + 1;
//                 characters[0].id = p->your_id;
//                 if (!hosting) print("Joined server.");
//             } break;
//         case PKT_KICK: {
//                 pkt_kick_t *p = data + 1;
//                 print("Kicked from server, reason: %s", p->reason);
//                 close_client(&client);
//                 connected = false;
//             } break;
//         case PKT_CHARACTER_DELETE: {
//                 pkt_character_delete_t *p = data + 1;
//                 for (u8 i = 1; i < arr_len(characters); i++) {
//                     if (characters[i].id == p->id) {
//                         str_del(characters[i].name);
//                         arr_remove(characters, i);
//                         break;
//                     }
//                 }
//             } break;
//         case PKT_CHARACTER_CREATE: {
//                 pkt_character_create_t *p = data + 1;
//                 str_t name_str = str_nnew(p->name, p->name_length);
//                 arr_add(characters, (character_t){
//                     .id = p->id, .name = name_str
//                 });
//             } break;
//         case PKT_CHARACTER_UPDATE: {
//                 pkt_character_update_t *p = data + 1;
//                 for (u8 i = 0; i < arr_len(characters); i++) {
//                     if (characters[i].id == p->id) {
//                         characters[i].position = vec2(p->x, p->y);
//                         characters[i].legs_frame = p->legs_frame;
//                         characters[i].flip_legs = p->flip_legs;
//                         characters[i].look_direction = p->look_direction;
//                         return;
//                     }
//                 }
//                 printf("[CLIENT] Warning: The server is trying to update a character that doesn't exist on this client. (ID %u)\n", p->id);
//             } break;
//         case PKT_PLAYER_NAME: {
//                 pkt_player_name_t *p = data + 1;
//                 for (u8 i = 0; i < arr_len(characters); i++)
//                     if (characters[i].id == p->id)
//                         str_nset(characters[i].name, p->name, p->name_length);
//             } break;
//         case PKT_LEVEL_CREATE: {
//                 pkt_level_create_t *p = data + 1;
//                 for (u16 y = 0; y < p->height; y++) {
//                     for (u16 x = 0; x < p->width; x++) {
//                         current_level->tiles[y * p->width + x].id = p->tile_ids[y * p->width + x];
//                     }
//                 }
//                 for (u32 i = 0; i < current_level->height * current_level->width; i++)
//                     update_tile_state(current_level, i % current_level->width, i / current_level->width, false);
//             } break;
//         case PKT_LEVEL_UPDATE: {
//                 pkt_level_update_t *p = data + 1;
//                 set_tile(current_level, p->x, p->y, p->tile_id);
//             } break;
//         default:
//             if (*(u8*)data < PKT_END)
//                 printf("[CLIENT] Warning: Recieved unhandled packet. (%s)\n", pkt_names[*(u8*)data]);
//             else printf("[CLIENT] Warning: Recieved unknown packet. (%u)\n", *(u8*)data);
//     }
// }

// void accept_client_cb(socket_wrapper_t *s) {
//     (void)socket;
//     client_pkt_send(&client, PKT_PING, NULL);
//     //print("[SERVER] Client %p Connected!", server.client_sockets[arr_len(server.client_sockets) - 1]);
// }

// void print_net_error(int status) {
//     const char *err = uv_strerror(status);
//     const unsigned char errlen = strlen(err);
//     char msg[errlen + 1];
//     strncpy(msg, err, errlen);
//     msg[errlen] = '.';
//     msg[0] = toupper(msg[0]);
//     print(msg);
// }

// void client_connect_cb(client_t *client, int status) {
//     if (status) {
//         print_net_error(status);
//         return;
//     }
//     pkt_join_t *pkt = malloc(sizeof(pkt_join_t) + str_len(characters[0].name));
//     pkt->version_major = VERSION_MAJOR;
//     pkt->version_minor = VERSION_MINOR;
//     pkt->name_length = str_len(characters[0].name);
//     strncpy(pkt->name, characters[0].name, str_len(characters[0].name));
//     client_pkt_nsend(client, PKT_JOIN, pkt, sizeof(pkt_join_t) + str_len(characters[0].name));
// }

// void client_disconnect_cb(client_t *client) {
//     (void)client;
//     if (!hosting) print("Disconnected from server.");
//     connected = false;
//     for (u8 i = 1; i < arr_len(characters); i++) {
//         str_del(characters[i].name);
//         arr_remove(characters, i);
//     }
// }

// void server_client_disconnect_cb(socket_wrapper_t *socket) {
//     for (u8 i = 0; i < arr_len(host_characters); i++) {
//         if (host_characters[i].socket == socket) {
//             pkt_character_delete_t pkt = {
//                 .id = host_characters[i].character.id
//             };
//             for (u8 j = 0; j < arr_len(server.client_sockets); j++) {
//                 if (server.client_sockets[j] == socket) continue;
//                 pkt_send(server.client_sockets[j], PKT_CHARACTER_DELETE, &pkt);
//                 pkt_msg_send(server.client_sockets[j], -1, "%s left the server.", host_characters[i].character.name);
//             }
//             printf("[SERVER] Client disconnected. (ID %u)\n", host_characters[i].character.id);
//             str_del(host_characters[i].character.name);
//             arr_remove(host_characters, i);
//         }
//     }
// }

// void join(const char *ip, int port) {
//     if (connected) return;
//     init_client(&client, client_receive_packet_cb);
//     if (!hosting) print("Connecting to %s:%d...", ip, port);
//     int err;
//     if ((err = connect_client(&client, ip, port, client_connect_cb, client_disconnect_cb))) {
//         print("Failed to connect to server.");
//         print_net_error(err);
//         return;
//     }
//     connected = true;
// }

// void host(const char *ip, int port) {
//     if (hosting) return;
//     if (host_characters) arr_delete(host_characters);
//     init_server(&server, server_receive_packet_cb);
//     print("Hosting at %s:%d...", ip, port);
//     int err;
//     if ((err = open_server(&server, ip, port, accept_client_cb, server_client_disconnect_cb))) {
//         print("Failed to open server.");
//         print_net_error(err);
//         return;
//     }
//     host_characters = arr_new(sizeof(struct host_character), 1, 1);
//     hosting = true;
//     join(ip, port);
// }

// static u32 net_tick = 0;

void update() {
    if (!(debug_options & SHOW_CONSOLE))
        update_player(player);
    for (u8 i = 0; i < arr_len(characters); i++)
        update_character(&characters[i]);
    // if (net_tick % 2 == 0) {
    //     if (connected) {
    //         pkt_character_update_t pkt = {
    //             .id = characters[0].id,
    //             .x = characters[0].position.x,
    //             .y = characters[0].position.y,
    //             .legs_frame = characters[0].legs_frame,
    //             .flip_legs = characters[0].flip_legs,
    //             .look_direction = characters[0].look_direction
    //         };
    //         client_pkt_send(&client, PKT_CHARACTER_UPDATE, &pkt);
    //     }
    // }
    // else {
    //     if (hosting) {
    //         for (u8 i = 0; i < arr_len(host_characters); i++) {
    //             for (u8 j = 0; j < arr_len(server.client_sockets); j++) {
    //                 if (server.client_sockets[j] == host_characters[i].socket) continue;
    //                 pkt_character_update_t pkt = {
    //                     .id = host_characters[i].character.id,
    //                     .x = host_characters[i].character.position.x,
    //                     .y = host_characters[i].character.position.y,
    //                     .legs_frame = host_characters[i].character.legs_frame,
    //                     .flip_legs = host_characters[i].character.flip_legs,
    //                     .look_direction = host_characters[i].character.look_direction
    //                 };
    //                 pkt_send(server.client_sockets[j], PKT_CHARACTER_UPDATE, &pkt);
    //             }
    //         }
    //     }
    // }
    // if (net_tick >= TPS * 4) {
    //     net_tick -= TPS * 4;
    //     if (connected) {
    //         if (pong == false) {
    //             close_client(&client);
    //             print("Disconnected: Timed out.");
    //             connected = false;
    //         }
    //         else {
    //             pong = false;
    //             ping_time = clock();
    //             client_pkt_send(&client, PKT_PING, NULL);
    //         }
    //     }
    // }
    // ++net_tick;
}

void draw_console(u8 corner, u32 max_width, u32 height) {
    static u32 width = 0;
    vec4 box = draw_debug_text(BOTTOM_LEFT, 0, width, "> %s", str_len(console->buffer) + 2, console->buffer);
    width = MAX(MAX(box.z, width), 256);
    i32 text_offset = 0;
    u8 original_offset = text_offset = box.y - 1;
    u32 max_height = original_offset;
    for (u8 i = (u8)floorf(debug_log_offset); i < min((u8)floorf(debug_log_offset) + 12, 128); i++) {
        if (!debug_log[i]) break;
        box = draw_debug_text(BOTTOM_LEFT, text_offset, width, " %s", str_len(debug_log[i]) + 1, debug_log[i]);
        width = MAX(MAX(box.z, width), 256);
        text_offset += box.y - 2;
        max_height += box.w + 2;
    }
    u8 s = 0;
    while (s < 128 && debug_log[s]) s++;
    max_height -= original_offset;
    if (s > 12) {
        float sf = (128 - (s - 12)) / 128.0f;
        draw_polygon(quad_polygon, vec2(0, 1), vec2(3, window->height - original_offset - lerpf(0, (1 - sf) * max_height, debug_log_offset / (s - 12))), -100, 2, sf * max_height, C_GRAY);
    }
    draw_polygon(quad_polygon, vec2(0, 1), vec2(0, window->height - original_offset + 1), -100, width + 4, 1, C_GRAY);
}

void render() {
    static i32 text_offset;
    static u8 corner;
    if (debug_options & SHOW_PLAYER) {
        vec2 p = vec2_scale(player->tile_h, TILE_SIZE);
        draw_texture(vec2(0, 0), p, get_depth(p.y) - 1, TILE_SIZE, TILE_SIZE * 2, C_BLUE, sprites, texrect(96, 0, TILE_SIZE, TILE_SIZE * 2, 0));
        draw_texture(vec2(0, 0), vec2(p.x + 1, p.y + 10), get_depth(p.y) + 1, TILE_SIZE - 2, 1, C_BLUE, sprites, texrect(81, 0, TILE_SIZE - 2, 1, 0));
        p = vec2_scale(player->tile_v, TILE_SIZE);
        draw_texture(vec2(0, 0), p, get_depth(p.y) - 1, TILE_SIZE, TILE_SIZE * 2, C_RED, sprites, texrect(96, 0, TILE_SIZE, TILE_SIZE * 2, 0));
        draw_texture(vec2(0, 0), vec2(p.x + 1, p.y + 10), get_depth(p.y) + 1, TILE_SIZE - 2, 1, C_RED, sprites, texrect(81, 0, TILE_SIZE - 2, 1, 0));
    }
    for (u8 i = 0; i < arr_len(characters); i++)
        draw_character(&characters[i]);
    draw_level(test_level);
    render_surface(game_surface);
    if (!(debug_options & SHOW_CONSOLE))
        draw_player_ui(player, game_scale);
    //if (connected) draw_debug_text(TOP_LEFT, 0, 0, " ping %.0fms", 0, (ping / (float)CLOCKS_PER_SEC) / 1000.0f);
    text_offset = 0;
    if (debug_options & SHOW_CONSOLE) {
        corner = TOP_RIGHT;
        draw_console(BOTTOM_LEFT, 256, 0);
    }
    else corner = BOTTOM_LEFT;
    text_offset = 0;
    if (debug_options & SHOW_FLAGS) {
        for (u8 i = 0; debug_show_names[i].name != NULL; i++) {
            bool showing = (debug_options & debug_show_names[i].mask) == debug_show_names[i].mask;
            text_offset += draw_debug_text(TOP_LEFT, text_offset, 0, "%c%u%s%s", 0, showing ? '[' : ' ', i + 1, showing ? "] " : " ", debug_show_names[i].name).y - 2;
        }
    }
    text_offset = 0;
    if (debug_options & SHOW_FPS) {
        text_offset = draw_debug_text(corner, 0, 0, "frame rate %d, time %.4f", 0, window->fps, window->frametime).y;
    }
    if (debug_options & SHOW_TILE) {
        text_offset += draw_debug_text(corner, text_offset, 0, "tile %u %u: id %u, state %u", 0, (u16)debug_tile_position.x, (u16)debug_tile_position.y, debug_tile.id, debug_tile.state).y;
    }
    if (debug_options & SHOW_CURSOR) {
        text_offset += draw_debug_text(corner, text_offset, 0, "cursor %d %d, %d %d", 0, (i32)cursor.x, (i32)cursor.y, (i32)game_cursor.x, (i32)game_cursor.y).y;
    }
    if (debug_options & SHOW_PLAYER) {
        draw_debug_text(corner, text_offset, 0, "player pos %0.2f %0.2f, vel %0.2f %0.2f, dir %0.2f\n       frame i %0.2f, head %u, body %u", 0,
            characters[0].position.x, characters[0].position.y,
            player->velocity.x, player->velocity.y,
            player->look_direction,
            player->animation_index,
            characters[0].head_frame,
            characters[0].body_frame
        );
    }
    if (debug_options & SHOW_CURSOR) {
        draw_polygon(quad_polygon, vec2(0, 0), vec2(cursor.x, 0), -999, 1, window->height, C_RED);
        draw_polygon(quad_polygon, vec2(0, 0), vec2(0, cursor.y), -999, window->width, 1, C_LIME);
    }
    draw_texture_ext(vec2(0.5f, 0.5f), vec2(window->width / 2, window->height / 2), 0, GAME_WIDTH, GAME_HEIGHT, vec2(game_scale, game_scale), 0, C_WHITE, (texture_t*)game_surface, texrect(0, 0, 0, 0, 0));
    render_surface(window_surface);
}

//
// Callbacks
// ================

void on_window_close(void *userdata, window_t *w) {
    close_window(w);
}

void on_key_press(void *userdata, int key, int scancode, int mods) {
    if (key == KEY_GRAVE_ACCENT) {
        debug_options ^= SHOW_CONSOLE;
    }
    else if ((debug_options & SHOW_CONSOLE)) {
        console_input(console, key, mods);
    }
    else if (key == KEY_F1) {
        debug_options ^= SHOW_FPS;
    }
    else if (key == KEY_F2) {
        debug_options ^= SHOW_FLAGS;
    }
    else if ((debug_options & SHOW_FLAGS) && key >= KEY_1 && key <= KEY_9) {
        for (u8 i = 0; debug_show_names[i].name != NULL; i++)
            if (key == KEY_1 + i) debug_options ^= debug_show_names[i].mask;
    }
}

static void on_window_resize(void *user_data, window_t *w, u32 width, u32 height) {
    (void)user_data;
    window_surface->view.width = width;
    window_surface->view.height = height;
    game_scale = ceilf(max(width / (float)GAME_WIDTH, height / (float)GAME_HEIGHT));
    window_surface->view.position = vec2(width / 2, height / 2);
    game_surface_cutoff.x = ((game_surface->width * game_scale) - window->width) / game_scale;
    game_surface_cutoff.y = ((game_surface->height * game_scale) - window->height) / game_scale;
    game_visible_width = game_surface->width - game_surface_cutoff.x;
    game_visible_height = game_surface->height - game_surface_cutoff.y;
}

static void on_cursor_move(void *userdata, int x, int y, float cx, float cy) {
    (void)userdata;
    cursor.x = x;
    cursor.y = y;
}

static void on_mouse_button(void *userdata, int button, int mods) {
    (void)userdata;
    // if ((debug_options & SHOW_TILE) && button == MOUSE_BUTTON_LEFT) {
    //     pkt_level_update_t pkt = {
    //         .x = debug_tile_position.x,
    //         .y = debug_tile_position.y
    //     };
    //     if (get_tile(test_level, debug_tile_position.x, debug_tile_position.y)->id)
    //         pkt.tile_id = 0;
    //     else pkt.tile_id = 1;
    //     set_tile(test_level, debug_tile_position.x, debug_tile_position.y, pkt.tile_id);
    //     if (connected) client_pkt_send(&client, PKT_LEVEL_UPDATE, &pkt);
    // }
}

static void on_mouse_scroll(void *userdata, double x, double y) {
    if (y > 0) {
        u8 s = (u8)floorf(debug_log_offset);
        for (u8 i = (u8)floorf(debug_log_offset) + 12; i < 128; i++)
            if (debug_log[i]) s++;
        debug_log_offset = min(min(debug_log_offset + y, s), 127);
    }
    else if (y < 0) debug_log_offset = max(debug_log_offset + y, 0);
}

static int init_shader_asset_cb(asset_t *asset) {
    *asset->ptr = new_shader();
    return 1;
}

static int load_shader_asset_cb(asset_t *asset) {
    return load_shader(*asset->ptr, asset->path);
}

static void free_shader_asset_cb(asset_t *asset) {
    delete_shader(*asset->ptr);
}

static int init_texture_asset_cb(asset_t *asset) {
    *asset->ptr = new_texture(0, 0, NULL, 0, T_RGB);
    return 1;
}

static int load_texture_asset_cb(asset_t *asset) {
    return !!load_texture(*asset->ptr, asset->path);
}

static void free_texture_asset_cb(asset_t *asset) {
    delete_texture(*asset->ptr);
}

static void console_cb(str_t buffer) {
    if (buffer[0] == '/') {
        if (debug_options & COMMAND_ECHO)
            print(buffer);
        command_exec(buffer);
    }
    // else if (connected) {
    //     client_pkt_msg_send(&client, characters[0].id, "%s", buffer);
    // }
    else print(": %s", buffer);
}








// COMMANDS

static char c_help(argument_t *args, unsigned char argc) {
    if (argc == 0) {
        command_t *cmds = get_command_list();
        for (u32 i = 0; i < arr_len(cmds); i++)
            print("%s: %s", cmds[i].name, cmds[i].description);
    }
    else if (argc == 1) {
        command_t *cmd = get_command(args[0].str_val);
        if (cmd) print("usage: %s", get_command_usage_str(cmd));
    }
    return 1;
}

static char c_show(argument_t *args, unsigned char argc) {
    set_debug_show_mask(args[0].str_val, true);
    print("Showing %s info.", args[0].str_val);
    return 1;
}

// static char c_name(argument_t *args, unsigned char argc) {
//     str_set(characters[0].name, args[0].str_val);
//     print("Set your name to %s.", args[0].str_val);
//     pkt_player_name_t *pkt = malloc(sizeof(pkt_player_name_t) + str_len(args[0].str_val));
//     pkt->id = characters[0].id;
//     pkt->name_length = str_len(args[0].str_val);
//     strncpy(pkt->name, characters[0].name, pkt->name_length);
//     client_pkt_nsend(&client, PKT_PLAYER_NAME, pkt, sizeof(pkt_player_name_t) + str_len(args[0].str_val));
//     return 1;
// }

static char c_join(argument_t *args, unsigned char argc) {
    //join(args[0].str_val, args[0].num_val);
    return 1;
}

static char c_host(argument_t *args, unsigned char argc) {
    //host("0.0.0.0", args[0].num_val);
    return 1;
}

// static char c_kick(argument_t *args, unsigned char argc) {
//     if (!hosting) {
//         print("You can only kick if you are hosting a server.");
//         return 1;
//     }
//     for (u8 i = 0; i < arr_len(host_characters); i++) {
//         if (!strncmp(host_characters[i].character.name, args[0].str_val, str_len(host_characters[i].character.name))) {
//             if (argc > 1) {
//                 u8 l = str_len(args[1].str_val);
//                 pkt_kick_t *pkt = malloc(sizeof(pkt_kick_t) + l);
//                 pkt->reason_length = l;
//                 strncpy(pkt->reason, args[1].str_val, l);
//                 pkt_nsend(host_characters[i].socket, PKT_KICK, pkt, sizeof(pkt_kick_t) + l);
//                 free(pkt);
//             }
//             else {
//                 pkt_kick_t pkt = {0};
//                 pkt_send(host_characters[i].socket, PKT_KICK, &pkt);
//             }
//         }
//     }
//     return 1;
// }

static char c_hide(argument_t *args, unsigned char argc) {
    set_debug_show_mask(args[0].str_val, false);
    print("Hiding %s info.", args[0].str_val);
    return 1;
}

static char c_reload(argument_t *args, unsigned char argc) {
    if (load_asset(args[0].str_val))
        print("Success.");
    return 1;
}

static char c_echo(argument_t *args, unsigned char argc) {
    if (!strcmp(args[0].str_val, "off")) {
        debug_options &= ~COMMAND_ECHO;
        print("Command echo is now off.");
    }
    else if (!strcmp(args[0].str_val, "on")) {
        debug_options |= COMMAND_ECHO;
        print("Command echo is now on.");
    }
    return 1;
}

static char c_version(argument_t *args, unsigned char argc) {
    print("Version %d.%d", VERSION_MAJOR, VERSION_MINOR);
    return 1;
}

static char c_window_info(argument_t *args, unsigned char argc) {
    print("Window: size %u %u, state %s%s", window->width, window->height, window->fullscreen ? "fullscreen" : "", window->vsync ? "vsync" : "");
    print("Viewport: size %u %u, scale %f", game_surface->width, game_surface->height, game_scale);
    return 1;
}

void setup_commands() {
    command_t *cmd = NULL;

    cmd = new_command("help", "Show this help message, provide a command name for usage details.", c_help);
    add_parameter(cmd, "command", PT_STRING, "");

    cmd = new_command("show", "Show all, or a specific debug tool.", c_show);
    add_parameter(cmd, "name", PT_STRING, "all");

    // cmd = new_command("name", "Set your name.", c_name);
    // add_parameter(cmd, "name", PT_STRING, NULL);

    cmd = new_command("join", "Join a server.", c_join);
    add_parameter(cmd, "address", PT_STRING, "0.0.0.0");
    add_parameter(cmd, "port", PT_NUMBER, "78787");

    cmd = new_command("host", "Host a server.", c_host);
    add_parameter(cmd, "port", PT_NUMBER, "78787");

    cmd = new_command("kick", "Kick a player off the server.", c_show);
    add_parameter(cmd, "name", PT_STRING, NULL);
    add_parameter(cmd, "reason", PT_STRING, "");

    cmd = new_command("hide", "Hide all, or a specific debug tool.", c_hide);
    add_parameter(cmd, "name", PT_STRING, "all");

    cmd = new_command("reload", "Reload all, or a specific asset.", c_reload);
    add_parameter(cmd, "name", PT_STRING, NULL);

    cmd = new_command("echo", "Turn command echo on or off.", c_echo);
    add_parameter(cmd, "on/off", PT_STRING, NULL);

    cmd = new_command("version", "Print the game version", c_version);

    cmd = new_command("winfo", "Print window and viewport information", c_window_info);
}

static tool_t pistol = { "PISTOL", { 240, 0, 10, 6, 0 }, 3, vec2(3, 3), vec2(6, 2) };
static tool_t help_gun = { "HELP GUN", { 254, 0, 16, 6, 0 }, 4, vec2(3, 3), vec2(6, 2) };

//             ########   #####    #####     #####                //
//                ##     ##   ##   ##  ##   ##   ##               //
//                ##     ##   ##   ##  ##   ##   ##               //
/*                ##      #####    #####     #####                //

    - Finish the game

*/

int main(void) {

    init_tile_states();

    window = new_window(640, 480, WINDOW_TITLE);
    handle_event(window, EV_WINDOW_CLOSE, on_window_close, NULL);
    handle_event(window, EV_KEY_PRESS, on_key_press, NULL);
    handle_event(window, EV_WINDOW_RESIZE, on_window_resize, NULL);
    handle_event(window, EV_CURSOR_MOVE, on_cursor_move, NULL);
    handle_event(window, EV_MOUSE_BUTTON_PRESS, on_mouse_button, NULL);
    handle_event(window, EV_MOUSE_SCROLL, on_mouse_scroll, NULL);

    setup_commands();

    shader_asset = new_asset_type(
        "shader",
        init_shader_asset_cb,
        load_shader_asset_cb,
        free_shader_asset_cb
    );

    texture_asset = new_asset_type(
        "texture",
        init_texture_asset_cb,
        load_texture_asset_cb,
        free_texture_asset_cb
    );

    load_asset("shader", "shader.glsl", shader, shader_asset);
    load_asset("sprites", "sprites2.png", sprites, texture_asset);

    window_surface = new_window_surface(window, shader, C_GRAY);
    game_surface = new_surface(shader, GAME_WIDTH, GAME_HEIGHT, C_WHITE, T_RGB);
    on_window_resize(NULL, window, WINDOW_WIDTH, WINDOW_HEIGHT);

    debug_font = new_font(FT_BITMAP, 16, 0);
    if (!load_font(debug_font, "fonts/LiberationMono-Regular.ttf", false)) {
        fprintf(stderr, "Failed to load debug font.\n");
        return EXIT_FAILURE;    
    }

    small_font = new_font(FT_BITMAP, 6, 6);
    if (!load_font(small_font, "fonts/small2.png", false)) {
        fprintf(stderr, "Failed to load small font.\n");
        return EXIT_FAILURE;
    }

    console = new_console(console_cb);

    characters = arr_new(sizeof(character_t), 1, 1);
    arr_add(characters, (character_t){0});
    player = new_player(&characters[0], window);
    player->tools[0] = &pistol;
    player->tools[1] = &help_gun;
    //characters[0].name = str_new("player");
    test_level = new_level(LEVEL_WIDTH, LEVEL_HEIGHT, vec2(0, 0));
    current_level = test_level;
    set_tile(test_level, 0, 0, 1);
    set_tile(test_level, 0, 1, 1);
    set_tile(test_level, 1, 0, 1);
    set_tile(test_level, 2, 0, 1);

    double lag = 0, update_speed = 1.0 / TPS;
    while (refresh_window(window)) {
        lag += window->frametime;
        while (lag >= update_speed) {
            game_cursor = to_game_coord(cursor);
            update();
            //uv_run(uv_default_loop(), UV_RUN_NOWAIT);
            lag -= update_speed;
        }
        render();
    }

    delete_window(&window);
    return EXIT_SUCCESS;
}