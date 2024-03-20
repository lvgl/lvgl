/**
 * @file lv_calendar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_calendar.h"
#include "../../../lvgl.h"
#if LV_USE_CALENDAR

#include "../../../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CALENDAR_CTRL_TODAY      LV_BTNMATRIX_CTRL_CUSTOM_1
#define LV_CALENDAR_CTRL_HIGHLIGHT  LV_BTNMATRIX_CTRL_CUSTOM_2

#define MY_CLASS &lv_calendar_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_calendar_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void draw_part_begin_event_cb(lv_event_t * e);

static uint8_t get_day_of_week(uint32_t year, uint32_t month, uint32_t day);
static uint8_t get_month_length(int32_t year, int32_t month);
static uint8_t is_leap_year(uint32_t year);
static void highlight_update(lv_obj_t * calendar);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_calendar_class = {
    .constructor_cb = lv_calendar_constructor,
    .width_def = (LV_DPI_DEF * 3) / 2,
    .height_def = (LV_DPI_DEF * 3) / 2,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size = sizeof(lv_calendar_t),
    .base_class = &lv_obj_class
};

static const char * day_names_def[7] = LV_CALENDAR_DEFAULT_DAY_NAMES;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_calendar_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_calendar_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_calendar_set_day_names(lv_obj_t * obj, const char * day_names[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    uint32_t i;
    for(i = 0; i < 7; i++) {
        calendar->map[i] = day_names[i];
    }
    lv_obj_invalidate(obj);
}

void lv_calendar_set_today_date(lv_obj_t * obj, uint32_t year, uint32_t month, uint32_t day)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    calendar->today.year         = year;
    calendar->today.month        = month;
    calendar->today.day          = day;

    highlight_update(obj);
}

void lv_calendar_set_highlighted_dates(lv_obj_t * obj, lv_calendar_date_t highlighted[], uint16_t date_num)
{
    LV_ASSERT_NULL(highlighted);

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    calendar->highlighted_dates     = highlighted;
    calendar->highlighted_dates_num = date_num;

    highlight_update(obj);
}

void lv_calendar_set_showed_date(lv_obj_t * obj, uint32_t year, uint32_t month)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    calendar->showed_date.year   = year;
    calendar->showed_date.month  = month;
    calendar->showed_date.day    = 1;

    lv_calendar_date_t d;
    d.year = calendar->showed_date.year;
    d.month = calendar->showed_date.month;
    d.day = calendar->showed_date.day;

    uint32_t i;

    /*Remove the disabled state but revert it for day names*/
    lv_btnmatrix_clear_btn_ctrl_all(calendar->btnm, LV_BTNMATRIX_CTRL_DISABLED);
    for(i = 0; i < 7; i++) {
        lv_btnmatrix_set_btn_ctrl(calendar->btnm, i, LV_BTNMATRIX_CTRL_DISABLED);
    }

    uint8_t act_mo_len = get_month_length(d.year, d.month);
    uint8_t day_first = get_day_of_week(d.year, d.month, 1);
    uint8_t c;
    for(i = day_first, c = 1; i < act_mo_len + day_first; i++, c++) {
        lv_snprintf(calendar->nums[i], sizeof(calendar->nums[0]), "%d", c);
    }

    uint8_t prev_mo_len = get_month_length(d.year, d.month - 1);
    for(i = 0, c = prev_mo_len - day_first + 1; i < day_first; i++, c++) {
        lv_snprintf(calendar->nums[i], sizeof(calendar->nums[0]), "%d", c);
        lv_btnmatrix_set_btn_ctrl(calendar->btnm, i + 7, LV_BTNMATRIX_CTRL_DISABLED);
    }

    for(i = day_first + act_mo_len, c = 1; i < 6 * 7; i++, c++) {
        lv_snprintf(calendar->nums[i], sizeof(calendar->nums[0]), "%d", c);
        lv_btnmatrix_set_btn_ctrl(calendar->btnm, i + 7, LV_BTNMATRIX_CTRL_DISABLED);
    }

    highlight_update(obj);

    /*Reset the focused button if the days changes*/
    if(lv_btnmatrix_get_selected_btn(calendar->btnm) != LV_BTNMATRIX_BTN_NONE) {
        lv_btnmatrix_set_selected_btn(calendar->btnm, day_first + 7);
    }

    lv_obj_invalidate(obj);

    /* The children of the calendar are probably headers.
     * Notify them to let the headers updated to the new date*/
    uint32_t child_cnt = lv_obj_get_child_cnt(obj);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(child == calendar->btnm) continue;
        lv_event_send(child, LV_EVENT_VALUE_CHANGED, obj);
    }
}

/*=====================
 * Getter functions
 *====================*/

lv_obj_t * lv_calendar_get_btnmatrix(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    const lv_calendar_t * calendar = (lv_calendar_t *)obj;
    return calendar->btnm;
}

const lv_calendar_date_t * lv_calendar_get_today_date(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    const lv_calendar_t * calendar = (lv_calendar_t *)obj;

    return &calendar->today;
}

const lv_calendar_date_t * lv_calendar_get_showed_date(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    const lv_calendar_t * calendar = (lv_calendar_t *)obj;

    return &calendar->showed_date;
}

lv_calendar_date_t * lv_calendar_get_highlighted_dates(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    return calendar->highlighted_dates;
}

uint16_t lv_calendar_get_highlighted_dates_num(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    return calendar->highlighted_dates_num;
}

lv_res_t lv_calendar_get_pressed_date(const lv_obj_t * obj, lv_calendar_date_t * date)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    uint16_t d = lv_btnmatrix_get_selected_btn(calendar->btnm);
    if(d == LV_BTNMATRIX_BTN_NONE) {
        date->year = 0;
        date->month = 0;
        date->day = 0;
        return LV_RES_INV;
    }

    const char * txt = lv_btnmatrix_get_btn_text(calendar->btnm, lv_btnmatrix_get_selected_btn(calendar->btnm));

    if(txt[1] == 0) date->day = txt[0] - '0';
    else date->day = (txt[0] - '0') * 10 + (txt[1] - '0');

    date->year = calendar->showed_date.year;
    date->month = calendar->showed_date.month;

    return LV_RES_OK;
}

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void lv_calendar_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_calendar_t * calendar = (lv_calendar_t *)obj;

    /*Initialize the allocated 'ext'*/
    calendar->today.year  = 2020;
    calendar->today.month = 1;
    calendar->today.day   = 1;

    calendar->showed_date.year  = 2020;
    calendar->showed_date.month = 1;
    calendar->showed_date.day   = 1;

    calendar->highlighted_dates      = NULL;
    calendar->highlighted_dates_num  = 0;

    lv_memset_00(calendar->nums, sizeof(calendar->nums));
    uint8_t i;
    uint8_t j = 0;
    for(i = 0; i < 8 * 7; i++) {
        /*Every 8th string is "\n"*/
        if(i != 0 && (i + 1) % 8 == 0) {
            calendar->map[i] = "\n";
        }
        else if(i < 7) {
            calendar->map[i] = day_names_def[i];
        }
        else {
            calendar->nums[j][0] = 'x';
            calendar->map[i] = calendar->nums[j];
            j++;
        }
    }
    calendar->map[8 * 7 - 1] = "";

    calendar->btnm = lv_btnmatrix_create(obj);
    lv_btnmatrix_set_map(calendar->btnm, calendar->map);
    lv_btnmatrix_set_btn_ctrl_all(calendar->btnm, LV_BTNMATRIX_CTRL_CLICK_TRIG | LV_BTNMATRIX_CTRL_NO_REPEAT);
    lv_obj_add_event_cb(calendar->btnm, draw_part_begin_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_set_width(calendar->btnm, lv_pct(100));

    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(calendar->btnm, 1);

    lv_calendar_set_showed_date(obj, calendar->showed_date.year, calendar->showed_date.month);
    lv_calendar_set_today_date(obj, calendar->today.year, calendar->today.month, calendar->today.day);

    lv_obj_add_flag(calendar->btnm, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static void draw_part_begin_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
    if(dsc->part == LV_PART_ITEMS) {
        /*Day name styles*/
        if(dsc->id < 7) {
            dsc->rect_dsc->bg_opa = LV_OPA_TRANSP;
            dsc->rect_dsc->border_opa = LV_OPA_TRANSP;
        }
        else if(lv_btnmatrix_has_btn_ctrl(obj, dsc->id, LV_BTNMATRIX_CTRL_DISABLED)) {
            dsc->rect_dsc->bg_opa = LV_OPA_TRANSP;
            dsc->rect_dsc->border_opa = LV_OPA_TRANSP;
            dsc->label_dsc->color = lv_palette_main(LV_PALETTE_GREY);
        }

        if(lv_btnmatrix_has_btn_ctrl(obj, dsc->id, LV_CALENDAR_CTRL_HIGHLIGHT)) {
            dsc->rect_dsc->bg_opa = LV_OPA_40;
            dsc->rect_dsc->bg_color = lv_theme_get_color_primary(obj);
            if(lv_btnmatrix_get_selected_btn(obj) == dsc->id) {
                dsc->rect_dsc->bg_opa = LV_OPA_70;
            }
        }

        if(lv_btnmatrix_has_btn_ctrl(obj, dsc->id, LV_CALENDAR_CTRL_TODAY)) {
            dsc->rect_dsc->border_opa = LV_OPA_COVER;
            dsc->rect_dsc->border_color = lv_theme_get_color_primary(obj);
            dsc->rect_dsc->border_width += 1;
        }

    }
}

/**
 * Get the number of days in a month
 * @param year a year
 * @param month a month. The range is basically [1..12] but [-11..0] or [13..24] is also
 *              supported to handle next/prev. year
 * @return [28..31]
 */
static uint8_t get_month_length(int32_t year, int32_t month)
{
    month--;
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
 * @param month a  month [1..12]
 * @param day a day [1..32]
 * @return [0..6] which means [Sun..Sat] or [Mon..Sun] depending on LV_CALENDAR_WEEK_STARTS_MONDAY
 */
static uint8_t get_day_of_week(uint32_t year, uint32_t month, uint32_t day)
{
    uint32_t a = month < 3 ? 1 : 0;
    uint32_t b = year - a;

#if LV_CALENDAR_WEEK_STARTS_MONDAY
    uint32_t day_of_week = (day + (31 * (month - 2 + 12 * a) / 12) + b + (b / 4) - (b / 100) + (b / 400) - 1) % 7;
#else
    uint32_t day_of_week = (day + (31 * (month - 2 + 12 * a) / 12) + b + (b / 4) - (b / 100) + (b / 400)) % 7;
#endif

    return day_of_week  ;
}

static void highlight_update(lv_obj_t * obj)
{
    lv_calendar_t * calendar = (lv_calendar_t *)obj;
    uint16_t i;

    /*Clear all kind of selection*/
    lv_btnmatrix_clear_btn_ctrl_all(calendar->btnm, LV_CALENDAR_CTRL_TODAY | LV_CALENDAR_CTRL_HIGHLIGHT);

    uint8_t day_first = get_day_of_week(calendar->showed_date.year, calendar->showed_date.month, 1);
    if(calendar->highlighted_dates) {
        for(i = 0; i < calendar->highlighted_dates_num; i++) {
            if(calendar->highlighted_dates[i].year == calendar->showed_date.year &&
               calendar->highlighted_dates[i].month == calendar->showed_date.month) {
                lv_btnmatrix_set_btn_ctrl(calendar->btnm, calendar->highlighted_dates[i].day - 1 + day_first + 7,
                                          LV_CALENDAR_CTRL_HIGHLIGHT);
            }
        }
    }

    if(calendar->showed_date.year == calendar->today.year && calendar->showed_date.month == calendar->today.month) {
        lv_btnmatrix_set_btn_ctrl(calendar->btnm, calendar->today.day - 1 + day_first + 7, LV_CALENDAR_CTRL_TODAY);
    }
}

#endif  /*LV_USE_CALENDAR*/
