#ifndef MISC_H
# define MISC_H

# include "list.h"

# define COUNTOF(array) (sizeof(array) / sizeof(*array))


long double
average(const list * const lst);

long double
stddev(
    const list * const lst,
    long double avg
);

char *
ft_strcpy(
    char * const dst,
    const char * const src
);

void *
ft_memcpy(
    void * const dst,
    const void * const src,
    const size_t size
);

void *
ft_memset(
    void * const dst,
    const int c,
    const size_t size
);

int
ft_strcmp(
    const char * const s1,
    const char * const s2
);

double
ft_pow(
    const double x,
    const int y
);

double
ft_sqrt(
    const double x
);

#endif
