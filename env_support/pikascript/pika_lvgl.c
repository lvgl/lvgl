#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT
#include "pika_lvgl.h"
#include "BaseObj.h"
#include "pika_lvgl_ALIGN.h"
#include "pika_lvgl_ANIM.h"
#include "pika_lvgl_EVENT.h"
#include "pika_lvgl_OPA.h"
#include "pika_lvgl_PALETTE.h"
#include "pika_lvgl_STATE.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_lv_color_t.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_indev_t.h"
#include "pika_lvgl_lv_timer_t.h"

PikaObj* pika_lv_event_listener_g;

void pika_lvgl_STATE___init__(PikaObj* self) {
    obj_setInt(self, "DEFAULT", LV_STATE_DEFAULT);
    obj_setInt(self, "CHECKED", LV_STATE_CHECKED);
    obj_setInt(self, "FOCUSED", LV_STATE_FOCUSED);
    obj_setInt(self, "FOCUS_KEY", LV_STATE_FOCUS_KEY);
    obj_setInt(self, "EDITED", LV_STATE_EDITED);
    obj_setInt(self, "HOVERED", LV_STATE_HOVERED);
    obj_setInt(self, "PRESSED", LV_STATE_PRESSED);
    obj_setInt(self, "SCROLLED", LV_STATE_SCROLLED);
    obj_setInt(self, "DISABLED", LV_STATE_DISABLED);
    obj_setInt(self, "USER_1", LV_STATE_USER_1);
    obj_setInt(self, "USER_2", LV_STATE_USER_2);
    obj_setInt(self, "USER_3", LV_STATE_USER_3);
    obj_setInt(self, "USER_4", LV_STATE_USER_4);
    obj_setInt(self, "ANY", LV_STATE_ANY);
}

void pika_lvgl_ANIM___init__(PikaObj* self) {
    obj_setInt(self, "ON", LV_ANIM_OFF);
    obj_setInt(self, "OFF", LV_ANIM_ON);
}

void pika_lvgl_ALIGN___init__(PikaObj* self) {
    obj_setInt(self, "CENTER", LV_ALIGN_CENTER);
    obj_setInt(self, "DEFAULT", LV_ALIGN_DEFAULT);
    obj_setInt(self, "TOP_LEFT", LV_ALIGN_TOP_LEFT);
    obj_setInt(self, "TOP_MID", LV_ALIGN_TOP_MID);
    obj_setInt(self, "TOP_RIGHT", LV_ALIGN_TOP_RIGHT);
    obj_setInt(self, "BOTTOM_LEFT", LV_ALIGN_BOTTOM_LEFT);
    obj_setInt(self, "BOTTOM_MID", LV_ALIGN_BOTTOM_MID);
    obj_setInt(self, "BOTTOM_RIGHT", LV_ALIGN_BOTTOM_RIGHT);
    obj_setInt(self, "LEFT_MID", LV_ALIGN_LEFT_MID);
    obj_setInt(self, "RIGHT_MID", LV_ALIGN_RIGHT_MID);
    obj_setInt(self, "OUT_TOP_LEFT", LV_ALIGN_OUT_TOP_LEFT);
    obj_setInt(self, "OUT_TOP_MID", LV_ALIGN_OUT_TOP_MID);
    obj_setInt(self, "OUT_TOP_RIGHT", LV_ALIGN_OUT_TOP_RIGHT);
    obj_setInt(self, "OUT_BOTTOM_LEFT", LV_ALIGN_OUT_BOTTOM_LEFT);
    obj_setInt(self, "OUT_BOTTOM_MID", LV_ALIGN_OUT_BOTTOM_MID);
    obj_setInt(self, "OUT_BOTTOM_RIGHT", LV_ALIGN_OUT_BOTTOM_RIGHT);
    obj_setInt(self, "OUT_LEFT_TOP", LV_ALIGN_OUT_LEFT_TOP);
    obj_setInt(self, "OUT_LEFT_MID", LV_ALIGN_OUT_LEFT_MID);
    obj_setInt(self, "OUT_LEFT_BOTTOM", LV_ALIGN_OUT_LEFT_BOTTOM);
    obj_setInt(self, "OUT_RIGHT_TOP", LV_ALIGN_OUT_RIGHT_TOP);
    obj_setInt(self, "OUT_RIGHT_MID", LV_ALIGN_OUT_RIGHT_MID);
    obj_setInt(self, "OUT_RIGHT_BOTTOM", LV_ALIGN_OUT_RIGHT_BOTTOM);
}

void pika_lvgl_EVENT___init__(PikaObj* self) {
    obj_setInt(self, "ALL", LV_EVENT_ALL);
    obj_setInt(self, "PRESSED", LV_EVENT_PRESSED);
    obj_setInt(self, "PRESSING", LV_EVENT_PRESSING);
    obj_setInt(self, "PRESS_LOST", LV_EVENT_PRESS_LOST);
    obj_setInt(self, "SHORT_CLICKED", LV_EVENT_SHORT_CLICKED);
    obj_setInt(self, "LONG_PRESSED", LV_EVENT_LONG_PRESSED);
    obj_setInt(self, "LONG_PRESSED_REPEAT", LV_EVENT_LONG_PRESSED_REPEAT);
    obj_setInt(self, "CLICKED", LV_EVENT_CLICKED);
    obj_setInt(self, "RELEASED", LV_EVENT_RELEASED);
    obj_setInt(self, "SCROLL_BEGIN", LV_EVENT_SCROLL_BEGIN);
    obj_setInt(self, "SCROLL_END", LV_EVENT_SCROLL_END);
    obj_setInt(self, "SCROLL", LV_EVENT_SCROLL);
    obj_setInt(self, "GESTURE", LV_EVENT_GESTURE);
    obj_setInt(self, "KEY", LV_EVENT_KEY);
    obj_setInt(self, "FOCUSED", LV_EVENT_FOCUSED);
    obj_setInt(self, "DEFOCUSED", LV_EVENT_DEFOCUSED);
    obj_setInt(self, "LEAVE", LV_EVENT_LEAVE);
    obj_setInt(self, "HIT_TEST", LV_EVENT_HIT_TEST);
    obj_setInt(self, "COVER_CHECK", LV_EVENT_COVER_CHECK);
    obj_setInt(self, "REFR_EXT_DRAW_SIZE", LV_EVENT_REFR_EXT_DRAW_SIZE);
    obj_setInt(self, "DRAW_MAIN_BEGIN", LV_EVENT_DRAW_MAIN_BEGIN);
    obj_setInt(self, "DRAW_MAIN", LV_EVENT_DRAW_MAIN);
    obj_setInt(self, "DRAW_MAIN_END", LV_EVENT_DRAW_MAIN_END);
    obj_setInt(self, "DRAW_POST_BEGIN", LV_EVENT_DRAW_POST_BEGIN);
    obj_setInt(self, "DRAW_POST", LV_EVENT_DRAW_POST);
    obj_setInt(self, "DRAW_POST_END", LV_EVENT_DRAW_POST_END);
    obj_setInt(self, "DRAW_PART_BEGIN", LV_EVENT_DRAW_PART_BEGIN);
    obj_setInt(self, "DRAW_PART_END", LV_EVENT_DRAW_PART_END);
    obj_setInt(self, "VALUE_CHANGED", LV_EVENT_VALUE_CHANGED);
    obj_setInt(self, "INSERT", LV_EVENT_INSERT);
    obj_setInt(self, "REFRESH", LV_EVENT_REFRESH);
    obj_setInt(self, "READY", LV_EVENT_READY);
    obj_setInt(self, "CANCEL", LV_EVENT_CANCEL);
    obj_setInt(self, "DELETE", LV_EVENT_DELETE);
    obj_setInt(self, "CHILD_CHANGED", LV_EVENT_CHILD_CHANGED);
    obj_setInt(self, "CHILD_CREATED", LV_EVENT_CHILD_CREATED);
    obj_setInt(self, "CHILD_DELETED", LV_EVENT_CHILD_DELETED);
    obj_setInt(self, "SCREEN_UNLOAD_START", LV_EVENT_SCREEN_UNLOAD_START);
    obj_setInt(self, "SCREEN_LOAD_START", LV_EVENT_SCREEN_LOAD_START);
    obj_setInt(self, "SCREEN_LOADED", LV_EVENT_SCREEN_LOADED);
    obj_setInt(self, "SCREEN_UNLOADED", LV_EVENT_SCREEN_UNLOADED);
    obj_setInt(self, "SIZE_CHANGED", LV_EVENT_SIZE_CHANGED);
    obj_setInt(self, "STYLE_CHANGED", LV_EVENT_STYLE_CHANGED);
    obj_setInt(self, "LAYOUT_CHANGED", LV_EVENT_LAYOUT_CHANGED);
    obj_setInt(self, "GET_SELF_SIZE", LV_EVENT_GET_SELF_SIZE);
    obj_setInt(self, "PREPROCESS", LV_EVENT_PREPROCESS);
}

void pika_lvgl_OPA___init__(PikaObj* self) {
    obj_setInt(self, "TRANSP", LV_OPA_TRANSP);
    obj_setInt(self, "COVER", LV_OPA_COVER);
}

void pika_lvgl_PALETTE___init__(PikaObj* self) {
    obj_setInt(self, "RED", LV_PALETTE_RED);
    obj_setInt(self, "PINK", LV_PALETTE_PINK);
    obj_setInt(self, "PURPLE", LV_PALETTE_PURPLE);
    obj_setInt(self, "DEEP_PURPLE", LV_PALETTE_DEEP_PURPLE);
    obj_setInt(self, "INDIGO", LV_PALETTE_INDIGO);
    obj_setInt(self, "BLUE", LV_PALETTE_BLUE);
    obj_setInt(self, "LIGHT_BLUE", LV_PALETTE_LIGHT_BLUE);
    obj_setInt(self, "CYAN", LV_PALETTE_CYAN);
    obj_setInt(self, "TEAL", LV_PALETTE_TEAL);
    obj_setInt(self, "GREEN", LV_PALETTE_GREEN);
    obj_setInt(self, "LIGHT_GREEN", LV_PALETTE_LIGHT_GREEN);
    obj_setInt(self, "LIME", LV_PALETTE_LIME);
    obj_setInt(self, "YELLOW", LV_PALETTE_YELLOW);
    obj_setInt(self, "AMBER", LV_PALETTE_AMBER);
    obj_setInt(self, "ORANGE", LV_PALETTE_ORANGE);
    obj_setInt(self, "DEEP_ORANGE", LV_PALETTE_DEEP_ORANGE);
    obj_setInt(self, "BROWN", LV_PALETTE_BROWN);
    obj_setInt(self, "BLUE_GREY", LV_PALETTE_BLUE_GREY);
    obj_setInt(self, "GREY", LV_PALETTE_GREY);
    obj_setInt(self, "NONE", LV_PALETTE_NONE);
}

PikaObj* pika_lvgl_screen_active(PikaObj* self) {
    PikaObj* new_obj = newNormalObj(New_TinyObj);
    lv_obj_t* lv_obj = lv_screen_active();
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

void pika_lvgl___init__(PikaObj* self) {
    obj_newDirectObj(self, "lv_event_listener", New_TinyObj);
    pika_lv_event_listener_g = obj_getObj(self, "lv_event_listener");
}

PikaObj* pika_lvgl_obj(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_obj_create(lv_parent);
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_obj);
    obj_setPtr(new_obj, "lv_obj", lv_obj);
    return new_obj;
}

PikaObj* pika_lvgl_palette_lighten(PikaObj *self, int p, int lvl){
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_color_t);
    lv_color_t lv_color = lv_palette_lighten(p, lvl);
    args_setStruct(new_obj->list, "lv_color_struct", lv_color);
    lv_color_t* plv_color = args_getStruct(new_obj->list, "lv_color_struct");
    obj_setPtr(new_obj, "lv_color", plv_color);
    return new_obj;
}

PikaObj* pika_lvgl_palette_main(PikaObj* self, int p) {
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_color_t);
    lv_color_t lv_color = lv_palette_main(p);
    args_setStruct(new_obj->list, "lv_color_struct", lv_color);
    obj_setPtr(new_obj, "lv_color",
               args_getStruct(new_obj->list, "lv_color_struct"));
    return new_obj;
}

PikaObj* pika_lvgl_indev_get_active(PikaObj *self){
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_indev_t);
    lv_indev_t *lv_indev = lv_indev_active();
    obj_setPtr(new_obj,"lv_indev", lv_indev);
    return new_obj;
}

PikaObj* pika_lvgl_timer_create_basic(PikaObj *self){
    PikaObj* new_obj = newNormalObj(New_pika_lvgl_lv_timer_t);
    lv_timer_t *lv_timer = lv_timer_create_basic();
    obj_setPtr(new_obj,"lv_timer", lv_timer);
    return new_obj;
}
#endif
