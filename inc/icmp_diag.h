#ifndef ICMP_DIAG_H
# define ICMP_DIAG_H

# include <netinet/ip_icmp.h>

# define ICMP_ALT_ADDR 6
# define ICMP_ROUT_ADV 9
# define ICMP_ROUT_SOL 10
# define ICMP_TRACEROUTE 30
# define ICMP_DGRAM_CONV_ERR 31
# define ICMP_MOBILE_HOST_REDIRECT 32
# define ICMP_IPV6_WHERE_ARE_YOU 33
# define ICMP_IPV6_HERE_I_AM 34
# define ICMP_MOBILE_REG_REQ 35
# define ICMP_MOBILE_REG_REPLY 36
# define ICMP_DOMAIN_NAME_REQ 37
# define ICMP_DOMAIN_NAME_REPLY 38
# define ICMP_SKIP 39
# define ICMP_PHOTURIS 40
# define ICMP_EXPERIMENT_MOBILITY 41
# define ICMP_EXTENDED_ECHO 42
# define ICMP_EXTENDED_ECHOREPLY 43
# define ICMP_EXPERIMENT_1 253
# define ICMP_EXPERIMENT_2 254


struct icmp_diag {
  int       type;
  char *    text;
  unsigned  ncode;
  char **   info;
};


void
print_icmp_diag(
    const uint8_t type,
    const uint8_t code
);

#endif
