#include <stdlib.h>

#include <list.h>

void
list_destroy(list * const self) {
  if (!self->size) return ;

  list_node * node = self->head;
  list_node * next;

  do {
    next = node->next;
    free(node->data);
    free(node);
    node = next;
  } while (node != self->head);
  self->head = NULL;
  self->size = 0;
}
