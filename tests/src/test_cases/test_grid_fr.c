#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

/**
 * Tests that green/blue checker pattern of various row and column sizes
 * completely covers underlying screen, no red background visible.
 */
void test_grid_fr(void)
{
    static int32_t col_dsc[] = {3 * 9, LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_FR(3), LV_GRID_FR(4), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {3 * 3, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_set_style_bg_color(active_screen, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    lv_obj_set_layout(active_screen, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(active_screen, col_dsc, row_dsc);
    lv_obj_set_style_pad_row(active_screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(active_screen, 0, LV_PART_MAIN);

    for(int col = 0; col < 5; col++) {
        for(int row = 0; row < 5; row++) {
            lv_obj_t * child = lv_obj_create(active_screen);
            lv_obj_set_size(child, 1, 1);
            lv_obj_set_grid_cell(child, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
            lv_obj_set_style_bg_color(child, lv_palette_main((row + col) % 2 ? LV_PALETTE_GREEN : LV_PALETTE_BLUE), LV_PART_MAIN);
            lv_obj_set_style_radius(child, 0, LV_PART_MAIN);
            lv_obj_set_style_border_width(child, 0, LV_PART_MAIN);
        }
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("grid_fr.png");
}

#endif
