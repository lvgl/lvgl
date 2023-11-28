/**
 * @file lv_demo_flex_layout_ctrl_pad.h
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

#define SPINBOX_UPDATE_STYLE_VALUE(item)                                \
    do {                                                                \
        lv_obj_t* sbox = ui->ctrl_pad.tab.layout.spinbox_##item;        \
        int32_t val = lv_obj_get_style_##item(obj, LV_PART_MAIN);    \
        lv_spinbox_set_value(sbox, val);                                \
    } while(0)

#define SPINBOX_EVENT_ATTACH(item)  \
    lv_obj_add_event_cb(ui->ctrl_pad.tab.layout.spinbox_##item, ctrl_pad_spinbox_event_handler, LV_EVENT_VALUE_CHANGED, ui);

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void ctrl_pad_spinbox_event_handler(lv_event_t * e);
static void ctrl_pad_checkbox_event_handler(lv_event_t * e);
static void ctrl_pad_btn_event_attach(view_t * ui);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ctrl_pad_attach(view_t * ui)
{
    ctrl_pad_btn_event_attach(ui);
    flex_loader_attach(ui);

    SPINBOX_EVENT_ATTACH(width);
    SPINBOX_EVENT_ATTACH(height);
    SPINBOX_EVENT_ATTACH(pad_top);
    SPINBOX_EVENT_ATTACH(pad_bottom);
    SPINBOX_EVENT_ATTACH(pad_left);
    SPINBOX_EVENT_ATTACH(pad_right);
    SPINBOX_EVENT_ATTACH(pad_column);
    SPINBOX_EVENT_ATTACH(pad_row);
    SPINBOX_EVENT_ATTACH(flex_grow);

    lv_obj_add_event_cb(
        ui->ctrl_pad.tab.flex.checkbox_scrl,
        ctrl_pad_checkbox_event_handler,
        LV_EVENT_VALUE_CHANGED,
        ui
    );
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ctrl_pad_btn_remove_event_handler(lv_event_t * e)
{
    view_t * ui = lv_event_get_user_data(e);
    if(ui->obj_cur) {
        if(ui->obj_cur == ui->root) {
            lv_obj_clean(ui->root);
        }
        else {
            lv_obj_delete(ui->obj_cur);
            ui->obj_cur = NULL;
        }
    }
}

void ctrl_pad_obj_update(lv_obj_t * obj, view_t * ui)
{
    lv_obj_t * spinbox = ui->ctrl_pad.tab.layout.spinbox_width;
    int32_t value = lv_obj_get_width(obj);
    lv_spinbox_set_value(spinbox, value);

    spinbox = ui->ctrl_pad.tab.layout.spinbox_height;
    value = lv_obj_get_height(obj);
    lv_spinbox_set_value(spinbox, value);

    SPINBOX_UPDATE_STYLE_VALUE(pad_top);
    SPINBOX_UPDATE_STYLE_VALUE(pad_bottom);
    SPINBOX_UPDATE_STYLE_VALUE(pad_left);
    SPINBOX_UPDATE_STYLE_VALUE(pad_right);
    SPINBOX_UPDATE_STYLE_VALUE(pad_column);
    SPINBOX_UPDATE_STYLE_VALUE(pad_row);
    SPINBOX_UPDATE_STYLE_VALUE(flex_grow);

    lv_obj_t * checkbox = ui->ctrl_pad.tab.flex.checkbox_scrl;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE)) {
        lv_obj_add_state(checkbox, LV_STATE_CHECKED);
    }
    else {
        lv_obj_remove_state(checkbox, LV_STATE_CHECKED);
    }
}

static void ctrl_pad_spinbox_event_handler(lv_event_t * e)
{
    view_t * ui = lv_event_get_user_data(e);
    lv_obj_t * spinbox = lv_event_get_target(e);
    lv_style_prop_t prop = (lv_style_prop_t)(lv_uintptr_t)lv_obj_get_user_data(spinbox);

    lv_style_value_t v = {
        .num = (int32_t)lv_spinbox_get_value(spinbox)
    };

    if(ui->obj_cur) {
        lv_obj_set_local_style_prop(ui->obj_cur, prop, v, LV_PART_MAIN);
    }
}

static void ctrl_pad_btn_add_event_handler(lv_event_t * e)
{
    view_t * ui = lv_event_get_user_data(e);
    obj_child_node_create(ui->obj_cur, ui);
}

static void ctrl_pad_checkbox_event_handler(lv_event_t * e)
{
    view_t * ui = lv_event_get_user_data(e);
    if(ui->obj_cur) {
        bool checked = lv_obj_has_state(lv_event_get_target(e), LV_STATE_CHECKED);
        checked ? lv_obj_add_flag(ui->obj_cur, LV_OBJ_FLAG_SCROLLABLE) : lv_obj_remove_flag(ui->obj_cur,
                                                                                            LV_OBJ_FLAG_SCROLLABLE);
    }
}

static void ctrl_pad_btn_event_attach(view_t * ui)
{
    lv_obj_add_event_cb(
        ui->ctrl_pad.btn.add,
        ctrl_pad_btn_add_event_handler,
        LV_EVENT_CLICKED,
        ui
    );

    lv_obj_add_event_cb(
        ui->ctrl_pad.btn.remove,
        ctrl_pad_btn_remove_event_handler,
        LV_EVENT_CLICKED,
        ui
    );
}

#endif
