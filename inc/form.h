#ifndef HANGANG_VIEW_FORM
#define HANGANG_VIEW_FORM

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "version.h"

namespace hangang_view {
class IForm {
 protected:
  lv_obj_t *base_;
  lv_style_t base_style_;

 public:
  const int kFrameBufferWidth = 480;
  const int kFrameBufferHeight = 320;

  IForm() {
    lv_style_init(&base_style_);
    lv_style_set_bg_color(&base_style_, lv_color_make(0, 0, 0));
    lv_style_set_border_color(&base_style_, lv_color_make(0, 0, 0));
    base_ = lv_obj_create(lv_scr_act());
    lv_obj_set_size(base_, IForm::kFrameBufferWidth, IForm::kFrameBufferHeight);
    lv_obj_add_style(base_, &base_style_, LV_PART_MAIN);
  };

  virtual ~IForm(){};
  virtual void Update() = 0;
  virtual void Draw() = 0;
};

class ErrorForm : public IForm {
 private:
  lv_obj_t *label_introduce_, *label_content_;

 public:
  int64_t error_code_;
  char error_message_[50];

  ErrorForm() {
    lv_style_set_bg_color(&base_style_, lv_color_make(255, 0, 0));
    lv_obj_add_style(base_, &base_style_, LV_PART_MAIN);
    lv_obj_align(base_, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_flex_flow(base_, LV_FLEX_FLOW_COLUMN);

    label_introduce_ = lv_label_create(base_);
    label_content_ = lv_label_create(base_);
  }

  ~ErrorForm() {
    lv_obj_clean(label_introduce_);
    lv_obj_clean(label_content_);
  }

  void Update() {}

  void Draw() {
    lv_label_set_text(
        label_introduce_,
        "!!fatal error caused!!\nsystem will be reset automatically");
    lv_label_set_text_fmt(label_content_, "error code : 0x%d\nerror message : %s",
                          error_code_, error_message_);
  }
};

class PriceForm : public IForm {
 private:
  lv_obj_t *label_name_, *label_price_, *label_percentile_;
  lv_style_t price_style_, name_style_, perc_style_;
  lv_color_t price_perc_color_;
  char percentile_prefix_[2];

 public:
  PriceForm() {
    lv_style_init(&price_style_);
    lv_style_set_text_font(&price_style_, &lv_font_montserrat_48);

    lv_style_init(&name_style_);
    lv_style_set_text_font(&name_style_, &lv_font_montserrat_24);

    lv_style_init(&perc_style_);
    lv_style_set_text_font(&perc_style_, &lv_font_montserrat_24);

    lv_obj_align(base_, LV_ALIGN_TOP_LEFT, 0, 0);

    label_price_ = lv_label_create(base_);
    lv_obj_align(label_price_, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_style(label_price_, &price_style_, LV_PART_MAIN);

    label_name_ = lv_label_create(base_);
    lv_obj_add_style(label_name_, &name_style_, LV_PART_MAIN);
    lv_label_set_long_mode(label_name_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align_to(label_name_, label_price_, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    label_percentile_ = lv_label_create(base_);
    lv_obj_add_style(label_percentile_, &perc_style_, LV_PART_MAIN);
    lv_obj_align_to(label_percentile_, label_price_, LV_ALIGN_OUT_BOTTOM_LEFT,
                    0, 0);
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

  void Update() {
    if (percentile_ > 0) {
      percentile_prefix_[0] = '+';
      price_perc_color_ = lv_palette_main(LV_PALETTE_RED);
    } else if (percentile_ == 0) {
      percentile_prefix_[0] = '\0';
      price_perc_color_ = lv_color_white();
    } else {
      percentile_prefix_[0] = '\0';
      price_perc_color_ = lv_palette_main(LV_PALETTE_BLUE);
    }
    percentile_prefix_[1] = '\0';
  }

  void Draw() {
    lv_label_set_text(label_name_, name_);
    lv_label_set_text_fmt(label_price_, "%.2f", price_);
    lv_label_set_text_fmt(label_percentile_, "%s%.2f%%", percentile_prefix_, percentile_);
    lv_style_set_text_color(&price_style_, price_perc_color_);
    lv_style_set_text_color(&perc_style_, price_perc_color_);
  }
};

class DebugForm : public IForm {
 private:
  char version_[25];
  lv_obj_t *label_title_, *label_version_, *label_eth_status_, *label_ip_,
      *label_dns_, *label_mqtt_, *label_boot_, *label_post_;

 public:
  DebugForm() {
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
    lv_label_set_text_fmt(
        label_post_,
        boot_success_ ? "!! price display started after 5 sec..." : "");
  }
};
};  // namespace hangang_view

#endif