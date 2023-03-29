#include "ae/str.h"

#include <stdio.h>

str_t str_nnew(const char *text, uint32_t l) {
    uint32_t *len = malloc(l + sizeof(uint32_t) + 1);
    *len = l;
    str_t s = (str_t)len + sizeof(uint32_t);
    strncpy(s, text, l);
    s[l] = '\0';
    return s;
}

str_t _str_nset(str_t *s, const char *text, uint32_t l) {
    if (!s) return NULL;
    if (*s) {
        if (l != str_len(*s)) {
            *s = (str_t)realloc(*s - sizeof(uint32_t), l + sizeof(uint32_t) + 1) + sizeof(uint32_t);
            str_len(*s) = l;
        }
        strncpy(*s, text, l);
        (*s)[l] = '\0';
    }
    else *s = str_new(text);
    return *s;
}

str_t _str_fset(str_t *s, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buf[128];
    int l = vsnprintf(buf, 128, format, args);
    return _str_nset(s, buf, l);
}

str_t _str_append(str_t *s, const char *text, uint32_t l) {
    if (!(s && *s)) return NULL;
    if (!l) l = strlen(text);
    uint32_t *len = (uint32_t*)(*s - sizeof(uint32_t));
    len = realloc(len, *len + l + sizeof(uint32_t) + 1);
    *s = (str_t)len + sizeof(uint32_t);
    memcpy(*s + *len, text, l);
    *len += l;
    (*s)[*len] = '\0';
    return *s;
}

str_t _str_prepend(str_t *s, const char *text, uint32_t l) {
    if (!(s && *s)) return NULL;
    if (!l) l = strlen(text);
    uint32_t *len = (uint32_t*)((int8_t*)*s - sizeof(uint32_t));
    len = realloc(len, *len + l + 1);
    *s = (str_t)len + sizeof(uint32_t);
    memmove(*s + l, *s, *len);
    memcpy(*s, text, l);
    *len += l;
    return *s;
}

str_t read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    uint32_t l = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint32_t *len = malloc(l + sizeof(uint32_t) + 1);
    *len = l;
    str_t s = (str_t)len + sizeof(uint32_t);
    s[l] = '\0';

    fread(s, l, 1, file);
    fclose(file);

    return s;
}