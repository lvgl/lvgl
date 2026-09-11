#include "../../lv_examples.h"
#if LV_USE_TINY_TTF && LV_BUILD_EXAMPLES

/**
 * @title Tiny TTF font from memory
 * @brief Render a multi-line label with a 30 px TTF decoded from an embedded byte array.
 *
 * `lv_tiny_ttf_create_data` loads the external `ubuntu_font` byte array at
 * 30 px. A style binds the resulting `lv_font_t` and centers text, then a
 * label is placed at the center of the active screen showing a four-line
 * greeting drawn with the decoded font.
 */
void lv_example_tiny_ttf_1(void)
{
    extern const uint8_t ubuntu_font[];
    extern const int ubuntu_font_size;

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_font_t * font = lv_tiny_ttf_create_data(ubuntu_font, ubuntu_font_size, 30);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\nI'm a font\ncreated\nwith Tiny TTF");
    lv_obj_center(label);
}
#endif
