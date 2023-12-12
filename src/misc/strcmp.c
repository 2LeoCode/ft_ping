#include <stddef.h>

int
ft_strcmp(const char * const s1, const char * const s2) {
  size_t i = 0;

  while (s1[i] && s2[i] && s1[i] == s2[i]) ++i;
  return s1[i] - s2[i];
}
