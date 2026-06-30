/**
 * @file lv_example_flex_new_track.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Flex new track
 * @brief Force an item to start a new wrap row.
 *
 * In a row_wrap container an item with flex_in_new_track="true" begins a fresh track
 * instead of continuing the previous one. Here the weekend labels are pushed onto their
 * own row regardless of how much horizontal space remains on the first row.
 */
void lv_example_flex_new_track(void)
{
    static lv_style_t style_day;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_day);
        lv_style_set_bg_opa(&style_day, (255 * 100 / 100));
        lv_style_set_text_color(&style_day, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_day, 6);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Demo container showing explicit new-track start */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_size(container, lv_pct(90), LV_SIZE_CONTENT);
    /* Track 1 */
    lv_obj_t * label_1 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_1, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_1, "Mon");
    lv_obj_add_style(label_1, &style_day, 0);

    lv_obj_t * label_2 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_2, "Tue");
    lv_obj_add_style(label_2, &style_day, 0);

    lv_obj_t * label_3 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_3, "Wed");
    lv_obj_add_style(label_3, &style_day, 0);

    lv_obj_t * label_4 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_4, "Thu");
    lv_obj_add_style(label_4, &style_day, 0);

    lv_obj_t * label_5 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_5, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_5, "Fri");
    lv_obj_add_style(label_5, &style_day, 0);

    /* Track 2 — forced onto a new track */
    lv_obj_t * label_6 = lv_label_create(container);
    lv_obj_set_flag(label_6, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK, true);
    lv_obj_set_style_bg_color(label_6, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(label_6, "Sat");
    lv_obj_add_style(label_6, &style_day, 0);

    lv_obj_t * label_7 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_7, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(label_7, "Sun");
    lv_obj_add_style(label_7, &style_day, 0);
}
#endif
