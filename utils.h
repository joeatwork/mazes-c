#ifndef __UTILS_H__
#define __UTILS_H__

#include <error.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* RANDOM_CHOICE IS A TOY. Randomness is a profound topic, but for the
   application at hand (generating fun Mazes) what follows is
   random and unbiased enough. */
#define RANDOM_CHOICE(name, n)\
  do { name = rand() / (RAND_MAX/n); } while (name == n && n != 0)

#define PERROR_EXIT(errnum, format, ...) \
  error_at_line(EXIT_FAILURE, errnum, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ERROR_EXIT(format, ...) \
  error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__, format, ##__VA_ARGS__)

inline void *_checked_calloc(size_t nmemb, size_t size, char* file, int line) {
  void *mem = calloc(nmemb, size);
  if (NULL == mem) {
	error_at_line(EXIT_FAILURE, errno, file, line, "calloc failed");
  }
  return mem;
}
#define checked_calloc(nmemb, size) _checked_calloc(nmemb, size, __FILE__, __LINE__)

inline void *_checked_malloc(size_t size, char* file, int line) {
  void *mem = malloc(size);
  if (NULL == mem) {
	error_at_line(EXIT_FAILURE, errno, file, line, "malloc failed");
  }
  return mem;
}
#define checked_malloc(size) _checked_malloc(size, __FILE__, __LINE__)

inline void *_checked_realloc(void *ptr, size_t size, char* file, int line) {
  void *mem = realloc(ptr, size);
  if (NULL == mem) {
	error_at_line(EXIT_FAILURE, errno, file, line, "realloc failed");
  }
  return mem;
}
#define checked_realloc(ptr, size) _checked_realloc(ptr, size, __FILE__, __LINE__)

inline int _checked_snprintf(char *file, int line, char *str, size_t size, const char *format, ...) {
  va_list argp;
  va_start(argp, format);
  int wrote = vsnprintf(str, size, format, argp);
  va_end(argp);
  if (0 > wrote || wrote >= size) {
	error_at_line(EXIT_FAILURE, 0, file, line, "str too small for format");
  }
  return wrote;
}
#define checked_snprintf(str, size, format, ...) \
  _checked_snprintf(__FILE__, __LINE__, str, size, format, ##__VA_ARGS__)

#endif
