#include "../inc/dns.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(dns_resolver);

using namespace hangang_view;

int DNSResolver::Resolve() {
  int ret;
  ret = dns_get_addr_info(dns_query_, DNS_QUERY_TYPE_A, &dns_id_,
                          &hangang_view::DNSResolver::Callback, (void *)this,
                          dns_timeout_);
  LOG_INF("DNS resolving started, id = %d, host = %s", dns_id_, dns_query_);
  if (ret < 0) {
    LOG_ERR("Cannot resolve IPv4 address (%d)", ret);
  }
  return ret;
};

void DNSResolver::Callback(enum dns_resolve_status status,
                           struct dns_addrinfo *info, void *user_data) {
  /* hr_* is just for logging*/
  char hr_addr[NET_IPV6_ADDR_LEN];
  void *addr;
  DNSResolver *instance = (DNSResolver *)user_data;

  switch (status) {
    case DNS_EAI_CANCELED:
      LOG_INF("DNS query was canceled");
      atomic_set_bit(&instance->dns_status_, kStatusBitComplete);
      return;
    case DNS_EAI_FAIL:
      LOG_INF("DNS resolve failed");
      atomic_set_bit(&instance->dns_status_, kStatusBitComplete);
      return;
    case DNS_EAI_NODATA:
      LOG_INF("Cannot resolve address");
      atomic_set_bit(&instance->dns_status_, kStatusBitComplete);
      return;
    case DNS_EAI_ALLDONE:
      LOG_INF("DNS resolving finished");
      atomic_set_bit(&instance->dns_status_, kStatusBitComplete);
      return;
    case DNS_EAI_INPROGRESS:
      atomic_clear_bit(&instance->dns_status_, kStatusBitError);
      break;
    default:
      LOG_ERR("DNS resolving error (%d)", status);
      atomic_set_bit(&instance->dns_status_, kStatusBitComplete);
      atomic_set_bit(&instance->dns_status_, kStatusBitError);
      return;
  }

  if (!info) {
    return;
  }

  if (info->ai_family == AF_INET) {
    instance->dns_addr4_ = net_sin(&info->ai_addr)->sin_addr;
    addr = (void *)&instance->dns_addr4_;
    atomic_set_bit(&instance->dns_status_, kStatusBitIPv4);
  } else if (info->ai_family == AF_INET6) {
    instance->dns_addr6_ = net_sin6(&info->ai_addr)->sin6_addr;
    addr = (void *)&instance->dns_addr6_;
    atomic_clear_bit(&instance->dns_status_, kStatusBitIPv4);
  } else {
    LOG_ERR("Invalid IP address family %d", info->ai_family);
    atomic_set_bit(&instance->dns_status_, kStatusBitError);
    return;
  }

  LOG_INF("DNS resolved successful: %s",
          net_addr_ntop(info->ai_family, addr, hr_addr, sizeof(hr_addr)));
  atomic_set_bit(&instance->dns_status_, kStatusBitSuccess);
}

bool DNSResolver::IsSuccess() {
  return atomic_test_bit(&dns_status_, kStatusBitSuccess);
}

bool DNSResolver::HasError() {
  return atomic_test_bit(&dns_status_, kStatusBitError);
}

bool DNSResolver::IsCompleted() {
  return atomic_test_bit(&dns_status_, kStatusBitComplete);
}

bool DNSResolver::IsIPv4() {
  return atomic_test_bit(&dns_status_, kStatusBitIPv4);
}

in_addr DNSResolver::GetIPv4Address() { return dns_addr4_; }
in6_addr DNSResolver::GetIPv6Address() { return dns_addr6_; }