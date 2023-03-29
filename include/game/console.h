#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "game/base.h"
#include <stdbool.h>
#include "ae/str.h"

#ifndef CONSOLE_HISTORY_MAX
#define CONSOLE_HISTORY_MAX 64
#endif

typedef void (*console_callback_t)(str_t);

typedef struct {
    i16 history_index;
    u8 history_size;
    str_t history[CONSOLE_HISTORY_MAX];
    str_t buffer;
    str_t buffer_temp;
    console_callback_t callback;
} console_t;

console_t* new_console(console_callback_t);
void delete_console(console_t**);
void console_history_push(console_t *console, const char *text, u32 len);
const char* get_console_history_entry(console_t *console, i16 i);
void console_input(console_t*, int key, int mods);
void console_clear(console_t*);
void console_insert(console_t*, const char *text, u32 len);

#endif