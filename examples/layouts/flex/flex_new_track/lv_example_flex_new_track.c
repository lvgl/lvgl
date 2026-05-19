/**
 * @file lv_example_flex_new_track.c
 */

#include "../../../lv_examples.h"
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

    /* 💡 Move or remove flex_in_new_track to control exactly where a new row starts. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Force items onto a new flex row");

    /* Demo container showing explicit new-track start */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_size(lv_obj_1, lv_pct(100), LV_SIZE_CONTENT);
    /* Track 1 */
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_1, lv_color_hex(0x3498db), 0);
    lv_label_set_text(lv_label_1, "Mon");
    lv_obj_add_style(lv_label_1, &style_day, 0);

    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_2, lv_color_hex(0x3498db), 0);
    lv_label_set_text(lv_label_2, "Tue");
    lv_obj_add_style(lv_label_2, &style_day, 0);

    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_3, lv_color_hex(0x3498db), 0);
    lv_label_set_text(lv_label_3, "Wed");
    lv_obj_add_style(lv_label_3, &style_day, 0);

    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_4, lv_color_hex(0x3498db), 0);
    lv_label_set_text(lv_label_4, "Thu");
    lv_obj_add_style(lv_label_4, &style_day, 0);

    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_5, lv_color_hex(0x3498db), 0);
    lv_label_set_text(lv_label_5, "Fri");
    lv_obj_add_style(lv_label_5, &style_day, 0);

    /* Track 2 — forced onto a new track */
    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_1);
    lv_obj_set_flag(lv_label_6, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK, true);
    lv_obj_set_style_bg_color(lv_label_6, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(lv_label_6, "Sat");
    lv_obj_add_style(lv_label_6, &style_day, 0);

    lv_obj_t * lv_label_7 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_7, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(lv_label_7, "Sun");
    lv_obj_add_style(lv_label_7, &style_day, 0);
}
#endif
