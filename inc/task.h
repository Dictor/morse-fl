#ifndef HANGANG_VIEW_TASK
#define HANGANG_VIEW_TASK

#include "../inc/mqtt.h"
namespace hangang_view {
namespace task {
struct AppContext {
  k_tid_t boot_task_id;
  k_tid_t price_task_id;
  MQTTClient *mqtt;
};

void InitTask(struct AppContext *ctx);
void BootTask(void *, void *, void *);
void PriceTask(void *, void *, void *);
};  // namespace task
};  // namespace hangang_view

#endif