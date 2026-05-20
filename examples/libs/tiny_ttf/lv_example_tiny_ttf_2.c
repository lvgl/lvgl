#include "../../lv_examples.h"

#if LV_BUILD_EXAMPLES

#if LV_USE_TINY_TTF && LV_TINY_TTF_FILE_SUPPORT

/**
 * @title Tiny TTF font from file
 * @brief Render a multi-line label with a 30 px TTF opened from the LVGL filesystem.
 *
 * `lv_tiny_ttf_create_file` reads `Ubuntu-Medium.ttf` at 30 px through the
 * filesystem driver registered under drive letter `A`. A style binds the
 * resulting `lv_font_t` and centers text, then a label placed at the
 * center of the active screen shows a four-line greeting drawn with the
 * decoded font. The file path requires `LV_TINY_TTF_FILE_SUPPORT`.
 */
void lv_example_tiny_ttf_2(void)
{
    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_font_t * font = lv_tiny_ttf_create_file("A:lvgl/examples/libs/tiny_ttf/Ubuntu-Medium.ttf", 30);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\nI'm a font\ncreated\nwith Tiny TTF");
    lv_obj_center(label);
}
#else

void lv_example_tiny_ttf_2(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "TinyTTF can't load files in the browser");
    lv_obj_center(label);
}

#endif /*LV_USE_TINY_TTF && LV_TINY_TTF_FILE_SUPPORT*/

#endif /*LV_BUILD_EXAMPLES*/
