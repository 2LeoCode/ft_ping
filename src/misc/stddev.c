#include <math.h>

#include <list.h>

long double
stddev(const list * const lst, const long double avg) {
  if (!lst->size) return 0;

  long double sum = 0;
  list_node * node = lst->head;

  do sum += pow(*(long double *)node->data - avg, 2);
  while (node != lst->head);
  return sqrt(sum / lst->size);
}
