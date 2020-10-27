#include <string.h>

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void strcpy(const char * src, char * dest) {
    dest[0] = src[0];
    for (int i = 1; src[i-1]; i++)
        dest[i] = src[i];
}
