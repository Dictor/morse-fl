#ifndef HANGANG_VIEW_FORM
#define HANGANG_VIEW_FORM

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "version.h"

namespace hangang_view {
class IForm {
 public:
  const int kFrameBufferWidth = 480;
  const int kFrameBufferHeight = 320;

  virtual ~IForm(){};
  virtual void Update() = 0;
  virtual void Draw() = 0;
};

class PriceForm : public IForm {
 private:
  lv_obj_t *base_, *label_name_, *label_price_, *label_percentile_;

 public:
  PriceForm() {
    base_ = lv_obj_create(lv_scr_act());  // TODO: create screen
    lv_obj_set_size(base_, IForm::kFrameBufferWidth, IForm::kFrameBufferHeight);
    lv_obj_align(base_, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_flex_flow(base_, LV_FLEX_FLOW_COLUMN);
    label_name_ = lv_label_create(base_);
    label_price_ = lv_label_create(base_);
    label_percentile_ = lv_label_create(base_);
  };

  ~PriceForm() {
    lv_obj_clean(label_name_);
    lv_obj_clean(label_price_);
    lv_obj_clean(label_percentile_);
    lv_obj_clean(base_);
  }

  char name_[20];
  float price_;
  float percentile_;

  void Update() {}
  void Draw() {
    lv_label_set_text(label_name_, name_);
    lv_label_set_text_fmt(label_price_, "%.2f", price_);
    lv_label_set_text_fmt(label_percentile_, "%.2f%%", percentile_);
  }
};

class DebugForm : public IForm {
 private:
  char version_[25];
  lv_obj_t *base_, *label_title_, *label_version_, *label_eth_status_, *label_ip_,
      *label_dns_, *label_mqtt_, *label_boot_, *label_post_;

 public:
  DebugForm() {
    base_ = lv_obj_create(lv_scr_act());  // TODO: create screen
    lv_obj_set_size(base_, IForm::kFrameBufferWidth, IForm::kFrameBufferHeight);
    lv_obj_align(base_, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_flex_flow(base_, LV_FLEX_FLOW_COLUMN);

    label_title_ = lv_label_create(base_);
    label_version_ = lv_label_create(base_);
    label_eth_status_ = lv_label_create(base_);
    label_ip_ = lv_label_create(base_);
    label_dns_ = lv_label_create(base_);
    label_mqtt_ = lv_label_create(base_);
    label_boot_ = lv_label_create(base_);
    label_post_ = lv_label_create(base_);
  };

  ~DebugForm() {
    lv_obj_clean(label_title_);
    lv_obj_clean(label_version_);
    lv_obj_clean(label_eth_status_);
    lv_obj_clean(label_ip_);
    lv_obj_clean(label_dns_);
    lv_obj_clean(label_mqtt_);
    lv_obj_clean(label_boot_);
    lv_obj_clean(label_post_);
    lv_obj_clean(base_);
  }

  bool ip_attached_ = false;
  bool dns_completed_ = false;
  bool mqtt_connected_ = false;
  bool boot_success_ = false;
  char ip_address_[25];
  char dns_address_[25];

  void Update() {
    sprintf(version_, "hangang-view %d.%d.%d", kMajorVersion, kMinorVersion,
            kHotfixVersion);
  }

  void Draw() {
    lv_label_set_text(label_title_, "<hangang-view BOOT>");
    lv_label_set_text(label_version_, version_);
    lv_label_set_text_fmt(label_eth_status_, "ip attached : %s",
                          ip_attached_ ? "O" : "X");
    lv_label_set_text_fmt(label_ip_, "ip address : %s", ip_address_);
    lv_label_set_text_fmt(label_dns_, "server address : %s", dns_address_);
    lv_label_set_text_fmt(label_mqtt_, "mqtt connected : %s",
                          mqtt_connected_ ? "O" : "X");
    lv_label_set_text_fmt(label_boot_, "boot complete : %s",
                          boot_success_ ? "O" : "X");
    if (boot_success_) {
      lv_label_set_text_fmt(label_post_,
                            "!! price display started after 5 sec...");
    }
  }
};
};  // namespace hangang_view

#endif