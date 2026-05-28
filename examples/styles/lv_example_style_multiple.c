/**
 * @file lv_example_style_multiple.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define BASE lv_color_hex(0x6366f1)

#define WARN lv_color_hex(0xf59e0b)

/**
 * @title Cascading styles
 * @brief Stack a second style that overrides only a few properties of a shared base.
 *
 * `style_base` defines the whole card — fill, border, shadow, radius,
 * padding, text color. `style_warning` sets only `bg_color`,
 * `border_color`, and `text_color`. The right card has both: the later
 * style wins for the properties it sets, the base shows through for the
 * rest, so one small style re-themes the card without redefining it.
 */
void lv_example_style_multiple(void)
{
    static lv_style_t style_base;
    static lv_style_t style_warning;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_base);
        lv_style_set_radius(&style_base, 14);
        lv_style_set_bg_opa(&style_base, (255 * 100 / 100));
        lv_style_set_bg_color(&style_base, BASE);
        lv_style_set_border_width(&style_base, 2);
        lv_style_set_border_color(&style_base, lv_color_hex(0x4338ca));
        lv_style_set_shadow_color(&style_base, lv_color_hex(0x312e81));
        lv_style_set_shadow_width(&style_base, 14);
        lv_style_set_shadow_offset_y(&style_base, 5);
        lv_style_set_shadow_opa(&style_base, 120);
        lv_style_set_pad_all(&style_base, 16);
        lv_style_set_text_color(&style_base, lv_color_hex(0xffffff));

        lv_style_init(&style_warning);
        lv_style_set_bg_color(&style_warning, WARN);
        lv_style_set_border_color(&style_warning, lv_color_hex(0xb45309));
        lv_style_set_text_color(&style_warning, lv_color_hex(0x1f2937));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 18, 0);

    /* 💡 Both cards share `style_base`; the right one adds `style_warning`, which only repaints color. */
    lv_obj_t * container_1 = lv_obj_create(screen);
    lv_obj_set_size(container_1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(container_1, 20, 0);
    lv_obj_set_style_bg_opa(container_1, 0, 0);
    lv_obj_set_style_border_width(container_1, 0, 0);
    lv_obj_set_style_pad_all(container_1, 0, 0);
    lv_obj_t * container_2 = lv_obj_create(container_1);
    lv_obj_set_size(container_2, 110, LV_SIZE_CONTENT);
    lv_obj_add_style(container_2, &style_base, 0);
    lv_obj_t * label_1 = lv_label_create(container_2);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Base");

    lv_obj_t * container_3 = lv_obj_create(container_1);
    lv_obj_set_size(container_3, 110, LV_SIZE_CONTENT);
    lv_obj_add_style(container_3, &style_base, 0);
    lv_obj_add_style(container_3, &style_warning, 0);
    lv_obj_t * label_2 = lv_label_create(container_3);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Warning");
}
#endif
