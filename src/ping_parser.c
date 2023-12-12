#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <ping_parser.h>
#include <misc.h>

int
ping_parse_flags(const char * const * const args) {
  int flags = 0;

  for (size_t i = 0; args[i]; ++i) {
    const char * arg = args[i];
    if (*arg && *arg == '-') {
      ++arg;
      if (*arg && *arg == '-') {
        ++arg;
        if (!ft_strcmp(arg, "help"))
          return PING_HELP;
        else if (!ft_strcmp(arg, "verbose"))
          flags |= PING_VERBOSE;
        else {
          fprintf(stderr, "ft_ping: unrecognized option '%s'\n", arg - 2);
          goto invalid_option;
        }
      } else {
        while (*arg) {
          switch (*arg) {
            case '?':
              return PING_HELP;
            case 'v':
              flags |= PING_VERBOSE;
              break ;
            default:
              fprintf(stderr, "ft_ping: invalid option -- '%c'\n", *arg);
              goto invalid_option;
          }
          ++arg;
        }
      }
    }
  }
  return flags;

  invalid_option:
  fprintf(
    stderr,
    "Try 'ft_ping --help' or 'ft_ping --usage' for more information.\n"
  );
  return -1;
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
