#include "utils.h"

extern void *_checked_calloc(size_t nmemb, size_t size, char* file, int line);
extern void *_checked_malloc(size_t size, char* file, int line);
extern void *_checked_realloc(void *ptr, size_t size, char* file, int line);
extern int _checked_snprintf(char *file, int line, char *str, size_t size, const char *format, ...);
