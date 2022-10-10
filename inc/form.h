#ifndef HANGANG_VIEW_FORM
#define HANGANG_VIEW_FORM

#include <stdio.h>
#include <string.h>

#include <lvgl.h>

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

class DebugForm : public IForm {
 private:
  char version_[25];

 public:
  DebugForm(){};
  bool ip_attached_ = false;
  char ip_address_[25];

  void Update() {
    sprintf(version_, "hangang-view %d.%d.%d", kMajorVersion, kMinorVersion,
            kHotfixVersion);
  }

  void Draw() {
    lv_obj_t* base = lv_obj_create(lv_scr_act());  // TODO: create screen
    lv_obj_set_size(base, IForm::kFrameBufferWidth, IForm::kFrameBufferHeight);
    lv_obj_align(base, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_flex_flow(base, LV_FLEX_FLOW_COLUMN);

    lv_obj_t* label_title = lv_label_create(base);
    lv_label_set_text(label_title, "<hangang-view BOOT>");

    lv_obj_t* label_version = lv_label_create(base);
    lv_label_set_text(label_version, version_);

    lv_obj_t* label_eth_status = lv_label_create(base);
    lv_label_set_text_fmt(label_eth_status, "ip attached : %s", ip_attached_ ? "O" : "X");
    
    lv_obj_t* label_ip = lv_label_create(base);
    lv_label_set_text_fmt(label_ip, "ip address : %s", ip_address_);
  }
};

class StockPriceForm : IForm {};
};  // namespace hangang_view

#endif