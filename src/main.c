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

struct icmp_diag icmp_diag[] = {
  { ICMP_ECHOREPLY, "Echo Reply", 1, (char *[]) {
    "Echo reply (used to ping)",
  } },
  { ICMP_DEST_UNREACH, "Destination Unreachable", 16, (char *[]) {
    "Destination network unreachable",
    "Destination host unreachable",
    "Destination protocol unreachable",
    "Destination port unreachable",
    "Fragmentation required, and DF flag set",
    "Source route failed",
    "Destination network unknown",
    "Destination host unknown",
    "Source host isolated",
    "Network administratively prohibited",
    "Host administratively prohibited",
    "Network unreachable for ToS",
    "Host unreachable for ToS",
    "Communication administratively prohibited",
    "Host Precedence Violation",
    "Precedence cutoff in effect",
  } },
  { ICMP_SOURCE_QUENCH, "Source Quench", 1, (char *[]) {
    "Source quench (congestion control)",
  } },
  { ICMP_REDIRECT, "Redirect Message", 4, (char *[]) {
    "Redirect Datagram for the Network",
    "Redirect Datagram for the Host",
    "Redirect Datagram for the ToS & network",
    "Redirect Datagram for the ToS & host",
  } },
  { ICMP_ALT_ADDR, NULL, 1, (char *[]) {
    "Alternate Host Address",
  } },
  { ICMP_ECHO, "Echo Request", 1, (char *[]) {
    "Echo request (used to ping)",
  } },
  { ICMP_ROUT_ADV, "Router Advertisement", 1, (char *[]) {
    "Router Advertisement",
  } },
  { ICMP_ROUT_SOL, "Router Solicitation", 1, (char *[]) {
    "Router discovery/selection/solicitation",
  } },
  { ICMP_TIME_EXCEEDED, "Time exceeded", 2, (char *[]) {
    "TTL expired in transit",
    "Fragment reassembly time exceeded",
  } },
  { ICMP_PARAMETERPROB, "Parameter Problem: Bad IP header", 3, (char *[]) {
    "Pointer indicates the error",
    "Missing a required option",
    "Bad length",
  } },
  { ICMP_TIMESTAMP, "Timestamp", 1, (char *[]) {
    "Timestamp",
  } },
  { ICMP_TIMESTAMPREPLY, "Timestamp Reply", 1, (char *[]) {
    "Timestamp reply",
  } },
  { ICMP_INFO_REQUEST, "Information Request", 1, (char *[]) {
    "Information Request",
  } },
  { ICMP_INFO_REPLY, "Information Reply", 1, (char *[]) {
    "Information Reply",
  } },
  { ICMP_ADDRESS, "Address Mask Request", 1, (char *[]) {
    "Address Mask Request",
  } },
  { ICMP_ADDRESSREPLY, "Address Mask Reply", 1, (char *[]) {
    "Address Mask Reply",
  } },
  { ICMP_TRACEROUTE, "Traceroute", 1, (char *[]) {
    "Information Request",
  } },
  { ICMP_DGRAM_CONV_ERR, NULL, 1, (char *[]) {
    "Datagram Conversion Error",
  } },
  { ICMP_MOBILE_HOST_REDIRECT, NULL, 1, (char *[]) {
    "Mobile Host Redirect",
  } },
  { ICMP_IPV6_WHERE_ARE_YOU, NULL, 1, (char *[]) {
    "Where-Are-You (originally meant for IPv6) ",
  } },
  { ICMP_IPV6_HERE_I_AM, NULL, 1, (char *[]) {
    "Here-I-Am (originally meant for IPv6)",
  } },
  { ICMP_MOBILE_REG_REQ, NULL, 1, (char *[]) {
    "Mobile Registration Request",
  } },
  { ICMP_MOBILE_REG_REPLY, NULL, 1, (char *[]) {
    "Mobile Registration Reply",
  } },
  { ICMP_DOMAIN_NAME_REQ, NULL, 1, (char *[]) {
    "Domain Name Request",
  } },
  { ICMP_DOMAIN_NAME_REPLY, NULL, 1, (char *[]) {
    "Domain Name Reply",
  } },
  { ICMP_SKIP, NULL, 1, (char *[]) {
    "SKIP Algorithm Discovery Protocol, Simple Key-Management for Internet Protocol",
  } },
  { ICMP_PHOTURIS, NULL, 1, (char *[]) {
    "Photuris, Security failures",
  } },
  { ICMP_EXPERIMENT_MOBILITY, NULL, 1, (char *[]) {
    "ICMP for experimental mobility protocols such as Seamoby [RFC4065]",
  } },
  { ICMP_EXTENDED_ECHO, "Extended Echo Request", 1, (char *[]) {
    "Request Extended Echo (XPing - see Extended Ping (Xping))",
  } },
  { ICMP_EXTENDED_ECHOREPLY, "Extended Echo Reply", 5, (char *[]) {
    "No Error",
    "Malformed Query",
    "No Such Interface",
    "No Such Table Entry",
    "Multiple Interfaces Satisfy Query",
  } },
  { ICMP_EXPERIMENT_1, NULL, 1, (char *[]) {
    "RFC3692-style Experiment 1 (RFC 4727)",
  } },
  { ICMP_EXPERIMENT_2, NULL, 1, (char *[]) {
    "RFC3692-style Experiment 2 (RFC 4727)",
  } },
};

void print_any_icmp(uint8_t type, uint8_t code) {
  printf(" - ");
  for (size_t i = 0; i < COUNTOF(icmp_diag); ++i) {
    if (icmp_diag[i].type == type) {
      if (code >= icmp_diag[i].ncode) {
        printf("Bad ICMP code for type %d", type);
        if (icmp_diag[i].text)
          printf(" (%s)", icmp_diag[i].text);
        printf(": %d\n", code);
        return ;
      }
      if (icmp_diag[i].text)
        printf("%s: ", icmp_diag[i].text);
      printf("%s", icmp_diag[i].info[code]);
      return ;
    }
  }
  printf("Bad ICMP type: %d\n", type);
}

static bool ping_stop = false;

void int_handler() {
  ping_stop = true;
}

uint16_t ping_checksum(const void * b, int len) {
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
        .sequence = 0,
      },
    }
  };

  for (size_t i = 0; i < MSG_SIZE - 1; ++i)
    data->pkt.msg[i] = i + '0';
  data->pkt.msg[MSG_SIZE - 1] = 0;
  data->tv_timeout = (struct timeval) {
    .tv_sec = PING_TIMEOUT,
  };
  data->ttl = PING_TTL;
  data->ts_end = (struct timespec) { };
  setsockopt(data->sockfd, SOL_IP, IP_TTL, &data->ttl, sizeof(data->ttl));
  setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, &data->tv_timeout, sizeof(data->tv_timeout));
  clock_gettime(CLOCK_MONOTONIC, &data->ts_start);
  return data;
}

long double average(list * lst) {
  if (!lst->size)
    return 0;

  long double sum = 0;
  list_node * node = lst->head;

  do {
    sum += *(long double *)node->data;
    node = node->next;
  } while (node != lst->head);
  return sum / lst->size;
}

long double standard_deviation(list * lst, long double avg) {
  if (!lst->size)
    return 0;

  long double sum = 0;
  list_node * node = lst->head;

  do {
    sum += pow(*(long double *)node->data - avg, 2);
  } while (node != lst->head);
  return sqrt(sum / lst->size);
}

void ping_end(ping_data * data, char * const * targets, size_t ntargets) {
  long double percent_lost, avg, stddev;

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
  unsigned ident =    getpid() & 0xffff;
  ping_data *         data;
  struct timespec     ts_send, ts_recv;
  long double         interval;
  uint8_t             ip_addr[NI_MAXHOST + 1];
  char                res_buffer[IP_HDR_MAX_SIZE + sizeof(ping_pkt)];
  struct sockaddr_in  addr;
  size_t              ip_hdr_size;
  ping_pkt *          response;

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
      continue ;
    }
    printf("PING %s (%s): %zu data bytes", targets[i], ip_addr, sizeof(data->pkt.msg));
    if (flags & PING_VERBOSE)
      printf(", id 0x%04x = %u", ident, ident);
    printf("\n");
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
      if (recvfrom(data->sockfd, &res_buffer, sizeof(res_buffer), 0, NULL, NULL) <= 0)
        continue ;
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
      ip_hdr_size = ((struct iphdr *)res_buffer)->ihl * 4;
      response = (ping_pkt *)(res_buffer + ip_hdr_size);
      printf(
        "%zu bytes from %s: icmp_seq=%d ttl=%d time=%Lf ms",
        sizeof(response->msg),
        ip_addr,
        data->pkt.hdr.un.echo.sequence,
        ((struct iphdr *)res_buffer)->ttl,
        interval
      );
      if (response->hdr.type != 0 || response->hdr.code != ICMP_ECHOREPLY) {
        if (flags & PING_VERBOSE)
          print_any_icmp(response->hdr.type, response->hdr.code);
      } else
        ++data->counters[i].received;
      printf("\n");
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
