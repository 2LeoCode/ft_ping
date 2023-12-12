#include <stddef.h>

void *
ft_memset(
    void * const dst,
    const int c,
    const size_t size
  ) {
  size_t i = 0;

  while (i < size) {
    ((char *)dst)[i] = c;
    ++i;
  }
  return dst;
}
