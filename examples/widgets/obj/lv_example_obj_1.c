#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Base objects with and without shadow
 * @brief Two base objects showing default styling next to a custom blue shadow.
 *
 * Two `lv_obj` base objects are placed on the active screen. The
 * first, sized 100 by 50, uses the default theme. The second keeps
 * the default size and picks up a shared `lv_style_t` with a blue
 * 10 px shadow spread by 5 px. Both are offset from center so the
 * shadow difference is visible side by side.
 */
void lv_example_obj_1(void)
{
    lv_obj_t * obj1;
    obj1 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj1, 100, 50);
    lv_obj_align(obj1, LV_ALIGN_CENTER, -60, -30);

    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow, 10);
    lv_style_set_shadow_spread(&style_shadow, 5);
    lv_style_set_shadow_color(&style_shadow, lv_palette_main(LV_PALETTE_BLUE));

    lv_obj_t * obj2;
    obj2 = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj2, &style_shadow, 0);
    lv_obj_align(obj2, LV_ALIGN_CENTER, 60, 30);
}
#endif
