/**
 * @file lv_example_qrcode_basic.c
 */

#include "../../../lv_examples.h"
#if LV_USE_QRCODE && LV_BUILD_EXAMPLES

#define QR_DARK lv_color_hex(0x1d4ed8)

#define QR_LIGHT lv_color_hex(0xdbeafe)

/**
 * @title QR code colors and quiet zone
 * @brief Encode a URL with custom dark/light colors and a padded quiet zone.
 *
 * The QR code is fixed at 150 px and recolored: a dark blue `dark_color` on a
 * light blue `light_color` instead of the default black-on-white. `quiet_zone`
 * adds the standard light margin so scanners lock on, and a matching border
 * frames it against the screen.
 */
void lv_example_qrcode_basic(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Swap dark_color/light_color for brand colors; keep enough contrast or scanners fail. */
    lv_obj_t * qrcode = lv_qrcode_create(screen);
    lv_qrcode_set_size(qrcode, 150);
    lv_qrcode_set_dark_color(qrcode, QR_DARK);
    lv_qrcode_set_light_color(qrcode, QR_LIGHT);
    lv_qrcode_set_data(qrcode, "https://lvgl.io");
    lv_qrcode_set_quiet_zone(qrcode, true);
    lv_obj_set_style_border_color(qrcode, QR_DARK, 0);
    lv_obj_set_style_border_width(qrcode, 4, 0);
}
#endif
