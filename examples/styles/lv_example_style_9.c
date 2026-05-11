#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_LINE

/**
 * @title Line stroke style
 * @brief Style a polyline with a thick grey stroke and rounded end caps.
 *
 * A style sets `line_color` to the main grey palette entry,
 * `line_width = 6`, and `line_rounded = true`. The style is applied to
 * an `lv_line` whose point array is set with `lv_line_set_points`, and
 * the line is centered on the active screen.
 */
void lv_example_style_9(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_line_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_width(&style, 6);
    lv_style_set_line_rounded(&style, true);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_line_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);

    static lv_point_precise_t p[] = {{10, 30}, {30, 50}, {100, 0}};
    lv_line_set_points(obj, p, 3);

    lv_obj_center(obj);
}

#endif
