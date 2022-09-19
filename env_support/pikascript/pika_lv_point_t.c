#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "pika_lvgl_point_t.h"

void pika_lvgl_point_t___init__(PikaObj* self) {
    lv_point_t lv_point = {0};
    args_setStruct(self->list, "lv_point_struct", lv_point);
    obj_setPtr(self, "lv_point", args_getStruct(self->list, "lv_point_struct"));
}
#endif
