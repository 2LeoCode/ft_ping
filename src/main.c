#include <ping.h>
#include <ping_arguments.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

static bool ping_stop = false;

void int_handler() {
  ping_stop = true;
}

uint16_t ping_checksum(const void * b, int len) {
  const uint16_t * buf = b;
  uint32_t sum = 0;
  uint16_t result;

  for (sum = 0; len > 1; len -= 2)
    sum += *buf++;
  if (len == 1)
    sum += *(uint8_t *)buf;
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;
  return result;
}

int dns_lookup(char * hostname, struct sockaddr_in * addr_dst, char * ip_dst) {
  struct hostent * host = gethostbyname(hostname);
  if (!host)
    return -1;
  strcpy(ip_dst, inet_ntoa(*(struct in_addr *)host->h_addr));
  addr_dst->sin_family = host->h_addrtype;
  addr_dst->sin_port = htons(0);
  addr_dst->sin_addr.s_addr = *(long *)host->h_addr;
  return 0;
}

int reverse_dns_lookup(const char * ip_addr, char * host_dst) {
  struct sockaddr_in  tmp;
  socklen_t           len;

  tmp.sin_family = AF_INET;
  tmp.sin_addr.s_addr = inet_addr(ip_addr);
  len = sizeof(struct sockaddr_in);
  return -!getnameinfo((struct sockaddr *)&tmp, len, host_dst, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
}

int ping_no_host(void) {
  printf(
    "ft_ping: missing host operand\n"
    "Try 'ping --help' or 'ping --usage' for more information.\n"
  );
  return 1;
}

ping_data * ping_init(size_t ntargets, int flags) {
  (void)flags;
  ping_data * data = malloc(sizeof(*data) + sizeof(*data->counters) * ntargets);

  if (!data)
    return NULL;
  bzero(data, sizeof(*data) + sizeof(*data->counters) * ntargets);
  data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (data->sockfd < 0)
    return NULL;
  data->pkt = (ping_pkt) {
    .hdr = {
      .type = ICMP_ECHO,
      .un.echo = {
        .id = getpid(),
        .sequence = 0
      },
    }
  };

  for (size_t i = 0; i < MSG_SIZE - 1; ++i)
    data->pkt.msg[i] = i + '0';
  data->pkt.msg[MSG_SIZE - 1] = 0;
  data->tv_timeout = (struct timeval) {
    .tv_sec = PING_TIMEOUT
  };
  data->ttl = 60;
  data->ts_end = (struct timespec) { };
  setsockopt(data->sockfd, SOL_IP, IP_TTL, &data->ttl, sizeof(data->ttl));
  setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, &data->tv_timeout, sizeof(data->tv_timeout));
  clock_gettime(CLOCK_MONOTONIC, &data->ts_start);
  return data;
}

long double average(list * lst) {
  long double sum = 0;
  list_node * node = lst->head;

  do {
    sum += *(long double *)node->data;
    node = node->next;
  } while (node != lst->head);
  return sum / lst->size;
}

long double standard_deviation(list * lst, long double avg) {
  long double sum = 0;
  list_node * node = lst->head;

  do {
    sum += pow(*(long double *)node->data - avg, 2);
  } while (node != lst->head);
  return sqrt(sum / lst->size);
}

void ping_end(ping_data * data, char * const * targets, size_t ntargets) {
  // long double time_elapsed;
  long double percent_lost, avg, stddev;

  // clock_gettime(CLOCK_MONOTONIC, &data->ts_end);
  // time_elapsed = (long double)(data->ts_end.tv_nsec - data->ts_start.tv_nsec) / 1000000.
  //   + (long double)(data->ts_end.tv_sec - data->ts_start.tv_sec) * 1000.;
  for (size_t i = 0; i < ntargets; ++i) {
    if (!data->counters[i].sent)
      percent_lost = avg = stddev = 0;
    else {
      percent_lost = ((data->counters[i].sent - data->counters[i].received) / data->counters[i].sent) * 100.0;
      avg = average(&data->counters[i].intervals);
      stddev = standard_deviation(&data->counters[i].intervals, avg);
    }
    printf("=== %s ping statistics ===\n", targets[i]);
    printf(
      "%d packets sent, %d packets received, %.0Lf%% packet loss\n"
      "round-trip min/avg/max/stddev %.3Lf/%.3Lf/%.3Lf/%.3Lf ms\n",
      data->counters[i].sent, data->counters[i].received, percent_lost, data->counters[i].min, avg, data->counters[i].max, stddev
    );
    list_destroy(&data->counters[i].intervals);
  }
  free(data);
}

int ping(char * const * targets, size_t ntargets, int flags) {
  ping_data *         data;
  ping_response       response;
  struct timespec     ts_send, ts_recv;
  long double         interval;
  uint8_t             ip_addr[NI_MAXHOST + 1];
  struct sockaddr_in  addr;

  if (flags & PING_HELP)
    return ping_help();
  if (!ntargets)
    return ping_no_host();
  if (!(data = ping_init(ntargets, flags)))
    return perror("ping_init"), 1;
  signal(SIGINT, int_handler);
  for (size_t i = 0; !ping_stop && i < ntargets; ++i) {
    if (dns_lookup((char *)targets[i], &addr, (char *)ip_addr) == -1) {
      printf("DNS lookup failed for %s\n", targets[i]);
      continue;
    }
    // if (reverse_dns_lookup((char *)ip_addr, (char *)rev_host) == -1)
    //   strcpy((char *)rev_host, (char *)ip_addr);
    printf("PING %s (%s): %zu data bytes\n", targets[i], ip_addr, sizeof(data->pkt.msg));
    data->pkt.hdr.un.echo.sequence = 0;
    while (!ping_stop) {
      clock_gettime(CLOCK_MONOTONIC, &ts_send);
      data->pkt.hdr.checksum = 0;
      data->pkt.hdr.checksum = ping_checksum(&data->pkt, sizeof(data->pkt));
      if (sendto(data->sockfd, &data->pkt, sizeof(data->pkt), 0, (void *)&addr, sizeof(addr)) <= 0) {
        perror("sendto");
        break ;
      }
      ++data->counters[i].sent;
      if (recv(data->sockfd, &response, sizeof(response), 0) <= 0) {
        perror("recv");
        continue ;
      }
      clock_gettime(CLOCK_MONOTONIC, &ts_recv);
      interval = (long double)(ts_recv.tv_nsec - ts_send.tv_nsec) / 1000000. + (long double)(ts_recv.tv_sec - ts_send.tv_sec) * 1000.;
      list_push_back(&data->counters[i].intervals, &interval, sizeof(interval));
      if (data->pkt.hdr.un.echo.sequence == 0)
        data->counters[i].min = data->counters[i].max = interval;
      else {
        if (interval < data->counters[i].min)
          data->counters[i].min = interval;
        if (interval > data->counters[i].max)
          data->counters[i].max = interval;
      }
      if (!(response.ip_icmp.icmp_type == 69 && response.ip_icmp.icmp_code == 0)) {
        if (flags & PING_VERBOSE)
        printf("Error: Packet received with ICMP type %d code %d\n", response.ip_icmp.icmp_type, response.ip_icmp.icmp_code);
      } else {
        printf(
          "%zu bytes from %s: icmp_seq=%d ttl=%d time=%Lf ms\n",
          sizeof(response.msg),
          ip_addr,
          data->pkt.hdr.un.echo.sequence,
          response.ip_hdr.ip_ttl,
          interval
        );
        ++data->counters[i].received;
      }
      ++data->pkt.hdr.un.echo.sequence;
      data->pkt.hdr.checksum = ping_checksum(&data->pkt, sizeof(data->pkt));
      usleep(PING_INTERVAL * 1000000);
    }
  }
  ping_end(data, targets, ntargets);
  return 0;
}

int main(int argc, char ** argv) {
  int     flags = ping_parse_flags(argv + 1);
  size_t  ntargets;

  (void)argc;
  ntargets = ping_parse_targets(argv + 1);
  if (flags < 0)
    return 1;
  return ping(argv + 1, ntargets, flags);
}
