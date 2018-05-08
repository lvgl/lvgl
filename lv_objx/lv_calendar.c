/**
 * @file lv_calendar.c
 *
 */

/* TODO Remove these instructions
 * Search an replace: calendar -> object normal name with lower case (e.g. button, label etc.)
 *                    calendar -> object short name with lower case(e.g. btn, label etc)
 *                    CALENDAR -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_CALENDAR != 0

#include "lv_calendar.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    DAY_DRAW_PREV_MONTH,
    DAY_DRAW_ACT_MONTH,
    DAY_DRAW_NEXT_MONTH,
}day_draw_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_calendar_design(lv_obj_t * calendar, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_calendar_signal(lv_obj_t * calendar, lv_signal_t sign, void * param);

static lv_coord_t get_header_height(lv_obj_t * calendar);
static void draw_header(lv_obj_t * calendar, const lv_area_t * mask);
static void draw_day_names(lv_obj_t * calendar, const lv_area_t * mask);
static void draw_days(lv_obj_t * calendar, const lv_area_t * mask);
static uint8_t get_day_of_week(uint32_t year, uint32_t month, uint32_t day);
static const char * get_month_name(int32_t year, int32_t month);
static uint8_t get_month_length(int32_t year, int32_t month);
static uint8_t is_leap_year(uint32_t year);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_design;
static const char * day_name[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char * month_name[12] = {"January",   "February",   "March",    "April",
                                      "May",       "June",       "July",     "August",
                                      "September", "October",    "November", "December"};
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
lv_obj_t * lv_calendar_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of calendar*/
    lv_obj_t * new_calendar = lv_obj_create(par, copy);
    lv_mem_assert(new_calendar);

    /*Allocate the calendar type specific extended data*/
    lv_calendar_ext_t * ext = lv_obj_allocate_ext_attr(new_calendar, sizeof(lv_calendar_ext_t));
    lv_mem_assert(ext);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_calendar);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_calendar);

    /*Initialize the allocated 'ext' */
    ext->today.year = 2018;
    ext->today.month= 5;
    ext->today.day= 4;

    ext->showed_date.year = 2018;
    ext->showed_date.month = 5;
    ext->showed_date.day= 4;

    ext->style_main = &lv_style_pretty;
    ext->style_header = &lv_style_plain_color;
    ext->style_marked_days = &lv_style_plain_color;
    ext->style_inactive_days = &lv_style_btn_ina;
    ext->style_week_box = &lv_style_plain_color;
    ext->style_today_box = &lv_style_pretty_color;
    ext->style_day_names = &lv_style_pretty;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_calendar, lv_calendar_signal);
    lv_obj_set_design_func(new_calendar, lv_calendar_design);

    /*Init the new calendar calendar*/
    if(copy == NULL) {

    }
    /*Copy an existing calendar*/
    else {
        lv_calendar_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_calendar);
    }

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

/*
 * New object specific "set" functions come here
 */


/**
 * Set a style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_calendar_set_style(lv_obj_t * calendar, lv_calendar_style_t type, lv_style_t *style)
{
    lv_calendar_ext_t *ext = lv_obj_get_ext_attr(calendar);

    switch (type) {
        case LV_CALENDAR_STYLE_BG:
            ext->style_main = style;
            break;
        case LV_CALENDAR_STYLE_DAY_NAMES:
            ext->style_day_names = style;
            break;
        case LV_CALENDAR_STYLE_HEADER:
            ext->style_header = style;
            break;
        case LV_CALENDAR_STYLE_MARKED_DAYS:
            ext->style_marked_days = style;
            break;
        case LV_CALENDAR_STYLE_INACTIVE_DAYS:
            ext->style_inactive_days = style;
            break;
        case LV_CALENDAR_STYLE_TODAY_BOX:
            ext->style_today_box = style;
            break;
        case LV_CALENDAR_STYLE_WEEK_BOX:
            ext->style_week_box = style;
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

/**
 * Get style of a calendar.
 * @param calendar pointer to calendar object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
lv_style_t * lv_calendar_get_style(lv_obj_t * calendar, lv_calendar_style_t type)
{
    lv_calendar_ext_t *ext = lv_obj_get_ext_attr(calendar);

    switch (type) {
        case LV_CALENDAR_STYLE_BG:              return ext->style_main;
        case LV_CALENDAR_STYLE_HEADER:          return ext->style_header;
        case LV_CALENDAR_STYLE_DAY_NAMES:       return ext->style_day_names;
        case LV_CALENDAR_STYLE_MARKED_DAYS:     return ext->style_marked_days;
        case LV_CALENDAR_STYLE_INACTIVE_DAYS:     return ext->style_inactive_days;
        case LV_CALENDAR_STYLE_WEEK_BOX:        return ext->style_week_box;
        case LV_CALENDAR_STYLE_TODAY_BOX:       return ext->style_today_box;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
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
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);
        lv_draw_rect(&calendar->coords, mask, ext->style_main);

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


    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_calendar";
    }

    return res;
}


static lv_coord_t get_header_height(lv_obj_t * calendar)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    return lv_font_get_height(ext->style_header->text.font) + ext->style_header->body.padding.ver * 2;
}

static void draw_header(lv_obj_t * calendar, const lv_area_t * mask)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    lv_area_t header_area;
    header_area.x1 = calendar->coords.x1;
    header_area.x2 = calendar->coords.x2;
    header_area.y1 = calendar->coords.y1;
    header_area.y2 = calendar->coords.y1 + get_header_height(calendar);

    lv_draw_rect(&header_area, mask, ext->style_header);

    /*Add the month name*/
//    lv_point_t txt_size;
//    lv_txt_get_size(&txt_size, month_name[ext->showed_month.month - 1], ext->style_header->text.font,
//                     ext->style_header->text.letter_space, ext->style_header->text.line_space,  lv_obj_get_width(calendar), LV_TXT_FLAG_NONE);


    header_area.y1 += ext->style_header->body.padding.ver;
    header_area.y2 += ext->style_header->body.padding.ver;
    lv_draw_label(&header_area, mask, ext->style_header, month_name[ext->showed_date.month - 1], LV_TXT_FLAG_CENTER, NULL);

//    lv_coord_t * text_h = lv_

}

static void draw_day_names(lv_obj_t * calendar, const lv_area_t * mask)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    lv_coord_t hpad = ext->style_day_names->body.padding.hor;
    lv_coord_t w = lv_obj_get_width(calendar) - 2 * hpad;
    lv_coord_t box_w = w / 7;
    lv_area_t label_area;
    label_area.y1 = calendar->coords.y1 + get_header_height(calendar) + ext->style_day_names->body.padding.ver;
    label_area.y2 = label_area.y1 + lv_font_get_height(ext->style_day_names->text.font);
    uint32_t i;
    for(i = 0; i < 7; i++) {
        label_area.x1 = calendar->coords.x1 + (w * i) / 7 + hpad;
        label_area.x2 = label_area.x1 + box_w;
        lv_draw_label(&label_area, mask, ext->style_day_names, day_name[i], LV_TXT_FLAG_CENTER, NULL);
    }

}

static void draw_days(lv_obj_t * calendar, const lv_area_t * mask)
{
    lv_calendar_ext_t * ext = lv_obj_get_ext_attr(calendar);

    lv_coord_t hpad = ext->style_day_names->body.padding.hor;
    lv_area_t label_area;
    label_area.y1 = calendar->coords.y1 + get_header_height(calendar) +
                    ext->style_day_names->body.padding.ver + lv_font_get_height(ext->style_day_names->text.font) +
                    ext->style_main->body.padding.ver;
    label_area.y2 = label_area.y1 + lv_font_get_height(ext->style_day_names->text.font);

    lv_coord_t w = lv_obj_get_width(calendar) - 2 * hpad;
    lv_coord_t h = calendar->coords.y2 - label_area.y1 - ext->style_main->body.padding.ver;
    lv_coord_t box_w = w / 7;
    lv_coord_t vert_space = (h - (6 * lv_font_get_height(ext->style_main->text.font))) / 5;

    uint32_t week;
    uint8_t day_cnt;
    uint8_t month_start_day = get_day_of_week(ext->showed_date.year, ext->showed_date.month, 1);
    day_draw_state_t draw_state;    /*true: Not the prev. or next month is drawn*/
    lv_style_t * act_style;
    /*If starting with the first day of the week then the previous month is not visible*/
    if(month_start_day == 0) {
        day_cnt = 1;
        draw_state = DAY_DRAW_ACT_MONTH;
        act_style = ext->style_main;
    } else {
        draw_state = DAY_DRAW_PREV_MONTH;
        day_cnt = get_month_length(ext->showed_date.year, ext->showed_date.month - 1); /*Length of the previous month*/
        day_cnt -= month_start_day - 1;             /*First visible number of the previous month*/
        act_style = ext->style_inactive_days;
    }


    bool today_shown = false;
    if(ext->showed_date.year ==  ext->today.year &&
       ext->showed_date.month ==  ext->today.month)
    {
        today_shown = true;
    }

    char buf[3];
    for(week = 0; week < 6; week++) {

        if(today_shown &&
          ((draw_state == DAY_DRAW_ACT_MONTH  && ext->today.day >= day_cnt && ext->today.day < day_cnt + 7) ||
           (draw_state == DAY_DRAW_PREV_MONTH && ext->today.day <= 7)))
        {
            lv_area_t week_box_area;
            lv_area_copy(&week_box_area, &label_area);      /*'label_area' is already set for this row*/
            week_box_area.x1 = calendar->coords.x1 + ext->style_main->body.padding.hor - ext->style_week_box->body.padding.hor;
            week_box_area.x2 = calendar->coords.x2 - ext->style_main->body.padding.hor + ext->style_week_box->body.padding.hor;

            week_box_area.y1 -= ext->style_week_box->body.padding.ver;
            week_box_area.y2 += ext->style_week_box->body.padding.ver;
            lv_draw_rect(&week_box_area, mask, ext->style_week_box);
        }


        uint32_t day;
        for(day = 0; day < 7; day++) {
            if(draw_state == DAY_DRAW_PREV_MONTH && day == month_start_day) {
                draw_state = DAY_DRAW_ACT_MONTH;
                day_cnt = 1;
                act_style = ext->style_main;
            }
            if(draw_state == DAY_DRAW_ACT_MONTH &&
               day_cnt > get_month_length(ext->showed_date.year, ext->showed_date.month))
            {
                draw_state = DAY_DRAW_NEXT_MONTH;
                day_cnt = 1;
                act_style = ext->style_inactive_days;
            }


            label_area.x1 = calendar->coords.x1 + (w * day) / 7 + hpad;
            label_area.x2 = label_area.x1 + box_w;

            if(draw_state == DAY_DRAW_ACT_MONTH && today_shown && ext->today.day == day_cnt) {
                lv_area_t today_box_area;
                lv_area_copy(&today_box_area, &label_area);
                today_box_area.x1 = label_area.x1;
                today_box_area.x2 = label_area.x2;

                today_box_area.y1 = label_area.y1 - ext->style_today_box->body.padding.ver;
                today_box_area.y2 = label_area.y2 + ext->style_today_box->body.padding.ver;
                lv_draw_rect(&today_box_area, mask, ext->style_today_box);
            }


            sprintf(buf, "%d", day_cnt);
            lv_draw_label(&label_area, mask, act_style, buf, LV_TXT_FLAG_CENTER, NULL);

            day_cnt ++;

        }

        label_area.y1 += vert_space + lv_font_get_height(ext->style_main->text.font);
        label_area.y2 += vert_space + lv_font_get_height(ext->style_main->text.font);
    }
}

static const char * get_month_name(int32_t year, int32_t month)
{
    month --;   /*Range of months id [1..12] but range of indexes is [0..11]*/
    if(month <=0) year--;   /*Already in the previous year (won't be less then -12 to skip a whole year)*/
    if(month > 12) year++;  /*ALready in the next year (won't be more then 12 to skip a whole year)*/
    month = month % 12;
    return month_name[month];
}

static uint8_t get_month_length(int32_t year, int32_t month)
{
    month --;   /*Range of months id [1..12] but range of indexes is [0..11]*/
    if(month <=0) year--;   /*Already in the previous year (won't be less then -12 to skip a whole year)*/
    if(month > 12) year++;  /*ALready in the next year (won't be more then 12 to skip a whole year)*/
    month = month % 12;

    /*month == 1 is february*/
    return (month == 1) ? (28 + is_leap_year(year)) : 31 - month % 7 % 2;


}

static uint8_t is_leap_year(uint32_t year)
{
    return (year % 4) || ((year % 100 == 0) && (year % 400)) ? 0 : 1;
}

static uint8_t get_day_of_week(uint32_t year, uint32_t month, uint32_t day)
{
    uint32_t a = month < 3 ? 1 : 0;
    uint32_t b = year - a;

    uint32_t day_of_week = (day + (31 * (month - 2 + 12 * a) / 12) +
          b + (b / 4) - (b / 100) + (b / 400)) % 7;

    return day_of_week;
}

#endif
