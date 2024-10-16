#include "../../lv_examples.h"
#if LV_USE_FREETYPE && LV_USE_FONT_MANAGER && LV_BUILD_EXAMPLES

#if LV_FREETYPE_USE_LVGL_PORT
    #define PATH_PREFIX "A:"
#else
    #define PATH_PREFIX "./"
#endif

static lv_font_manager_t * g_font_manager = NULL;

void lv_example_font_manager_1(void)
{
    /* Create font manager, with 8 fonts recycling buffers */
    g_font_manager = lv_font_manager_create(8);

    /* Add font path mapping to font manager */
    lv_font_manager_add_path_static(g_font_manager,
                                    "Lato-Regular",
                                    PATH_PREFIX "lvgl/examples/libs/freetype/Lato-Regular.ttf");

    /* Create font from font manager */
    lv_font_t * font = lv_font_manager_create_font(g_font_manager,
                                                   "Lato-Regular",
                                                   LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                   24,
                                                   LV_FREETYPE_FONT_STYLE_NORMAL);

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
    lv_label_set_text(label, "FreeType or font_manager is not enabled");
    lv_obj_center(label);
}

#endif
