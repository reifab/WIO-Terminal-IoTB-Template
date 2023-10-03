#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int snprintf_safe(char *str, size_t size, const char *format, ...);

#ifdef __cplusplus
}
#endif