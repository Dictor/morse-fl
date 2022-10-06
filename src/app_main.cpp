#include "../inc/app_main.h"

#include <lvgl.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>

#include "../inc/hardware.h"
#include "../inc/version.h"

LOG_MODULE_REGISTER(app_main);

using namespace hangang_view;

void LVGLMain(void) {
  if (!device_is_ready(hardware::display)) {
    LOG_ERR("Device not ready, aborting test");
    return;
  }

  lv_obj_t *hello_world_label;
  hello_world_label = lv_label_create(lv_scr_act());
  lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);
  lv_task_handler();

  LOG_INF("display complete");
}

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
  LVGLMain();
  for (;;) {
    gpio_pin_toggle_dt(&hangang_view::hardware::run_led);
    k_sleep(K_MSEC(1000));
  }
}