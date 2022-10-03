#ifndef HANGANG_VIEW_HARDWARE
#define HANGANG_VIEW_HARDWARE

/* zephyr device and driver */
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

/* zephyr network */
#include <stdint.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>

namespace hangang_view {
namespace hardware {

extern const struct gpio_dt_spec run_led, err_led, act_led;
extern const struct device *display;
extern struct net_mgmt_event_callback net_mgmt_cb;
extern struct net_if *net_iface;

static void NetMgmtHandler(struct net_mgmt_event_callback *cb,
                           uint32_t mgmt_event, struct net_if *iface);
int CheckHardware();
int InitHardware();

}  // namespace hardware
}  // namespace hangang_view

#endif