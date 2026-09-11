/**
 * @file lv_example_style_shadow.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Box shadow
 * @brief Lift a card off the page with a soft, offset shadow.
 *
 * `shadow_width="30"` sets the blur and `shadow_color` tints it.
 * `shadow_offset_y="12"` drops the shadow below the card so it reads as
 * elevation, and `shadow_opa="80"` keeps it soft. With both offsets at 0
 * the same blur becomes an even glow instead.
 */
void lv_example_style_shadow(void)
{
    static lv_style_t style_elevated;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_elevated);
        lv_style_set_radius(&style_elevated, 20);
        lv_style_set_bg_opa(&style_elevated, (255 * 100 / 100));
        lv_style_set_bg_color(&style_elevated, lv_color_hex(0xffffff));
        lv_style_set_border_width(&style_elevated, 0);
        lv_style_set_shadow_color(&style_elevated, lv_color_hex(0x312e81));
        lv_style_set_shadow_width(&style_elevated, 30);
        lv_style_set_shadow_offset_y(&style_elevated, 12);
        lv_style_set_shadow_opa(&style_elevated, 80);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* 💡 Set `shadow_offset_x`/`shadow_offset_y` to 0 to turn the elevation into an even glow. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_align(container, LV_ALIGN_CENTER);
    lv_obj_set_size(container, 210, 130);
    lv_obj_add_style(container, &style_elevated, 0);
    lv_obj_t * label = lv_label_create(container);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Elevated");
}
#endif
