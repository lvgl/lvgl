#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_clean(lv_screen_active());
}

void tearDown(void)
{
    /* Function run after every test */
}

static void button_create(lv_obj_t * parent, const char * text, int32_t x, int32_t x_span, int32_t y, int32_t y_span)
{
    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, x, x_span, LV_GRID_ALIGN_STRETCH, y, y_span);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);

}

void test_subgrid_row(void)
{

    const int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    const int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont_main = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont_main, 700, 300);
    lv_obj_center(cont_main);
    lv_obj_set_grid_dsc_array(cont_main, col_dsc, row_dsc);

    const int32_t col_dsc2[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_t * cont_sub = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_sub, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 1, 2);
    lv_obj_set_grid_dsc_array(cont_sub, col_dsc2, NULL);
    lv_obj_set_style_pad_all(cont_sub, 0, 0);

    button_create(cont_main, "Main 0,0", 0, 1, 0, 1);
    button_create(cont_main, "Main 3,3", 3, 1, 3, 1);
    button_create(cont_main, "Main 2,2", 2, 1, 2, 1);
    button_create(cont_sub, "Sub 0,0", 0, 1, 0, 1);
    button_create(cont_sub, "Sub 1,0", 1, 1, 0, 1);
    button_create(cont_sub, "Sub 2,0", 2, 1, 0, 1);
    button_create(cont_sub, "Sub 3,0", 3, 1, 0, 1);
    button_create(cont_sub, "Sub 1,1", 1, 1, 1, 1);
    button_create(cont_sub, "Sub 0,1", 0, 1, 1, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("subgrid_row.png");
}

void test_subgrid_col(void)
{

    const int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    const int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont_main = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont_main, 700, 300);
    lv_obj_center(cont_main);
    lv_obj_set_grid_dsc_array(cont_main, col_dsc, row_dsc);

    const int32_t row_dsc2[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_t * cont_sub = lv_obj_create(cont_main);
    lv_obj_set_grid_cell(cont_sub, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 1, 2);
    lv_obj_set_grid_dsc_array(cont_sub, NULL, row_dsc2);
    lv_obj_set_style_pad_all(cont_sub, 0, 0);

    button_create(cont_main, "Main 0,0", 0, 1, 0, 1);
    button_create(cont_main, "Main 3,3", 3, 1, 3, 1);
    button_create(cont_main, "Main 2,2", 2, 1, 2, 1);
    button_create(cont_sub, "Sub 0,0", 0, 1, 0, 1);
    button_create(cont_sub, "Sub 0,1", 0, 1, 1, 1);
    button_create(cont_sub, "Sub 0,2", 0, 1, 2, 1);
    button_create(cont_sub, "Sub 0,3", 0, 1, 3, 1);
    button_create(cont_sub, "Sub 1,0", 1, 1, 0, 1);
    button_create(cont_sub, "Sub 1,1", 1, 1, 1, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("subgrid_col.png");
}

void test_grid_ltr(void)
{
    const int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    const int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * grid = lv_obj_create(lv_screen_active());
    lv_obj_set_style_base_dir(grid, LV_BASE_DIR_LTR, LV_PART_MAIN);
    lv_obj_set_size(grid, lv_pct(100), lv_pct(100));
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);

    button_create(grid, "Button 1", 0, 2, 0, 1);
    button_create(grid, "Button 2", 2, 2, 0, 2);
    button_create(grid, "Btn 3", 0, 1, 1, 1);
    button_create(grid, "Btn 4", 1, 1, 1, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("grid_ltr.png");
}

void test_grid_rtl(void)
{
    const int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    const int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * grid = lv_obj_create(lv_screen_active());
    lv_obj_set_style_base_dir(grid, LV_BASE_DIR_RTL, LV_PART_MAIN);
    lv_obj_set_size(grid, lv_pct(100), lv_pct(100));
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);

    button_create(grid, "Button 1", 0, 2, 0, 1);
    button_create(grid, "Button 2", 2, 2, 0, 2);
    button_create(grid, "Btn 3", 0, 1, 1, 1);
    button_create(grid, "Btn 4", 1, 1, 1, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("grid_rtl.png");
}

#endif
