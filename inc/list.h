#ifndef LIST_H
# define LIST_H

# include <stddef.h>


typedef struct list_node
list_node;

typedef struct list
list;


struct list_node {
  void *      data;
  list_node * next;
  list_node * prev;
};

struct list {
  size_t      size;
  list_node * head;
};


list
list_new(void);

int
list_push_back(
    list * const self,
    const void * const data,
    const size_t size
);

void
list_destroy(list * const self);

#endif
