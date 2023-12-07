#ifndef PING_H
# define PING_H

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <netinet/ip_icmp.h>
#include "list.h"

# define PACKET_SIZE 64
# define PING_TIMEOUT 1
# define PING_INTERVAL 1
# define MSG_SIZE (PACKET_SIZE - sizeof(struct icmphdr))

int ping_help(void);

typedef struct ping_pkt {
  struct icmphdr      hdr;
  char                msg[MSG_SIZE];
} ping_pkt;

typedef struct ping_response {
  union {
    struct ip         ip_hdr;
    struct icmp       ip_icmp;
  };
  char                msg[MSG_SIZE];
} ping_response;

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

#endif
