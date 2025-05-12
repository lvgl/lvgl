#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_FREETYPE

#if LV_FREETYPE_USE_LVGL_PORT
    #define PATH_PREFIX "A:"
#else
    #define PATH_PREFIX "./"
#endif

/*
 * Load a vector font
 * ThorVG needs to be enabled, LV_USE_VECTOR_GRAPHICS=1
 */
void lv_example_freetype_2_vector_font(uint32_t font_size, uint32_t border_width)
{
    lv_font_t * font = lv_freetype_font_create(PATH_PREFIX "lvgl/examples/libs/freetype/Lato-Regular.ttf",
                                               LV_FREETYPE_FONT_RENDER_MODE_OUTLINE,
                                               font_size,
                                               LV_FREETYPE_FONT_STYLE_NORMAL);


    if(!font) {
        LV_LOG_ERROR("Freetype font create failed.");
        return;
    }

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&style, lv_color_hex(0xFF0000));
    lv_style_set_text_opa(&style, LV_OPA_100);
    lv_style_set_text_outline_stroke_opa(&style, LV_OPA_100);
    lv_style_set_text_outline_stroke_color(&style, lv_color_hex(0x00FF00));
    lv_style_set_text_outline_stroke_width(&style, border_width);

    /*Avoid overlapping issue when using letter outlines*/
    lv_style_set_text_letter_space(&style, border_width);
    lv_style_set_text_line_space(&style, border_width);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label,
                      "Hello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\n"
                      "I'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\nHello world\nI'm a font created with FreeType\n");

}

/**
 * Load a font with FreeType
 */
void lv_example_freetype_2(void)
{
    /*Create a font*/
    lv_font_t * font = lv_freetype_font_create(PATH_PREFIX "lvgl/examples/libs/freetype/Lato-Regular.ttf",
                                               LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                               400,
                                               LV_FREETYPE_FONT_STYLE_NORMAL);

    /* this font is created from a downscaled NotoColorEmoji to 34x32px
     * Subset containing only a single emoji was created using fonttools:
     * Command: fonttools subset NotoColorEmoji.ttf --text=😀 */
    lv_font_t * font_emoji = lv_freetype_font_create(PATH_PREFIX "lvgl/examples/libs/freetype/NotoColorEmoji-32.subset.ttf",
                                                     LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                     200,
                                                     LV_FREETYPE_FONT_STYLE_NORMAL);

    if(!font || !font_emoji) {
        LV_LOG_ERROR("freetype font create failed.");
        return;
    }

    font->fallback = font_emoji;

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\nI'm a font created with FreeType 😀");
    lv_obj_center(label);
}
#else

void lv_example_freetype_2_vector_font(uint32_t font_size, uint32_t border_width)
{
    /*TODO
     *fallback for online examples*/
    LV_UNUSED(font_size);
    LV_UNUSED(border_width);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FreeType is not installed");
    lv_obj_center(label);
}

void lv_example_freetype_2(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FreeType is not installed");
    lv_obj_center(label);
}

#endif
#endif
