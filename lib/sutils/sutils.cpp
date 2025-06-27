#include "sutils.h"

bool safe_str_copy(char *dest, const char *src, size_t size) {
    if (strlen(src) < size) {
        strlcpy(dest, src, size);
    } else {
        return false;
    }
    return true;
}
