#include "../inc/app_main.h"

#include <lvgl.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>

#include "../inc/form.h"
#include "../inc/hardware.h"
#include "../inc/task.h"
#include "../inc/version.h"
#include "../inc/custom_font.h"

LOG_MODULE_REGISTER(app_main);

using namespace hangang_view;

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

  struct task::AppContext app_ctx;
  task::InitTask(&app_ctx);
  k_thread_start(app_ctx.boot_task_id);
  uint32_t error_event_arg;

  for (;;) {
    gpio_pin_toggle_dt(&hangang_view::hardware::run_led);
    lv_task_handler();

    if (!app_ctx.boot_task_complete &&
        (k_uptime_get() - app_ctx.boot_task_started_uptime >
         task::kBootWatchdogInterval)) {
          k_event_post(&app_ctx.error_event, (uint32_t)task::ErrorEventArgument::kBootTimeout);
          LOG_ERR("Boot watchdog fired!, current=%llu, started=%llu", k_uptime_get(), app_ctx.boot_task_started_uptime);
    }

    error_event_arg =
        k_event_wait(&app_ctx.error_event, 0xFFFFFFFF, false, K_MSEC(100));
    if (error_event_arg != 0) {
      LOG_ERR("error event received! : %d", error_event_arg);
      ErrorForm frm;
      task::ErrorEventArgument arg =
          static_cast<task::ErrorEventArgument>(error_event_arg);
      frm.error_code_ = (int64_t)error_event_arg;
      strcpy(frm.error_message_, task::ErrorEventArgumentToString(arg));
      frm.Update();
      frm.Draw();
      for (;;) {
        lv_task_handler();
      }
    }
    
    k_sleep(K_MSEC(100));
  }
}