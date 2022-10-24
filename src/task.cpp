#include "../inc/task.h"

#include <zephyr/net/net_ip.h>

#include "../inc/dns.h"
#include "../inc/form.h"
#include "../inc/hardware.h"

LOG_MODULE_REGISTER(task);

using namespace hangang_view;

K_THREAD_STACK_DEFINE(task_boot_stack, 4096);
struct k_thread task_boot_data;
K_THREAD_STACK_DEFINE(task_price_stack, 4096);
struct k_thread task_price_data;

void task::InitTask(struct AppContext *ctx) {
  ctx->boot_task_id = k_thread_create(
      &task_boot_data, task_boot_stack, K_THREAD_STACK_SIZEOF(task_boot_stack),
      task::BootTask, (void *)ctx, NULL, NULL, 5, 0, K_FOREVER);
  ctx->price_task_id =
      k_thread_create(&task_price_data, task_price_stack,
                      K_THREAD_STACK_SIZEOF(task_price_stack), task::PriceTask,
                      (void *)ctx, NULL, NULL, 5, 0, K_FOREVER);
  ctx->mqtt = NULL;
  memset((void *)&ctx->symbols, 0, sizeof(ctx->symbols));
  return;
}

void task::PriceTask(void *ctx, void *, void *) {
  LOG_INF("PriceTask started!");
  struct task::AppContext *app_ctx = (struct task::AppContext *)ctx;
  PriceForm frm;
  static const int kTaskLoopInterval = 100;
  static const int kSymbolChangeInterval = 3000;
  int current_symbol_index = 0, symbol_interval_counter = 0;
  while (true) {
    app_ctx->mqtt->Input();
    k_sleep(K_MSEC(kTaskLoopInterval));
    symbol_interval_counter++;

    if (symbol_interval_counter > kSymbolChangeInterval / kTaskLoopInterval) {
      symbol_interval_counter = 0;
      if (app_ctx->symbols.symbols_len > 0) {
        LOG_INF("display %dth symbol", current_symbol_index);
        struct json::symbol symbol =
            app_ctx->symbols.symbols[current_symbol_index];
        strcpy(frm.name_, symbol.name);
        frm.price_ = (float)symbol.price / 100.0f;
        frm.percentile_ = (float)symbol.percentile / 100.0f;
        frm.Update();
        frm.Draw();
        current_symbol_index++;
        if (current_symbol_index > app_ctx->symbols.symbols_len - 1)
          current_symbol_index = 0;
      }
    }
  };
}

void task::BootTask(void *ctx, void *, void *) {
  struct task::AppContext *app_ctx = (struct task::AppContext *)ctx;
  bool successBoot = false;

  LOG_INF("BootTask [1/4] draw debug form");
  DebugForm frm;
  frm.Update();
  frm.Draw();

  LOG_INF("BootTask [2/4] attach ipv4 & dhcpv4");
  while (true) {
    if (hardware::NetIPv4Attached()) {
      frm.ip_attached_ = true;
      in_addr addr = hardware::NetGetIPv4Address();
      net_addr_ntop(AF_INET, &addr, frm.ip_address_, sizeof(frm.ip_address_));
      frm.Update();
      frm.Draw();
    } else {
      k_sleep(K_MSEC(100));
      continue;
    }
    break;
  }

  LOG_INF("BootTask [3/4] resolving DNS for MQTT server");
  DNSResolver dns("192.168.0.8");
  dns.Resolve();
  while (!dns.IsSuccess()) {
    if (dns.HasError()) {
      LOG_ERR("dns resolving failure");
      return;
    }
    k_sleep(K_MSEC(100));
  }
  if (!dns.IsIPv4()) {
    LOG_ERR("IPv6 not supported yet");
    return;
  }

  in_addr server_addr = dns.GetIPv4Address();
  net_addr_ntop(AF_INET, &server_addr, frm.dns_address_,
                sizeof(frm.dns_address_));
  frm.Update();
  frm.Draw();

  int err = 0;
  LOG_INF("BootTask [4/4] connect to MQTT server");
  auto mqtt = new MQTTClient(server_addr, 3000, &app_ctx->symbols);
  mqtt->Connect();
  if (err = mqtt->WaitEstablished(100) < 0) {
    LOG_ERR("MQTT wait socket fail : %d", err);
  }
  while (true) {
    if (mqtt->HasError()) {
      LOG_ERR("MQTT error caused");
      return;
    }
    if (mqtt->IsConnected()) {
      LOG_INF("MQTT connected!");
      break;
    }
  }
  frm.mqtt_connected_ = true;
  frm.Update();
  frm.Draw();

  if (err = mqtt->Subscribe("symbol") < 0) {
    LOG_ERR("MQTT subscribe fail : %d", err);
  } else {
    successBoot = true;
  }

  if (successBoot) {
    LOG_INF("BootTask completed!");
    app_ctx->mqtt = mqtt;
    frm.boot_success_ = true;
    frm.Update();
    frm.Draw();
    k_sleep(K_MSEC(5000));
    k_thread_start(app_ctx->price_task_id);
  } else {
    LOG_INF("BootTask failed!");
  }

  return;
}