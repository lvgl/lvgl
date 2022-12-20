#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "pika_lvgl_lv_event.h"

int pika_lvgl_lv_event_get_code(PikaObj *self){
    lv_event_t *lv_event = obj_getPtr(self, "lv_event");
    return lv_event_get_code(lv_event);
}

PikaObj *New_pika_lvgl_lv_obj(Args *args);
PikaObj* pika_lvgl_lv_event_get_target(PikaObj *self){
    lv_event_t *lv_event = obj_getPtr(self, "lv_event");
    lv_obj_t* lv_obj = lv_event_get_target(lv_event);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}
#endif
