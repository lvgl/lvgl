/**
 * @file lv_example_dropdown_direction.c
 */

#include "../../lv_examples.h"
#if LV_USE_DROPDOWN && LV_BUILD_EXAMPLES

/**
 * @title Dropdown open direction
 * @brief Open the option list towards a specific edge.
 *
 * Four dropdowns sit on the four sides of the screen and use the dir attribute to choose
 * which way their option list pops. The default is bottom, but top, left, and right are
 * useful when a dropdown sits close to a screen edge and there is no room to expand
 * downward.
 */
void lv_example_dropdown_direction(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* Default: list opens downward */
    lv_obj_t * dropdown_1 = lv_dropdown_create(screen);
    lv_obj_set_align(dropdown_1, LV_ALIGN_TOP_MID);
    lv_obj_set_y(dropdown_1, 10);
    lv_dropdown_set_options(dropdown_1, "Apple\nBanana\nOrange\nMelon");

    /* List opens upward */
    lv_obj_t * dropdown_2 = lv_dropdown_create(screen);
    lv_obj_set_align(dropdown_2, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(dropdown_2, -10);
    lv_dropdown_set_dir(dropdown_2, LV_DIR_TOP);
    lv_dropdown_set_options(dropdown_2, "Apple\nBanana\nOrange\nMelon");

    /* List opens to the right */
    lv_obj_t * dropdown_3 = lv_dropdown_create(screen);
    lv_obj_set_align(dropdown_3, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(dropdown_3, 10);
    lv_obj_set_width(dropdown_3, 80);
    lv_dropdown_set_dir(dropdown_3, LV_DIR_RIGHT);
    lv_dropdown_set_options(dropdown_3, "Apple\nBanana\nOrange\nMelon");

    /* List opens to the left */
    lv_obj_t * dropdown_4 = lv_dropdown_create(screen);
    lv_obj_set_align(dropdown_4, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(dropdown_4, -10);
    lv_obj_set_width(dropdown_4, 80);
    lv_dropdown_set_dir(dropdown_4, LV_DIR_LEFT);
    lv_dropdown_set_options(dropdown_4, "Apple\nBanana\nOrange\nMelon");
}
#endif
