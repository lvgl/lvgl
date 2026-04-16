#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_IMAGE

/**
 * @title Image recolor and rotation
 * @brief Tint an image blue and rotate it using style properties.
 *
 * A style sets a grey background, a blue border, `image_recolor` to
 * blue with `image_recolor_opa = 50%`, and `transform_rotation = 300`
 * (30 degrees). The style is applied to an `lv_image` whose source is
 * `img_cogwheel_argb`, then the image is centered on the active screen.
 */
void lv_example_style_6(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    /*Set a background color and a radius*/
    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_set_image_recolor(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_image_recolor_opa(&style, LV_OPA_50);
    lv_style_set_transform_rotation(&style, 300);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_image_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);

    LV_IMAGE_DECLARE(img_cogwheel_argb);
    lv_image_set_src(obj, &img_cogwheel_argb);

    lv_obj_center(obj);
}

#endif
