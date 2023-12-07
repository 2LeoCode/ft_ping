#include <list.h>
#include <stdlib.h>
#include <string.h>

int list_push_back(list * self, void * data, size_t size) {
  list_node * node = malloc(sizeof(list_node));

  if (!node)
    return -1;
  ++self->size;
  node->data = malloc(size);
  if (!node->data) {
    free(node);
    return -1;
  }
  memcpy(node->data, data, size);
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
