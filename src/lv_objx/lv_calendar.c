/**
 * @file lv_calendar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_calendar.h"
#if LV_USE_CALENDAR != 0

#include "../lv_core/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_hal/lv_hal_indev.h"
#include "../lv_misc/lv_utils.h"
#include "../lv_core/lv_indev.h"
#include "../lv_themes/lv_theme.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_calendar"

/**********************
 *      TYPEDEFS
 **********************/
enum {
    DAY_DRAW_PREV_MONTH,
    DAY_DRAW_ACT_MONTH,
    DAY_DRAW_NEXT_MONTH,
};
typedef uint8_t day_draw_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_calendar_design(lv_obj_t * calendar, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_calendar_signal(lv_obj_t * calendar, lv_signal_t sign, void * param);
static bool calculate_touched_day(lv_obj_t * calendar, const lv_point_t * touched_point);
static lv_coord_t get_header_height(lv_obj_t * calendar);
static lv_coord_t get_day_names_height(lv_obj_t * calendar);
static void draw_header(lv_obj_t * calendar, const lv_area_t * mask);
static void draw_day_names(lv_obj_t * calendar, const lv_area_t * mask);
static void draw_days(lv_obj_t * calendar, const lv_area_t * mask);
static uint8_t get_day_of_week(uint32_t year, uint32_t month, uint32_t day);
static bool is_highlighted(lv_obj_t * calendar, int32_t year, int32_t month, int32_t day);
static const char * get_day_name(lv_obj_t * calendar, uint8_t day);
static const char * get_month_name(lv_obj_t * calendar, int32_t month);
static uint8_t get_month_length(int32_t year, int32_t month);
static uint8_t is_leap_year(uint32_t year);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;
static const char * day_name[7]    = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
static const char * month_name[12] = {"January", "February", "March",     "April",   "May",      "June",
                                      "July",    "August",   "September", "October", "November", "December"};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a calendar object
 * @param par pointer to an object, it will be the parent of the new calendar
 * @param copy pointer to a calendar object, if not NULL then the new object will be copied from it
 * @return pointer to the created calendar
 */
lv_obj_t * lv_calendar_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("calendar create started");

    /*Create the ancestor of calendar*/
    lv_obj_t * new_calendar = lv_obj_create(par, copy);
    LV_ASSERT_MEM(new_calendar);
    if(new_calendar == NULL) return NULL;

    /*Allocate the calendar type specific extended data*/
    lv_calendar_ext_t * ext = lv_obj_allocate_ext_attr(new_calendar, sizeof(lv_calendar_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_calendar);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_calendar);

    /*Initialize the allocated 'ext' */
    ext->today.year  = 2018;
    ext->today.month = 1;
    ext->today.day   = 1;

    ext->showed_date.year  = 2018;
    ext->showed_date.month = 1;
    ext->showed_date.day   = 1;

    ext->pressed_date.year  = 0;
    ext->pressed_date.month = 0;
    ext->pressed_date.day   = 0;

    ext->highlighted_dates      = NULL;
    ext->highlighted_dates_num  = 0;
    ext->day_names              = NULL;
    ext->month_names            = NULL;
    ext->style_header           = &lv_style_plain_color;
    ext->style_header_pr        = &lv_style_pretty_color;
    ext->style_highlighted_days = &lv_style_plain_color;
    ext->style_inactive_days    = &lv_style_btn_ina;
    ext->style_week_box         = &lv_style_plain_color;
    ext->style_today_box        = &lv_style_pretty_color;
    ext->style_day_names        = &lv_style_pretty;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_calendar, lv_calendar_signal);
    lv_obj_set_design_cb(new_calendar, lv_calendar_design);

    /*Init the new calendar calendar*/
    if(copy == NULL) {
        lv_obj_set_size(new_calendar, LV_DPI * 2, LV_DPI * 2);
        lv_obj_set_style(new_calendar, &lv_style_pretty);

        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_BG, th->style.calendar.bg);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_HEADER, th->style.calendar.header);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_HEADER_PR, th->style.calendar.header_pr);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_DAY_NAMES, th->style.calendar.day_names);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_WEEK_BOX, th->style.calendar.week_box);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_TODAY_BOX, th->style.calendar.today_box);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS,
                                  th->style.calendar.highlighted_days);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_INACTIVE_DAYS, th->style.calendar.inactive_days);
        } else {
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_BG, &lv_style_pretty);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_HEADER, ext->style_header);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_HEADER_PR, ext->style_header_pr);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_DAY_NAMES, ext->style_day_names);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_WEEK_BOX, ext->style_week_box);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_TODAY_BOX, ext->style_today_box);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS, ext->style_highlighted_days);
            lv_calendar_set_style(new_calendar, LV_CALENDAR_STYLE_INACTIVE_DAYS, ext->style_inactive_days);
        }
    }
    /*Copy an existing calendar*/
    else {
        lv_calendar_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->today.year              = copy_ext->today.year;
        ext->today.month             = copy_ext->today.month;
        ext->today.day               = copy_ext->today.day;

        ext->showed_date.year  = copy_ext->showed_date.year;
        ext->showed_date.month = copy_ext->showed_date.month;
        ext->showed_date.day   = copy_ext->showed_date.day;

        ext->highlighted_dates     = copy_ext->highlighted_dates;
        ext->highlighted_dates_num = copy_ext->highlighted_dates_num;
        ext->day_names             = copy_ext->day_names;

        ext->month_names            = copy_ext->month_names;
        ext->style_header           = copy_ext->style_header;
        ext->style_header_pr        = copy_ext->style_header_pr;
        ext->style_highlighted_days = copy_ext->style_highlighted_days;
        ext->style_inactive_days    = copy_ext->style_inactive_days;
        ext->style_week_box         = copy_ext->style_week_box;
        ext->style_today_box        = copy_ext->style_today_box;
        ext->style_day_names        = copy_ext->style_day_names;
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_calendar);
    }

    LV_LOG_INFO("calendar created");

    return new_calendar;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the today's date
 * @param calendar pointer to a calendar object
 * @param today pointer to an `lv_calendar_date_t` variable containing the date of today. The value
 * will be saved it can be local variable too.
 */
void lv_calendar_set_today_date(lv_obj_t * calendar, lv_calendar_date_t * today)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);
    LV_ASSERT_NULL(today);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    ext->today.year         = today->year;
    ext->today.month        = today->month;
    ext->today.day          = today->day;

    lv_obj_invalidate(calendar);
}

/**
 * Set the currently showed
 * @param calendar pointer to a calendar object
 * @param showed pointer to an `lv_calendar_date_t` variable containing the date to show. The value
 * will be saved it can be local variable too.
 */
void lv_calendar_set_showed_date(lv_obj_t * calendar, lv_calendar_date_t * showed)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);
    LV_ASSERT_NULL(showed);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    ext->showed_date.year   = showed->year;
    ext->showed_date.month  = showed->month;
    ext->showed_date.day    = showed->day;

    lv_obj_invalidate(calendar);
}

/**
 * Set the the highlighted dates
 * @param calendar pointer to a calendar object
 * @param highlighted pointer to an `lv_calendar_date_t` array containing the dates. ONLY A POINTER
 * WILL BE SAVED! CAN'T BE LOCAL ARRAY.
 * @param date_num number of dates in the array
 */
void lv_calendar_set_highlighted_dates(lv_obj_t * calendar, lv_calendar_date_t highlighted[], uint16_t date_num)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);
    LV_ASSERT_NULL(highlighted);

    lv_calendar_ext_t * ext    = lv_obj_get_ext_attr(calendar);
    ext->highlighted_dates     = highlighted;
    ext->highlighted_dates_num = date_num;

    lv_obj_invalidate(calendar);
}

/**
 * Set the name of the days
 * @param calendar pointer to a calendar object
 * @param day_names pointer to an array with the names. E.g. `const char * days[7] = {"Sun", "Mon",
 * ...}` Only the pointer will be saved so this variable can't be local which will be destroyed
 * later.
 */
void lv_calendar_set_day_names(lv_obj_t * calendar, const char ** day_names)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);
    LV_ASSERT_NULL(day_names);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    ext->day_names          = day_names;
    lv_obj_invalidate(calendar);
}

/**
 * Set the name of the month
 * @param calendar pointer to a calendar object
 * @param month_names pointer to an array with the names. E.g. `const char * days[12] = {"Jan", "Feb",
 * ...}` Only the pointer will be saved so this variable can't be local which will be destroyed
 * later.
 */
void lv_calendar_set_month_names(lv_obj_t * calendar, const char ** month_names)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);
    LV_ASSERT_NULL(month_names);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    ext->month_names        = month_names;
    lv_obj_invalidate(calendar);
}

/**
 * Set a style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_calendar_set_style(lv_obj_t * calendar, lv_calendar_style_t type, const lv_style_t * style)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    switch(type) {
        case LV_CALENDAR_STYLE_BG: lv_obj_set_style(calendar, style); break;
        case LV_CALENDAR_STYLE_DAY_NAMES: ext->style_day_names = style; break;
        case LV_CALENDAR_STYLE_HEADER: ext->style_header = style; break;
        case LV_CALENDAR_STYLE_HEADER_PR: ext->style_header_pr = style; break;
        case LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS: ext->style_highlighted_days = style; break;
        case LV_CALENDAR_STYLE_INACTIVE_DAYS: ext->style_inactive_days = style; break;
        case LV_CALENDAR_STYLE_TODAY_BOX: ext->style_today_box = style; break;
        case LV_CALENDAR_STYLE_WEEK_BOX: ext->style_week_box = style; break;
    }

    lv_obj_invalidate(calendar);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the today's date
 * @param calendar pointer to a calendar object
 * @return return pointer to an `lv_calendar_date_t` variable containing the date of today.
 */
lv_calendar_date_t * lv_calendar_get_today_date(const lv_obj_t * calendar)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    return &ext->today;
}

/**
 * Get the currently showed
 * @param calendar pointer to a calendar object
 * @return pointer to an `lv_calendar_date_t` variable containing the date is being shown.
 */
lv_calendar_date_t * lv_calendar_get_showed_date(const lv_obj_t * calendar)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    return &ext->showed_date;
}

/**
 * Get the the pressed date.
 * @param calendar pointer to a calendar object
 * @return pointer to an `lv_calendar_date_t` variable containing the pressed date.
 * `NULL` if not date pressed (e.g. the header)
 */
lv_calendar_date_t * lv_calendar_get_pressed_date(const lv_obj_t * calendar)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    return ext->pressed_date.year != 0 ? &ext->pressed_date : NULL;
}

/**
 * Get the the highlighted dates
 * @param calendar pointer to a calendar object
 * @return pointer to an `lv_calendar_date_t` array containing the dates.
 */
lv_calendar_date_t * lv_calendar_get_highlighted_dates(const lv_obj_t * calendar)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    return ext->highlighted_dates;
}

/**
 * Get the number of the highlighted dates
 * @param calendar pointer to a calendar object
 * @return number of highlighted days
 */
uint16_t lv_calendar_get_highlighted_dates_num(const lv_obj_t * calendar)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    return ext->highlighted_dates_num;
}

/**
 * Get the name of the days
 * @param calendar pointer to a calendar object
 * @return pointer to the array of day names
 */
const char ** lv_calendar_get_day_names(const lv_obj_t * calendar)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    return ext->day_names;
}

/**
 * Get the name of the month
 * @param calendar pointer to a calendar object
 * @return pointer to the array of month names
 */
const char ** lv_calendar_get_month_names(const lv_obj_t * calendar)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    return ext->month_names;
}

/**
 * Get style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const lv_style_t * lv_calendar_get_style(const lv_obj_t * calendar, lv_calendar_style_t type)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    const lv_style_t * style = NULL;
    lv_calendar_ext_t * ext  = lv_obj_get_ext_attr(calendar);

    switch(type) {
        case LV_CALENDAR_STYLE_BG: style = lv_obj_get_style(calendar); break;
        case LV_CALENDAR_STYLE_HEADER: style = ext->style_header; break;
        case LV_CALENDAR_STYLE_HEADER_PR: style = ext->style_header_pr; break;
        case LV_CALENDAR_STYLE_DAY_NAMES: style = ext->style_day_names; break;
        case LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS: style = ext->style_highlighted_days; break;
        case LV_CALENDAR_STYLE_INACTIVE_DAYS: style = ext->style_inactive_days; break;
        case LV_CALENDAR_STYLE_WEEK_BOX: style = ext->style_week_box; break;
        case LV_CALENDAR_STYLE_TODAY_BOX: style = ext->style_today_box; break;
        default: style = NULL; break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the calendars
 * @param calendar pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_calendar_design(lv_obj_t * calendar, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(calendar, mask, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_opa_t opa_scale = lv_obj_get_opa_scale(calendar);
        lv_draw_rect(&calendar->coords, mask, lv_calendar_get_style(calendar, LV_CALENDAR_STYLE_BG), opa_scale);

        draw_header(calendar, mask);
        draw_day_names(calendar, mask);
        draw_days(calendar, mask);

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
    }

    return true;
}

/**
 * Signal function of the calendar
 * @param calendar pointer to a calendar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_calendar_signal(lv_obj_t * calendar, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(calendar, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_PRESSING) {
        lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
        lv_area_t header_area;
        lv_area_copy(&header_area, &calendar->coords);
        header_area.y2 = header_area.y1 + get_header_height(calendar);

        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t p;
        lv_indev_get_point(indev, &p);

        /*If the header is pressed mark an arrow as pressed*/
        if(lv_area_is_point_on(&header_area, &p)) {
            if(p.x < header_area.x1 + lv_area_get_width(&header_area) / 2) {
                if(ext->btn_pressing != -1) lv_obj_invalidate(calendar);
                ext->btn_pressing = -1;
            } else {
                if(ext->btn_pressing != 1) lv_obj_invalidate(calendar);
                ext->btn_pressing = 1;
            }

            ext->pressed_date.year  = 0;
            ext->pressed_date.month = 0;
            ext->pressed_date.day   = 0;
        }
        /*If a day is pressed save it*/
        else if(calculate_touched_day(calendar, &p)) {
            if(ext->btn_pressing != 0) lv_obj_invalidate(calendar);
            ext->btn_pressing = 0;
        }
        /*ELse set a deafault state*/
        else {
            if(ext->btn_pressing != 0) lv_obj_invalidate(calendar);
            ext->btn_pressing       = 0;
            ext->pressed_date.year  = 0;
            ext->pressed_date.month = 0;
            ext->pressed_date.day   = 0;
        }
    } else if(sign == LV_SIGNAL_PRESS_LOST) {
        lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
        ext->btn_pressing       = 0;
        lv_obj_invalidate(calendar);

    } else if(sign == LV_SIGNAL_RELEASED) {
        lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
        if(ext->btn_pressing < 0) {
            if(ext->showed_date.month <= 1) {
                ext->showed_date.month = 12;
                ext->showed_date.year--;
            } else {
                ext->showed_date.month--;
            }
        } else if(ext->btn_pressing > 0) {
            if(ext->showed_date.month >= 12) {
                ext->showed_date.month = 1;
                ext->showed_date.year++;
            } else {
                ext->showed_date.month++;
            }
        } else if(ext->pressed_date.year != 0) {
            res = lv_event_send(calendar, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }

        ext->btn_pressing = 0;
        lv_obj_invalidate(calendar);
    } else if(sign == LV_SIGNAL_CONTROL) {
        uint8_t c               = *((uint8_t *)param);
        lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            if(ext->showed_date.month >= 12) {
                ext->showed_date.month = 1;
                ext->showed_date.year++;
            } else {
                ext->showed_date.month++;
            }
            lv_obj_invalidate(calendar);
        } else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            if(ext->showed_date.month <= 1) {
                ext->showed_date.month = 12;
                ext->showed_date.year--;
            } else {
                ext->showed_date.month--;
            }
            lv_obj_invalidate(calendar);
        }
    }

    return res;
}

/**
 * It will check if the days part of calendar is touched
 * and if it is, it will calculate the day and put it in pressed_date of calendar object.
 * @param calendar pointer to a calendar object
 * @param pointer to a point
 * @return true: days part of calendar is touched and its related date is put in pressed date
 * false: the point is out of days part area.
 */
static bool calculate_touched_day(lv_obj_t * calendar, const lv_point_t * touched_point)
{
    lv_area_t days_area;
    lv_area_copy(&days_area, &calendar->coords);
    const lv_style_t * style_bg = lv_calendar_get_style(calendar, LV_CALENDAR_STYLE_BG);
    days_area.x1 += style_bg->body.padding.left;
    days_area.x2 -= style_bg->body.padding.right;
    days_area.y1 =
        calendar->coords.y1 + get_header_height(calendar) + get_day_names_height(calendar) - style_bg->body.padding.top;

    if(lv_area_is_point_on(&days_area, touched_point)) {
        lv_coord_t w  = (days_area.x2 - days_area.x1 + 1) / 7;
        lv_coord_t h  = (days_area.y2 - days_area.y1 + 1) / 6;
        uint8_t x_pos = 0;
        x_pos         = (touched_point->x - days_area.x1) / w;
        if(x_pos > 6) x_pos = 6;
        uint8_t y_pos = 0;
        y_pos         = (touched_point->y - days_area.y1) / h;
        if(y_pos > 5) y_pos = 5;

        uint8_t i_pos           = 0;
        i_pos                   = (y_pos * 7) + x_pos;
        lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
        if(i_pos < get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1)) {
            ext->pressed_date.year  = ext->showed_date.year - (ext->showed_date.month == 1 ? 1 : 0);
            ext->pressed_date.month = ext->showed_date.month == 1 ? 12 : (ext->showed_date.month - 1);
            ext->pressed_date.day   = get_month_length(ext->pressed_date.year, ext->pressed_date.month) -
                                    get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1) + 1 + i_pos;
        } else if(i_pos < (get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1) +
                           get_month_length(ext->showed_date.year, ext->showed_date.month))) {
            ext->pressed_date.year  = ext->showed_date.year;
            ext->pressed_date.month = ext->showed_date.month;
            ext->pressed_date.day   = i_pos + 1 - get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1);
        } else if(i_pos < 42) {
            ext->pressed_date.year  = ext->showed_date.year + (ext->showed_date.month == 12 ? 1 : 0);
            ext->pressed_date.month = ext->showed_date.month == 12 ? 1 : (ext->showed_date.month + 1);
            ext->pressed_date.day   = i_pos + 1 - get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1) -
                                    get_month_length(ext->showed_date.year, ext->showed_date.month);
        }
        return true;
    } else {
        return false;
    }
}

/**
 * Get the height of a calendar's header based on it's style
 * @param calendar point to a calendar
 * @return the header's height
 */
static lv_coord_t get_header_height(lv_obj_t * calendar)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    return lv_font_get_line_height(ext->style_header->text.font) + ext->style_header->body.padding.top +
           ext->style_header->body.padding.bottom;
}

/**
 * Get the height of a calendar's day_names based on it's style
 * @param calendar point to a calendar
 * @return the day_names's height
 */
static lv_coord_t get_day_names_height(lv_obj_t * calendar)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    return lv_font_get_line_height(ext->style_day_names->text.font) + ext->style_day_names->body.padding.top +
           ext->style_day_names->body.padding.bottom;
}

/**
 * Draw the calendar header with month name and arrows
 * @param calendar point to a calendar
 * @param mask a mask for drawing
 */
static void draw_header(lv_obj_t * calendar, const lv_area_t * mask)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    lv_bidi_dir_t bidi_dir = lv_obj_get_base_dir(calendar);

    lv_opa_t opa_scale      = lv_obj_get_opa_scale(calendar);

    lv_area_t header_area;
    header_area.x1 = calendar->coords.x1;
    header_area.x2 = calendar->coords.x2;
    header_area.y1 = calendar->coords.y1;
    header_area.y2 = calendar->coords.y1 + get_header_height(calendar);

    lv_draw_rect(&header_area, mask, ext->style_header, opa_scale);

    /*Add the year + month name*/
    char txt_buf[64];
    lv_utils_num_to_str(ext->showed_date.year, txt_buf);
    txt_buf[4] = ' ';
    txt_buf[5] = '\0';
    strcpy(&txt_buf[5], get_month_name(calendar, ext->showed_date.month));
    header_area.y1 += ext->style_header->body.padding.top;
    lv_draw_label(&header_area, mask, ext->style_header, opa_scale, txt_buf, LV_TXT_FLAG_CENTER, NULL, NULL, NULL, bidi_dir);

    /*Add the left arrow*/
    const lv_style_t * arrow_style = ext->btn_pressing < 0 ? ext->style_header_pr : ext->style_header;
    header_area.x1 += ext->style_header->body.padding.left;
    lv_draw_label(&header_area, mask, arrow_style, opa_scale, LV_SYMBOL_LEFT, LV_TXT_FLAG_NONE, NULL, NULL, NULL, bidi_dir);

    /*Add the right arrow*/
    arrow_style    = ext->btn_pressing > 0 ? ext->style_header_pr : ext->style_header;
    header_area.x1 = header_area.x2 - ext->style_header->body.padding.right -
                     lv_txt_get_width(LV_SYMBOL_RIGHT, (uint16_t)strlen(LV_SYMBOL_RIGHT), arrow_style->text.font,
                                      arrow_style->text.line_space, LV_TXT_FLAG_NONE);
    lv_draw_label(&header_area, mask, arrow_style, opa_scale, LV_SYMBOL_RIGHT, LV_TXT_FLAG_NONE, NULL, NULL, NULL, bidi_dir);
}

/**
 * Draw the day's name below the header
 * @param calendar point to a calendar
 * @param mask a mask for drawing
 */
static void draw_day_names(lv_obj_t * calendar, const lv_area_t * mask)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    lv_bidi_dir_t bidi_dir = lv_obj_get_base_dir(calendar);
    lv_opa_t opa_scale      = lv_obj_get_opa_scale(calendar);

    lv_coord_t l_pad = ext->style_day_names->body.padding.left;
    lv_coord_t w =
        lv_obj_get_width(calendar) - ext->style_day_names->body.padding.left - ext->style_day_names->body.padding.right;
    lv_coord_t box_w = w / 7;
    lv_area_t label_area;
    label_area.y1 = calendar->coords.y1 + get_header_height(calendar) + ext->style_day_names->body.padding.top;
    label_area.y2 = label_area.y1 + lv_font_get_line_height(ext->style_day_names->text.font);
    uint32_t i;
    for(i = 0; i < 7; i++) {
        label_area.x1 = calendar->coords.x1 + (w * i) / 7 + l_pad;
        label_area.x2 = label_area.x1 + box_w - 1;
        lv_draw_label(&label_area, mask, ext->style_day_names, opa_scale, get_day_name(calendar, i), LV_TXT_FLAG_CENTER,
                      NULL, NULL, NULL, bidi_dir);
    }
}

/**
 * Draw the date numbers in a matrix
 * @param calendar point to a calendar
 * @param mask a mask for drawing
 */
static void draw_days(lv_obj_t * calendar, const lv_area_t * mask)
{
    lv_calendar_ext_t * ext     = lv_obj_get_ext_attr(calendar);
    lv_bidi_dir_t bidi_dir = lv_obj_get_base_dir(calendar);
    const lv_style_t * style_bg = lv_calendar_get_style(calendar, LV_CALENDAR_STYLE_BG);
    lv_area_t label_area;
    lv_opa_t opa_scale = lv_obj_get_opa_scale(calendar);
    label_area.y1      = calendar->coords.y1 + get_header_height(calendar) + ext->style_day_names->body.padding.top +
                    lv_font_get_line_height(ext->style_day_names->text.font) +
                    ext->style_day_names->body.padding.bottom;
    label_area.y2 = label_area.y1 + lv_font_get_line_height(style_bg->text.font);

    lv_coord_t w          = lv_obj_get_width(calendar) - style_bg->body.padding.left - style_bg->body.padding.right;
    lv_coord_t h          = calendar->coords.y2 - label_area.y1 - style_bg->body.padding.bottom;
    lv_coord_t box_w      = w / 7;
    lv_coord_t vert_space = (h - (6 * lv_font_get_line_height(style_bg->text.font))) / 5;

    uint32_t week;
    uint8_t day_cnt;
    uint8_t month_start_day = get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1);
    day_draw_state_t draw_state; /*true: Not the prev. or next month is drawn*/
    const lv_style_t * act_style;

    /*If starting with the first day of the week then the previous month is not visible*/
    if(month_start_day == 0) {
        day_cnt    = 1;
        draw_state = DAY_DRAW_ACT_MONTH;
        act_style  = style_bg;
    } else {
        draw_state = DAY_DRAW_PREV_MONTH;
        day_cnt = get_month_length(ext->showed_date.year, ext->showed_date.month - 1); /*Length of the previous month*/
        day_cnt -= month_start_day - 1; /*First visible number of the previous month*/
        act_style = ext->style_inactive_days;
    }

    bool month_of_today_shown = false;
    if(ext->showed_date.year == ext->today.year && ext->showed_date.month == ext->today.month) {
        month_of_today_shown = true;
    }

    char buf[3];
    bool in_week_box = false;

    /*Draw 6 weeks*/
    for(week = 0; week < 6; week++) {

        /*Draw the "week box"*/
        if(month_of_today_shown &&
           ((draw_state == DAY_DRAW_ACT_MONTH && ext->today.day >= day_cnt && ext->today.day < day_cnt + 7) ||
            (draw_state == DAY_DRAW_PREV_MONTH && ext->today.day <= 7 - month_start_day && week == 0))) {
            lv_area_t week_box_area;
            lv_area_copy(&week_box_area, &label_area); /*'label_area' is already set for this row*/
            week_box_area.x1 =
                calendar->coords.x1 + style_bg->body.padding.left - ext->style_week_box->body.padding.left;
            week_box_area.x2 =
                calendar->coords.x2 - style_bg->body.padding.right + ext->style_week_box->body.padding.right;

            week_box_area.y1 -= ext->style_week_box->body.padding.top;
            week_box_area.y2 += ext->style_week_box->body.padding.bottom;
            lv_draw_rect(&week_box_area, mask, ext->style_week_box, opa_scale);

            in_week_box = true;
        } else {
            in_week_box = false;
        }

        /*Draw the 7 days of a week*/
        uint32_t day;
        for(day = 0; day < 7; day++) {
            /*The previous month is over*/
            if(draw_state == DAY_DRAW_PREV_MONTH && day == month_start_day) {
                draw_state = DAY_DRAW_ACT_MONTH;
                day_cnt    = 1;
                act_style  = style_bg;
            }
            /*The current month is over*/
            if(draw_state == DAY_DRAW_ACT_MONTH &&
               day_cnt > get_month_length(ext->showed_date.year, ext->showed_date.month)) {
                draw_state = DAY_DRAW_NEXT_MONTH;
                day_cnt    = 1;
                act_style  = ext->style_inactive_days;
            }

            label_area.x1 =
                calendar->coords.x1 + (w * day) / 7 + style_bg->body.padding.left;
            label_area.x2 = label_area.x1 + box_w - 1;

            /*Draw the "today box"*/
            if(draw_state == DAY_DRAW_ACT_MONTH && month_of_today_shown && ext->today.day == day_cnt) {
                lv_area_t today_box_area;
                lv_area_copy(&today_box_area, &label_area);
                today_box_area.x1 = label_area.x1;
                today_box_area.x2 = label_area.x2;

                today_box_area.y1 = label_area.y1 - ext->style_today_box->body.padding.top;
                today_box_area.y2 = label_area.y2 + ext->style_today_box->body.padding.bottom;
                lv_draw_rect(&today_box_area, mask, ext->style_today_box, opa_scale);
            }

            /*Get the final style : highlighted/week box/today box/normal*/
            const lv_style_t * final_style;
            if(draw_state == DAY_DRAW_PREV_MONTH &&
               is_highlighted(calendar, ext->showed_date.year - (ext->showed_date.month == 1 ? 1 : 0),
                              ext->showed_date.month == 1 ? 12 : ext->showed_date.month - 1, day_cnt)) {
                final_style = ext->style_highlighted_days;
            } else if(draw_state == DAY_DRAW_ACT_MONTH &&
                      is_highlighted(calendar, ext->showed_date.year, ext->showed_date.month, day_cnt)) {
                final_style = ext->style_highlighted_days;
            } else if(draw_state == DAY_DRAW_NEXT_MONTH &&
                      is_highlighted(calendar, ext->showed_date.year + (ext->showed_date.month == 12 ? 1 : 0),
                                     ext->showed_date.month == 12 ? 1 : ext->showed_date.month + 1, day_cnt)) {
                final_style = ext->style_highlighted_days;
            } else if(month_of_today_shown && day_cnt == ext->today.day && draw_state == DAY_DRAW_ACT_MONTH)
                final_style = ext->style_today_box;
            else if(in_week_box && draw_state == DAY_DRAW_ACT_MONTH)
                final_style = ext->style_week_box;
            else
                final_style = act_style;

            /*Write the day's number*/
            lv_utils_num_to_str(day_cnt, buf);
            lv_draw_label(&label_area, mask, final_style, opa_scale, buf, LV_TXT_FLAG_CENTER, NULL, NULL, NULL, bidi_dir);

            /*Go to the next day*/
            day_cnt++;
        }

        /*Got to the next weeks row*/
        label_area.y1 += vert_space + lv_font_get_line_height(style_bg->text.font);
        label_area.y2 += vert_space + lv_font_get_line_height(style_bg->text.font);
    }
}

/**
 * Check weather a date is highlighted or not
 * @param calendar pointer to a calendar object
 * @param year a year
 * @param month a  month [1..12]
 * @param day a day [1..31]
 * @return true: highlighted
 */
static bool is_highlighted(lv_obj_t * calendar, int32_t year, int32_t month, int32_t day)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    if(ext->highlighted_dates == NULL || ext->highlighted_dates_num == 0) return false;

    uint32_t i;
    for(i = 0; i < ext->highlighted_dates_num; i++) {
        if(ext->highlighted_dates[i].year == year && ext->highlighted_dates[i].month == month &&
           ext->highlighted_dates[i].day == day) {
            return true;
        }
    }

    return false;
}

/**
 * Get the day name
 * @param calendar pointer to a calendar object
 * @param day a day in [0..6]
 * @return
 */
static const char * get_day_name(lv_obj_t * calendar, uint8_t day)
{

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    if(ext->day_names)
        return ext->day_names[day];
    else
        return day_name[day];
}

/**
 * Get the month name
 * @param calendar pointer to a calendar object
 * @param month a month. The range is basically [1..12] but [-11..1] is also supported to handle
 * previous year
 * @return
 */
static const char * get_month_name(lv_obj_t * calendar, int32_t month)
{
    month--; /*Range of months id [1..12] but range of indexes is [0..11]*/
    if(month < 0) month = 12 + month;

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    if(ext->month_names)
        return ext->month_names[month];
    else
        return month_name[month];
}

/**
 * Get the number of days in a month
 * @param year a year
 * @param month a month. The range is basically [1..12] but [-11..1] is also supported to handle
 * previous year
 * @return [28..31]
 */
static uint8_t get_month_length(int32_t year, int32_t month)
{
    month--; /*Range of months id [1..12] but range of indexes is [0..11]*/
    if(month < 0) {
        year--;             /*Already in the previous year (won't be less then -12 to skip a whole year)*/
        month = 12 + month; /*`month` is negative, the result will be < 12*/
    }
    if(month >= 12) {
        year++;
        month -= 12;
    }

    /*month == 1 is february*/
    return (month == 1) ? (28 + is_leap_year(year)) : 31 - month % 7 % 2;
}

/**
 * Tells whether a year is leap year or not
 * @param year a year
 * @return 0: not leap year; 1: leap year
 */
static uint8_t is_leap_year(uint32_t year)
{
    return (year % 4) || ((year % 100 == 0) && (year % 400)) ? 0 : 1;
}

/**
 * Get the day of the week
 * @param year a year
 * @param month a  month
 * @param day a day
 * @return [0..6] which means [Sun..Sat]
 */
static uint8_t get_day_of_week(uint32_t year, uint32_t month, uint32_t day)
{
    uint32_t a = month < 3 ? 1 : 0;
    uint32_t b = year - a;

    uint32_t day_of_week = (day + (31 * (month - 2 + 12 * a) / 12) + b + (b / 4) - (b / 100) + (b / 400)) % 7;

    return day_of_week;
}

#endif
