#ifndef __UTILITY_H__
#define __UTILITY_H__

#ifndef LOG_CATEGORY_FORMAT
#define LOG_CATEGORY_FORMAT "[%s] "
#endif

#ifndef LOG_FORMAT
#define LOG_FORMAT "%s%s\n"
#endif

#ifndef ERR_CODE_FORMAT
#define ERR_CODE_FORMAT " (%d)"
#endif

#ifndef ERR_LOG_FORMAT
#define ERR_LOG_FORMAT "%sError%s: %s\n"
#endif

void putlog(const char *category, const char *format, ...);
void puterr(const char *category, const char *format, int code, ...);

#endif