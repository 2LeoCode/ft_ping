#include <stddef.h>

char * ft_strcpy(char * const dst, const char * const src) {
  size_t i = 0;

  while (src[i]) {
    dst[i] = src[i];
    ++i;
  }
  dst[i] = 0;
  return dst;
}
