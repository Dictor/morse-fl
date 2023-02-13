#ifndef MORSE_FL_FORM
#define MORSE_FL_FORM

#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "version.h"

namespace kimdictor_morse_fl
{
  class IForm
  {
  protected:
    lv_obj_t *base_;
    lv_style_t base_style_;

  public:
    const int kFrameBufferWidth = 480;
    const int kFrameBufferHeight = 320;

    IForm()
    {
      lv_style_init(&base_style_);
      lv_style_set_bg_color(&base_style_, lv_color_make(0, 0, 0));
      lv_style_set_border_color(&base_style_, lv_color_make(0, 0, 0));
      base_ = lv_obj_create(lv_scr_act());
      lv_obj_set_size(base_, IForm::kFrameBufferWidth, IForm::kFrameBufferHeight);
      lv_obj_add_style(base_, &base_style_, LV_PART_MAIN);
    };

    virtual ~IForm() { lv_obj_clean(base_); };
    virtual void Update() = 0;
    virtual void Draw() = 0;
  };
}; // namespace kimdictor_morse_fl

#endif