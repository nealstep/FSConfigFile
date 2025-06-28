#include "sutils.h"

bool safe_str_copy(char *dest, const char *src, size_t size) {
    if (strlen(src) < size) {
        strlcpy(dest, src, size);
    } else {
        return false;
    }
    return true;
}

const char *splitString(const char *input, char split) {
    auto ptr = strchr(input, split);
    if (ptr) {
        *ptr = '\0';
        return ++ptr;
    }
    return nullptr;
}
