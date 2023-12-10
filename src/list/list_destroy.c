#include <list.h>
#include <stdlib.h>

void list_destroy(list * self) {
  list_node * node = self->head;
  list_node * next;

  if (node)
    do {
      next = node->next;
      free(node->data);
      free(node);
      node = next;
    } while (node != self->head);
  self->head = NULL;
  self->size = 0;
}
