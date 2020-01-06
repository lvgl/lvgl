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
static lv_design_res_t lv_calendar_design(lv_obj_t * calendar, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_calendar_signal(lv_obj_t * calendar, lv_signal_t sign, void * param);
static lv_style_dsc_t * lv_calendar_get_style(lv_obj_t * calendar, uint8_t part);
static bool calculate_touched_day(lv_obj_t * calendar, const lv_point_t * touched_point);
static lv_coord_t get_header_height(lv_obj_t * calendar);
static lv_coord_t get_day_names_height(lv_obj_t * calendar);
static void draw_header(lv_obj_t * calendar, const lv_area_t * mask);
static void draw_day_names(lv_obj_t * calendar, const lv_area_t * mask);
static void draw_days(lv_obj_t * calendar, const lv_area_t * mask);
static uint8_t get_day_of_week(uint32_t year, uint32_t month, uint32_t day);
static bool is_highlighted(lv_obj_t * calendar, day_draw_state_t draw_state, int32_t year, int32_t month, int32_t day);
static bool is_pressed(lv_obj_t * calendar, day_draw_state_t draw_state, int32_t year, int32_t month, int32_t day);
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
    if(ext == NULL) {
        lv_obj_del(new_calendar);
        return NULL;
    }

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


    lv_style_dsc_init(&ext->style_date_nums);
    lv_style_dsc_init(&ext->style_day_names);
    lv_style_dsc_init(&ext->style_header);
    lv_style_dsc_init(&ext->style_today_box);
    lv_style_dsc_init(&ext->style_week_box);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_calendar, lv_calendar_signal);
    lv_obj_set_design_cb(new_calendar, lv_calendar_design);

    /*Init the new calendar calendar*/
    if(copy == NULL) {

        /*Different styles will be used from the styles while rendering so disable caching*/
        ext->style_date_nums.cache.enabled = 0;
        ext->style_day_names.cache.enabled = 0;
        ext->style_header.cache.enabled = 0;
        ext->style_today_box.cache.enabled = 0;
        ext->style_week_box.cache.enabled = 0;

        lv_style_dsc_reset(&new_calendar->style_dsc);
        lv_style_dsc_add_class(&new_calendar->style_dsc, lv_theme_get_style(LV_THEME_CALENDAR_BG));
        lv_style_dsc_add_class(&ext->style_date_nums, lv_theme_get_style(LV_THEME_CALENDAR_DATE_NUMS));
        lv_style_dsc_add_class(&ext->style_day_names, lv_theme_get_style(LV_THEME_CALENDAR_DAY_NAMES));
        lv_style_dsc_add_class(&ext->style_header, lv_theme_get_style(LV_THEME_CALENDAR_HEADER));
        lv_style_dsc_add_class(&ext->style_today_box, lv_theme_get_style(LV_THEME_CALENDAR_TODAY_BOX));
        lv_style_dsc_add_class(&ext->style_week_box, lv_theme_get_style(LV_THEME_CALENDAR_WEEK_BOX));

        lv_obj_refresh_style(new_calendar, LV_OBJ_PART_ALL);

        lv_obj_set_size(new_calendar, LV_DPI * 2, LV_DPI * 2);

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
        ext->style_week_box         = copy_ext->style_week_box;
        ext->style_today_box        = copy_ext->style_today_box;
        ext->style_day_names        = copy_ext->style_day_names;
        /*Refresh the style with new signal function*/
//        lv_obj_refresh_style(new_calendar);
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
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_calendar_design(lv_obj_t * calendar, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(calendar, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(calendar, clip_area, mode);

        draw_header(calendar, clip_area);
        draw_day_names(calendar, clip_area);
        draw_days(calendar, clip_area);

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
    }

    return LV_DESIGN_RES_OK;
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
    if(sign == LV_SIGNAL_GET_STYLE) {
        uint8_t ** part_p = param;
        lv_style_dsc_t ** style_dsc_p = param;
        *style_dsc_p = lv_calendar_get_style(calendar, **part_p);
        return LV_RES_OK;
    }

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
        if(lv_area_is_point_on(&header_area, &p, 0)) {
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
            ext->btn_pressing = 0;
            lv_obj_invalidate(calendar);
        }
        /*ELse set a default state*/
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
        ext->pressed_date.year  = 0;
        ext->pressed_date.month = 0;
        ext->pressed_date.day   = 0;
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
        ext->pressed_date.year  = 0;
        ext->pressed_date.month = 0;
        ext->pressed_date.day   = 0;
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
 * Get the style descriptor of a part of the object
 * @param page pointer the object
 * @param part the part from `lv_calendar_part_t`. (LV_CALENDAR_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_dsc_t * lv_calendar_get_style(lv_obj_t * calendar, uint8_t part)
{
    LV_ASSERT_OBJ(calendar, LV_OBJX_NAME);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
    lv_style_dsc_t * style_dsc_p;


    switch(part) {
    case LV_CALENDAR_PART_BG:
        style_dsc_p = &calendar->style_dsc;
        break;
    case LV_CALENDAR_PART_HEADER:
        style_dsc_p = &ext->style_header;
        break;
    case LV_CALENDAR_PART_DAY_NAMES:
        style_dsc_p = &ext->style_day_names;
        break;
    case LV_CALENDAR_PART_DATE_NUMS:
        style_dsc_p = &ext->style_date_nums;
        break;
    case LV_CALENDAR_PART_WEEK_BOX:
        style_dsc_p = &ext->style_week_box;
        break;
    case LV_CALENDAR_PART_TODAY_BOX:
        style_dsc_p = &ext->style_today_box;
        break;
    default:
        style_dsc_p = NULL;
    }

    return style_dsc_p;
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
    lv_style_int_t left = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_BG, LV_STYLE_PAD_LEFT);
    lv_style_int_t right = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_BG, LV_STYLE_PAD_RIGHT);
    lv_style_int_t top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_BG, LV_STYLE_PAD_TOP);

    days_area.x1 += left;
    days_area.x2 -= right;
    days_area.y1 = calendar->coords.y1 + get_header_height(calendar) + get_day_names_height(calendar) - top;

    if(lv_area_is_point_on(&days_area, touched_point, 0)) {
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
    const lv_font_t * font = lv_obj_get_style_ptr(calendar, LV_CALENDAR_PART_HEADER, LV_STYLE_FONT);
    lv_style_int_t top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_HEADER, LV_STYLE_PAD_TOP);
    lv_style_int_t bottom = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_HEADER, LV_STYLE_PAD_BOTTOM);

    return lv_font_get_line_height(font) + top + bottom;
}

/**
 * Get the height of a calendar's day_names based on it's style
 * @param calendar point to a calendar
 * @return the day_names's height
 */
static lv_coord_t get_day_names_height(lv_obj_t * calendar)
{
    const lv_font_t * font = lv_obj_get_style_ptr(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_FONT);
    lv_style_int_t top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_PAD_TOP);
    lv_style_int_t bottom = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_PAD_BOTTOM);

    return lv_font_get_line_height(font) + top + bottom;
}

/**
 * Draw the calendar header with month name and arrows
 * @param calendar point to a calendar
 * @param mask a mask for drawing
 */
static void draw_header(lv_obj_t * calendar, const lv_area_t * mask)
{

    lv_style_int_t top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_HEADER, LV_STYLE_PAD_TOP);
    lv_style_int_t left = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_HEADER, LV_STYLE_PAD_LEFT);
    lv_style_int_t right = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_HEADER, LV_STYLE_PAD_RIGHT);
    const lv_font_t * font = lv_obj_get_style_ptr(calendar, LV_CALENDAR_PART_HEADER, LV_STYLE_FONT);

    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    lv_area_t header_area;
    header_area.x1 = calendar->coords.x1;
    header_area.x2 = calendar->coords.x2;
    header_area.y1 = calendar->coords.y1;
    header_area.y2 = calendar->coords.y1 + get_header_height(calendar);

    lv_draw_rect_dsc_t header_rect_dsc;
    lv_draw_rect_dsc_init(&header_rect_dsc);
    lv_obj_init_draw_rect_dsc(calendar, LV_CALENDAR_PART_HEADER, &header_rect_dsc);
    lv_draw_rect(&header_area, mask, &header_rect_dsc);

    /*Add the year + month name*/
    char txt_buf[64];
    lv_utils_num_to_str(ext->showed_date.year, txt_buf);
    txt_buf[4] = ' ';
    txt_buf[5] = '\0';
    strcpy(&txt_buf[5], get_month_name(calendar, ext->showed_date.month));
    header_area.y1 += top;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_HEADER, &label_dsc);
    label_dsc.flag = LV_TXT_FLAG_CENTER;
    lv_draw_label(&header_area, mask, &label_dsc,txt_buf, NULL);

    /*Add the left arrow*/

    /*The state changes without re-caching the styles, disable the use of cache*/
    calendar->style_dsc.cache.enabled = 0;
    lv_obj_state_t state_ori = calendar->state;

    if(ext->btn_pressing < 0) calendar->state |= LV_OBJ_STATE_PRESSED;
    else calendar->state &= ~(LV_OBJ_STATE_PRESSED);

    header_area.x1 += left;

    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_HEADER, &label_dsc);
    lv_draw_label(&header_area, mask, &label_dsc, LV_SYMBOL_LEFT, NULL);

    calendar->state = state_ori;    /*Restore the state*/

    /*Add the right arrow*/
    if(ext->btn_pressing > 0) calendar->state |= LV_OBJ_STATE_PRESSED;
    else calendar->state &= ~(LV_OBJ_STATE_PRESSED);

    header_area.x1 = header_area.x2 - right - lv_txt_get_width(LV_SYMBOL_RIGHT, (uint16_t)strlen(LV_SYMBOL_RIGHT), font, 0, LV_TXT_FLAG_NONE);

    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_HEADER, &label_dsc);
    lv_draw_label(&header_area, mask, &label_dsc, LV_SYMBOL_RIGHT, NULL);

    calendar->state = state_ori;    /*Restore the state*/
    calendar->style_dsc.cache.enabled = 1;
}

/**
 * Draw the day's name below the header
 * @param calendar point to a calendar
 * @param mask a mask for drawing
 */
static void draw_day_names(lv_obj_t * calendar, const lv_area_t * mask)
{
    lv_style_int_t left = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_PAD_LEFT);
    lv_style_int_t right = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_PAD_RIGHT);
    lv_style_int_t top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_PAD_TOP);
    const lv_font_t * font = lv_obj_get_style_ptr(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_FONT);
    lv_coord_t w = lv_obj_get_width(calendar) - left - right;
    lv_coord_t box_w = w / 7;
    lv_area_t label_area;
    label_area.y1 = calendar->coords.y1 + get_header_height(calendar) + top;
    label_area.y2 = label_area.y1 + lv_font_get_line_height(font);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_DAY_NAMES, &label_dsc);
    label_dsc.flag = LV_TXT_FLAG_CENTER;

    uint32_t i;
    for(i = 0; i < 7; i++) {
        label_area.x1 = calendar->coords.x1 + (w * i) / 7 + left;
        label_area.x2 = label_area.x1 + box_w - 1;
        lv_draw_label(&label_area, mask, &label_dsc, get_day_name(calendar, i), NULL);
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


    lv_style_int_t daynames_top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_PAD_TOP);
    lv_style_int_t daynames_bottom = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_PAD_BOTTOM);
    const lv_font_t * daynames_font = lv_obj_get_style_ptr(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STYLE_FONT);

    const lv_font_t * nums_font = lv_obj_get_style_ptr(calendar, LV_CALENDAR_PART_DATE_NUMS, LV_STYLE_FONT);
    lv_area_t label_area;
    label_area.y1      = calendar->coords.y1 + get_header_height(calendar) + daynames_top +
                    lv_font_get_line_height(daynames_font) + daynames_bottom;
    label_area.y2 = label_area.y1 + lv_font_get_line_height(nums_font);


    lv_style_int_t bg_bottom = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_BG, LV_STYLE_PAD_BOTTOM);
    lv_style_int_t bg_left = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_BG, LV_STYLE_PAD_LEFT);
    lv_style_int_t bg_right = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_BG, LV_STYLE_PAD_RIGHT);

    lv_coord_t w          = lv_obj_get_width(calendar) - bg_left - bg_right;
    lv_coord_t h          = calendar->coords.y2 - label_area.y1 - bg_bottom;
    lv_coord_t box_w      = w / 7;
    lv_coord_t vert_space = (h - (6 * lv_font_get_line_height(nums_font))) / 5;

    uint32_t week;
    uint8_t day_cnt;
    uint8_t month_start_day = get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1);
    day_draw_state_t draw_state; /*true: Not the prev. or next month is drawn*/

    lv_style_int_t wb_left = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_WEEK_BOX, LV_STYLE_PAD_LEFT);
    lv_style_int_t wb_right = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_WEEK_BOX, LV_STYLE_PAD_RIGHT);
    lv_style_int_t wb_top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_WEEK_BOX, LV_STYLE_PAD_TOP);
    lv_style_int_t wb_bottom = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_WEEK_BOX, LV_STYLE_PAD_BOTTOM);

    /*The state changes without re-caching the styles, disable the use of cache*/
    calendar->style_dsc.cache.enabled = 0;
    lv_obj_state_t state_ori = calendar->state;

    lv_draw_label_dsc_t wb_label_dsc;
    lv_draw_label_dsc_t tb_label_dsc;
    lv_draw_label_dsc_t normal_label_dsc;
    lv_draw_label_dsc_t chk_label_dsc;
    lv_draw_label_dsc_t ina_label_dsc;
    lv_draw_label_dsc_t pr_label_dsc;

    lv_draw_label_dsc_init(&wb_label_dsc);
    lv_draw_label_dsc_init(&tb_label_dsc);
    lv_draw_label_dsc_init(&normal_label_dsc);
    lv_draw_label_dsc_init(&chk_label_dsc);
    lv_draw_label_dsc_init(&ina_label_dsc);
    lv_draw_label_dsc_init(&pr_label_dsc);

    tb_label_dsc.flag = LV_TXT_FLAG_CENTER;
    wb_label_dsc.flag = LV_TXT_FLAG_CENTER;
    normal_label_dsc.flag = LV_TXT_FLAG_CENTER;
    chk_label_dsc.flag = LV_TXT_FLAG_CENTER;
    ina_label_dsc.flag = LV_TXT_FLAG_CENTER;
    pr_label_dsc.flag = LV_TXT_FLAG_CENTER;

    calendar->state = 0;
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_WEEK_BOX, &wb_label_dsc);
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_TODAY_BOX, &tb_label_dsc);
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_DATE_NUMS, &normal_label_dsc);

    calendar->state = LV_OBJ_STATE_CHECKED;
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_DATE_NUMS, &chk_label_dsc);

    calendar->state = LV_OBJ_STATE_DISABLED;
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_DATE_NUMS, &ina_label_dsc);

    calendar->state = LV_OBJ_STATE_PRESSED;
    lv_obj_init_draw_label_dsc(calendar, LV_CALENDAR_PART_DATE_NUMS, &pr_label_dsc);
    pr_label_dsc.color = LV_COLOR_RED;


    calendar->state = state_ori;
    calendar->style_dsc.cache.enabled = 1;

    lv_draw_label_dsc_t * act_label_dsc = &ina_label_dsc;

    /*If starting with the first day of the week then the previous month is not visible*/
    if(month_start_day == 0) {
        day_cnt    = 1;
        draw_state = DAY_DRAW_ACT_MONTH;
        act_label_dsc  = &normal_label_dsc;
    } else {
        draw_state = DAY_DRAW_PREV_MONTH;
        day_cnt = get_month_length(ext->showed_date.year, ext->showed_date.month - 1); /*Length of the previous month*/
        day_cnt -= month_start_day - 1; /*First visible number of the previous month*/
        act_label_dsc = &ina_label_dsc;
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
            week_box_area.x1 = calendar->coords.x1 + bg_left - wb_left;
            week_box_area.x2 = calendar->coords.x2 - bg_right + wb_right;

            week_box_area.y1 -= wb_top;
            week_box_area.y2 += wb_bottom;

            lv_draw_rect_dsc_t wb_rect_dsc;
            lv_draw_rect_dsc_init(&wb_rect_dsc);
            lv_obj_init_draw_rect_dsc(calendar, LV_CALENDAR_PART_WEEK_BOX, &wb_rect_dsc);
            lv_draw_rect(&week_box_area, mask, &wb_rect_dsc);

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
                act_label_dsc  = &normal_label_dsc;
            }
            /*The current month is over*/
            if(draw_state == DAY_DRAW_ACT_MONTH &&
               day_cnt > get_month_length(ext->showed_date.year, ext->showed_date.month)) {
                draw_state = DAY_DRAW_NEXT_MONTH;
                day_cnt    = 1;
                act_label_dsc  = &ina_label_dsc;
            }

            label_area.x1 = calendar->coords.x1 + (w * day) / 7 + bg_left;
            label_area.x2 = label_area.x1 + box_w - 1;

            /*Draw the "today box"*/
            if(draw_state == DAY_DRAW_ACT_MONTH && month_of_today_shown && ext->today.day == day_cnt) {
                lv_area_t today_box_area;
                lv_area_copy(&today_box_area, &label_area);
                today_box_area.x1 = label_area.x1;
                today_box_area.x2 = label_area.x2;

                lv_style_int_t tb_top = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_TODAY_BOX, LV_STYLE_PAD_TOP);
                lv_style_int_t tb_bottom = lv_obj_get_style_int(calendar, LV_CALENDAR_PART_TODAY_BOX, LV_STYLE_PAD_BOTTOM);
                today_box_area.y1 = label_area.y1 - tb_top;
                today_box_area.y2 = label_area.y2 + tb_bottom;

                lv_draw_rect_dsc_t tb_rect_dsc;
                lv_draw_rect_dsc_init(&tb_rect_dsc);
                lv_obj_init_draw_rect_dsc(calendar, LV_CALENDAR_PART_WEEK_BOX, &tb_rect_dsc);
                lv_draw_rect(&today_box_area, mask, &tb_rect_dsc);
            }

            /*Get the final style : highlighted/week box/today box/normal*/
            lv_draw_label_dsc_t * final_label_dsc;
            if(is_pressed(calendar, draw_state, ext->showed_date.year, ext->showed_date.month, day_cnt)) {
                final_label_dsc = &pr_label_dsc;
            } else if(is_highlighted(calendar, draw_state, ext->showed_date.year, ext->showed_date.month, day_cnt)) {
                final_label_dsc = &chk_label_dsc;
            } else if(month_of_today_shown && day_cnt == ext->today.day && draw_state == DAY_DRAW_ACT_MONTH)
                final_label_dsc = &tb_label_dsc;
            else if(in_week_box && draw_state == DAY_DRAW_ACT_MONTH)
                final_label_dsc = &wb_label_dsc;
            else
                final_label_dsc = act_label_dsc;

            /*Write the day's number*/
            lv_utils_num_to_str(day_cnt, buf);
            lv_draw_label(&label_area, mask, final_label_dsc, buf, NULL);

            /*Go to the next day*/
            day_cnt++;
        }

        /*Got to the next weeks row*/
        label_area.y1 += vert_space + lv_font_get_line_height(nums_font);
        label_area.y2 += vert_space + lv_font_get_line_height(nums_font);
    }
}

/**
 * Check weather a date is highlighted or not
 * @param calendar pointer to a calendar object
 * @param draw_state which month is drawn (previous, active, next)
 * @param year a year
 * @param month a  month [1..12]
 * @param day a day [1..31]
 * @return true: highlighted
 */
static bool is_highlighted(lv_obj_t * calendar, day_draw_state_t draw_state, int32_t year, int32_t month, int32_t day)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);


    if(draw_state == DAY_DRAW_PREV_MONTH) {
        year -= month == 1 ? 1 : 0;
        month = month == 1 ? 12 : month - 1;
    } else if(draw_state == DAY_DRAW_NEXT_MONTH) {
        year += month == 12 ? 1 : 0;
        month = month == 12 ? 1 : month + 1;
    }

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
 * Check weather a date is highlighted or not
 * @param calendar pointer to a calendar object
 * @param draw_state which month is drawn (previous, active, next)
 * @param year a year
 * @param month a  month [1..12]
 * @param day a day [1..31]
 * @return true: highlighted
 */
static bool is_pressed(lv_obj_t * calendar, day_draw_state_t draw_state, int32_t year, int32_t month, int32_t day)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);


    if(draw_state == DAY_DRAW_PREV_MONTH) {
        year -= month == 1 ? 1 : 0;
        month = month == 1 ? 12 : month - 1;
    } else if(draw_state == DAY_DRAW_NEXT_MONTH) {
        year += month == 12 ? 1 : 0;
        month = month == 12 ? 1 : month + 1;
    }

    if(year == ext->pressed_date.year && month == ext->pressed_date.month && day == ext->pressed_date.day) return true;
    else return false;
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
