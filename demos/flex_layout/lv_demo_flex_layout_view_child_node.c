/**
 * @file lv_demo_flex_layout_view_child_node.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_flex_layout_main.h"

#if LV_USE_DEMO_FLEX_LAYOUT

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void obj_child_node_event_handler(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * obj_child_node_create(lv_obj_t * par, view_t * ui)
{
    if(par == NULL) {
        return NULL;
    }

    uint32_t cnt = lv_obj_get_child_count(par);

    lv_obj_t * obj = lv_obj_create(par);
    lv_obj_add_style(obj, &ui->obj_def_style, 0);
    lv_obj_add_style(obj, &ui->obj_checked_style, LV_STATE_CHECKED);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text_fmt(label, "%" LV_PRIu32, cnt);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_FLOATING);
    lv_obj_center(label);

    lv_obj_add_event_cb(obj, obj_child_node_event_handler, LV_EVENT_CLICKED, ui);

    return obj;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void obj_child_node_event_handler(lv_event_t * e)
{
    view_t * ui = (view_t *)lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(ui->obj_cur) {
        lv_obj_remove_state(ui->obj_cur, LV_STATE_CHECKED);
    }

    lv_obj_add_state(obj, LV_STATE_CHECKED);

    ui->obj_cur = obj;
    flex_loader_obj_update(obj, ui);
    ctrl_pad_obj_update(obj, ui);
}

#endif
