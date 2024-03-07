#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "pika_lvgl_indev_t.h"

void pika_lvgl_indev_t_get_vect(PikaObj* self, PikaObj* point) {
    lv_indev_t* lv_indev = obj_getPtr(self, "lv_indev");
    lv_point_t* lv_point = obj_getPtr(point, "lv_point");
    lv_indev_get_vect(lv_indev, lv_point);
    obj_setInt(point, "x", lv_point->x);
    obj_setInt(point, "y", lv_point->y);
}
#endif
