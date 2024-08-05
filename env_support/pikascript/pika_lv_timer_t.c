#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "pika_lvgl_lv_timer_t.h"

PikaEventListener* g_pika_lv_timer_event_listener;

void __pika_timer_cb(lv_timer_t* timer) {
    PikaObj* eventHandleObj = pks_eventListener_getEventHandleObj(
        g_pika_lv_timer_event_listener, (uint32_t)timer);
    obj_newDirectObj(eventHandleObj, "timer", New_pika_lvgl_lv_timer_t);
    obj_setPtr(obj_getPtr(eventHandleObj, "timer"), "lv_timer", timer);
    obj_run(eventHandleObj, "eventCallBack(timer)");
}

void pika_lvgl_lv_timer_t_set_period(PikaObj* self, int period) {
    lv_timer_t* lv_timer = obj_getPtr(self, "lv_timer");
    lv_timer_set_period(lv_timer, period);
}

void pika_lvgl_lv_timer_t_set_cb(PikaObj* self, Arg* cb) {
    lv_timer_t* lv_timer = obj_getPtr(self, "lv_timer");
    lv_timer_set_cb(lv_timer, __pika_timer_cb);

    obj_setArg(self, "eventCallBack", cb);
    /* init event_listener for the first time */
    if (NULL == g_pika_lv_timer_event_listener) {
        pks_eventListener_init(&g_pika_lv_timer_event_listener);
    }
    pks_eventListener_registerEvent(g_pika_lv_timer_event_listener,
                                 (uint32_t)lv_timer, self);

}

void pika_lvgl_lv_timer_t__delete(PikaObj* self) {
    lv_timer_t* lv_timer = obj_getPtr(self, "lv_timer");
    lv_timer_delete(lv_timer);
}
#endif
