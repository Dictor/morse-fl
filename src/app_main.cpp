#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>

#include "../inc/version.h"
#include "../inc/app_main.h"

LOG_MODULE_REGISTER(app_main);

void AppMain(void) {
  LOG_INF("application started");
  LOG_INF("hangang-view %d.%d.%d", hangang_view::kMajorVersion,
          hangang_view::kMinorVersion, hangang_view::kHotfixVersion);
}