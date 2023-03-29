#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "ae/str.h"
#include "game/base.h"
#include <stdbool.h>

#define SHOW_FPS         (1 << 1)
#define SHOW_TILE        (1 << 2)
#define SHOW_PLAYER      (1 << 3)
#define SHOW_CURSOR      (1 << 4)
#define SHOW_CONSOLE     (1 << 5)
#define SHOW_ALL         (SHOW_FPS | SHOW_TILE | SHOW_PLAYER | SHOW_CURSOR)
#define SHOW_FLAGS       (1 << 6)
#define COMMAND_ECHO     (1 << 7)

struct name_mask {
    const char *name;
    u32 mask;
};

extern struct name_mask debug_show_names[];
extern u32 debug_options;

void set_debug_show_mask(const char *name, bool show);

typedef enum {
    PT_NUMBER,
    PT_STRING
} parameter_type_t;

typedef struct {
    char* str_val;
    float num_val;
    parameter_type_t type;
} argument_t;

typedef char (*command_callback_t)(argument_t*, unsigned char);

typedef struct {
    str_t name;
    str_t default_value;
    parameter_type_t type;
    bool optional;
} parameter_t;

typedef struct {
    str_t name, description;
    command_callback_t callback;
    parameter_t *parameters;
} command_t;

command_t* new_command(const char *name, const char *description, command_callback_t callback);
command_t* get_command(const char *name);
str_t get_command_usage_str(command_t *command);
void add_parameter(command_t *command, const char *name, parameter_type_t type, const char *default_value);
void delete_command(const char *name);
void command_exec(const char *input);
command_t* get_command_list();

#endif