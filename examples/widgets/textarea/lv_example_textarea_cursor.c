/**
 * @file lv_example_textarea_cursor.c
 */

#include "../../lv_examples.h"
#if LV_USE_TEXTAREA && LV_BUILD_EXAMPLES

/**
 * @title Text area cursor styling
 * @brief Style the cursor part as a solid block or a thin bar.
 *
 * Both text areas style the `cursor` part with a named style. The first fills
 * the cell with an opaque background for a block cursor; the second leaves the
 * background transparent and draws only a left border for a bar cursor — the
 * same part, two looks.
 */
void lv_example_textarea_cursor(void)
{
    static lv_style_t style_block_cursor;
    static lv_style_t style_bar_cursor;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_block_cursor);
        lv_style_set_bg_opa(&style_block_cursor, (255 * 100 / 100));
        lv_style_set_bg_color(&style_block_cursor, lv_color_hex(0x2563eb));
        lv_style_set_text_color(&style_block_cursor, lv_color_hex(0xffffff));
        lv_style_set_border_side(&style_block_cursor, LV_BORDER_SIDE_NONE);

        lv_style_init(&style_bar_cursor);
        lv_style_set_bg_opa(&style_bar_cursor, (255 * 0 / 100));
        lv_style_set_border_side(&style_bar_cursor, LV_BORDER_SIDE_LEFT);
        lv_style_set_border_width(&style_bar_cursor, 2);
        lv_style_set_border_color(&style_bar_cursor, lv_color_hex(0xdc2626));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Block cursor */
    lv_obj_t * textarea_1 = lv_textarea_create(screen);
    lv_obj_set_width(textarea_1, lv_pct(60));
    lv_textarea_set_one_line(textarea_1, true);
    lv_textarea_set_text(textarea_1, "Block");
    lv_textarea_set_cursor_pos(textarea_1, 2);
    lv_obj_set_state(textarea_1, LV_STATE_FOCUSED, true);
    lv_obj_add_style(textarea_1, &style_block_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);

    /* Bar cursor */
    lv_obj_t * textarea_2 = lv_textarea_create(screen);
    lv_obj_set_width(textarea_2, lv_pct(60));
    lv_textarea_set_one_line(textarea_2, true);
    lv_textarea_set_text(textarea_2, "Bar");
    lv_textarea_set_cursor_pos(textarea_2, 2);
    lv_obj_set_state(textarea_2, LV_STATE_FOCUSED, true);
    lv_obj_add_style(textarea_2, &style_bar_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);
}
#endif
