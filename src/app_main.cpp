#include "../inc/app_main.h"

#include <lvgl.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>

#include "../inc/hardware.h"
#include "../inc/version.h"
#include "../inc/form.h"
#include "../inc/task.h"

LOG_MODULE_REGISTER(app_main);

using namespace hangang_view;

K_THREAD_STACK_DEFINE(task_boot_stack, 4096);
struct k_thread task_boot_data;

void AppMain(void) {
  /* hardware initialization */
  LOG_INF("hardware initialization start");
  if (int ret = hardware::CheckHardware() < 0) {
    LOG_ERR("fail to check hardware, ret=%d", ret);
    return;
  }
  if (int ret = hardware::InitHardware() < 0) {
    LOG_ERR("fail to initiate hardware, ret=%d", ret);
    return;
  }
  LOG_INF("hardware initialization complete");

  /* application logic */
  LOG_INF("application started");
  LOG_INF("hangang-view %d.%d.%d", hangang_view::kMajorVersion,
          hangang_view::kMinorVersion, hangang_view::kHotfixVersion);

  k_tid_t tid_boot = k_thread_create(&task_boot_data, task_boot_stack,
                                 K_THREAD_STACK_SIZEOF(task_boot_stack),
                                 task::BootTask,
                                 NULL, NULL, NULL,
                                 5, 0, K_NO_WAIT);

  for (;;) {
    gpio_pin_toggle_dt(&hangang_view::hardware::run_led);
    lv_task_handler();
    k_sleep(K_MSEC(100));
  }
}