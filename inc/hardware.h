#ifndef MORSE_FL_HARDWARE
#define MORSE_FL_HARDWARE

/* zephyr device and driver */
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

namespace kimdictor_morse_fl {
namespace hardware {

extern const struct gpio_dt_spec run_led, err_led, act_led;
extern const struct device *display;

int CheckHardware();
int InitHardware();

}  // namespace hardware
}  // namespace kimdictor_morse_fl

#endif