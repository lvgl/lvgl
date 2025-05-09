#include "../../lv_examples.h"
#if LV_USE_TEXTAREA && LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

static void create_styled_textarea_cursor(const char * txt, lv_coord_t y_ofs, lv_style_t * cursor_style)
{
    lv_obj_t * ta = lv_textarea_create(lv_screen_active());
    lv_textarea_set_text(ta, txt);
    lv_obj_set_width(ta, 280);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, y_ofs);
    lv_textarea_set_one_line(ta, true);
    lv_obj_add_state(ta, LV_STATE_FOCUSED);
    lv_obj_add_style(ta, cursor_style, LV_PART_CURSOR | LV_STATE_FOCUSED);
}

void lv_example_textarea_4(void)
{
    static lv_style_t style_simple, style_block, style_underline;

    /* Thin left bar cursor (simple) */
    lv_style_init(&style_simple);
    lv_style_set_border_color(&style_simple, lv_palette_main(LV_PALETTE_RED));

    /* Full block cursor */
    lv_style_init(&style_block);
    lv_style_set_bg_color(&style_block, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_opa(&style_block, LV_OPA_COVER);
    lv_style_set_border_color(&style_block, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_border_side(&style_block, LV_BORDER_SIDE_RIGHT);
    lv_style_set_pad_right(&style_block, 5); // set width of cursor using pad

    /* Underline cursor */
    lv_style_init(&style_underline);
    lv_style_set_bg_opa(&style_underline, LV_OPA_TRANSP);
    lv_style_set_border_color(&style_underline, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_border_side(&style_underline, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_right(&style_underline, 5); // set width of cursor using pad
    lv_style_set_border_width(&style_underline, 3); // set thickness of underline cursor

    /* Create 3 independent textareas, each with a unique styled cursor */
    create_styled_textarea_cursor("this is a simple red cursor",   10,  &style_simple);
    create_styled_textarea_cursor("this is a green block cursor",    60,  &style_block);
    create_styled_textarea_cursor("this is an underline blue cursor", 110, &style_underline);
}

#endif
