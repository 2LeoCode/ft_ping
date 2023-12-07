#include <stdio.h>

int ping_help(void) {
  printf(
    "Usage: ping [options] [targets...]\n"
    "Options:\n"
    "  -?, --help: Display this help message\n"
    "  -v, --verbose: Enable verbose mode\n"
  );
  return 0;
}
