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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Swap dark_color/light_color for brand colors; keep enough contrast or scanners fail. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "QR code: colors");

    lv_obj_t * lv_qrcode_0 = lv_qrcode_create(screen);
    lv_qrcode_set_size(lv_qrcode_0, 150);
    lv_qrcode_set_dark_color(lv_qrcode_0, QR_DARK);
    lv_qrcode_set_light_color(lv_qrcode_0, QR_LIGHT);
    lv_qrcode_set_data(lv_qrcode_0, "https://lvgl.io");
    lv_qrcode_set_quiet_zone(lv_qrcode_0, true);
    lv_obj_set_style_border_color(lv_qrcode_0, QR_DARK, 0);
    lv_obj_set_style_border_width(lv_qrcode_0, 4, 0);
}
#endif
