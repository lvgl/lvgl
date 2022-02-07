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
    /* Newly created tables have 1 column and 1 row */
    uint16_t original_column_count = lv_table_get_col_cnt(table);
    TEST_ASSERT_EQUAL_UINT16(1, original_column_count);

    /* Table currently only has a cell at 0,0 (row, colum) */
    lv_table_set_cell_value_fmt(table, 0, 1, "LVGL %s", "Rocks!");

    /* Table now should have cells at 0,0 and 0,1, so 2 columns */
    uint16_t expected_column_count = original_column_count + 1;
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

/* We're using a newly created table */
void test_table_cell_value_should_return_empty_string_when_cell_is_empty(void)
{
    TEST_ASSERT_EQUAL_STRING("", lv_table_get_cell_value(table, 0, 0));
}

void test_table_row_height_should_increase_with_multiline_cell_value(void)
{
    lv_table_t * table_ptr = (lv_table_t *) table;
    const char * singleline_value = "LVGL";
    const char * multiline_value = "LVGL\nRocks";

    lv_table_set_cell_value(table, 0, 0, singleline_value);
    lv_coord_t singleline_row_height = table_ptr->row_h[0];

    lv_table_set_cell_value(table, 0, 0, multiline_value);
    lv_coord_t multiline_row_height = table_ptr->row_h[0];

    TEST_ASSERT_GREATER_THAN(singleline_row_height, multiline_row_height);
}

#endif
