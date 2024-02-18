#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "string_utils.h"
#include "xmalloc.h"

size_t strsize(const char *s1) {
    size_t i = 0;
    for (; s1[i] != '\0'; ++i);
    return i;    
}

char *strreserve(const char *s1) {
    char *result = xmalloc(strsize(s1) + 1);
    strcpy(result, s1);
    return result;
}

int streql(const char *s1, const char *s2) {
    return !strcmp(s1, s2);
}

void strlower(char *s1) {
    for (unsigned int i = 0; s1[i] != '\0'; ++i) {
        s1[i] = tolower((unsigned char) s1[i]);
    }
}
