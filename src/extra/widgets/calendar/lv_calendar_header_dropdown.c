/**
 * @file lv_calendar_header_dropdown.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_calendar_header_dropdown.h"
#if LV_USE_CALENDAR_HEADER_DROPDOWN

#include "lv_calendar.h"
#include "../../../widgets/lv_dropdown.h"
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
static void year_event_cb(lv_event_t * e);
static void month_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * month_list = "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12";
static const char * year_list = {
        "2023\n2022\n2021\n"
        "2020\n2019\n2018\n2017\n2016\n2015\n2014\n2013\n2012\n2011\n2010\n2009\n2008\n2007\n2006\n2005\n2004\n2003\n2002\n2001\n"
        "2000\n1999\n1998\n1997\n1996\n1995\n1994\n1993\n1992\n1991\n1990\n1989\n1988\n1987\n1986\n1985\n1984\n1983\n1982\n1981\n"
        "1980\n1979\n1978\n1977\n1976\n1975\n1974\n1973\n1972\n1971\n1970\n1969\n1968\n1967\n1966\n1965\n1964\n1963\n1962\n1961\n"
        "1960\n1959\n1958\n1957\n1956\n1955\n1954\n1953\n1952\n1951\n1950\n1949\n1948\n1947\n1946\n1945\n1944\n1943\n1942\n1941\n"
        "1940\n1939\n1938\n1937\n1936\n1935\n1934\n1933\n1932\n1931\n1930\n1929\n1928\n1927\n1926\n1925\n1924\n1923\n1922\n1921\n"
        "1920\n1919\n1918\n1917\n1916\n1915\n1914\n1913\n1912\n1911\n1910\n1909\n1908\n1907\n1906\n1905\n1904\n1903\n1902\n1901"
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_calendar_header_dropdown_create(lv_obj_t * parent, lv_obj_t * calendar)
{
    lv_obj_t * header = lv_obj_create(parent);

    /*Use the same paddings as the calendar*/
    lv_obj_set_style_pad_left(header,lv_obj_get_style_pad_left(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_right(header,lv_obj_get_style_pad_right(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_top(header,lv_obj_get_style_pad_top(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_bottom(header,lv_obj_get_style_pad_bottom(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_column(header,lv_obj_get_style_pad_column(calendar, LV_PART_MAIN), 0);
    lv_obj_set_style_radius(header,lv_obj_get_style_radius(calendar, LV_PART_MAIN), 0);

    const lv_calendar_date_t * cur_date = lv_calendar_get_showed_date(calendar);

    lv_obj_update_layout(calendar);
    lv_coord_t w = lv_obj_get_width(calendar);
    lv_obj_set_size(header,  w, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t * year_dd = lv_dropdown_create(header);
    lv_dropdown_set_options(year_dd, year_list);
    lv_dropdown_set_selected(year_dd, 2023 - cur_date->year);
    lv_obj_add_event_cb(year_dd, year_event_cb, LV_EVENT_VALUE_CHANGED, calendar);
    lv_obj_set_flex_grow(year_dd, 1);

    lv_obj_t * month_dd = lv_dropdown_create(header);
    lv_dropdown_set_options(month_dd, month_list);
    lv_dropdown_set_selected(month_dd, cur_date->month - 1);
    lv_obj_add_event_cb(month_dd, month_event_cb, LV_EVENT_VALUE_CHANGED, calendar);
    lv_obj_set_flex_grow(month_dd, 1);

    lv_obj_align_to(header, calendar, LV_ALIGN_OUT_TOP_MID, 0, 0);

    return header;
}

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void month_event_cb(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_target(e);
    lv_obj_t * calendar = lv_event_get_user_data(e);

    uint16_t sel = lv_dropdown_get_selected(dropdown);

    const lv_calendar_date_t * d;
    d = lv_calendar_get_showed_date(calendar);
    lv_calendar_date_t newd = *d;
    newd.month = sel + 1;

    lv_calendar_set_showed_date(calendar, newd.year, newd.month);
}
static void year_event_cb(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_target(e);
    lv_obj_t * calendar = lv_event_get_user_data(e);

    uint16_t sel = lv_dropdown_get_selected(dropdown);

    const lv_calendar_date_t * d;
    d = lv_calendar_get_showed_date(calendar);
    lv_calendar_date_t newd = *d;
    newd.year = 2023 - sel;

    lv_calendar_set_showed_date(calendar, newd.year, newd.month);
}

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/

