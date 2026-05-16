#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SLIDER && LV_USE_LOG

/**
 * @title Modal overlay timing
 * @brief Compare a full-screen dim layer against a recolor overlay for modal dialogs.
 *
 * The scene from `lv_example_style_12` is reused as the background,
 * then a modal overlay is drawn over it. By default a semi-transparent
 * black background is set on `lv_layer_top()`; toggling the `#if 0`
 * branch switches to `lv_obj_set_style_recolor` on the active screen
 * instead. A slider is added to `lv_layer_top()` and centered, then
 * `lv_refr_now` and `lv_tick_elaps` print the render cost of the chosen
 * approach through `LV_LOG_USER`.
 */
void lv_example_style_20(void)
{
    /*Add lv_example_style_12 as background*/
    lv_example_style_12();

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
