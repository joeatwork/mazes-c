#ifndef __UTILS_H__
#define __UTILS_H__

#include <error.h>
#include <errno.h>
#include <stdlib.h>

#define PERROR_EXIT(errnum, format, ...) \
  error_at_line(EXIT_FAILURE, errnum, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ERROR_EXIT(format, ...) \
  error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__, format, ##__VA_ARGS__)


inline void *checked_calloc(size_t nmemb, size_t size) {
  void *mem = calloc(nmemb, size);
  if (NULL == mem) {
    PERROR_EXIT(errno, "malloc failed");
  }
  return mem;
}

inline void *checked_malloc(size_t size) {
  void *mem = malloc(size);
  if (NULL == mem) {
    PERROR_EXIT(errno, "malloc failed");
  }
  return mem;
}

inline void *checked_realloc(void *ptr, size_t size) {
  void *mem = realloc(ptr, size);
  if (NULL == mem) {
    PERROR_EXIT(errno, "realloc failed");
  }
  return mem;
}

#endif
