#include "game/console.h"
#include "ae/window.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// static u8 history_index = 0;
// static str_t history[CONSOLE_HISTORY_MAX] = { NULL };
// static str_t input = NULL;
// static str_t input_save = NULL;

console_t* new_console(console_callback_t cb) {
    console_t *console = calloc(1, sizeof(console_t));

    console->history_index = -1;
    console->buffer = str_new("");
    console->callback = cb;

    return console;
}

void delete_console(console_t **console) {
    console_t *c = *console;
    str_del(c->buffer);
    if (c->buffer_temp) str_del(c->buffer_temp);
    for (u8 i = 0; i < CONSOLE_HISTORY_MAX; i++)
        if (c->history[i]) str_del(c->history[i]);
    free(*console);
    *console = NULL;
}

void console_history_push(console_t *console, const char *text, u32 len) {
    u8 m = CONSOLE_HISTORY_MAX;
    if (console->history_size < m)
        m = console->history_size;
    memmove(&console->history[1], &console->history[0], sizeof(str_t) * m);
    console->history[0] = NULL;
    str_nset(console->history[0], text, len);
    if (console->history_size < CONSOLE_HISTORY_MAX)
        console->history_size += 1;
}

const char* get_console_history_entry(console_t *console, i16 i) {
    if (i < 0) return console->buffer_temp;
    if (i > CONSOLE_HISTORY_MAX - 1)
        i = CONSOLE_HISTORY_MAX - 1;
    return console->history[i];
}

static i16 clamp_index(console_t *console, i16 i) {
    if (i < 0) return 0;
    if (i > CONSOLE_HISTORY_MAX - 1) return CONSOLE_HISTORY_MAX - 1;
    if (i > console->history_size - 1) return console->history_size - 1;
    return i;
}

void console_input(console_t *console, int key, int mods) {
    if (key == KEY_BACKSPACE) {
        if (str_len(console->buffer))
            str_len(console->buffer) -= 1;
    }
    else if (key == KEY_UP) {
        if (console->history_index == -1) {
            str_nset(console->buffer_temp, console->buffer, str_len(console->buffer));
        }
        console->history_index = clamp_index(console, console->history_index + 1);
        str_nset(console->buffer, console->history[console->history_index], str_len(console->history[console->history_index]));
    }
    else if (key == KEY_DOWN) {
        console->history_index -= 1;
        if (console->history_index < 0) {
            console->history_index = -1;
            str_nset(console->buffer, console->buffer_temp, str_len(console->buffer_temp));
        }
        else str_nset(console->buffer, console->history[console->history_index], str_len(console->history[console->history_index]));
    }
    else if (key == KEY_ENTER) {
        if (str_len(console->buffer)) {
            console->callback(console->buffer);
            if (!console->history[0] || (console->history[0] && strncmp(console->buffer, console->history[0], str_len(console->buffer))))
                console_history_push(console, console->buffer, str_len(console->buffer));
            console->history_index = -1;
            str_set(console->buffer, "");
            str_set(console->buffer_temp, "");
        }
    }
    else if (isprint(key) && key != KEY_LEFT_SHIFT) {
        char c = tolower(key);
        if (mods & KEY_MOD_SHIFT) {
            switch (key) {
                case KEY_1: c = '!'; break;
                case KEY_2: c = '@'; break;
                case KEY_3: c = '#'; break;
                case KEY_4: c = '$'; break;
                case KEY_5: c = '%'; break;
                case KEY_6: c = '^'; break;
                case KEY_7: c = '&'; break;
                case KEY_8: c = '*'; break;
                case KEY_9: c = '('; break;
                case KEY_0: c = ')'; break;
                case KEY_MINUS: c = '_'; break;
                default: c = key;
            }
        }
        str_append(console->buffer, &c, 1);
    }
}

void console_clear(console_t *console) {
    str_set(console->buffer, "");
}

void console_insert(console_t *console, const char *text, u32 len) {
    str_append(console->buffer, text, len ? len : strlen(text));
}
