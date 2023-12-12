#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <ping_parser.h>

int
ping_parse_flags(const char * const * const args) {
  int flags = 0;

  for (size_t i = 0; args[i]; ++i) {
    const char * arg = args[i];
    if (*arg && *arg == '-') {
      ++arg;
      if (*arg && *arg == '-') {
        ++arg;
        if (!strcmp(arg, "help") || !strcmp(arg, "usage")) {
          flags |= PING_HELP;
        } else if (!strcmp(arg, "verbose")) {
          flags |= PING_VERBOSE;
        } else {
          fprintf(stderr, "Unknown flag: `%s'\n", arg);
          return -1;
        }
      } else if (!strcmp(arg, "?")) {
        flags |= PING_HELP;
      } else if (!strcmp(arg, "v")) {
        flags |= PING_VERBOSE;
      } else {
        fprintf(stderr, "Unknown flag: `%s'\n", arg);
        return -1;
      }
    }
  }
  return flags;
}

size_t
ping_parse_targets(const char ** const args) {
  size_t i;

  for (i = 0; args[i]; ++i) {
    if (*args[i] == '-') {
      for (size_t j = i; (args[j] = args[j + 1]); ++j)
        continue ;
      --i;
    }
  }
  return i;
}
