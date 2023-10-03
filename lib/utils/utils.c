#include "utils.h"

#include <stdio.h>
#include <stdarg.h>

int snprintf_safe(char *str, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int ret = vsnprintf(str, size, format, args);
    if (ret >= size) {
        str[size - 1] = '\0';
    }

    va_end(args);
    return ret;
}