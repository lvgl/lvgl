#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Outline with padding gap
 * @brief Wrap an object in a blue outline offset from its edge.
 *
 * A grey-filled style with `radius = 5` also sets `outline_width = 2`,
 * `outline_color` to blue, and `outline_pad = 8` so the outline sits 8
 * pixels outside the object's border box. The styled object is centered
 * on the active screen.
 */
void lv_example_style_4(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    /*Set a background color and a radius*/
    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_GREY, 1));

    /*Add outline*/
    lv_style_set_outline_width(&style, 2);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_outline_pad(&style, 8);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_center(obj);
}

#endif
