#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "vector.h"
#include "keycodes.h"
#include "str.h"

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef void (*event_callback_t)(void);

typedef struct {
    int id;
    event_callback_t callback;
    void *userdata;
} event_handler_t;

typedef struct {
    vec2_t dpi;
    int refresh_rate;
    struct { int mm, px; float in; } width, height;
    int blue_bits, red_bits, green_bits;
} display_t;

typedef struct {
    unsigned int fps_accumulator;
    double fps_start;
    double frametime_start;
    int client_x, client_y;
    unsigned int client_width, client_height;
    bool fullscreen, vsync, iconified;
} window_header_t;

typedef struct {
    GLFWwindow *handle;
    int x, y, width, height;
    unsigned int fps;
    double time, frametime;
    bool fullscreen, vsync, iconified;
    display_t display;
    event_handler_t *event_handlers;
    
} window_t;

typedef void (*window_close_callback_t)(void *userdata, window_t *window);
typedef void (*window_resize_callback_t)(void *userdata, window_t *window, unsigned int width, unsigned int height);
typedef void (*window_iconify_callback_t)(void *userdata, window_t *window, int iconify);
typedef void (*window_move_callback_t)(void *userdata, window_t *window, int x, int y);
typedef void (*key_callback_t)(void *userdata, int key, int scancode, int mods);
typedef void (*char_callback_t)(void *userdata, unsigned int codepoint, int mods);
typedef void (*mouse_button_callback_t)(void *userdata, int button, int mods);
typedef void (*cursor_move_callback_t)(void *userdata, int x, int y, float cx, float cy);
typedef void (*mouse_scroll_callback_t)(void *userdata, double x, double y);
typedef window_close_callback_t cursor_enter_leave_callback_t;
typedef window_close_callback_t window_refresh_callback_t;
typedef window_close_callback_t window_focus_callback_t;

#define EV_WINDOW_REFRESH       -1
#define EV_WINDOW_CLOSE         -2
#define EV_WINDOW_RESIZE        -3
#define EV_WINDOW_ICONIFY       -4
#define EV_WINDOW_MOVE          -5
#define EV_WINDOW_FOCUS         -6
#define EV_WINDOW_UNFOCUS       -7
#define EV_KEY_PRESS            -8
#define EV_KEY_RELEASE          -9
#define EV_KEY_CHAR             -10
#define EV_MOUSE_BUTTON_PRESS   -11
#define EV_MOUSE_BUTTON_RELEASE -12
#define EV_MOUSE_SCROLL         -13
#define EV_CURSOR_MOVE          -14
#define EV_CURSOR_ENTER         -15
#define EV_CURSOR_LEAVE         -16

// extern struct cursor_s {
// 	int x, y;
//     float cx, cy;
// } Cursor;

#define emit_event(window, event_id, callback_type, ...) do {\
    window_t *_w = window;\
    const int _id = event_id;\
    if (_w) for (unsigned int i = 0; i < arr_len(_w->event_handlers); i++)\
        if (_w->event_handlers[i].id == _id)\
            ((callback_type)_w->event_handlers[i].callback)(_w->event_handlers[i].userdata, __VA_ARGS__);\
    } while (0)
unsigned int _handle_event(window_t *window, int id, event_callback_t callback, void *userdata);
bool unhandle_event(window_t *window, unsigned int id);
#define handle_event(window, id, callback, userdata) _handle_event(window, id, (event_callback_t)callback, userdata)
//#define unhandle_event(window, i) _unhandle_event(window, i)

#define PRIMARY_MONITOR 0

display_t get_monitor_display(unsigned int monitor);
window_t* new_window(int width, int height, const char *title);
bool close_window(window_t*);
void delete_window(window_t **);
bool refresh_window(window_t*);
display_t get_window_display(window_t*);

//static inline double get_time() { return glfwGetTime(); }

//static inline void set_time(double t) { glfwSetTime(t); }

//static inline void set_window_position(int x, int y) {
//    glfwSetWindowPos(Window.handle, x, y);
//}

int get_key(int key);
int get_mouse_button(int button);

//static inline const char* get_key_name(int key) { return glfwGetKeyName(key, 0); }

const char* get_clipboard();

static inline void set_window_title(window_t *w, const char *title) {
    glfwSetWindowTitle(w->handle, title);
}

static inline void set_window_size(window_t *w, int width, int height) {
    glfwSetWindowSize(w->handle, width, height);
}

// static inline void set_window_size_min(int width, int height) {
//     if (Window.handle) glfwSetWindowSizeLimits(Window.handle, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE);
// }

// static inline void set_window_size_max(int width, int height) {
//     if (Window.handle) glfwSetWindowSizeLimits(Window.handle, GLFW_DONT_CARE, GLFW_DONT_CARE, width, height);
// }

#endif