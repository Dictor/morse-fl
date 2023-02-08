#ifndef HANGANG_VIEW_TASK
#define HANGANG_VIEW_TASK

#include "../inc/mqtt.h"
#include "../inc/json.h"
#include <zephyr/kernel.h>
namespace hangang_view {
namespace task {

static const int kPriceTaskLoopInterval = 100;
static const int kSymbolDisplayChangeInterval = 3000;
static const int kPublishWatchdogInterval = 60000;
static const int kBootWatchdogInterval = 30000;

struct AppContext {
  k_tid_t boot_task_id;
  k_tid_t price_task_id;
  MQTTClient *mqtt;
  struct json::symbols symbols;
  struct k_event error_event;
  uint32_t boot_task_started_uptime;
  uint32_t price_task_latest_update_time;
  bool boot_task_complete;
};

enum class ErrorEventArgument : uint32_t {
  kBootTimeout = 1U << 0,
  kPublishWatchdogFired = 1U << 1,
  kDNSFailure = 1U << 2,
  kMQTTFailure = 1U << 3,
};

const char* ErrorEventArgumentToString(ErrorEventArgument arg);

void InitTask(struct AppContext *ctx);
void BootTask(void *, void *, void *);
void PriceTask(void *, void *, void *);
};  // namespace task
};  // namespace hangang_view

#endif