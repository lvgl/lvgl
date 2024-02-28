/**
 * @file lv_demo_flex_layout_main.h
 *
 */

#ifndef LV_DEMO_FLEX_LAYOUT_MAIN_H
#define LV_DEMO_FLEX_LAYOUT_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

/*********************
 *      DEFINES
 *********************/

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t * root;
    lv_obj_t * obj_cur;
    lv_style_t obj_def_style;
    lv_style_t obj_checked_style;

    struct {
        lv_obj_t * cont;

        struct {
            lv_obj_t * view;

            struct {
                lv_obj_t * tab;
                lv_obj_t * ddlist_flow;
                lv_obj_t * checkbox_scrl;
            } flex;

            struct {
                lv_obj_t * tab;
                lv_obj_t * ddlist_align_main;
                lv_obj_t * ddlist_align_cross;
                lv_obj_t * ddlist_align_track;
            } align;

            struct {
                lv_obj_t * tab;
                lv_obj_t * group_width_and_height;
                lv_obj_t * spinbox_width;
                lv_obj_t * spinbox_height;
                lv_obj_t * group_width_and_height_min;
                lv_obj_t * spinbox_min_width;
                lv_obj_t * spinbox_min_height;
                lv_obj_t * group_width_and_height_max;
                lv_obj_t * spinbox_max_width;
                lv_obj_t * spinbox_max_height;
                lv_obj_t * spinbox_pad_top;
                lv_obj_t * spinbox_pad_bottom;
                lv_obj_t * spinbox_pad_left;
                lv_obj_t * spinbox_pad_right;
                lv_obj_t * spinbox_pad_column;
                lv_obj_t * spinbox_pad_row;
                lv_obj_t * spinbox_flex_grow;
            } layout;
        } tab;

        struct {
            lv_obj_t * cont;
            lv_obj_t * add;
            lv_obj_t * remove;
        } btn;

    } ctrl_pad;
} view_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void view_create(lv_obj_t * par, view_t * ui);
void view_ctrl_pad_create(lv_obj_t * par, view_t * ui);
lv_obj_t * obj_child_node_create(lv_obj_t * par, view_t * ui);

void ctrl_pad_attach(view_t * ui);
void ctrl_pad_obj_update(lv_obj_t * obj, view_t * ui);
void flex_loader_obj_update(lv_obj_t * obj, view_t * ui);
void flex_loader_attach(view_t * ui);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_FLEX_LAYOUT_MAIN_H*/
