#ifndef _LIST_H
# define _LIST_H

# include "../list.h"

typedef struct _list {
  size_t      size;
  list_node * head;
} _list;

#endif
