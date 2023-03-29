#ifndef __UI_H__
#define __UI_H__

#include "str.h"
#include "text.h"
#include "color.h"
#include "window.h"
#include <stdbool.h>

#define UI_NONE   0
#define UI_LABEL  1
#define UI_BUTTON 2
#define UI_INPUT  3
#define UI_SLIDER 4
#define UI_LIST   5

typedef void* el_t;

typedef struct label_s {
    str_t title;
} ui_label_t;

typedef void (*ui_callback_t)(el_t);

typedef void (*ui_click_callback_t)(el_t, int cx, int cy);
typedef void (*ui_focus_callback_t)(el_t);
typedef void (*ui_activate_callback_t)(el_t);
typedef void (*ui_input_callback_t)(el_t, char);

typedef enum el_event_e {
    EL_CLICK_EVENT,
    EL_FOCUS_EVENT,
    EL_UNFOCUS_EVENT,
    EL_ACTIVATE_EVENT,
    EL_INPUT_EVENT
} el_event_t;

typedef struct button_s {
    str_t title;
    bool focused, active;
    ui_click_callback_t click_cb;
    ui_focus_callback_t focus_cb, unfocus_cb;
    ui_activate_callback_t activate_cb;
} ui_button_t;

typedef struct input_s {
    char value[128];
    str_t filter;
    unsigned char value_length;
    unsigned int max;
    bool focused, active;
    ui_click_callback_t click_cb;
    ui_focus_callback_t focus_cb, unfocus_cb;
    ui_activate_callback_t activate_cb;
    ui_input_callback_t input_cb;
} ui_input_t;

typedef struct slider_s {
    float value;
    bool focused;
} ui_slider_t;

#define EL_STACK_VERT 0
#define EL_STACK_HORI 1
#define EL_STACK_NONE 2

typedef struct list_s {
    el_t *elements;
    unsigned int element_count;
    unsigned char stacking;
    int item_width, item_height;
} ui_list_t;

typedef struct menu_s {
    str_t title;
    font_t *font, *title_font;
    unsigned int width, height;
    vec2_t position, origin;
    float text_size;
    int text_spacing;
    int depth;
    color_t background_color, foreground_color, focus_color, title_color;
    unsigned char border_size;
    el_t root_element;
    int last_cursor_x, last_cursor_y;
    bool mouse_moved;
    window_t *window;
} ui_menu_t;

#define EL_HIDDEN     (1 << 0)
#define EL_RELATIVE_X (1 << 1)
#define EL_RELATIVE_Y (1 << 2)
#define EL_RELATIVE_W (1 << 3)
#define EL_RELATIVE_H (1 << 4)
#define EL_RELATIVE_POSITION (EL_RELATIVE_X | EL_RELATIVE_Y)
#define EL_RELATIVE_SIZE (EL_RELATIVE_WIDTH | EL_RELATIVE_HEIGHT)
#define EL_RELATIVE_ALL (EL_RELATIVE_HEIGHT | EL_RELATIVE_HEIGHT | EL_RELATIVE_HEIGHT | EL_RELATIVE_HEIGHT)

#define UI_ALIGN_LEFT   -1
#define UI_ALIGN_TOP    -1
#define UI_ALIGN_CENTER 0
#define UI_ALIGN_RIGHT  1
#define UI_ALIGN_BOTTOM 1

typedef struct {
    unsigned char type;
    ui_menu_t *menu;
    float x, y, w, h;
    char mask;
    char text_align_x, text_align_y;
} el_header_t;

typedef enum {
    ELA_POSITION_X,
    ELA_POSITION_Y,
    ELA_WIDTH,
    ELA_HEIGHT,
    ELA_ALIGN_X,
    ELA_ALIGN_Y,
    ELA_TITLE,
    ELA_VALUE,
    ELA_FILTER,
    ELA_MIN,
    ELA_MAX,
    ELA_STACKING,
    ELA_ITEM_WIDTH,
    ELA_ITEM_HEIGHT
} el_attrib_t;

#define el_head(el) ((el_header_t*)((int8_t*)el - sizeof(el_header_t)))
#define el_type(el) (el_head(el)->type)
#define el_menu(el) (el_head(el)->menu)

el_t el_new(unsigned char type, float x, float y, float w, float h, char umask);
void _el_del(el_t *el);
#define el_del(el) _el_del(&el)

bool el_set_string(el_t el, el_attrib_t attrib, const char *val);
bool el_set_integer(el_t el, el_attrib_t attrib, int val);
bool el_set_float(el_t el, el_attrib_t attrib, float val);
bool _el_set_callback(el_t el, el_event_t event, ui_callback_t cb);
#define el_set_callback(el, event, cb) _el_set_callback(el, event, (ui_callback_t)cb)
bool el_set_focus(el_t el, bool focus);

const char* el_get_string(el_t el, el_attrib_t attrib);
int el_get_integer(el_t el, el_attrib_t attrib);
float el_get_float(el_t el, el_attrib_t attrib);
bool el_get_focus(el_t el);

int el_list_add(el_t list_el, el_t el);
el_t el_list_get(el_t el, int i);
el_t el_list_remove(el_t el, int i);
static inline void el_list_del(el_t el, int i) { el_t rm_el = el_list_remove(el, i); el_del(rm_el); }
el_t el_list_replace(el_t list_el, int i, el_t el);
static inline void el_list_set(el_t list_el, int i, el_t el) { el_t rm_el = el_list_replace(list_el, i, el); el_del(rm_el); }

char el_list_focus(ui_list_t *list, int i, char incr);
char el_list_focus_next(ui_list_t *list);
char el_list_focus_previous(ui_list_t *list);

ui_menu_t* create_menu(const char *title, window_t *window, font_t *title_font, font_t *item_font, float text_size, vec2_t position, unsigned int width, unsigned int height);
void delete_menu(ui_menu_t **menu);
void el_set_menu(el_t el, ui_menu_t *menu);
void set_menu_root(ui_menu_t *menu, el_t el);
void draw_menu(ui_menu_t *menu);
void update_menu(ui_menu_t *menu, int cursor_x, int cursor_y);

#endif