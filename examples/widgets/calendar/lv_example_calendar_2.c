#include "../../lv_examples.h"
#if LV_USE_CALENDAR && LV_USE_CALENDAR_CHINESE && LV_BUILD_EXAMPLES

/**
 * @title Chinese calendar mode
 * @brief Render a calendar with Chinese lunar annotations for March 2024.
 *
 * A 300x300 `lv_calendar` is aligned to the top center of the active
 * screen with today set to 2024-03-22 via `lv_calendar_set_today_date`
 * and the shown month fixed by `lv_calendar_set_month_shown`. An arrow
 * or dropdown header is attached when available.
 * `lv_calendar_set_chinese_mode` enables lunar labels and the CJK font
 * `lv_font_source_han_sans_sc_14_cjk` is applied on `LV_PART_MAIN`.
 * When the Chinese calendar is disabled the example falls back to a
 * centered "not enabled" label.
 */
void lv_example_calendar_2(void)
{
    lv_obj_t  * calendar = lv_calendar_create(lv_screen_active());
    lv_obj_set_size(calendar, 300, 300);
    lv_obj_align(calendar, LV_ALIGN_TOP_MID, 0, 0);

    lv_calendar_set_today_date(calendar, 2024, 03, 22);
    lv_calendar_set_month_shown(calendar, 2024, 03);

#if LV_USE_CALENDAR_HEADER_DROPDOWN
    lv_calendar_add_header_dropdown(calendar);
#elif LV_USE_CALENDAR_HEADER_ARROW
    lv_calendar_add_header_arrow(calendar);
#endif

    lv_calendar_set_chinese_mode(calendar, true);
    lv_obj_set_style_text_font(calendar, &lv_font_source_han_sans_sc_14_cjk, LV_PART_MAIN);
}

#else

void lv_example_calendar_2(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "chinese calendar is not enabled");
    lv_obj_center(label);
}

#endif
