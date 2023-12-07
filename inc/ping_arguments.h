#ifndef PING_ARGUMENTS_H
# define PING_ARGUMENTS_H

# include <stddef.h>


enum ping_arguments {
  PING_HELP = 1,
  PING_VERBOSE = 1 << 1,
};

int ping_parse_flags(char ** args);
size_t ping_parse_targets(char ** args);


#endif
