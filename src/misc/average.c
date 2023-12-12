#include <list.h>

long double
average(const list * const lst) {
  if (!lst->size) return 0;

  long double sum = 0;
  list_node * node = lst->head;

  do {
    sum += *(long double *)node->data;
    node = node->next;
  } while (node != lst->head);
  return sum / lst->size;
}
