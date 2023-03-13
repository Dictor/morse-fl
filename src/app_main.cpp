#include "../inc/app_main.h"

#include <lvgl.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/zephyr.h>

#include <memory>

#include "../inc/convolution_layer.h"
#include "../inc/form.h"
#include "../inc/hardware.h"
#include "../inc/version.h"

LOG_MODULE_REGISTER(app_main);

using namespace kimdictor_morse_fl;

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

  int8_t test_data[] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
                        1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
                        0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
  int8_t *in_buffer, *out_buffer;

  for (auto &d : test_data) if (d > 0) d = 127;

  in_buffer = test_data;
  out_buffer = (int8_t *)malloc(sizeof(int8_t) * 64 * 24);
  if (out_buffer == nullptr) {
    LOG_ERR("failed to allocate conv1 output buffer");
    return;
  }

  LOG_HEXDUMP_INF(in_buffer, 64, "input of conv1 (first 300 bytes)");
  LOG_INF("start conv1");
  convolution_layer::ConvolutionLayer1((q7_t *)in_buffer, (q7_t *)out_buffer);
  LOG_INF("finish conv1");
  LOG_HEXDUMP_INF(out_buffer, 300, "output of conv1 (first 300 bytes)");

  in_buffer = out_buffer;
  out_buffer =
      (int8_t *)malloc(sizeof(int8_t) * 32 * 48);
  if (out_buffer == nullptr) {
    LOG_ERR("failed to allocate conv2 output buffer");
    return;
  }
  LOG_INF("start conv2");
  convolution_layer::ConvolutionLayer2((q7_t *)in_buffer, (q7_t *)out_buffer);
  free(in_buffer);
  LOG_INF("finish conv2");

  LOG_HEXDUMP_INF(out_buffer, 300, "output of conv2 (first 300 bytes)");

  for (;;) {
    gpio_pin_toggle_dt(&hardware::run_led);
    lv_task_handler();
    k_sleep(K_MSEC(100));
  }
}
