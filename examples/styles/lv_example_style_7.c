#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_ARC

/**
 * @title Arc color and width
 * @brief Style the arc stroke with a red color and thin width.
 *
 * A style sets `arc_color` to the main red palette entry and
 * `arc_width = 4`, then applies it to an `lv_arc` centered on the
 * active screen so the background arc renders as a thin red ring.
 */
void lv_example_style_7(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_arc_color(&style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_arc_width(&style, 4);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_arc_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_center(obj);
}
#endif
