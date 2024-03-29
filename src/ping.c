#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <icmp_diag.h>
#include <misc.h>
#include <ping.h>
#include <ping_help.h>
#include <ping_parser.h>

static bool
ping_stop = false;

static void
int_handler(const int sig) {
  (void)sig;
  ping_stop = true;
}

static int
ping_no_host(void) {
  printf(
    "ft_ping: missing host operand\n"
    "Try 'ping --help' or 'ping --usage' for more information.\n"
  );
  return 1;
}

static uint16_t
ping_checksum(const void * b, int len) {
  const uint16_t * buf = b;
  uint32_t sum = 0;

  for (sum = 0; len > 1; len -= 2)
    sum += *buf++;
  if (len == 1)
    sum += *(uint8_t *)buf;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

static int
dns_lookup(
    const char * const domain_name,
    struct sockaddr_in * const addr_dst
  ) {
  static const struct addrinfo hints = {
    .ai_family = AF_INET,
    .ai_socktype = SOCK_RAW,
    .ai_protocol = IPPROTO_ICMP,
  };
  struct addrinfo * result;

  if (getaddrinfo(domain_name, NULL, &hints, &result) != 0)
    return -1;
  *addr_dst = *(struct sockaddr_in *)result->ai_addr;
  freeaddrinfo(result);
  return 0;
}

static ping_data *
ping_init(const size_t ntargets) {
  ping_data * data = malloc(sizeof(*data) + sizeof(*data->counters) * ntargets);
  if (!data) return NULL;

  signal(SIGINT, int_handler);
  ft_memset(data, 0, sizeof(*data) + sizeof(*data->counters) * ntargets);
  data->ident = getpid() & 0xffff;
  if ((data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    return NULL;
  data->pkt = (ping_pkt) {
    .hdr = {
      .type = ICMP_ECHO,
      .un.echo = {
        .id = getpid(),
        .sequence = 0,
      },
    }
  };
  for (size_t i = 0; i < PING_MSGSIZE - 1; ++i)
    data->pkt.msg[i] = i + '0';
  data->pkt.msg[PING_MSGSIZE - 1] = 0;
  data->tv_timeout = (struct timeval) {
    .tv_sec = PING_TIMEOUT,
  };
  data->ttl = PING_TTL;
  data->ts_end = (struct timespec) { };
  setsockopt(data->sockfd, SOL_IP, IP_TTL, &data->ttl, sizeof(data->ttl));
  setsockopt(
    data->sockfd, SOL_SOCKET, SO_RCVTIMEO,
    &data->tv_timeout, sizeof(data->tv_timeout)
  );
  clock_gettime(CLOCK_MONOTONIC, &data->ts_start);
  return data;
}

static void
ping_fini(
    ping_data * const data,
    const char * const * const targets,
    const size_t ntargets
  ) {
  long double percent_lost, avg, std;

  for (size_t i = 0; i < ntargets; ++i) {
    if (!data->counters[i].sent)
      percent_lost = avg = std = 0;
    else {
      percent_lost = ((data->counters[i].sent - data->counters[i].received) / data->counters[i].sent) * 100.0;
      avg = average(&data->counters[i].intervals);
      std = stddev(&data->counters[i].intervals, avg);
    }
    printf("=== %s ping statistics ===\n", targets[i]);
    printf(
      "%d packets sent, %d packets received, %.0Lf%% packet loss\n"
      "round-trip min/avg/max/stddev %.3Lf/%.3Lf/%.3Lf/%.3Lf ms\n",
      data->counters[i].sent, data->counters[i].received, percent_lost,
      data->counters[i].min, avg, data->counters[i].max, std
    );
    list_destroy(&data->counters[i].intervals);
  }
  free(data);
}

int
ping(
    const char * const * const targets,
    const size_t ntargets,
    const int flags
  ) {
  ping_data *         data;
  struct timespec     ts_send, ts_recv;
  long double         interval;
  struct sockaddr_in  dst_addr;
  char                ip_addr[NI_MAXHOST + 1];
  char                res_buffer[IP_HDR_MAXLEN + sizeof(ping_pkt)];
  size_t              ip_hdr_size;
  struct iphdr *      ip_hdr;
  ping_pkt *          response;
  struct iovec        res_iov = {
    .iov_base = res_buffer,
    .iov_len = sizeof(res_buffer),
  };
  struct msghdr       res_msg = {
    .msg_name = NULL,
    .msg_namelen = 0,
    .msg_iov = &res_iov,
    .msg_iovlen = 1,
    .msg_control = NULL,
    .msg_controllen = 0,
    .msg_flags = 0,
  };

  if (flags & PING_HELP) return ping_help();
  if (!ntargets) return ping_no_host();
  if (!(data = ping_init(ntargets))) {
    fprintf(stderr, "ping_init: %s\n", strerror(errno));
    return 1;
  }
  for (size_t i = 0; !ping_stop && i < ntargets; ++i) {
    if (dns_lookup(targets[i], &dst_addr) == -1) {
      fprintf(stderr, "DNS lookup failed for %s\n", targets[i]);
      continue ;
    }
    ft_strcpy(ip_addr, inet_ntoa(dst_addr.sin_addr));
    printf("PING %s (%s): %zu data bytes", targets[i], ip_addr, sizeof(data->pkt.msg));
    if (flags & PING_VERBOSE)
      printf(", id 0x%04x = %u", data->ident, data->ident);
    printf("\n");
    data->pkt.hdr.un.echo.sequence = 0;
    while (!ping_stop) {
      clock_gettime(CLOCK_MONOTONIC, &ts_send);
      data->pkt.hdr.checksum = 0;
      data->pkt.hdr.checksum = ping_checksum(&data->pkt, sizeof(data->pkt));
      if (sendto(
            data->sockfd,
            &data->pkt, sizeof(data->pkt),
            0,
            (void *)&dst_addr, sizeof(dst_addr)
          ) <= 0) {
        fprintf(stderr, "sendto: %s", strerror(errno));
        break ;
      }
      ++data->counters[i].sent;
      if (recvmsg(data->sockfd, &res_msg, 0) <= 0) continue;
      clock_gettime(CLOCK_MONOTONIC, &ts_recv);
      interval = (long double)(ts_recv.tv_nsec - ts_send.tv_nsec) / 1000000.
        + (long double)(ts_recv.tv_sec - ts_send.tv_sec) * 1000.;
      list_push_back(&data->counters[i].intervals, &interval, sizeof(interval));
      if (data->pkt.hdr.un.echo.sequence == 0)
        data->counters[i].min = data->counters[i].max = interval;
      else {
        if (interval < data->counters[i].min)
          data->counters[i].min = interval;
        if (interval > data->counters[i].max)
          data->counters[i].max = interval;
      }
      ip_hdr = (struct iphdr *)res_buffer;
      ip_hdr_size = ip_hdr->ihl * 4;
      response = (ping_pkt *)(res_buffer + ip_hdr_size);
      printf(
        "%zu bytes from %s: icmp_seq=%d ttl=%d time=%Lf ms",
        sizeof(*response),
        ip_addr,
        data->pkt.hdr.un.echo.sequence,
        ((struct iphdr *)res_buffer)->ttl,
        interval
      );
      if (response->hdr.type != 0 || response->hdr.code != ICMP_ECHOREPLY) {
        if (flags & PING_VERBOSE)
          print_icmp_diag(response->hdr.type, response->hdr.code);
      } else
        ++data->counters[i].received;
      printf("\n");
      ++data->pkt.hdr.un.echo.sequence;
      data->pkt.hdr.checksum = ping_checksum(&data->pkt, sizeof(data->pkt));
      usleep(PING_INTERVAL * 1000000);
    }
  }
  ping_fini(data, targets, ntargets);
  return 0;
}
