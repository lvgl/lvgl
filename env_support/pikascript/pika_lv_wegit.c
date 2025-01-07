#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifdef PIKASCRIPT
#include "BaseObj.h"
#include "pika_lvgl.h"
#include "pika_lvgl_arc.h"
#include "pika_lvgl_bar.h"
#include "pika_lvgl_btn.h"
#include "pika_lvgl_checkbox.h"
#include "pika_lvgl_dropdown.h"
#include "pika_lvgl_label.h"
#include "pika_lvgl_lv_obj.h"
#include "pika_lvgl_roller.h"
#include "pika_lvgl_slider.h"
#include "pika_lvgl_switch.h"
#include "pika_lvgl_table.h"
#include "pika_lvgl_textarea.h"

void pika_lvgl_arc___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_arc_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
    obj_setInt(self, "MODE_NORMAL", LV_ARC_MODE_NORMAL);
    obj_setInt(self, "MODE_SYMMETRICAL", LV_ARC_MODE_SYMMETRICAL);
    obj_setInt(self, "MODE_REVERSE", LV_ARC_MODE_REVERSE);
}

void pika_lvgl_arc_set_end_angle(PikaObj* self, int angle) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_end_angle(lv_obj, angle);
}

void pika_lvgl_arc_set_bg_angles(PikaObj* self, int start, int end) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_bg_angles(lv_obj, start, end);
}

void pika_lvgl_arc_set_angles(PikaObj* self, int start, int end) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_angles(lv_obj, start, end);
}

int pika_lvgl_arc_get_angle_end(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_angle_end(lv_obj);
}
int pika_lvgl_arc_get_angle_start(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_angle_start(lv_obj);
}
int pika_lvgl_arc_get_bg_angle_end(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_bg_angle_end(lv_obj);
}
int pika_lvgl_arc_get_bg_angle_start(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_bg_angle_start(lv_obj);
}
int pika_lvgl_arc_get_max_value(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_max_value(lv_obj);
}
int pika_lvgl_arc_get_min_value(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_min_value(lv_obj);
}
int pika_lvgl_arc_get_mode(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_mode(lv_obj);
}
// int pika_lvgl_arc_get_rotation(PikaObj *self){
//     lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
//     return lv_arc_get_rotation(lv_obj);
// }
int pika_lvgl_arc_get_value(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_arc_get_value(lv_obj);
}
void pika_lvgl_arc_set_mode(PikaObj *self, int mode){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_mode(lv_obj, mode);
}
void pika_lvgl_arc_set_range(PikaObj *self, int min, int max){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_range(lv_obj, min, max);
}
void pika_lvgl_arc_set_rotation(PikaObj *self, int rotation){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_rotation(lv_obj, rotation);
}
void pika_lvgl_arc_set_start_angle(PikaObj *self, int start){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_start_angle(lv_obj, start);
}
void pika_lvgl_arc_set_value(PikaObj *self, int value){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_value(lv_obj, value);
}
void pika_lvgl_arc_set_bg_end_angle(PikaObj *self, int angle){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_bg_end_angle(lv_obj, angle);
}
void pika_lvgl_arc_set_bg_start_angle(PikaObj *self, int start){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_bg_start_angle(lv_obj, start);
}

void pika_lvgl_arc_set_change_rate(PikaObj *self, int rate){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_arc_set_change_rate(lv_obj, rate);
}

void pika_lvgl_bar___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_bar_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_bar_set_value(PikaObj* self, int value, int anim) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_bar_set_value(lv_obj, value, value);
}

int pika_lvgl_bar_get_max_value(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_bar_get_max_value(lv_obj);
}
int pika_lvgl_bar_get_min_value(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_bar_get_min_value(lv_obj);
}
int pika_lvgl_bar_get_mode(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_bar_get_mode(lv_obj);
}
int pika_lvgl_bar_get_start_value(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_bar_get_start_value(lv_obj);
}
int pika_lvgl_bar_get_value(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_bar_get_value(lv_obj);
}
void pika_lvgl_bar_set_mode(PikaObj *self, int mode){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_bar_set_mode(lv_obj, mode);
}
void pika_lvgl_bar_set_range(PikaObj *self, int min, int max){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_bar_set_range(lv_obj, min, max);
}
void pika_lvgl_bar_set_start_value(PikaObj *self, int start_value, int anim){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_bar_set_start_value(lv_obj, start_value, anim);
}

void pika_lvgl_btn___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_button_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_checkbox___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_checkbox_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_checkbox_set_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_checkbox_set_text(lv_obj, txt);
}

void pika_lvgl_checkbox_set_text_static(PikaObj *self, char* txt){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_checkbox_set_text_static(lv_obj, txt);
}

char* pika_lvgl_checkbox_get_text(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return (char*) lv_checkbox_get_text(lv_obj);
}

void pika_lvgl_dropdown___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_dropdown_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_dropdown_set_options(PikaObj* self, char* options) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_set_options(lv_obj, options);
}

void pika_lvgl_dropdown_add_option(PikaObj *self, char* options, int pos){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_add_option(lv_obj, options, pos);
}
void pika_lvgl_dropdown_clear_options(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_clear_options(lv_obj);
}
void pika_lvgl_dropdown_close(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_close(lv_obj);
}
int pika_lvgl_dropdown_get_dir(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_dropdown_get_dir(lv_obj);
}
// PikaObj* pika_lvgl_dropdown_get_list(PikaObj *self){
//     lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
//     return obj_getObj(lv_dropdown_get_list(lv_obj));
// }
int pika_lvgl_dropdown_get_option_count(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_dropdown_get_option_count(lv_obj);
}
int pika_lvgl_dropdown_get_option_index(PikaObj *self, char* txt){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_dropdown_get_option_index(lv_obj, txt);
}
char* pika_lvgl_dropdown_get_options(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return (char*) lv_dropdown_get_options(lv_obj);
}
int pika_lvgl_dropdown_get_selected(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_dropdown_get_selected(lv_obj);
}
int pika_lvgl_dropdown_get_selected_highlight(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_dropdown_get_selected_highlight(lv_obj);
}

char* pika_lvgl_dropdown_get_selected_str(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    obj_setBytes(self, "_buff",NULL, 128);
    char* _buff = (char*)obj_getBytes(self, "_buff");
    lv_dropdown_get_selected_str(lv_obj, _buff, 128);
    return _buff;
}
char* pika_lvgl_dropdown_get_symbol(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return (char*)lv_dropdown_get_symbol(lv_obj);
}
char* pika_lvgl_dropdown_get_text(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return (char*)lv_dropdown_get_text(lv_obj);
}
int pika_lvgl_dropdown_is_open(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    return lv_dropdown_is_open(lv_obj);
}
void pika_lvgl_dropdown_open(PikaObj *self){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_open(lv_obj);
}
void pika_lvgl_dropdown_set_dir(PikaObj *self, int dir){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_set_dir(lv_obj, dir);
}
void pika_lvgl_dropdown_set_selected(PikaObj *self, int sel_opt, int anim){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_set_selected(lv_obj, sel_opt, anim);
}
void pika_lvgl_dropdown_set_selected_highlight(PikaObj *self, int en){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_set_selected_highlight(lv_obj, en);
}
void pika_lvgl_dropdown_set_symbol(PikaObj *self, char* symbol){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_set_symbol(lv_obj, symbol);
}
void pika_lvgl_dropdown_set_text(PikaObj *self, char* txt){
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_dropdown_set_text(lv_obj, txt);
}

void pika_lvgl_label___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_label_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_label_set_long_mode(PikaObj* self, int mode) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_label_set_long_mode(lv_obj, mode);
}

void pika_lvgl_label_set_recolor(PikaObj* self, int en) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_label_set_recolor(lv_obj, en);
}

void pika_lvgl_label_set_text(PikaObj* self, char* txt) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_label_set_text(lv_obj, txt);
}

void pika_lvgl_label_set_style_text_align(PikaObj* self,
                                          int value,
                                          int selector) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_obj_set_style_text_align(lv_obj, value, selector);
}

void pika_lvgl_roller___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_roller_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_roller_set_options(PikaObj* self, char* options, int mode) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_roller_set_options(lv_obj, options, mode);
}

void pika_lvgl_roller_set_visible_row_count(PikaObj* self, int row_cnt) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_roller_set_visible_row_count(lv_obj, row_cnt);
}

void pika_lvgl_slider___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_slider_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_switch___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_switch_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_table___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_table_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_table_set_cell_value(PikaObj* self,
                                    int row,
                                    int col,
                                    char* txt) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_table_set_cell_value(lv_obj, row, col, txt);
}

void pika_lvgl_textarea___init__(PikaObj* self, PikaObj* parent) {
    lv_obj_t* lv_parent = obj_getPtr(parent, "lv_obj");
    lv_obj_t* lv_obj = lv_textarea_create(lv_parent);
    obj_setPtr(self, "lv_obj", lv_obj);
}

void pika_lvgl_textarea_set_one_line(PikaObj* self, int en) {
    lv_obj_t* lv_obj = obj_getPtr(self, "lv_obj");
    lv_textarea_set_one_line(lv_obj, en);
}
#endif
