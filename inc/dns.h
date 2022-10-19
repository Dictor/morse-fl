#ifndef HANGANG_VIEW_DNS
#define HANGANG_VIEW_DNS

#include <stdint.h>
#include <zephyr/net/dns_resolve.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/sys/atomic.h>

namespace hangang_view {
class DNSResolver {
 private:
  int dns_timeout_;
  uint16_t dns_id_;
  bool is_ipv4_;
  in_addr dns_addr4_;
  in6_addr dns_addr6_;

  atomic_t dns_status_;
  enum MQTTStatusBit {
    kBitSuccess = 0,
    kBitError = 1,
    kBitComplete = 1,
    kBitIPv4 = 7
  };

  static void Callback(enum dns_resolve_status status,
                       struct dns_addrinfo *info, void *user_data);

 public:
  const char *dns_query_;
  DNSResolver(const char *query) : dns_timeout_(1000), dns_query_(query) {
    dns_status_ = ATOMIC_INIT(0);
  };
  int Resolve();
  bool IsSuccess();
  bool HasError();
  bool IsCompleted();
  bool IsIPv4();
  in_addr GetIPv4Address();
  in6_addr GetIPv6Address();
};
};  // namespace hangang_view

#endif