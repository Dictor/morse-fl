#include "../inc/hardware.h"

#include <zephyr/logging/log.h>
#include <errno.h>
#include <lvgl.h>

LOG_MODULE_REGISTER(hardware);

using namespace kimdictor_morse_fl;

const struct gpio_dt_spec hardware::run_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(run_led), gpios);
const struct gpio_dt_spec hardware::err_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(err_led), gpios);
const struct gpio_dt_spec hardware::act_led =
    GPIO_DT_SPEC_GET(DT_NODELABEL(act_led), gpios);
const struct device* hardware::display = DEVICE_DT_GET(DT_NODELABEL(ltdc));

int hardware::CheckHardware() {
  /*
  check_list is pointer array of device struct and kChecklistSize define its
  size. CPP container is intentionally hestatied because it's system initialize
  code
  */
  const int kChecklistSize = 4;
  const device *check_list[kChecklistSize] = {run_led.port, err_led.port,
                                              act_led.port, display};

  for (int i = 0; i < kChecklistSize; i++) {
    if (check_list[i] == NULL) return -EINVAL;
    if (!device_is_ready(check_list[i])) return -ENODEV;
  }
  return 0;
}

int hardware::InitHardware() {
  gpio_pin_configure_dt(&hardware::run_led, GPIO_OUTPUT);
  gpio_pin_configure_dt(&hardware::err_led, GPIO_OUTPUT);
  gpio_pin_configure_dt(&hardware::act_led, GPIO_OUTPUT);

  return 0;
}