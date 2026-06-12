#include "../../lv_examples.h"
#if LV_USE_FONT_MANAGER && LV_USE_FREETYPE && LV_BUILD_EXAMPLES

#if LV_FREETYPE_USE_LVGL_PORT
    #define PATH_PREFIX "A:"
#else
    #define PATH_PREFIX "./"
#endif

static lv_font_manager_t * g_font_manager = NULL;

/**
 * @title Font manager with a FreeType source
 * @brief Register a TTF path with the font manager and render a label with it.
 *
 * `lv_font_manager_create(8)` builds a manager with an 8-slot recycling cache.
 * `lv_font_manager_add_src_static` maps the name `"Lato-Regular"` to
 * `Lato-Regular.ttf` through `lv_freetype_font_class`. `lv_font_manager_create_font`
 * then resolves that name at size 24 with `LV_FREETYPE_FONT_RENDER_MODE_BITMAP`,
 * `LV_FREETYPE_FONT_STYLE_NORMAL`, and `LV_FONT_KERNING_NONE`. The returned font
 * is applied as the text font of a centered label reading "Hello Font Manager!".
 */
void lv_example_font_manager_1(void)
{
    /* Create font manager, with 8 fonts recycling buffers */
    g_font_manager = lv_font_manager_create(8);

    /* Add font path mapping to font manager */
    lv_font_manager_add_src_static(g_font_manager,
                                   "Lato-Regular",
                                   PATH_PREFIX "lvgl/examples/libs/freetype/Lato-Regular.ttf",
                                   &lv_freetype_font_class);

    /* Create font from font manager */
    lv_font_t * font = lv_font_manager_create_font(g_font_manager,
                                                   "Lato-Regular",
                                                   LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                   24,
                                                   LV_FREETYPE_FONT_STYLE_NORMAL,
                                                   LV_FONT_KERNING_NONE);

    if(!font) {
        LV_LOG_ERROR("Could not create font");
        return;
    }

    /* Create label with the font */
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, font, 0);
    lv_label_set_text(label, "Hello Font Manager!");
    lv_obj_center(label);
}

#else

void lv_example_font_manager_1(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Font Manager or FreeType is not enabled");
    lv_obj_center(label);
}

#endif
