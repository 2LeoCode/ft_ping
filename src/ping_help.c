#include <stdio.h>

int
ping_help(void) {
  printf(
    "Usage: ft_ping [OPTION...] HOST ...\n"
    "Send ICMP ECHO_REQUEST packets to network hosts.\n"
    "\n"
    " Options:\n"
    "\n"
    "  -?, --help                 give this help list\n"
    "  -v, --verbose              verbose output\n"
  );
  return 0;
}
