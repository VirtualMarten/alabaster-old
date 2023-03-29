#include "ae/utility.h"

#include <stdio.h>
#include <stdarg.h>

static FILE *logfile = NULL;

void _putlog(FILE *f, const char *category, const char *format, va_list args) {
    char msg[128];
    vsprintf(msg, format, args);
    char _category[32] = "";
    if (category) sprintf(_category, LOG_CATEGORY_FORMAT, category);
    fprintf(f, LOG_FORMAT, _category, msg);
}

void _puterr(FILE *f, const char *category, const char *format, int code, va_list args) {
    char msg[128];
    vsprintf(msg, format, args);
    char _category[32] = "";
    if (category) sprintf(_category, LOG_CATEGORY_FORMAT, category);
    char _code[16] = "";
    if (code) sprintf(_code, ERR_CODE_FORMAT, code);
    fprintf(f, ERR_LOG_FORMAT, _category, _code, msg);
}

void putlog(const char *category, const char *format, ...) {
    va_list args;
    va_start(args, format);
    _putlog(stdout, category, format, args);
    va_end(args);
    if (logfile) {
        va_start(args, format);
        _putlog(logfile, category, format, args);
        fflush(logfile);
        va_end(args);
    }
}

void puterr(const char *category, const char *format, int code, ...) {
    va_list args;
    va_start(args, code);
    _puterr(stderr, category, format, code, args);
    va_end(args);
    if (logfile) {
        va_start(args, code);
        _puterr(logfile, category, format, code, args);
        fflush(logfile);
        va_end(args);
    }
}