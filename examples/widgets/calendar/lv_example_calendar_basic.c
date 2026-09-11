/**
 * @file lv_example_calendar_basic.c
 */

#include "../../lv_examples.h"
#if LV_USE_CALENDAR && LV_BUILD_EXAMPLES

/**
 * @title Calendar basics
 * @brief Month view with an arrow header for navigation.
 *
 * `today_*` marks the current date (rendered with the today highlight). `shown_*`
 * controls which month the calendar opens on — here we start a month behind
 * today so the user immediately sees that the today indicator only fires when
 * the shown month matches. A `<header_arrow/>` child adds prev/next arrows to
 * the top of the calendar; swap it for `<header_dropdown/>` to get the
 * month/year selector variant.
 */
void lv_example_calendar_basic(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Tap a day to select it; use the header arrows to jump between months. */
    lv_obj_t * calendar = lv_calendar_create(screen);
    lv_obj_set_size(calendar, 300, 230);
    lv_obj_set_align(calendar, LV_ALIGN_CENTER);
    lv_calendar_set_today_year(calendar, 2026);
    lv_calendar_set_today_month(calendar, 5);
    lv_calendar_set_today_day(calendar, 15);
    lv_calendar_set_shown_year(calendar, 2026);
    lv_calendar_set_shown_month(calendar, 5);
    lv_calendar_add_header_arrow(calendar);
}
#endif
