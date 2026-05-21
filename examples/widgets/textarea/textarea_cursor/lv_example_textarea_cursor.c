/**
 * @file lv_example_textarea_cursor.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 The cursor is its own part — restyle it without touching the field's look. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Text area: cursor styling");

    /* Block cursor */
    lv_obj_t * lv_textarea_0 = lv_textarea_create(screen);
    lv_obj_set_width(lv_textarea_0, lv_pct(90));
    lv_textarea_set_one_line(lv_textarea_0, true);
    lv_textarea_set_text(lv_textarea_0, "Block");
    lv_textarea_set_cursor_pos(lv_textarea_0, 2);
    lv_obj_set_state(lv_textarea_0, LV_STATE_FOCUSED, true);
    lv_obj_add_style(lv_textarea_0, &style_block_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);

    /* Bar cursor */
    lv_obj_t * lv_textarea_1 = lv_textarea_create(screen);
    lv_obj_set_width(lv_textarea_1, lv_pct(90));
    lv_textarea_set_one_line(lv_textarea_1, true);
    lv_textarea_set_text(lv_textarea_1, "Bar");
    lv_textarea_set_cursor_pos(lv_textarea_1, 2);
    lv_obj_set_state(lv_textarea_1, LV_STATE_FOCUSED, true);
    lv_obj_add_style(lv_textarea_1, &style_bar_cursor, LV_PART_CURSOR | LV_STATE_FOCUSED);
}
#endif
