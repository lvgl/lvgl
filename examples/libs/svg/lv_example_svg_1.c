#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_SVG && LV_USE_VECTOR_GRAPHIC

/**
 * @title SVG from inline markup
 * @brief Render an SVG circle described by a literal string as an image source.
 *
 * A static `lv_image_dsc_t` is populated with `LV_IMAGE_HEADER_MAGIC`, a
 * 450x150 display size, and a pointer to the literal SVG markup drawing
 * a red circle with a blue stroke. The descriptor is handed to
 * `lv_image_set_src` on an image widget placed on the active screen so
 * the SVG decoder renders the markup on demand.
 */
void lv_example_svg_1(void)
{
    static const char svg_data[] = "<svg width=\"12cm\" height=\"4cm\" viewBox=\"0 0 1200 400\">"
                                   "<circle cx=\"600\" cy=\"200\" r=\"100\" fill=\"red\" stroke=\"blue\" stroke-width=\"10\"/></svg>";

    static lv_image_dsc_t svg_dsc;
    svg_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    svg_dsc.header.w = 450;
    svg_dsc.header.h = 150;
    svg_dsc.data_size = sizeof(svg_data) - 1;
    svg_dsc.data = (const uint8_t *) svg_data;

    lv_obj_t * svg = lv_image_create(lv_screen_active());
    lv_image_set_src(svg, &svg_dsc);
}
#else

void lv_example_svg_1(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "SVG is not enabled");
    lv_obj_center(label);
}

#endif
#endif
