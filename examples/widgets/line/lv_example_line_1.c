#include "../../lv_examples.h"
#if LV_USE_LINE && LV_BUILD_EXAMPLES

/**
 * @title Styled polyline through five points
 * @brief Draw a rounded blue polyline through five fixed coordinates.
 *
 * A static array of five `lv_point_precise_t` coordinates defines the
 * polyline. An `lv_style_t` sets `line_width` to 8, `line_color` to
 * `lv_palette_main(LV_PALETTE_BLUE)`, and `line_rounded` to true.
 * `lv_line_create` and `lv_line_set_points` build the shape on the
 * active screen, the style is attached, and the line is centered.
 */
void lv_example_line_1(void)
{
    /*Create an array for the points of the line*/
    static lv_point_precise_t line_points[] = { {5, 5}, {70, 70}, {120, 10}, {180, 60}, {240, 10} };

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line, true);

    /*Create a line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create(lv_screen_active());
    lv_line_set_points(line1, line_points, 5);     /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    lv_obj_center(line1);
}

#endif
