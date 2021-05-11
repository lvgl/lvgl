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
static void month_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * month_names_def[12] = LV_CALENDAR_DEFAULT_MONTH_NAMES;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_calendar_header_arrow_create(lv_obj_t * parent, lv_obj_t * calendar, lv_coord_t btn_size)
{
    lv_obj_t * header = lv_obj_create(parent);

    /*Use the same paddings as the calendar*/
    lv_obj_set_style_pad_left(header, lv_obj_get_style_pad_left(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_right(header, lv_obj_get_style_pad_right(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_top(header, lv_obj_get_style_pad_top(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_bottom(header, lv_obj_get_style_pad_bottom(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_column(header, lv_obj_get_style_pad_column(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_radius(header, lv_obj_get_style_radius(calendar, LV_PART_MAIN), 0);

    const lv_calendar_date_t * cur_date = lv_calendar_get_showed_date(calendar);

    lv_obj_update_layout(calendar);
    lv_coord_t w = lv_obj_get_width(calendar);
    lv_obj_set_size(header,  w, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t * mo_prev = lv_btn_create(header);
    lv_obj_set_style_bg_img_src(mo_prev, LV_SYMBOL_LEFT, 0);
    lv_obj_set_size(mo_prev, btn_size, btn_size);
    lv_obj_add_event_cb(mo_prev, month_event_cb, LV_EVENT_CLICKED, calendar);
    lv_obj_clear_flag(mo_prev, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_t * label = lv_label_create(header);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_flex_grow(label, 1);
    lv_label_set_text_fmt(label, "%d %s", cur_date->year, month_names_def[cur_date->month - 1]);

    lv_obj_t * mo_next = lv_btn_create(header);
    lv_obj_set_style_bg_img_src(mo_next, LV_SYMBOL_RIGHT, 0);
    lv_obj_set_size(mo_next, btn_size, btn_size);
    lv_obj_add_event_cb(mo_next, month_event_cb, LV_EVENT_CLICKED, calendar);
    lv_obj_clear_flag(mo_next, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_obj_align_to(header, calendar, LV_ALIGN_OUT_TOP_MID, 0, 0);

    return header;
}

/**********************
 *  STATIC FUNCTIONS
 **********************/

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

