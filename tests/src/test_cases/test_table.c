#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * scr = NULL;
static lv_obj_t * table = NULL;

void setUp(void)
{
    scr = lv_scr_act();
    table = lv_table_create(scr);
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_table_should_return_assigned_cell_value(void)
{
    uint16_t row = 0;
    uint16_t column = 0;
    const char *value = "LVGL";

    lv_table_set_cell_value(table, row, column, value);

    TEST_ASSERT_EQUAL_STRING(value, lv_table_get_cell_value(table, row, column));
}

void test_table_should_grow_columns_automatically_when_setting_formatted_cell_value(void)
{
    uint16_t original_column_count = lv_table_get_col_cnt(table);
    uint16_t expected_column_count = original_column_count + 1;

    lv_table_set_cell_value_fmt(table, 0, 1, "LVGL %s", "Rocks!");

    TEST_ASSERT_EQUAL_UINT16(expected_column_count, lv_table_get_col_cnt(table));
}

void test_table_should_identify_cell_with_ctrl(void)
{
    bool has_ctrl = false;

    has_ctrl = lv_table_has_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);

    TEST_ASSERT_FALSE(has_ctrl);

    lv_table_add_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    has_ctrl = lv_table_has_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    TEST_ASSERT_TRUE(has_ctrl);
}

void test_table_should_clear_selected_cell_ctrl(void)
{
    bool has_ctrl = false;

    lv_table_add_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    has_ctrl = lv_table_has_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    TEST_ASSERT_TRUE(has_ctrl);

    lv_table_clear_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    has_ctrl = lv_table_has_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    TEST_ASSERT_FALSE(has_ctrl);
}

void test_table_should_keep_not_selected_cell_ctrl(void)
{
    bool has_ctrl = false;

    lv_table_add_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT | LV_TABLE_CELL_CTRL_TEXT_CROP);

    lv_table_clear_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    has_ctrl = lv_table_has_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    TEST_ASSERT_FALSE(has_ctrl);

    has_ctrl = lv_table_has_cell_ctrl(table, 0, 0, LV_TABLE_CELL_CTRL_TEXT_CROP);
    TEST_ASSERT_TRUE(has_ctrl);
}

#endif
