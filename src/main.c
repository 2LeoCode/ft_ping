#include <stdbool.h>

#include <ping_parser.h>
#include <ping.h>


int
main(const int argc, const char ** const argv) {
  (void)argc;

  const int flags = ping_parse_flags(argv + 1);
  if (flags < 0) return 1;

  const size_t ntargets = ping_parse_targets(argv + 1);

  return ping(argv + 1, ntargets, flags);
}
