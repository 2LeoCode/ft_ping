#ifndef PING_H
# define PING_H

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <netinet/ip_icmp.h>
# include "list.h"

# define PACKET_SIZE 64
# define PING_TIMEOUT 1
# define PING_INTERVAL 1
# define PING_TTL 60

# define MSG_SIZE (PACKET_SIZE - sizeof(struct icmphdr))
# define IP_HDR_MAX_SIZE (MAX_IPOPTLEN + sizeof(struct iphdr))

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

# define COUNTOF(array) (sizeof(array) / sizeof(*array))

int ping_help(void);

typedef struct ping_pkt {
  struct icmphdr      hdr;
  char                msg[MSG_SIZE];
} ping_pkt;

typedef struct ping_counter {
  list                intervals;
  long double         min;
  long double         max;
  int                 sent;
  int                 received;
} ping_counter;

typedef struct ping_data {
  int                 sockfd;
  ping_pkt            pkt;
  int                 ttl;
  struct timespec     ts_start, ts_end;
  struct timeval      tv_timeout;
  ping_counter        counters[];
} ping_data;

struct icmp_diag {
  int                 type;
  char *              text;
  unsigned            ncode;
  char **             info;
};

extern struct icmp_diag icmp_diag[];

#endif
