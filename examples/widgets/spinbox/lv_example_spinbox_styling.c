/**
 * @file lv_example_spinbox_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_SPINBOX && LV_BUILD_EXAMPLES

/**
 * @title Spinbox styling
 * @brief Style the main box and the editing cursor as separate parts.
 *
 * Spinbox inherits its drawable surface from the base widget, so `MAIN`
 * accepts the full background style stack — backgrounds, borders, radii,
 * padding, and text. `text_letter_space` on MAIN spreads the digits apart
 * so the cursor's frame visually sits around a single digit rather than
 * hugging two together.
 *
 * The `CURSOR` selector targets the highlight that marks the currently
 * editable digit. A clean pattern is to leave the cursor background
 * transparent (`bg_opa="0"`) and draw only a coloured border — the active
 * digit then reads as framed. Set `text_opa="0"` on the cursor so it
 * doesn't re-paint the digit on top of MAIN's render (avoids subtle
 * double-drawing), and use `pad_ver` to let the frame breathe vertically
 * around the glyph.
 */
void lv_example_spinbox_styling(void)
{
    static lv_style_t style_spinbox_main;
    static lv_style_t style_spinbox_cursor;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_spinbox_main);
        lv_style_set_bg_color(&style_spinbox_main, lv_color_hex(0xf3f4f6));
        lv_style_set_bg_opa(&style_spinbox_main, (255 * 100 / 100));
        lv_style_set_radius(&style_spinbox_main, 8);
        lv_style_set_border_color(&style_spinbox_main, lv_color_hex(0xd1d5db));
        lv_style_set_border_width(&style_spinbox_main, 1);
        lv_style_set_pad_all(&style_spinbox_main, 8);
        lv_style_set_text_color(&style_spinbox_main, lv_color_hex(0x111827));
        lv_style_set_text_letter_space(&style_spinbox_main, 4);

        lv_style_init(&style_spinbox_cursor);
        lv_style_set_bg_opa(&style_spinbox_cursor, 0);
        lv_style_set_text_opa(&style_spinbox_cursor, 0);
        lv_style_set_border_color(&style_spinbox_cursor, lv_color_hex(0x6366f1));
        lv_style_set_border_width(&style_spinbox_cursor, 2);
        lv_style_set_radius(&style_spinbox_cursor, 4);
        lv_style_set_pad_ver(&style_spinbox_cursor, 2);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tap a digit position to move the cursor and see the cursor style highlight it. */
    lv_obj_t * spinbox = lv_spinbox_create(screen);
    lv_obj_set_width(spinbox, 180);
    lv_spinbox_set_digit_count(spinbox, 5);
    lv_spinbox_set_value(spinbox, 12345);
    lv_spinbox_set_dec_point_pos(spinbox, 2);
    lv_spinbox_set_step(spinbox, 1);
    lv_obj_add_style(spinbox, &style_spinbox_main, LV_PART_MAIN);
    lv_obj_add_style(spinbox, &style_spinbox_cursor, LV_PART_CURSOR);
}
#endif
