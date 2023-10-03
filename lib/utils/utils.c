#include "utils.h"

/**
 * @file
 * 
 * @brief General utility functions to simplify string manipulation
 * 
 * @author Ueli Niederer, GBS St.Gallen 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/**
 * @brief snprintf with guaraneed terminal NUL charachter
 * 
 * @see snprintf
 */
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

/**
 * @brief Concatenates two strings and returns a freshly allocated buffer containing the resulting string
 * 
 * @param   left    String to place first in the buffer
 * @param   right   String to place second in the buffer
 * 
 * @return  Buffer containing the combination of @p left and @p right .
 * 
 * @note    As with @ref strdup , the user is responsible to invoke @ref free , whenever the 
 *          buffer is discarded.
 */
char *strcattodup(const char *left, const char *right) {
  size_t left_len = strlen(left);
  size_t right_len = strlen(right);

  char *result = (char *)malloc(right_len + left_len + 1);
  char *left_start = result;
  char *right_start = &(result[left_len]);

  memcpy(left_start, left, left_len);
  memcpy(right_start, right, right_len);

  result[left_len + right_len] = '\0';

  return result;
}
