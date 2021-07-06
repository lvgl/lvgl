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

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_calendar_header_arrow_class = {
     .base_class = &lv_obj_class,
     .constructor_cb = my_constructor
};

static const char * month_names_def[12] = LV_CALENDAR_DEFAULT_MONTH_NAMES;
static lv_obj_t * calendar_param;
static lv_coord_t btn_size_param;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_calendar_header_arrow_create(lv_obj_t * parent, lv_obj_t * calendar, lv_coord_t btn_size)
{
    calendar_param = calendar;
    btn_size_param = btn_size;
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

    /*Use the same paddings as the calendar_param*/
    lv_obj_set_style_pad_left(obj, lv_obj_get_style_pad_left(calendar_param, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_right(obj, lv_obj_get_style_pad_right(calendar_param, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_top(obj, lv_obj_get_style_pad_top(calendar_param, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_bottom(obj, lv_obj_get_style_pad_bottom(calendar_param, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_column(obj, lv_obj_get_style_pad_column(calendar_param, LV_PART_MAIN), 0);
    lv_obj_set_style_radius(obj, lv_obj_get_style_radius(calendar_param, LV_PART_MAIN), 0);

    const lv_calendar_date_t * cur_date = lv_calendar_get_showed_date(calendar_param);

    lv_obj_update_layout(calendar_param);
    lv_coord_t w = lv_obj_get_width(calendar_param);
    lv_obj_set_size(obj,  w, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t * mo_prev = lv_btn_create(obj);
    lv_obj_set_style_bg_img_src(mo_prev, LV_SYMBOL_LEFT, 0);
    lv_obj_set_size(mo_prev, btn_size_param, btn_size_param);
    lv_obj_add_event_cb(mo_prev, month_event_cb, LV_EVENT_CLICKED, calendar_param);
    lv_obj_clear_flag(mo_prev, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_flex_grow(label, 1);
    lv_label_set_text_fmt(label, "%d %s", cur_date->year, month_names_def[cur_date->month - 1]);

    lv_obj_t * mo_next = lv_btn_create(obj);
    lv_obj_set_style_bg_img_src(mo_next, LV_SYMBOL_RIGHT, 0);
    lv_obj_set_size(mo_next, btn_size_param, btn_size_param);
    lv_obj_add_event_cb(mo_next, month_event_cb, LV_EVENT_CLICKED, calendar_param);
    lv_obj_clear_flag(mo_next, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_align_to(obj, calendar_param, LV_ALIGN_OUT_TOP_MID, 0, 0);
}

static void month_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);

    lv_obj_t * header = lv_obj_get_parent(btn);
    lv_obj_t * calendar = lv_event_get_user_data(e);

    const lv_calendar_date_t * d;
    d = lv_calendar_get_showed_date(calendar);
    lv_calendar_date_t newd = *d;

    /*The last child is the right button*/
    if(lv_obj_get_child(header, 0) == btn) {
        if(newd.month == 1) {
            newd.month = 12;
            newd.year --;
        } else {
            newd.month --;
        }
    } else {
        if(newd.month == 12) {
            newd.month = 1;
            newd.year ++;
        } else {
            newd.month ++;
        }
    }

    lv_calendar_set_showed_date(calendar, newd.year, newd.month);

    lv_obj_t * label = lv_obj_get_child(header, 1);
    lv_label_set_text_fmt(label, "%d %s", newd.year, month_names_def[newd.month - 1]);
}

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/

