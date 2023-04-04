#include "../inc/hardware.h"

#include <errno.h>
#include <lvgl.h>
#include <zephyr/logging/log.h>

#include <vector>

LOG_MODULE_REGISTER(hardware);

using namespace kimdictor_morse_fl;

const struct gpio_dt_spec hardware::run_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(run_led), gpios);
const struct gpio_dt_spec hardware::err_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(err_led), gpios);
const struct gpio_dt_spec hardware::act_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(act_led), gpios);

const struct gpio_dt_spec hardware::user_button =
    GPIO_DT_SPEC_GET(DT_NODELABEL(user_button), gpios);

const struct device* hardware::display = DEVICE_DT_GET(DT_NODELABEL(ltdc));

int hardware::CheckHardware() {
  const std::vector<const device*> check_list = {
      run_led.port, err_led.port, act_led.port, user_button.port, display};
  for (auto d : check_list) {
    if (d == nullptr) return -EINVAL;
    if (!device_is_ready(d)) return -ENODEV;
  }
  return 0;
}

int hardware::InitHardware() {
  gpio_pin_configure_dt(&hardware::run_led, GPIO_OUTPUT);
  gpio_pin_configure_dt(&hardware::err_led, GPIO_OUTPUT);
  gpio_pin_configure_dt(&hardware::act_led, GPIO_OUTPUT);
  gpio_pin_configure_dt(&hardware::user_button, GPIO_INPUT);
  return 0;
}