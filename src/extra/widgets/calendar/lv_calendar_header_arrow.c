/**
 * @file lv_calendar_header_arrow.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_calendar_header_arrow.h"
#if LV_USE_CALENDAR_HEADER_ARROW

#include "lv_calendar.h"
#include "../../../widgets/lv_btn.h"
#include "../../../widgets/lv_label.h"
#include "../../layouts/flex/lv_flex.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void my_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void month_event_cb(lv_event_t * e);
static void value_changed_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_calendar_header_arrow_class = {
    .base_class = &lv_obj_class,
    .constructor_cb = my_constructor,
    .width_def = LV_PCT(100),
    .height_def = LV_DPI_DEF / 3
};

static const char * month_names_def[12] = LV_CALENDAR_DEFAULT_MONTH_NAMES;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_calendar_header_arrow_create(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_calendar_header_arrow_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void my_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    LV_UNUSED(class_p);

    lv_obj_move_to_index(obj, 0);

    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t * mo_prev = lv_btn_create(obj);
    lv_obj_set_style_bg_img_src(mo_prev, LV_SYMBOL_LEFT, 0);
    lv_obj_set_height(mo_prev, lv_pct(100));
    lv_obj_update_layout(mo_prev);
    lv_coord_t btn_size = lv_obj_get_height(mo_prev);
    lv_obj_set_width(mo_prev, btn_size);

    lv_obj_add_event_cb(mo_prev, month_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_clear_flag(mo_prev, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_flex_grow(label, 1);

    lv_obj_t * mo_next = lv_btn_create(obj);
    lv_obj_set_style_bg_img_src(mo_next, LV_SYMBOL_RIGHT, 0);
    lv_obj_set_size(mo_next, btn_size, btn_size);

    lv_obj_add_event_cb(mo_next, month_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_clear_flag(mo_next, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_add_event_cb(obj, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    /*Refresh the drop downs*/
    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

static void month_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);

    lv_obj_t * header = lv_obj_get_parent(btn);
    lv_obj_t * calendar = lv_obj_get_parent(header);

    const lv_calendar_date_t * d;
    d = lv_calendar_get_showed_date(calendar);
    lv_calendar_date_t newd = *d;

    /*The last child is the right button*/
    if(lv_obj_get_child(header, 0) == btn) {
        if(newd.month == 1) {
            newd.month = 12;
            newd.year --;
        }
        else {
            newd.month --;
        }
    }
    else {
        if(newd.month == 12) {
            newd.month = 1;
            newd.year ++;
        }
        else {
            newd.month ++;
        }
    }

    lv_calendar_set_showed_date(calendar, newd.year, newd.month);

    lv_obj_t * label = lv_obj_get_child(header, 1);
    lv_label_set_text_fmt(label, "%d %s", newd.year, month_names_def[newd.month - 1]);
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * header = lv_event_get_target(e);
    lv_obj_t * calendar = lv_obj_get_parent(header);

    const lv_calendar_date_t * cur_date = lv_calendar_get_showed_date(calendar);
    lv_obj_t * label = lv_obj_get_child(header, 1);
    lv_label_set_text_fmt(label, "%d %s", cur_date->year, month_names_def[cur_date->month - 1]);
}

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/

