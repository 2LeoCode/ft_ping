#include <list.h>
#include <misc.h>

long double
stddev(const list * const lst, const long double avg) {
  if (!lst->size) return 0;

  long double sum = 0;
  list_node * node = lst->head;

  do sum += ft_pow(*(long double *)node->data - avg, 2);
  while (node != lst->head);
  return ft_sqrt(sum / lst->size);
}
