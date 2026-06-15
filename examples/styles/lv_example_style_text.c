/**
 * @file lv_example_style_text.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Text styling
 * @brief Color text, widen letter/line spacing, and underline it.
 *
 * `text_color` recolors the glyphs, `text_letter_space="4"` and
 * `text_line_space="14"` open up tracking and leading, and
 * `text_decor="underline"` underlines every line — the spacing makes the
 * two lines read as a styled heading rather than body text.
 */
void lv_example_style_text(void)
{
    static lv_style_t style_card;
    static lv_style_t style_heading;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_card);
        lv_style_set_radius(&style_card, 16);
        lv_style_set_bg_opa(&style_card, (255 * 100 / 100));
        lv_style_set_bg_color(&style_card, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_card, 22);

        lv_style_init(&style_heading);
        lv_style_set_text_color(&style_heading, ACCENT);
        lv_style_set_text_letter_space(&style_heading, 4);
        lv_style_set_text_line_space(&style_heading, 14);
        lv_style_set_text_decor(&style_heading, LV_TEXT_DECOR_UNDERLINE);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* 💡 Tune `text_letter_space`/`text_line_space`, or switch `text_decor` to `strikethrough`. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 240, LV_SIZE_CONTENT);
    lv_obj_set_align(container, LV_ALIGN_CENTER);
    lv_obj_add_style(container, &style_card, 0);
    lv_obj_t * label = lv_label_create(container);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label, "Custom\nTypography");
    lv_obj_add_style(label, &style_heading, 0);
}
#endif
