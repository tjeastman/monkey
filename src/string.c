#include <stdlib.h>
#include <string.h>

char* string_copy(char* source)
{
    size_t n = strlen(source);
    char* destination = (char*)malloc(n + 1);
    strncpy(destination, source, n);
    destination[n] = 0;
    return destination;
}
