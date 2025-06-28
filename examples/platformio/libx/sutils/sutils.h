#ifndef SUTILS_H
#define SUTILS_H

#include <Arduino.h>

bool safe_str_copy(char *dest, const char *src, size_t size);
const char *splitString(const char *input, char split=' ');

#endif  // SUTILS_H
