#ifndef __STR_H__
#define __STR_H__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

typedef char* str_t;
#define str_len(s) (*(uint32_t*)(s-sizeof(uint32_t)))
#define str_length(s) str_len(s)
#define str(text) str_new(text)
str_t str_nnew(const char *text, uint32_t len);
static inline str_t str_new(const char *text) {
    return str_nnew(text, strlen(text));
}
#define str_delete(s) do { free(s-sizeof(uint32_t));s=NULL; } while (0)
#define str_del(s) str_delete(s)
str_t _str_nset(str_t *s, const char *text, uint32_t len);
static inline str_t _str_set(str_t *s, const char *text) {
    return _str_nset(s, text, strlen(text));
}
#define str_nset(s, text, len) _str_nset(&(s), text, len)
#define str_set(s, text) _str_set(&(s), text)
str_t _str_fset(str_t *s, const char *format, ...);
#define str_fset(s, format, ...) _str_fset(&(s), format, __VA_ARGS__)
str_t _str_append(str_t *s, const char *text, uint32_t length);
#define str_append(s, text, len) _str_append(&(s), text, len)
str_t _str_prepend(str_t *s, const char *text, uint32_t length);
#define str_prepend(s, text, len) _str_prepend(&(s), text, len)

str_t read_file(const char *path);

#endif