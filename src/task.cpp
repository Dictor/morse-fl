#include "../inc/task.h"

#include <zephyr/net/net_ip.h>

#include "../inc/form.h"
#include "../inc/hardware.h"

using namespace hangang_view;

void hangang_view::task::BootTask(void *, void *, void *) {
  DebugForm frm;
  frm.Update();
  frm.Draw();
  
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
}