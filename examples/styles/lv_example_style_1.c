#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_IMAGE

/**
 * @title Size, position, and padding
 * @brief Style an object's width, height, coordinates, and padding, then add a child label.
 *
 * A single `lv_style_t` sets `radius`, `width` to 150, `height` to
 * `LV_SIZE_CONTENT`, vertical padding to 20, left padding to 5, and
 * positions the object at `x = 50%` of the parent with `y = 80`. The
 * style is applied to a base object on the active screen, and a label
 * with the text `Hello` is added as a child.
 */
void lv_example_style_1(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 5);

    /*Make a gradient*/
    lv_style_set_width(&style, 150);
    lv_style_set_height(&style, LV_SIZE_CONTENT);

    lv_style_set_pad_ver(&style, 20);
    lv_style_set_pad_left(&style, 5);

    lv_style_set_x(&style, lv_pct(50));
    lv_style_set_y(&style, 80);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label, "Hello");
}

#endif
