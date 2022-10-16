#include "../inc/task.h"

#include <zephyr/net/net_ip.h>

#include "../inc/dns.h"
#include "../inc/form.h"
#include "../inc/hardware.h"

LOG_MODULE_REGISTER(task);

using namespace hangang_view;

void hangang_view::task::BootTask(void *, void *, void *) {
  LOG_INF("BootTask [1/4] draw debug form");
  DebugForm frm;
  frm.Update();
  frm.Draw();

  LOG_INF("BootTask [2/4] attach ipv4 & dhcpv4");
  while (true) {
    if (hardware::NetIPv4Attached()) {
      frm.ip_attached_ = true;
      in_addr addr = hardware::NetGetIPv4Address();
      net_addr_ntop(AF_INET, &addr, frm.ip_address_, sizeof(frm.ip_address_));
      frm.Update();
      frm.Draw();
    } else {
      k_sleep(K_MSEC(100));
      continue;
    }
    break;
  }

  LOG_INF("BootTask [3/4] resolving DNS for MQTT server");
  DNSResolver dns("api.chinchister.com");
  dns.Resolve();
  while (!dns.IsSuccess()) {
    if (dns.HasError()) {
      LOG_ERR("dns resolving failure");
      return;
    }
    k_sleep(K_MSEC(100));
  }
  if (!dns.IsIPv4()) {
    LOG_ERR("IPv6 not supported yet");
    return;
  }
}