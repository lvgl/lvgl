#if LV_BUILD_TEST
#include "../lvgl.h"

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

static void button_create(lv_obj_t * parent, const char * text, int32_t x, int32_t y)
{
    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, x, 1, LV_GRID_ALIGN_STRETCH, y, 1);

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

    button_create(cont_main, "Main 0,0", 0, 0);
    button_create(cont_main, "Main 3,3", 3, 3);
    button_create(cont_main, "Main 2,2", 2, 2);
    button_create(cont_sub, "Sub 0,0", 0, 0);
    button_create(cont_sub, "Sub 1,0", 1, 0);
    button_create(cont_sub, "Sub 2,0", 2, 0);
    button_create(cont_sub, "Sub 3,0", 3, 0);
    button_create(cont_sub, "Sub 1,1", 1, 1);
    button_create(cont_sub, "Sub 0,1", 0, 1);

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

    button_create(cont_main, "Main 0,0", 0, 0);
    button_create(cont_main, "Main 3,3", 3, 3);
    button_create(cont_main, "Main 2,2", 2, 2);
    button_create(cont_sub, "Sub 0,0", 0, 0);
    button_create(cont_sub, "Sub 0,1", 0, 1);
    button_create(cont_sub, "Sub 0,2", 0, 2);
    button_create(cont_sub, "Sub 0,3", 0, 3);
    button_create(cont_sub, "Sub 1,0", 1, 0);
    button_create(cont_sub, "Sub 1,1", 1, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("subgrid_col.png");
}

#endif
