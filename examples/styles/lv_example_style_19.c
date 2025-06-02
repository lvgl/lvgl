#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SLIDER

/**
 * Test between a full background modal and a recolor modal
 */
void lv_example_style_19(void)
{
    /*Add lv_example_style_11 as background*/
    lv_example_style_11();

    /* Set to 1 to enable recolor overlay instead of solid background */
#if 0
    /* Apply a screen-wide tint using recolor (efficient overlay).
     * This modifies the visual appearance by blending a semi-transparent color
     * over existing content without creating additional objects.
     * It’s lighter on performance compared to a full-size background object. */
    lv_obj_set_style_recolor(lv_screen_active(), lv_color_black(), 0);
    lv_obj_set_style_recolor_opa(lv_screen_active(), LV_OPA_50, 0);
#else
    /* Simulate a modal background by setting a semi-transparent black background
     * on lv_layer_top(), the highest built-in layer.
     * This method creates a new full-screen object and can consume more resources
     * compared to recolor, especially when using images or gradients. */
    lv_obj_set_style_bg_color(lv_layer_top(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
#endif

    lv_obj_t * obj = lv_slider_create(lv_layer_top());
    lv_obj_center(obj);

    lv_refr_now(NULL); /*Update layouts and render*/

    lv_obj_invalidate(lv_screen_active());

    uint32_t t = lv_tick_get();
    lv_refr_now(NULL); /*Render only*/
    LV_LOG_USER("%" LV_PRIu32 " ms\n", lv_tick_elaps(t));
}

#endif
