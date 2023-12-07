#include <list.h>
#include <stdlib.h>

void list_destroy(list * lst) {
  list_node * node = lst->head;
  list_node * next;

  if (node)
    do {
      next = node->next;
      free(node->data);
      free(node);
      node = next;
    } while (node != lst->head);
  lst->head = NULL;
  lst->size = 0;
}
