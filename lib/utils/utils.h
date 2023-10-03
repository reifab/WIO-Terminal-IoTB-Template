#pragma once

/**
 * @file
 * 
 * @brief General utility functions to simplify string manipulation
 * 
 * @author Ueli Niederer, GBS St.Gallen 
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int   snprintf_safe(char *str, size_t size, const char *format, ...);
char *strcattodup(const char *left, const char *right);

#ifdef __cplusplus
}
#endif