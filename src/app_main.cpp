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
#include "../inc/genann.h"
#include "../inc/hardware.h"
#include "../inc/version.h"
#include "../inc/weight_bias.h"

LOG_MODULE_REGISTER(app_main);

using namespace kimdictor_morse_fl;

genann *ann;
const int conv1_size = convolution_layer::conv1_out_dims.w * convolution_layer::conv1_out_dims.c;
const int conv2_size = convolution_layer::conv2_out_dims.w * convolution_layer::conv2_out_dims.c;

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

  int8_t test_data[] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int8_t *in_buffer, *out_buffer;

  for (auto &d : test_data)
    if (d > 0) d = 127;

  in_buffer = test_data;
  out_buffer = (int8_t *)malloc(sizeof(int8_t) * conv1_size);
  if (out_buffer == nullptr) {
    LOG_ERR("failed to allocate conv1 output buffer");
    return;
  }
  memset(out_buffer, 0, sizeof(int8_t) * conv1_size);

  convolution_layer::Init();
  LOG_HEXDUMP_INF(in_buffer, 64, "input of conv1");
  LOG_INF("start conv1");
  convolution_layer::ConvolutionLayer1((q7_t *)in_buffer, (q7_t *)out_buffer);
  LOG_INF("finish conv1");

  LOG_HEXDUMP_INF(out_buffer, 300, "output of conv1 (first 300 bytes)");

  in_buffer = out_buffer;
  out_buffer = (int8_t *)malloc(sizeof(int8_t) * conv2_size);
  if (out_buffer == nullptr) {
    LOG_ERR("failed to allocate conv2 output buffer");
    return;
  }
  LOG_INF("start conv2");
  convolution_layer::ConvolutionLayer2((q7_t *)in_buffer, (q7_t *)out_buffer);
  free(in_buffer);
  LOG_INF("finish conv2");

  // actual data on here is 768 bytes
  LOG_HEXDUMP_INF(out_buffer, 300, "output of conv2 (first 300 bytes)");

  ann = genann_init(conv2_size, 0, 0, 37);
  double *nn_input = (double *)malloc(sizeof(double) * conv2_size);
  for (int i = 0; i < conv2_size; i++) {
    nn_input[i] = out_buffer[i] * convolution_layer::conv2_real_scale;
  }

  memcpy(ann->weight, convolution_layer::fc, sizeof(convolution_layer::fc));
  const double *nn_output = genann_run(ann, nn_input);
  double nn_output_max_value = 0.f;
  int nn_output_max_index = 0;
  for (int i = 0; i < 37; i++) {
    if (nn_output[i] >= nn_output_max_value) {
      nn_output_max_value = nn_output[i];
      nn_output_max_index = i;
    }
  }
  LOG_INF("result is %d -> %f", nn_output_max_index, nn_output_max_value);

  for (;;) {
    gpio_pin_toggle_dt(&hardware::run_led);
    lv_task_handler();
    k_sleep(K_MSEC(100));
  }
}