#include <stdio.h>

#include <icmp_diag.h>
#include <misc.h>

static const struct icmp_diag
icmp_diag[] = {
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

void
print_icmp_diag(const uint8_t type, const uint8_t code) {
  printf(" - ");
  for (size_t i = 0; i < COUNTOF(icmp_diag); ++i) {
    if (icmp_diag[i].type == type) {
      if (code >= icmp_diag[i].ncode) {
        printf("Bad ICMP code for type %d", type);
        if (icmp_diag[i].text)
          printf(" (%s)", icmp_diag[i].text);
        printf(": %d", code);
        return ;
      }
      if (icmp_diag[i].text)
        printf("%s: ", icmp_diag[i].text);
      printf("%s", icmp_diag[i].info[code]);
      return ;
    }
  }
  printf("Bad ICMP type: %d", type);
}
