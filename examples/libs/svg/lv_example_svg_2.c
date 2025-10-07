#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_SVG && LV_USE_VECTOR_GRAPHIC

/**
 * Load an SVG from a file
 */
void lv_example_svg_2(void)
{
    lv_obj_t * svg = lv_image_create(lv_screen_active());
    lv_image_set_src(svg, "A:lvgl/examples/assets/circle.svg");
}
#else

void lv_example_svg_2(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "SVG is not enabled");
    lv_obj_center(label);
}

#endif
#endif
