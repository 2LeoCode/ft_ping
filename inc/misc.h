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

#endif
