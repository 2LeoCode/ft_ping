#ifndef PING_H
# define PING_H

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <time.h>

# include "list.h"

# define PING_TIMEOUT 1
# define PING_INTERVAL 1
# define PING_TTL 60
# define PING_PKTSIZE 64
# define PING_MSGSIZE (PING_PKTSIZE - sizeof(struct icmphdr))

# define IP_HDR_MAXLEN (MAX_IPOPTLEN + sizeof(struct iphdr))


typedef struct ping_pkt
ping_pkt;

typedef struct ping_counter
ping_counter;

typedef struct ping_data
ping_data;


struct ping_pkt {
  struct icmphdr  hdr;
  char            msg[PING_MSGSIZE];
};

struct ping_counter {
  list            intervals;
  long double     min;
  long double     max;
  int             sent;
  int             received;
};

struct ping_data {
  unsigned        ident;
  int             sockfd;
  ping_pkt        pkt;
  int             ttl;
  struct timespec ts_start, ts_end;
  struct timeval  tv_timeout;
  ping_counter    counters[];
};


int
ping(
    const char * const * const targets,
    const size_t ntargets,
    const int flags
);

#endif
