#include "ae/ui.h"

#include "ae/math.h"
#include "ae/utility.h"

#include <GLFW/glfw3.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

el_t el_new(unsigned char type, float x, float y, float w, float h, char umask) {
    unsigned int size;

    switch (type) {
        case UI_LABEL:  size = sizeof(ui_label_t);  break;
        case UI_BUTTON: size = sizeof(ui_button_t); break;
        case UI_INPUT:  size = sizeof(ui_input_t);  break;
        case UI_SLIDER: size = sizeof(ui_slider_t); break;
        case UI_LIST:   size = sizeof(ui_list_t);   break;
        case UI_NONE:
        default: return NULL;
    }

    el_header_t *head = calloc(1, size + sizeof(el_header_t));
    head->type = type;
    head->x = x;
    head->y = y;
    head->w = w;
    head->h = h;
    head->mask = umask;
    return (int8_t*)head + sizeof(el_header_t);
}

void _el_del(el_t *el) {
    if (!(el && *el)) return;
    const unsigned char type = el_type(*el);
    if (type == UI_LIST) {
        ui_list_t *list = *el;
        unsigned int i = 0;
        for (; i < list->element_count; i++)
            el_del(list->elements[i]);
    }
    free((int8_t*)*el - sizeof(el_header_t));
    *el = NULL;
}


bool el_set_string(el_t el, el_attrib_t attrib, const char *val) {
    if (!el) return false;
    const unsigned int type = el_type(el);

    switch (attrib) {
        case ELA_POSITION_X: {
                float x;
                char unit;
                int s = sscanf(val, "%f%c", &x, &unit);
                el_head(el)->x = x;
                if (s > 1 && unit == '%')
                    el_head(el)->mask |= EL_RELATIVE_X;
                else el_head(el)->mask &= ~EL_RELATIVE_X;
                return true;
            }
            break;
        case ELA_POSITION_Y: {
                float y;
                char unit;
                int s = sscanf(val, "%f%c", &y, &unit);
                el_head(el)->y = y;
                if (s > 1 && unit == '%')
                    el_head(el)->mask |= EL_RELATIVE_Y;
                else el_head(el)->mask &= ~EL_RELATIVE_Y;
                return true;
            }
            break;
        case ELA_WIDTH: {
                float width;
                char unit;
                int s = sscanf(val, "%f%c", &width, &unit);
                el_head(el)->w = width;
                if (s > 1 && unit == '%')
                    el_head(el)->mask |= EL_RELATIVE_W;
                else el_head(el)->mask &= ~EL_RELATIVE_W;
                return true;
            }
            break;
        case ELA_HEIGHT: {
                float height;
                char unit;
                int s = sscanf(val, "%f%c", &height, &unit);
                el_head(el)->h = height;
                if (s > 1 && unit == '%')
                    el_head(el)->mask |= EL_RELATIVE_H;
                else el_head(el)->mask &= ~EL_RELATIVE_H;
                return true;
            }
            break;
        case ELA_ALIGN_X:
            if (val[0] == 'l' || val[0] == 'L') {
                el_head(el)->text_align_x = UI_ALIGN_LEFT;
                return true;
            }
            else if (val[0] == 'c' || val[0] == 'C') {
                el_head(el)->text_align_x = UI_ALIGN_CENTER;
                return true;
            }
            else if (val[0] == 'r' || val[0] == 'R') {
                el_head(el)->text_align_x = UI_ALIGN_RIGHT;
                return true;
            }
            break;
        case ELA_ALIGN_Y:
            if (val[0] == 't' || val[0] == 'T') {
                el_head(el)->text_align_x = UI_ALIGN_TOP;
                return true;
            }
            else if (val[0] == 'c' || val[0] == 'C') {
                el_head(el)->text_align_x = UI_ALIGN_CENTER;
                return true;
            }
            else if (val[0] == 'b' || val[0] == 'B') {
                el_head(el)->text_align_x = UI_ALIGN_BOTTOM;
                return true;
            }
            break;
        case ELA_TITLE:
            if (type == UI_LABEL || type == UI_BUTTON) {
                str_set(((ui_label_t*)el)->title, val);
                return true;
            }
            break;
        case ELA_VALUE:
            if (type == UI_INPUT) {
                //str_set(((ui_input_t*)el)->value, val);
                ((ui_input_t*)el)->value_length = strlen(val);
                memcpy(((ui_input_t*)el)->value, val, ((ui_input_t*)el)->value_length + 1);
                return true;
            }
            else if (type == UI_SLIDER) {
                sscanf(val, "%f", &((ui_slider_t*)el)->value);
                return true;
            }
            break;
        case ELA_FILTER:
            if (type == UI_INPUT) {
                str_set(((ui_input_t*)el)->filter, val);
                return true;
            }
            break;
        case ELA_STACKING:
            if (type == UI_LIST) {
                if (val[0] == 'v' || val[0] == 'V' || val[0] == 'y' || val[0] == 'Y') {
                    ((ui_list_t*)el)->stacking = EL_STACK_VERT;
                    return true;
                }
                else if (val[0] == 'h' || val[0] == 'H' || val[0] == 'x' || val[0] == 'X') {
                    ((ui_list_t*)el)->stacking = EL_STACK_HORI;
                    return true;
                }
                else if (val[0] == 'n' || val[0] == 'N') {
                    ((ui_list_t*)el)->stacking = EL_STACK_NONE;
                    return true;
                }
            }
            break;
        case ELA_ITEM_WIDTH:
            if (type == UI_LIST) {
                int width;
                char unit;
                int s = sscanf(val, "%d%c", &width, &unit);
                if (s > 1 && unit == '%')
                    ((ui_list_t*)el)->item_width = (int)((width / 100.0f) * el_head(el)->w);
                else ((ui_list_t*)el)->item_width = width;
                return true;
            }
            break;
        case ELA_ITEM_HEIGHT:
            if (type == UI_LIST) {
                int height;
                char unit;
                int s = sscanf(val, "%d%c", &height, &unit);
                if (s > 1 && unit == '%')
                    ((ui_list_t*)el)->item_height = (int)((height / 100.0f) * el_head(el)->h);
                else ((ui_list_t*)el)->item_height = height;
                return true;
            }
            break;
        //case EL_MIN: // TODO
        case ELA_MAX:
            if (type == UI_INPUT) {
                int max;
                sscanf(val, "%d", &max);
                ((ui_input_t*)el)->max = max;
                return true;
            }
            break;
        default: break;
    }

    return false;
}

bool el_set_integer(el_t el, el_attrib_t attrib, int val) {
    if (!el) return false;
    const unsigned int type = el_type(el);

    switch (attrib) {
        case ELA_POSITION_X: el_head(el)->x = val; break;
        case ELA_POSITION_Y: el_head(el)->y = val; break;
        case ELA_WIDTH: el_head(el)->w = val; break;
        case ELA_HEIGHT: el_head(el)->h = val; break;
        case ELA_ALIGN_X: el_head(el)->text_align_x = val; break;
        case ELA_ALIGN_Y: el_head(el)->text_align_y = val; break;
        case ELA_TITLE:
            if (type == UI_LABEL || type == UI_BUTTON) {
                char buf[12] = {0};
                sprintf(buf, "%d", val);
                str_set(((ui_label_t*)el)->title, buf);
                return true;
            }
            break;
        case ELA_VALUE:
            if (type == UI_INPUT) {
                char buf[12] = {0};
                sprintf(buf, "%d", val);
                ((ui_input_t*)el)->value_length = strlen(buf);
                memcpy(((ui_input_t*)el)->value, buf, ((ui_input_t*)el)->value_length + 1);
                //str_set(((ui_input_t*)el)->value, buf);
                return true;
            }
            else if (type == UI_SLIDER) {
                ((ui_slider_t*)el)->value = val;
                return true;
            }
            break;
        case ELA_FILTER:
            if (type == UI_INPUT) {
                char buf[12] = {0};
                sprintf(buf, "%d", val);
                str_set(((ui_input_t*)el)->filter, buf);
                return true;
            }
            break;
        case ELA_STACKING:
            if (type == UI_LIST) {
                if (val <= EL_STACK_NONE) {
                    ((ui_list_t*)el)->stacking = val;
                    return true;
                }
            }
            break;
        case ELA_ITEM_WIDTH:
            if (type == UI_LIST) {
                ((ui_list_t*)el)->item_width = val;
                return true;
            }
            break;
        case ELA_ITEM_HEIGHT:
            if (type == UI_LIST) {
                ((ui_list_t*)el)->item_height = val;
                return true;
            }
            break;
       // case EL_MIN: // TODO
       case ELA_MAX:
            if (type == UI_INPUT) {
                ((ui_input_t*)el)->max = val;
                return true;
            }
            break;
       default: break;
    }

    return false;
}

bool el_set_float(el_t el, el_attrib_t attrib, float val) {
    if (!el) return false;
    const unsigned int type = el_type(el);

    switch (attrib) {
        case ELA_POSITION_X: el_head(el)->x = val; break;
        case ELA_POSITION_Y: el_head(el)->y = val; break;
        case ELA_WIDTH: el_head(el)->w = val; break;
        case ELA_HEIGHT: el_head(el)->h = val; break;
        case ELA_ALIGN_X: el_head(el)->text_align_x = val; break;
        case ELA_ALIGN_Y: el_head(el)->text_align_y = val; break;
        case ELA_TITLE:
            if (type == UI_LABEL || type == UI_BUTTON) {
                char buf[48] = {0};
                sprintf(buf, "%f", val);
                str_set(((ui_label_t*)el)->title, buf);
                return true;
            }
            break;
        case ELA_VALUE:
            if (type == UI_INPUT) {
                char buf[48] = {0};
                sprintf(buf, "%f", val);
                //str_set(((ui_input_t*)el)->value, buf);
                ((ui_input_t*)el)->value_length = strlen(buf);
                memcpy(((ui_input_t*)el)->value, buf, ((ui_input_t*)el)->value_length + 1);
                return true;
            }
            else if (type == UI_SLIDER) {
                ((ui_slider_t*)el)->value = val;
                return true;
            }
            break;
        case ELA_FILTER:
            if (type == UI_INPUT) {
                char buf[48] = {0};
                sprintf(buf, "%f", val);
                str_set(((ui_input_t*)el)->filter, buf);
                return true;
            }
            break;
        case ELA_STACKING:
            if (type == UI_LIST) {
                if ((int)val <= EL_STACK_NONE) {
                    ((ui_list_t*)el)->stacking = (int)val;
                    return true;
                }
            }
            break;
        case ELA_ITEM_WIDTH:
            if (type == UI_LIST) {
                ((ui_list_t*)el)->item_width = (int)val;
                return true;
            }
            break;
        case ELA_ITEM_HEIGHT:
            if (type == UI_LIST) {
                ((ui_list_t*)el)->item_height = (int)val;
                return true;
            }
            break;
        //case EL_MIN: // TODO
        case ELA_MAX:
            if (type == UI_INPUT) {
                ((ui_input_t*)el)->max = (int)val;
                return true;
            }
            break;
        default: break;
    }

    return false;
}

bool _el_set_callback(el_t el, el_event_t event, ui_callback_t cb) {
    switch (event) {
        case EL_FOCUS_EVENT:
            if (el_type(el) == UI_BUTTON)
                ((ui_button_t*)el)->focus_cb = (ui_focus_callback_t)cb;
            else if (el_type(el) == UI_INPUT)
                ((ui_input_t*)el)->focus_cb = (ui_focus_callback_t)cb;
            else return false;
            return true;
        case EL_UNFOCUS_EVENT:
            if (el_type(el) == UI_BUTTON)
                ((ui_button_t*)el)->unfocus_cb = (ui_focus_callback_t)cb;
            else if (el_type(el) == UI_INPUT)
                ((ui_input_t*)el)->unfocus_cb = (ui_focus_callback_t)cb;
            else return false;
            return true;
        case EL_CLICK_EVENT:
            if (el_type(el) == UI_BUTTON)
                ((ui_button_t*)el)->click_cb = (ui_click_callback_t)cb;
            else if (el_type(el) == UI_INPUT)
                ((ui_input_t*)el)->click_cb = (ui_click_callback_t)cb;
            else return false;
            return true;
        case EL_ACTIVATE_EVENT:
            if (el_type(el) == UI_BUTTON)
                ((ui_button_t*)el)->activate_cb = (ui_activate_callback_t)cb;
            else if (el_type(el) == UI_INPUT)
                ((ui_input_t*)el)->activate_cb = (ui_activate_callback_t)cb;
            else return false;
            return true;
        case EL_INPUT_EVENT:
            if (el_type(el) == UI_INPUT)
                ((ui_input_t*)el)->input_cb = (ui_input_callback_t)cb;
            else return false;
            return true;
        default: break;
    }
    return false;
}

bool el_set_focus(el_t el, bool focus) {
    if (!el) return false;
    const unsigned int type = el_type(el);
    switch (type) {
        case UI_BUTTON:
            ((ui_button_t*)el)->focused = focus;
            return true;
        case UI_INPUT:
            ((ui_input_t*)el)->focused = focus;
            return true;
        default: break;
    }
    return false;
}

static char el_get_string_buffer[128];

const char* el_get_string(el_t el, el_attrib_t attrib) {
    if (!el) return false;
    const unsigned int type = el_type(el);

    switch (attrib) {
        case ELA_POSITION_X: 
            sprintf(
                el_get_string_buffer, "%f%s", el_head(el)->x,
                (el_head(el)->mask & EL_RELATIVE_X) ? "%" : ""
            );
            return el_get_string_buffer;
        case ELA_POSITION_Y:
            sprintf(
                el_get_string_buffer, "%f%s", el_head(el)->y,
                (el_head(el)->mask & EL_RELATIVE_Y) ? "%" : ""
            );
            return el_get_string_buffer;
        case ELA_WIDTH:
            sprintf(
                el_get_string_buffer, "%f%s", el_head(el)->w,
                (el_head(el)->mask & EL_RELATIVE_W) ? "%" : ""
            );
            return el_get_string_buffer;
        case ELA_HEIGHT:
            sprintf(
                el_get_string_buffer, "%f%s", el_head(el)->h,
                (el_head(el)->mask & EL_RELATIVE_H) ? "%" : ""
            );
            return el_get_string_buffer;
        case ELA_ALIGN_X: {
                char a = el_head(el)->text_align_x;
                sprintf(
                    el_get_string_buffer, "%s", a < 0 ? "left" : a > 0 ? "right" : "center"
                );
                return el_get_string_buffer;
            } 
        case ELA_ALIGN_Y: {
                char a = el_head(el)->text_align_y;
                sprintf(
                    el_get_string_buffer, "%s", a < 0 ? "top" : a > 0 ? "bottom" : "center"
                );
                return el_get_string_buffer;
            }
        case ELA_TITLE:
            if (type == UI_LABEL || type == UI_BUTTON)
                return ((ui_label_t*)el)->title;
            break;
        case ELA_VALUE:
            if (type == UI_INPUT) {
                return ((ui_input_t*)el)->value;
            }
            else if (type == UI_SLIDER) {
                sprintf(el_get_string_buffer, "%f", ((ui_slider_t*)el)->value);
                return el_get_string_buffer;
            }
            break;
        case ELA_FILTER:
            if (type == UI_INPUT) {
                return ((ui_input_t*)el)->filter;
            }
            break;
        case ELA_STACKING:
            if (type == UI_LIST) {
                unsigned char s = ((ui_list_t*)el)->stacking;
                sprintf(
                    el_get_string_buffer, "%s",
                    s == EL_STACK_VERT ? "vertical" : s == EL_STACK_HORI ? "horizontal" : "none"
                );
                return el_get_string_buffer;
            }
            break;
        case ELA_ITEM_WIDTH:
            if (type == UI_LIST) {
                sprintf(el_get_string_buffer, "%d", ((ui_list_t*)el)->item_width);
                return el_get_string_buffer;
            }
            break;
        case ELA_ITEM_HEIGHT:
            if (type == UI_LIST) {
                sprintf(el_get_string_buffer, "%d", ((ui_list_t*)el)->item_height);
                return el_get_string_buffer;
            }
            break;
        //case EL_MIN: // TODO
        case ELA_MAX:
            if (type == UI_INPUT) {
                sprintf(el_get_string_buffer, "%u", ((ui_input_t*)el)->max);
                return el_get_string_buffer;
            }
            break;
        default: break;
    }

    return NULL;
}

int el_get_integer(el_t el, el_attrib_t attrib) {
    if (!el) return false;
    const unsigned int type = el_type(el);

    switch (attrib) {
        case ELA_POSITION_X: return el_head(el)->x;
        case ELA_POSITION_Y: return el_head(el)->y;
        case ELA_WIDTH: return el_head(el)->w;
        case ELA_HEIGHT: return el_head(el)->h;
        case ELA_ALIGN_X: return el_head(el)->text_align_x;
        case ELA_ALIGN_Y: return el_head(el)->text_align_y;
        case ELA_TITLE:
            if (type == UI_LABEL || type == UI_BUTTON)
                return atoi(((ui_label_t*)el)->title);
            break;
        case ELA_VALUE:
            if (type == UI_INPUT)
                return atoi(((ui_input_t*)el)->value);
            else if (type == UI_SLIDER)
                return ((ui_slider_t*)el)->value;
            break;
        case ELA_FILTER:
            if (type == UI_INPUT)
                return atoi(((ui_input_t*)el)->filter);
            break;
        case ELA_STACKING:
            if (type == UI_LIST)
                return ((ui_list_t*)el)->stacking;
            break;
        case ELA_ITEM_WIDTH:
            if (type == UI_LIST)
                return ((ui_list_t*)el)->item_width;
            break;
        case ELA_ITEM_HEIGHT:
            if (type == UI_LIST)
                return ((ui_list_t*)el)->item_height;
            break;
        //case EL_MIN: // TODO
        case ELA_MAX:
            if (type == UI_INPUT)
                return ((ui_input_t*)el)->max;
            break;
        default: break;
    }

    return 0;
}

float el_get_float(el_t el, el_attrib_t attrib) {
    if (!el) return false;
    const unsigned int type = el_type(el);

    switch (attrib) {
        case ELA_POSITION_X: return el_head(el)->x;
        case ELA_POSITION_Y: return el_head(el)->y;
        case ELA_WIDTH: return el_head(el)->w;
        case ELA_HEIGHT: return el_head(el)->h;
        case ELA_ALIGN_X: return el_head(el)->text_align_x;
        case ELA_ALIGN_Y: return el_head(el)->text_align_y;
        case ELA_TITLE:
            if (type == UI_LABEL || type == UI_BUTTON)
                return atof(((ui_label_t*)el)->title);
            break;
        case ELA_VALUE:
            if (type == UI_INPUT)
                return atof(((ui_input_t*)el)->value);
            else if (type == UI_SLIDER)
                return ((ui_slider_t*)el)->value;
            break;
        case ELA_FILTER:
            if (type == UI_INPUT)
                return atof(((ui_input_t*)el)->filter);
            break;
        case ELA_STACKING:
            if (type == UI_LIST)
                return ((ui_list_t*)el)->stacking;
            break;
        case ELA_ITEM_WIDTH:
            if (type == UI_LIST)
                return ((ui_list_t*)el)->item_width;
            break;
        case ELA_ITEM_HEIGHT:
            if (type == UI_LIST)
                return ((ui_list_t*)el)->item_height;
            break;
        //case ELA_MIN: // TODO
        case ELA_MAX:
            if (type == UI_INPUT)
                return ((ui_input_t*)el)->max;
            break;
        default: break;
    }

    return 0;
}

bool el_get_focus(el_t el) {
    if (!el) return false;
    const unsigned int type = el_type(el);
    switch (type) {
        case UI_BUTTON:
            return ((ui_button_t*)el)->focused;
        case UI_INPUT:
            return ((ui_input_t*)el)->focused;
        default: return false;
    }
}

int el_list_add(el_t list_el, el_t el) {
    if (!(list_el && el && el_type(list_el) == UI_LIST)) return -1;

    ui_list_t *list = list_el;
    list->elements = realloc(list->elements, sizeof(el_t) * ++list->element_count);
    list->elements[list->element_count - 1] = el;
    el_set_menu(el, el_menu(list_el));

    return list->element_count - 1;
}

el_t el_list_get(el_t el, int i) {
    if (!(el && el_type(el) == UI_LIST)) return NULL;
    ui_list_t *list = el;
    if (i < 0) i = list->element_count - i;
    if (i >= list->element_count) return NULL;
    return list->elements[i];
}

el_t el_list_remove(el_t el, int i) {
    if (!(el && el_type(el) == UI_LIST)) return NULL;

    ui_list_t *list = el;
    if (i < 0) i = list->element_count - i;
    el_t rm_el = el_list_get(el, i);
    if (!rm_el) return NULL;

    for (++i; (unsigned)i < list->element_count; i++) {
        list->elements[(unsigned)i - 1] = list->elements[(unsigned)i];
    }

    list->elements = realloc(list->elements, sizeof(el_t) * --list->element_count);

    return rm_el;
}

el_t el_list_replace(el_t list_el, int i, el_t el) {
    if (!(list_el && el_type(list_el) == UI_LIST)) return NULL;

    ui_list_t *list = list_el;
    if (i < 0) i = list->element_count - i;
    if (i >= list->element_count) return NULL;
    
    el_t old_el = el_list_remove(list_el, i);
    list->elements[i] = el;
    return old_el;
}


static GLFWcursor *pointer_cursor;
static GLFWcursor *caret_cursor;
static ui_input_t *active_input = NULL;
#ifndef CARET_BLINK_RATE
#define CARET_BLINK_RATE 60
#endif
static unsigned char caret_blink = 0;
static unsigned int input_caret = 0;

char el_list_focus(ui_list_t *list, int _i, char direction) {
    int i = _i;
    if (i < 0) i = list->element_count + i;
    if ((unsigned)i >= list->element_count) i -= list->element_count;

    if (el_type(list->elements[(unsigned)i]) == UI_LABEL) {
        if (!direction || _i + direction < 0 || (unsigned)(_i + direction) >= list->element_count)
            return -1;
        return el_list_focus(list, i + direction, direction);
    }
    if (el_type(list->elements[(unsigned)i]) == UI_LIST)
        return el_list_focus(list->elements[(unsigned)i], direction == -1 ? -1 : 0, direction);

    return el_set_focus(list->elements[(unsigned)i], true);
}

char el_list_focus_next(ui_list_t *list) {
    unsigned int i = 0;
    for (; i < list->element_count; i++) {
        if (el_type(list->elements[i]) == UI_LIST) {
            char s = el_list_focus_next(list->elements[i]);
            if (s == -1) return el_list_focus(list, i + 1, 1);
            if (s == true) return true;
        }
        else if (el_get_focus(list->elements[i])) {
            el_set_focus(list->elements[i], false);
            return el_list_focus(list, i + 1, 1);
        }
    }
    return false;
}

char el_list_focus_previous(ui_list_t *list) {
    unsigned int i = 0;
    for (; i < list->element_count; i++) {
        if (el_type(list->elements[i]) == UI_LIST) {
            char s = el_list_focus_previous(list->elements[i]);
            if (s == -1) return el_list_focus(list, (int)i - 1, -1);
            if (s == true) return true;
        }
        else if (el_get_focus(list->elements[i])) {
            el_set_focus(list->elements[i], false);
            return el_list_focus(list, (int)i - 1, -1);
        }
    }
    return false;
}

static void on_keyboard_press(void *userdata, int key, int scancode, int mods) {
    ui_menu_t *menu = userdata;
    if (!active_input) return;
    if (key == KEY_RIGHT) {
        if (input_caret < active_input->value_length)
            input_caret += 1;
        caret_blink = 0;
    }
    else if (key == KEY_LEFT) {
        if (input_caret > 0) input_caret -= 1;
        caret_blink = 0;
    }
    else if (key == KEY_BACKSPACE && input_caret > 0) {
        unsigned int i = input_caret - 1;
        for (; i < active_input->value_length; i++) active_input->value[i] = active_input->value[i + 1];
        active_input->value_length -= 1;
        input_caret -= 1;
        caret_blink = 0;
    }
    else if ((isascii(key) && isprint(key)) || (mods & KEY_MOD_NUM_LOCK && key >= KEY_KP_0 && key <= KEY_KP_9)) {
        if (key == KEY_V && (mods & KEY_MOD_CONTROL)) {
            const char *clipboard = get_clipboard();
            unsigned int i, l = strlen(clipboard);
            for (i = 0; i < l; i++)
                on_keyboard_press(userdata, clipboard[i], clipboard[i], 0);
            return;
        }

        bool caps = mods & KEY_MOD_CAPS_LOCK;
        if (mods & KEY_MOD_SHIFT) caps = !caps;
        int c = key;

        if (isalpha(key)) {
            c = caps ? key : tolower(key);
        }
        else if (caps) {
            switch (key) {
                case '`': c = '~'; break;
                case '1': c = '!'; break;
                case '2': c = '@'; break;
                case '3': c = '#'; break;
                case '4': c = '$'; break;
                case '5': c = '%'; break;
                case '6': c = '^'; break;
                case '7': c = '&'; break;
                case '8': c = '*'; break;
                case '9': c = '('; break;
                case '0': c = ')'; break;
                case '-': c = '_'; break;
                case '=': c = '+'; break;
                case '[': c = '{'; break;
                case ']': c = '}'; break;
                case '\\': c = '|'; break;
                case ';': c = ':'; break;
                case '\'': c = '"'; break;
                case ',': c = '<'; break;
                case '.': c = '>'; break;
                case '/': c = '?'; break;
            }
        }
        else if (mods & KEY_MOD_NUM_LOCK) {
            switch (key) {
                case KEY_KP_0: c = '0'; break;
                case KEY_KP_1: c = '1'; break;
                case KEY_KP_2: c = '2'; break;
                case KEY_KP_3: c = '3'; break;
                case KEY_KP_4: c = '4'; break;
                case KEY_KP_5: c = '5'; break;
                case KEY_KP_6: c = '6'; break;
                case KEY_KP_7: c = '7'; break;
                case KEY_KP_8: c = '8'; break;
                case KEY_KP_9: c = '9'; break;
            }
        }

        int s = 0, i;
        if (active_input->filter) {
            i = str_len(active_input->filter) - 1;
            while (i >= 0 && s == 0)
                if (active_input->filter[i--] == c) s = 1;
        }
        else s = 1;
        if (s && active_input->max) s = active_input->value_length < active_input->max;
        if (s) {
            for (i = active_input->value_length; i > input_caret; --i)
                active_input->value[i] = active_input->value[i - 1];
            active_input->value[input_caret] = c;
            active_input->value_length += 1;
            input_caret += 1;
            caret_blink = 0;
            if (active_input->input_cb) active_input->input_cb((el_t)active_input, c);
        }
    }
}

ui_menu_t* create_menu(const char *title, window_t *window, font_t *title_font, font_t *item_font, float text_size, vec2_t position, unsigned int width, unsigned int height) {
    ui_menu_t *menu = malloc(sizeof(ui_menu_t));
    menu->font = item_font;
    menu->title_font = title_font;
    menu->title = str_new(title);
    menu->position = position;
    menu->width = width;
    menu->height = height;
    menu->root_element = NULL;
    menu->origin = vec2(0, 0);
    menu->text_size = text_size;
    menu->depth = 0;
    menu->background_color = C_GRAY;
    menu->foreground_color = C_BLACK;
    menu->focus_color = C_WHITE;
    menu->border_size = 2;
    menu->window = window;
    if (!pointer_cursor) pointer_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    if (!caret_cursor) caret_cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    handle_event(window, EV_KEY_PRESS, on_keyboard_press, menu);
    return menu;
}

void delete_menu(ui_menu_t **menu) {
    if (!(menu && *menu)) return;
    str_del((*menu)->title);
    el_del((*menu)->root_element);
    free(*menu);
    *menu = NULL;
}

void el_set_menu(el_t el, ui_menu_t *menu) {
    if (!el) return;
    el_menu(el) = menu;
    if (el_type(el) == UI_LIST) {
        ui_list_t *list = el;
        unsigned int i = 0;
        for (; i < list->element_count; i++)
            el_set_menu(list->elements[i], menu);
    }
}

void set_menu_root(ui_menu_t *menu, el_t el) {
    if (!menu) return;
    if (!el) el_set_menu(menu->root_element, NULL);
    else el_set_menu(el, menu);
    menu->root_element = el;
}

vec4_t el_get_metrics(el_t el, float container_width, float container_height) {
    vec4_t r = vec4(
        el_head(el)->x,
        el_head(el)->y,
        el_head(el)->w,
        el_head(el)->h
    );
    char mask = el_head(el)->mask;
    if (r.z < 1) r.z = container_width + r.z;
    if (r.w < 1) r.w = container_height + r.w;
    if (mask & EL_RELATIVE_X) r.x = (r.x / 100.0f) * container_width;
    if (mask & EL_RELATIVE_Y) r.y = (r.y / 100.0f) * container_height;
    if (mask & EL_RELATIVE_W) r.z = (r.z / 100.0f) * container_width;
    if (mask & EL_RELATIVE_H) r.w = (r.w / 100.0f) * container_height;
    return r;
}

void draw_element(el_t el, vec2_t offset, int depth, float container_width, float container_height) {
    ui_menu_t *menu = el_menu(el);
    if (!menu) return;
    if (el_head(el)->mask & EL_HIDDEN) return;

    vec4_t met = el_get_metrics(el, container_width, container_height);
    vec2_t pos = vec2(met.x + offset.x, met.y + offset.y);
    char align_x = el_head(el)->text_align_x;
    char align_y = el_head(el)->text_align_y;
    vec2_t text_pos = vec2(
        pos.x + (align_x < 0 ? 0 : align_x > 0 ? met.z : (met.z / 2.0f)),
        pos.y + (align_y < 0 ? 0 : align_y > 0 ? met.w : (met.w / 2.0f))
    );
    vec2_t text_origin = vec2(
        align_x < 0 ? 0 : align_x > 0 ? 1 : 0.5f,
        align_y < 0 ? 0 : align_y > 0 ? 1 : 0.5f
    );

    switch (el_type(el)) {
        case UI_LABEL: case UI_BUTTON: {
                ui_label_t *label = el;
                const bool focused = (el_type(el) == UI_BUTTON) ? ((ui_button_t*)el)->focused : false;
                if (label->title) {
                    draw_text(menu->font, label->title, str_len(label->title), menu->text_spacing, menu->text_size, text_origin, text_pos, depth - 2, focused ? menu->focus_color : menu->foreground_color);
                }
                draw_polygon(quad_polygon, vec2(0, 0), focused ? vec2(pos.x + menu->border_size, pos.y + menu->border_size) : pos, depth - 1, met.z - (focused ? menu->border_size : 0) * 2, met.w - (focused ? menu->border_size : 0) * 2, menu->background_color);
                if (focused) draw_polygon(quad_polygon, vec2(0, 0), pos, depth, met.z, met.w, menu->focus_color);
            }
            break;
        case UI_INPUT: {
                ui_input_t *input = el;
                color_t c = (input->focused && !input->active) ? menu->focus_color : menu->foreground_color;
                vec2_t bbox = draw_text(menu->font, input->value, input->value_length, menu->text_spacing, menu->text_size, text_origin, text_pos, depth - 2, c);
                if (input->active && (caret_blink < CARET_BLINK_RATE / 2)) {
                    vec2_t r = get_text_size(menu->font, input->value, input_caret, menu->text_size, menu->text_spacing);
                    draw_polygon(quad_polygon, vec2(0, 0), vec2(text_pos.x - (bbox.y * text_origin.x) + r.y, text_pos.y), depth - 3, 1, bbox.y, c);
                }
                draw_polygon(quad_polygon, vec2(0, 0), input->focused ? vec2(pos.x + menu->border_size, pos.y + menu->border_size) : pos, depth - 1, met.z - (input->focused ? menu->border_size : 0) * 2, met.w - (input->focused ? menu->border_size : 0) * 2, menu->focus_color);
                if (input->focused) draw_polygon(quad_polygon, vec2(0, 0), pos, depth, met.z, met.w, menu->focus_color);
            }
            break;
        case UI_LIST: {
                ui_list_t *list = el;
                unsigned int i = 0;
                float vd = met.w / (float)list->element_count;
                float hd = met.z / (float)list->element_count;
                int iw = list->item_width < 0 ? met.z + list->item_width : list->item_width;
                int ih = list->item_height < 0 ? met.w + list->item_height : list->item_height;
                for (; i < list->element_count; i++) {
                    draw_element(list->elements[i], vec2(
                        pos.x + (list->stacking == EL_STACK_HORI ? (i * (iw ? iw : hd)) : 0),
                        pos.y + (list->stacking == EL_STACK_VERT ? (i * (ih ? ih : vd)) : 0)
                    ), depth, iw ? iw : list->stacking == EL_STACK_HORI ? hd : met.z, ih ? ih : list->stacking == EL_STACK_VERT ? vd : met.w);
                }
            }
            break;
    }
}

void draw_menu(ui_menu_t *menu) {
    vec2_t pos = sub2(menu->position, mul2(vec2(menu->width, menu->height), menu->origin));
    draw_text(menu->title_font, menu->title, str_len(menu->title), menu->text_spacing, menu->text_size, vec2(menu->origin.x, 1.1f), vec2(menu->position.x, pos.y - 2), menu->depth, menu->title_color);
    if (menu->root_element) draw_element(menu->root_element, pos, menu->depth, menu->width, menu->height);
}

static char clicking = 0;

void update_element(el_t el, vec2_t offset, int cursor_x, int cursor_y, float container_width, float container_height) {

    vec4_t met = el_get_metrics(el, container_width, container_height);
    vec2_t pos = vec2(met.x + offset.x, met.y + offset.y);

    bool hover = cursor_x >= pos.x && cursor_x < pos.x + met.z &&
        cursor_y >= pos.y && cursor_y < pos.y + met.w;

    switch (el_type(el)) {
        case UI_BUTTON: {
                ui_button_t *button = el;
                if (el_menu(el)->mouse_moved) {
                    if (hover) {
                        if (!button->focused) {
                            button->focused = true;
                            if (button->focus_cb) button->focus_cb(el);
                        }
                        glfwSetCursor(el_menu(el)->window->handle, pointer_cursor);
                    }
                    else {
                        if (button->focused) {
                            button->focused = false;
                            glfwSetCursor(el_menu(el)->window->handle, NULL);
                            if (button->unfocus_cb) button->unfocus_cb(el);
                        }
                    }
                }
                if (button->focused && (clicking == true || get_key(KEY_ENTER))) {
                    if (!button->active) {
                        button->active = true;
                        glfwSetCursor(el_menu(el)->window->handle, NULL);
                        if (button->click_cb) button->click_cb(el, cursor_x - (int)pos.x, cursor_y - (int)pos.y);
                        if (button->activate_cb) button->activate_cb(el);
                        clicking = -1;
                    }
                }
                else button->active = false;
            }
            break;
        case UI_INPUT: {
                ui_input_t *input = el;
                if (active_input != input) input->active = false;
                if (el_menu(el)->mouse_moved) {
                    if (hover) {
                        if (!input->focused) {
                            input->focused = true;
                            if (input->focus_cb) input->focus_cb(el);
                        }
                        glfwSetCursor(el_menu(el)->window->handle, caret_cursor);
                    }
                    else {
                       if (input->focused) {
                           input->focused = false;
                           glfwSetCursor(el_menu(el)->window->handle, NULL);
                           if (input->unfocus_cb) input->unfocus_cb(el);
                       }
                    }
                }
                if (input->focused) {
                    if (clicking == true || get_key(KEY_ENTER)) {
                        if (!input->active) {
                            input->active = true;
                            active_input = input;
                            if (input->click_cb) input->click_cb(el, cursor_x - (int)pos.x, cursor_y - (int)pos.y);
                            if (input->activate_cb) input->activate_cb(el);
                            input_caret = input->value_length;
                            clicking = -1;
                        }
                    }
                }
                else if (clicking == true && active_input == input) {
                    input->active = false;
                    active_input = NULL;
                }
            }
            break;
        case UI_LIST: {
                ui_list_t *list = el;
                unsigned int i = 0;
                float vd = met.z / (float)list->element_count;
                float hd = met.w / (float)list->element_count;
                int iw = list->item_width < 0 ? met.z + list->item_width : list->item_width;
                int ih = list->item_height < 0 ? met.w + list->item_height : list->item_height;
                for (; i < list->element_count; i++) {
                    update_element(list->elements[i], vec2(
                        pos.x + (list->stacking == EL_STACK_HORI ? (i * (iw ? iw : hd)) : 0),
                        pos.y + (list->stacking == EL_STACK_VERT ? (i * (ih ? ih : vd)) : 0)
                    ), cursor_x, cursor_y, iw ? iw : list->stacking == EL_STACK_HORI ? hd : met.z, ih ? ih : list->stacking == EL_STACK_VERT ? vd : met.w);
                }
            }
            break;
    }
}

static bool changing_focus = false;

void update_menu(ui_menu_t *menu, int cursor_x, int cursor_y) {
    if (
        cursor_x != menu->last_cursor_x ||
        cursor_y != menu->last_cursor_y
    ) {
        menu->mouse_moved = true;
        menu->last_cursor_x = cursor_x;
        menu->last_cursor_y = cursor_y;
    }
    else menu->mouse_moved = false;

    vec2_t pos = sub2(menu->position, mul2(vec2(menu->width, menu->height), menu->origin));

    if ((MOUSE_BUTTON_LEFT)) {
        if (!clicking) clicking = 1;
    }
    else clicking = 0;

    if (menu->root_element) {
        update_element(menu->root_element, pos, cursor_x, cursor_y, menu->width, menu->height);
        if (get_key(KEY_DOWN)) {
            if (!changing_focus) {
                if (el_list_focus_next(menu->root_element) != true)
                    el_list_focus(menu->root_element, 0, 1);
                changing_focus = true;
            }
        }
        else if (get_key(KEY_UP)) {
            if (!changing_focus) {
                if (el_list_focus_previous(menu->root_element) != true)
                    el_list_focus(menu->root_element, -1, -1);
                changing_focus = true;
            }
        }
        else changing_focus = false;

        if (changing_focus) {
            if (active_input && !active_input->focused) {
                active_input->active = false;
                active_input = NULL;
            }
        }
    }

    if (get_mouse_button(MOUSE_BUTTON_LEFT) && clicking == 1) clicking = -1;
    caret_blink = (caret_blink + 1) % CARET_BLINK_RATE;
}