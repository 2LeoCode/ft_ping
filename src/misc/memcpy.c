#include <stddef.h>

void *
ft_memcpy(void * const dst, const void * const src, const size_t size) {
  size_t i = 0;

  while (i < size) {
    ((char *)dst)[i] = ((char *)src)[i];
    ++i;
  }
  return dst;
}
