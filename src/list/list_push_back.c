#include <stdlib.h>
#include <string.h>

#include <list.h>
#include <misc.h>

int
list_push_back(
    list * const self,
    const void * const data,
    const size_t size
  ) {
  list_node * node = malloc(sizeof(list_node));
  if (!node) return -1;

  ++self->size;
  if (!(node->data = malloc(size))) {
    free(node);
    return -1;
  }
  ft_memcpy(node->data, data, size);
  if (!self->head) {
    self->head = node->next = node->prev = node;
    return 0;
  }
  node->prev = self->head->prev;
  node->next = self->head;
  self->head->prev->next = node;
  self->head->prev = node;
  return 0;
}
