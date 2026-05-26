/**
 * @file lv_example_image_src.c
 */

#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

/**
 * @title Image source
 * @brief Display an image registered globally for the project.
 *
 * `lv_image` paints whatever is set as its `src`. In XML the value must
 * resolve through `lv_xml_get_image`, so `src` references a name that was
 * registered via `<file>` or `<data>` in `<images>` — here `img_example_lvgl_logo`
 * declared in `examples/xml_project/globals.xml`. The C API also accepts
 * a symbol string or a raw image variable pointer; those alternatives are
 * documented in this page but cannot appear in XML.
 */
void lv_example_image_src(void)
{
    LV_IMAGE_DECLARE(img_example_lvgl_logo);

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Register another image in `globals.xml` and swap `src` to its name to see a different bitmap. */
    lv_obj_t * image = lv_image_create(screen);
    lv_image_set_src(image, &img_example_lvgl_logo);
}
#endif
