#include "ae/window.h"

#include "ae/utility.h"
#include "ae/array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static window_t **windows = NULL;

unsigned int _handle_event(window_t *w, int id, event_callback_t callback, void *userdata) {
    if (!w) return 0;
    if (!w->event_handlers)
        w->event_handlers = arr_new(sizeof(event_handler_t), 16, 8);
    arr_add(w->event_handlers, (event_handler_t){ id, callback, userdata });
    return arr_len(w->event_handlers) - 1;
}

bool unhandle_event(window_t *w, unsigned int i) {
    if (!w) return false;
    if (!w->event_handlers) return false;
    arr_remove(w->event_handlers, i);
    return true;
}

static display_t _get_display(GLFWmonitor *m) {
    const GLFWvidmode *vmode = glfwGetVideoMode(m);
    display_t d;
    glfwGetMonitorPhysicalSize(m, &d.width.mm, &d.height.mm);
    d.width.in = d.width.mm / 25.4f;
    d.height.in = d.height.mm / 25.4f;
    d.width.px = vmode->width;
    d.height.px = vmode->height;
    d.dpi.x = vmode->width / (double)(d.width.mm / 25.4);
    d.dpi.y = vmode->height / (double)(d.height.mm / 25.4);
    d.refresh_rate = vmode->refreshRate;
    d.blue_bits = vmode->blueBits;
    d.red_bits = vmode->redBits;
    d.green_bits = vmode->greenBits;
    return d;
}

display_t get_monitor_display(unsigned int n) {
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    if (n >= count) return (display_t){0};
    return _get_display(monitors[n]);

    // putlog("GRAPHICS", "Screen info:");
    // putlog(NULL, "\tResolution: %dpx, %dpx", vmode->width, vmode->height);
    // putlog(NULL, "\tRefresh rate: %dHz", vmode->refreshRate);
    // putlog(NULL, "\tPhysical size: %dmm x %dmm", Display.width.mm, Display.height.mm);
    // putlog(NULL, "\tDPI: %.2f x %.2f", Display.dpi.x, Display.dpi.y);
}

static inline window_t* get_window_from_handle(GLFWwindow *handle) {
    for (unsigned int i = 0; i < arr_len(windows); i++)
        if (windows[i]->handle == handle) return windows[i];
    return NULL;
}

static void glfw_error_cb(int error, const char* description) {
    puterr("GLFW", description, error);
}

static void key_cb(GLFWwindow *window, int key, int code, int action, int mods) {
    emit_event(get_window_from_handle(window), (action == GLFW_PRESS || action == GLFW_REPEAT) ? EV_KEY_PRESS : EV_KEY_RELEASE, key_callback_t, key, code, mods);
}

static void char_cb(GLFWwindow *window, unsigned int code) {
    emit_event(get_window_from_handle(window), EV_KEY_CHAR, char_callback_t, code, 0);
}

static void charmod_cb(GLFWwindow *window, unsigned int code, int mods) {
    emit_event(get_window_from_handle(window), EV_KEY_CHAR, char_callback_t, code, mods);
}

static void mouse_button_cb(GLFWwindow *window, int button, int action, int mods) {
    emit_event(get_window_from_handle(window), (action == GLFW_PRESS) ? EV_MOUSE_BUTTON_PRESS : EV_MOUSE_BUTTON_RELEASE, mouse_button_callback_t, button, mods);
}

static void mouse_scroll_cb(GLFWwindow *window, double x, double y) {
    emit_event(get_window_from_handle(window), EV_MOUSE_SCROLL, mouse_scroll_callback_t, x, y);
}

static void resize_cb(GLFWwindow *window, int width, int height) {
    window_t *w = get_window_from_handle(window);
    w->width = width;
    w->height = height;
    emit_event(w, EV_WINDOW_RESIZE, window_resize_callback_t, w, width, height);
}

static void move_cb(GLFWwindow *window, int x, int y) {
    window_t *w = get_window_from_handle(window);
    w->x = x;
    w->y = y;
    emit_event(w, EV_WINDOW_MOVE, window_move_callback_t, w, x, y);
}

static void iconify_cb(GLFWwindow *window, int iconify) {
    window_t *w = get_window_from_handle(window);
    w->iconified = iconify;
    emit_event(w, EV_WINDOW_ICONIFY, window_iconify_callback_t, w, iconify);
}

static void enter_leave_cb(GLFWwindow *window, int enter) {
    window_t *w = get_window_from_handle(window);
    emit_event(w, enter ? EV_CURSOR_ENTER : EV_CURSOR_LEAVE, cursor_enter_leave_callback_t, w);
}

static void cursor_position_cb(GLFWwindow *window, double x, double y) {
    window_t *w = get_window_from_handle(window);
    emit_event(w, EV_CURSOR_MOVE, cursor_move_callback_t, x, y, x / w->width, y / w->height);
}

static void close_cb(GLFWwindow *window) {
    window_t *w = get_window_from_handle(window);
    emit_event(w, EV_WINDOW_CLOSE, window_close_callback_t, w);
}

static void refresh_cb(GLFWwindow* window) {
    window_t *w = get_window_from_handle(window);
    emit_event(w, EV_WINDOW_REFRESH, window_refresh_callback_t, w);
}

static window_t *focused_window = NULL;

static void focus_cb(GLFWwindow* window, int focused) {
    window_t *w = get_window_from_handle(window);
    if (focused) focused_window = w;
    emit_event(w, focused ? EV_WINDOW_FOCUS : EV_WINDOW_UNFOCUS, window_focus_callback_t, w);
}

static bool initialized = false;

window_t* new_window(int width, int height, const char *title) {
    window_header_t *window_header = calloc(1, sizeof(window_t) + sizeof(window_header_t));
    window_t *window = (window_t*)((char*)window_header + sizeof(window_header_t));

    if (!initialized) {
        if (!glfwInit()) {
            puterr("SYS", "Failed to initialize GLFW.", 0);
            return NULL;
        }
    }

    window->display = _get_display(glfwGetPrimaryMonitor());

    glfwWindowHint(GLFW_RED_BITS, window->display.red_bits);
    glfwWindowHint(GLFW_GREEN_BITS, window->display.green_bits);
    glfwWindowHint(GLFW_BLUE_BITS, window->display.blue_bits);
    glfwWindowHint(GLFW_REFRESH_RATE, window->display.refresh_rate);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window->handle = glfwCreateWindow(width, height, title, NULL, NULL);

	if (!window->handle) {
        puterr("SYS", "Failed to create window.", 0);
		glfwTerminate();
		return NULL;
	}

    glfwSetErrorCallback(glfw_error_cb);
    glfwSetWindowIconifyCallback(window->handle, iconify_cb);
    glfwSetCursorEnterCallback(window->handle, enter_leave_cb);
    glfwSetKeyCallback(window->handle, key_cb);
    glfwSetCharCallback(window->handle, char_cb);
    glfwSetCharModsCallback(window->handle, charmod_cb);
    glfwSetWindowSizeCallback(window->handle, resize_cb);
    glfwSetWindowPosCallback(window->handle, move_cb);
    glfwSetMouseButtonCallback(window->handle, mouse_button_cb);
    glfwSetScrollCallback(window->handle, mouse_scroll_cb);
    glfwSetCursorPosCallback(window->handle, cursor_position_cb);
    glfwSetWindowCloseCallback(window->handle, close_cb);
    glfwSetWindowRefreshCallback(window->handle, refresh_cb);

    glfwSetInputMode(window->handle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    glfwGetWindowSize(window->handle, &window->width, &window->height);
    glfwGetWindowPos(window->handle, &window->x, &window->y);

    window_header->client_width = window->width;
    window_header->client_height = window->height;




    //putlog("GRAPHICS", "Creating default shader program...");

    //ae_shader_t *shader = aeShader.create();
    // if (!aeShader.update(shader, (char*)default_glsl)) {
    //     err = glGetError();
    //     puterr("GRAPHICS", "Failed to create default shader.", err, glewGetErrorString(err));
    //     return false;
    // }
    // else putlog("GRAPHICS", "Successfully created default shader program.");

    // putlog("GRAPHICS", "Creating window surface...");
    // Window.surface = calloc(1, sizeof(ae_surface_t));
    // Window.surface->shader = shader;
    // *(GLuint*)&Window.surface->framebuffer = 0;
    // Window.surface->color = aeColor.black;
    // Window.surface->texture = NULL;
    // *(bool*)&Window.surface->owns_texture = true;
    // Window.add_callback(AE_WINDOW_RESIZE, window_surface_resize_cb);
    // window_surface_resize_cb();

    // putlog("GRAPHICS", "Creating basic quad geometry...");
    // float vertices[] = { 0, 0, 0, 1, 1, 1, 1, 0 };
    // aeQuad = aeShape.create(vertices, 4);
    // if (aeQuad == NULL) {
    //     err = glGetError();
    //     puterr(NULL, "\tFailed to create quad. %s", err, glewGetErrorString(err));
    //     return false;
    // }
    // else putlog(NULL, "\tSuccess.");

    // we need a seperate function for this
    glfwMakeContextCurrent(window->handle);

    glewExperimental = 1;
    GLenum err;
	if ((err = glewInit()) != GLEW_OK) {
        puterr("SYS", "Failed to initialize GLEW.", err, glewGetErrorString(err));
        return false;
    }

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    if (!windows) windows = arr_new(sizeof(window_t*), 1, 1);
    arr_add(windows, window);

    if (glfwGetWindowAttrib(window->handle, GLFW_FOCUSED))
        focused_window = window;

    return window;
}

void delete_window(window_t **window) {
    window_header_t *wh = (window_header_t*)((char*)*window - sizeof(window_header_t));
    if ((*window)->event_handlers)
        arr_delete((*window)->event_handlers);
    for (unsigned int i = 0; i < arr_len(windows); i++) {
        if (windows[i] == *window) arr_remove(windows, i);
    }
    free(wh);
    *window = NULL;
}

bool close_window(window_t *window) {
    if (!window) return false;
	glfwDestroyWindow(window->handle);
    window->handle = NULL;
    return true;
}

bool refresh_window(window_t *window) {
    if (!(window && window->handle)) return false;
    window_header_t *wh = (window_header_t*)((char*)window - sizeof(window_header_t));

    if (window->fullscreen != wh->fullscreen) {
        if (window->fullscreen) {
            wh->client_width = window->width;
            wh->client_height = window->height;
            wh->client_x = window->x;
            wh->client_y = window->y;
            glfwSetWindowMonitor(
                window->handle,
                glfwGetWindowMonitor(window->handle),
                0, 0,
                window->display.width.px,
                window->display.height.px,
                window->display.refresh_rate
            );
        }
        else {
            glfwSetWindowMonitor(
                window->handle, NULL,
                wh->client_x, wh->client_y,
                wh->client_width, wh->client_height,
                window->display.refresh_rate
            );
        }
        wh->fullscreen = window->fullscreen;
    }

    if (window->vsync != wh->vsync) {
        glfwSwapInterval(window->vsync);
        wh->vsync = window->vsync;
    }

    if (window->iconified != wh->iconified) {
        if (window->iconified) glfwIconifyWindow(window->handle);
        else glfwRestoreWindow(window->handle);
        wh->iconified = window->iconified;
    }

    glfwSwapBuffers(window->handle);
    glfwPollEvents();
    if (!(window && window->handle)) return false;
	window->time = glfwGetTime();
	window->frametime = (double)(window->time - wh->frametime_start);
	wh->frametime_start = window->time;
	if (window->time - wh->fps_start >= 1) {
		window->fps = wh->fps_accumulator;
		wh->fps_accumulator = 0;
		wh->fps_start = window->time;
	}
	++wh->fps_accumulator;
    return true;
}

int get_key(int key) {
    return focused_window ? glfwGetKey(focused_window->handle, key) : KEY_RELEASE;
}

int get_mouse_button(int button) {
    return focused_window ? glfwGetMouseButton(focused_window->handle, button) : BUTTON_RELEASE;
}

const char* get_clipboard() {
    return focused_window ? glfwGetClipboardString(focused_window->handle) : NULL;
}

// bool set_window_icon(ae_image_t *icon) {
//     if (!icon) return false;
//     GLFWimage img = {
//         icon->width,
//         icon->height,
//         icon->data
//     };
//     glfwSetWindowIcon(Window.handle, 1, &img);
//     return true;
// }