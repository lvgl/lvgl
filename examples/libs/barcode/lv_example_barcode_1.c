#include "../../lv_examples.h"
#if LV_USE_BARCODE && LV_BUILD_EXAMPLES

/**
 * @title Barcode with palette colors
 * @brief Render a barcode encoding an LVGL URL with custom dark and light colors.
 *
 * A barcode widget is centered on the active screen with its height set to 50 px.
 * `lv_barcode_set_dark_color` and `lv_barcode_set_light_color` use darkened and
 * lightened entries from `LV_PALETTE_BLUE` and `LV_PALETTE_LIGHT_BLUE` for the
 * bars and background, a matching border color is applied, and
 * `lv_barcode_update` encodes `https://lvgl.io`.
 */
void lv_example_barcode_1(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * barcode = lv_barcode_create(lv_screen_active());
    lv_obj_set_height(barcode, 50);
    lv_obj_center(barcode);

    /*Set color*/
    lv_barcode_set_dark_color(barcode, fg_color);
    lv_barcode_set_light_color(barcode, bg_color);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(barcode, bg_color, 0);

    /*Set data*/
    lv_barcode_update(barcode, "https://lvgl.io");
}

#endif
