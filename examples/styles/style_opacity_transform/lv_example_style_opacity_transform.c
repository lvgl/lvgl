/**
 * @file lv_example_style_opacity_transform.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Opacity and transform
 * @brief Fade and geometrically transform a Widget via style properties.
 *
 * Three identical buttons: the first is untouched, the second sets
 * `style_opa="128"` (50%), the third also rotates 15° with
 * `style_transform_rotation="150"` and scales to 1.25× with
 * `style_transform_scale_x/y="320"`. `opa` and the transforms render the
 * button to a layer first, so its label fades and transforms with it.
 */
void lv_example_style_opacity_transform(void)
{
    static lv_style_t style_btn;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_btn);
        lv_style_set_radius(&style_btn, 10);
        lv_style_set_bg_opa(&style_btn, (255 * 100 / 100));
        lv_style_set_bg_color(&style_btn, ACCENT);
        lv_style_set_pad_all(&style_btn, 12);
        lv_style_set_text_color(&style_btn, lv_color_hex(0xffffff));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 The third button shares the same style; only `opa` + the `transform_*` props change it. */
    lv_obj_t * lv_button_0 = lv_button_create(screen);
    lv_obj_set_width(lv_button_0, 130);
    lv_obj_add_style(lv_button_0, &style_btn, 0);
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "Normal");

    lv_obj_t * lv_button_1 = lv_button_create(screen);
    lv_obj_set_width(lv_button_1, 130);
    lv_obj_set_style_opa(lv_button_1, 128, 0);
    lv_obj_add_style(lv_button_1, &style_btn, 0);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Opa 50%");

    lv_obj_t * lv_button_2 = lv_button_create(screen);
    lv_obj_set_width(lv_button_2, 130);
    lv_obj_set_style_opa(lv_button_2, 128, 0);
    lv_obj_set_style_transform_rotation(lv_button_2, 110, 0);
    lv_obj_set_style_transform_pivot_x(lv_button_2, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(lv_button_2, lv_pct(50), 0);
    lv_obj_set_style_transform_scale_x(lv_button_2, 420, 0);
    lv_obj_add_style(lv_button_2, &style_btn, 0);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Transformed");
}
#endif
