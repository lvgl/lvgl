#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT

#include "pika_lvgl_lv_obj.h"
#include "BaseObj.h"
#include "dataStrs.h"
#include "pika_lvgl.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_lv_event.h"

extern PikaObj* pika_lv_event_listener_g;

void pika_lvgl_lv_obj_center(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_center(lv_obj);
}

void pika_lvgl_lv_obj_set_size(PikaObj* self, int size_x, int size_y) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_size(lv_obj, size_x, size_y);
}

void pika_lvgl_lv_obj_align(PikaObj* self, int align, int x_ofs, int y_ofs) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_align(lv_obj, align, x_ofs, y_ofs);
}

void pika_lvgl_lv_obj_set_height(PikaObj* self, int h) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_height(lv_obj, h);
}

void pika_lvgl_lv_obj_update_layout(PikaObj* self) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_update_layout(lv_obj);
}

void pika_lvgl_lv_obj_set_width(PikaObj* self, int w) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_width(lv_obj, w);
}

void pika_lvgl_lv_obj_add_state(PikaObj* self, int state) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_add_state(lv_obj, state);
}

PikaObj* eventListener_getHandler(PikaObj* self, uintptr_t event_id) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%d", event_id);
    PikaObj* event_item = obj_getObj(self, event_name);
    PikaObj* event_handler = obj_getPtr(event_item, "handler");
    strsDeinit(&buffs);
    return event_handler;
}

static void __pika_event_cb(lv_event_t* e) {
    lv_obj_t* target = lv_event_get_target(e);
    PikaObj* event_handler =
        eventListener_getHandler(pika_lv_event_listener_g, (uintptr_t)target);
    PikaObj* evt = obj_getObj(event_handler, "_event_evt");
    obj_setPtr(evt, "lv_event", e);
    obj_run(event_handler, "_event_cb(_event_evt)");
}

void eventListener_registerEvent(PikaObj* self,
                              uintptr_t event_id,
                              PikaObj* event_handler) {
    Args buffs = {0};
    char* event_name =
        strsFormat(&buffs, PIKA_SPRINTF_BUFF_SIZE, "%d", event_id);
    obj_newDirectObj(self, event_name, New_TinyObj);
    PikaObj* event_item = obj_getObj(self, event_name);
    obj_setRef(event_item, "handler", event_handler);
    strsDeinit(&buffs);
}

void pika_lvgl_lv_obj_add_event_cb(PikaObj* self,
                                   Arg* event_cb,
                                   int filter,
                                   void* user_data) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_add_event_cb(lv_obj, __pika_event_cb, filter, NULL);
    obj_setArg(self, "_event_cb", event_cb);
    obj_setPtr(self, "_event_user_data", user_data);
    obj_newDirectObj(self, "_event_evt", New_pika_lvgl_lv_event);
    eventListener_registerEvent(pika_lv_event_listener_g, (uintptr_t)lv_obj, self);
}

void pika_lvgl_lv_obj_add_style(PikaObj *self, PikaObj* style, int selector){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_style_t* lv_style = obj_getPtr(style, "lv_style");
    lv_obj_add_style(lv_obj, lv_style, selector);
}

int pika_lvgl_lv_obj_get_x(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_x(lv_obj);
}

int pika_lvgl_lv_obj_get_y(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_obj_get_y(lv_obj);
}

void pika_lvgl_lv_obj_set_pos(PikaObj *self, int x, int y){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_pos(lv_obj, x, y);
}
#endif
