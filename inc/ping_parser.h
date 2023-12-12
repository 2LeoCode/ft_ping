#ifndef PING_PARSER_H
# define PING_PARSER_H

# include <stddef.h>


enum ping_flags {
  PING_HELP = 1,
  PING_VERBOSE = 1 << 1,
};


int
ping_parse_flags(const char * const * const args);

size_t
ping_parse_targets(const char ** const args);

#endif
