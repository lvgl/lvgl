/**
 * @file lv_example_style_border.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Border
 * @brief Frame a card with a colored, partially transparent border.
 *
 * `border_width="4"` and `border_color` draw the frame and
 * `border_opa="60%"` lets the surface show through it, so the border reads
 * as a soft accent instead of a hard line. `radius="20"` rounds the corners
 * and the border follows them.
 */
void lv_example_style_border(void)
{
    static lv_style_t style_framed;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_framed);
        lv_style_set_radius(&style_framed, 20);
        lv_style_set_bg_opa(&style_framed, (255 * 100 / 100));
        lv_style_set_bg_color(&style_framed, lv_color_hex(0xffffff));
        lv_style_set_border_color(&style_framed, ACCENT);
        lv_style_set_border_width(&style_framed, 4);
        lv_style_set_border_opa(&style_framed, (255 * 60 / 100));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tune `border_width`/`border_opa`, or set `border_side` to `none` to drop the frame. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 210, 140);
    lv_obj_add_style(container, &style_framed, 0);
    lv_obj_t * label = lv_label_create(container);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Bordered card");
}
#endif
