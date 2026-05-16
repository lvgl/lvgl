#include "../../lv_examples.h"
#if LV_USE_QRCODE && LV_BUILD_EXAMPLES
#include <string.h>

/**
 * @title QR code with palette colors
 * @brief Render a 150 px QR code encoding an LVGL URL with custom dark and light colors.
 *
 * A QR code widget is centered on the active screen and sized to 150 px.
 * `lv_qrcode_set_dark_color` and `lv_qrcode_set_light_color` use
 * darkened and lightened entries from `LV_PALETTE_BLUE` and
 * `LV_PALETTE_LIGHT_BLUE`, `lv_qrcode_update` encodes `https://lvgl.io`,
 * and a matching 5 px border is applied via local style properties.
 */
void lv_example_qrcode_1(void)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(lv_screen_active());
    lv_qrcode_set_size(qr, 150);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    /*Set data*/
    const char * data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);
}

#endif
