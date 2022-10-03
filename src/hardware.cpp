#include "../inc/hardware.h"

#include <errno.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_core.h>

LOG_MODULE_REGISTER(hardware);

using namespace hangang_view;

const struct gpio_dt_spec hardware::run_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(run_led), gpios);
const struct gpio_dt_spec hardware::err_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(err_led), gpios);
const struct gpio_dt_spec hardware::act_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(act_led), gpios);
//const struct device* hardware::display = DEVICE_DT_GET(DT_NODELABEL(ltdc));

struct net_mgmt_event_callback hardware::net_mgmt_cb;
struct net_if *hardware::net_iface;

int hardware::CheckHardware() {
  /*
  check_list is pointer array of device struct and kChecklistSize define its
  size. CPP container is intentionally hestatied because it's system initialize
  code
  */
  const int kChecklistSize = 3;
  const device *check_list[kChecklistSize] = {run_led.port, err_led.port,
                                              act_led.port};

  for (int i = 0; i < kChecklistSize; i++) {
    if (check_list[i] == NULL) return -EINVAL;
    if (!device_is_ready(check_list[i])) return -ENODEV;
  }
  return 0;
}

void hardware::NetMgmtHandler(struct net_mgmt_event_callback *cb,
                              uint32_t mgmt_event, struct net_if *iface) {
  LOG_DBG("NetMgmtHandler fired");
  switch (mgmt_event) {
    case NET_EVENT_IPV4_ADDR_ADD:
      LOG_INF("IPv4 address attached!");
      for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
        char buf[NET_IPV4_ADDR_LEN];

        if (iface->config.ip.ipv4->unicast[i].addr_type != NET_ADDR_DHCP) {
          continue;
        }

        LOG_INF("Address %d -----------", i);
        LOG_INF("IP address: %s",
                net_addr_ntop(
                    AF_INET, &iface->config.ip.ipv4->unicast[i].address.in_addr,
                    buf, sizeof(buf)));
        LOG_INF("Lease time: %u seconds", iface->config.dhcpv4.lease_time);
        LOG_INF("Subnet: %s",
                net_addr_ntop(AF_INET, &iface->config.ip.ipv4->netmask, buf,
                              sizeof(buf)));
        LOG_INF("Router: %s", net_addr_ntop(AF_INET, &iface->config.ip.ipv4->gw,
                                            buf, sizeof(buf)));
        LOG_INF("----------------------");
      }
      break;
    default:
      break;
  }
}

int hardware::InitHardware() {
  gpio_pin_configure_dt(&hardware::run_led, GPIO_OUTPUT);
  gpio_pin_configure_dt(&hardware::err_led, GPIO_OUTPUT);
  gpio_pin_configure_dt(&hardware::act_led, GPIO_OUTPUT);

  net_mgmt_init_event_callback(&hardware::net_mgmt_cb, hardware::NetMgmtHandler,
                               NET_EVENT_IPV4_ADDR_ADD);
  net_mgmt_add_event_callback(&hardware::net_mgmt_cb);
  hardware::net_iface = net_if_get_default();
  net_dhcpv4_start(hardware::net_iface);

  return 0;
}