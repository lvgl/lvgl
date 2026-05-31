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
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tap a day to select it; use the header arrows to jump between months. */
    lv_obj_t * calendar = lv_calendar_create(screen);
    lv_obj_set_size(calendar, lv_pct(90), 200);
    lv_calendar_set_today_year(calendar, 2026);
    lv_calendar_set_today_month(calendar, 5);
    lv_calendar_set_today_day(calendar, 15);
    lv_calendar_set_shown_year(calendar, 2026);
    lv_calendar_set_shown_month(calendar, 5);
    lv_calendar_add_header_arrow(calendar);
}
#endif
