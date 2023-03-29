#include "game/debug.h"
#include "ae/array.h"
#include "game/base.h"
#include <string.h>

struct name_mask debug_show_names[] = {
    { "fps", SHOW_FPS },
    { "tile", SHOW_TILE },
    { "player", SHOW_PLAYER },
    { "cursor", SHOW_CURSOR },
    { "all", SHOW_ALL },
    { NULL }
};

u32 debug_options = COMMAND_ECHO;

void set_debug_show_mask(const char *name, bool show) {
    u8 i = 0;
    while (1) {
        if (!debug_show_names[i].name) break;
        if (!strcmp(debug_show_names[i].name, name)) {
            if (show) debug_options |= debug_show_names[i].mask;
            else debug_options &= ~debug_show_names[i].mask;
            return;
        }
        ++i;
    }
}

static command_t *commands = NULL;

command_t* get_command_list() {
    return commands;
}

command_t* new_command(const char *name, const char *description, command_callback_t callback) {
    if (!commands)
        commands = arr_new(sizeof(command_t), 1, 1);
    command_t cmd;
    cmd.name = str_new(name);
    cmd.description = str_new(description);
    cmd.callback = callback;
    cmd.parameters = arr_new(sizeof(parameter_t), 1, 1);
    arr_add(commands, cmd);
    return arr_get(commands, -1);
}

command_t* get_command(const char *name) {
    for (unsigned int i = 0; i < arr_len(commands); i++)
        if (!strncmp(commands[i].name, name, str_len(commands[i].name)))
            return &commands[i];
    return NULL;
}

str_t get_command_usage_str(command_t *cmd) {
    static str_t usage = NULL;
    if (!usage) usage = str_new("/");
    str_append(usage, cmd->name, str_len(cmd->name));
    for (unsigned int i = 0; i < arr_len(cmd->parameters); i++) {
        str_append(usage, cmd->parameters[i].optional ? " [" : " <", 2);
        str_append(usage, cmd->parameters[i].name, str_len(cmd->parameters[i].name));
        str_append(usage, cmd->parameters[i].optional ? "]" : ">", 1);
    }
    return usage;
}

void add_parameter(command_t *command, const char *name, parameter_type_t type, const char *default_value) {
    parameter_t p;
    p.name = str_new(name);
    p.type = type;
    if (default_value) {
        p.optional = true;
        p.default_value = str_new(default_value);
    }
    arr_add(command->parameters, p);
}

void delete_command(const char *name) {
    command_t *cmd = NULL;
    for (unsigned int i = 0; i < arr_len(commands); i++)
        if (!strncmp(commands[i].name, name, str_len(commands[i].name)))
            cmd = arr_remove(commands, i);
    if (!cmd) return;
    str_del(cmd->name);
    str_del(cmd->description);
    for (unsigned int i = 0; i < arr_len(cmd->parameters); i++) {
        str_del(cmd->parameters[i].name);
        if (cmd->parameters[i].default_value)
            str_del(cmd->parameters[i].default_value);
    }
    arr_delete(cmd->parameters);
}

void command_exec(const char *input) {
    // if (debug_options & COMMAND_ECHO) print("/%s", input);

    unsigned int i, l = strlen(input);
    unsigned int offset = ((input[0] == '/') ? 1 : 0);
    command_t *cmd = NULL;
    unsigned char argi, argc = 0;
    argument_t *args = NULL;

    for (i = 0; i < arr_len(commands); i++) {
        if (!strncmp(&input[offset], commands[i].name, str_len(commands[i].name))) {
            cmd = &commands[i];
            offset += str_len(cmd->name);
            while (input[offset] == ' ') ++offset;
            argc = arr_len(cmd->parameters);
            args = malloc(sizeof(argument_t) * argc);
            break;
        }
    }

    if (!cmd) {
        print("Error: Command not found.");
        return;
    }

    parameter_t *p;
    char last_space = offset - 1;
    argi = 0;

    for (i = offset; i <= l; i++) {
        if ((i < l && input[i] == ' ') || i == l) {
            if (argi == argc) break;
            p = &cmd->parameters[argi];
            args[argi].type = p->type;
            args[argi].str_val = str_nnew(&input[last_space + 1], i - last_space - 1);
            switch (p->type) {
                case PT_NUMBER:
                    args[argi].num_val = atof(args[argi].str_val);
                    break;
                default:
                    break;
            }
            last_space = i;
            ++argi;
        }
    }

    unsigned int m = argc;
    argc = argi - 1;

    for (; argi < m; argi++) {
        p = &cmd->parameters[argi];
        args[argi].type = p->type;
        args[argi].str_val = str_new(p->default_value);
    }

    cmd->callback(args, argc);

    for (i = 0; i < m; i++)
        str_del(args[i].str_val);

    free(args);
}