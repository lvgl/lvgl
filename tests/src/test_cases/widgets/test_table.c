#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * scr = NULL;
static lv_obj_t * table = NULL;

void setUp(void)
{
    scr = lv_screen_active();
    table = lv_table_create(scr);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_table_should_set_row_count_to_zero(void)
{
    lv_table_set_row_count(table, 0);

    TEST_ASSERT_EQUAL_UINT32(0, lv_table_get_row_count(table));
}

void test_table_should_return_assigned_cell_value(void)
{
    uint16_t row = 0;
    uint16_t column = 0;
    const char * value = "LVGL";

    lv_table_set_cell_value(table, row, column, value);

    TEST_ASSERT_EQUAL_STRING(value, lv_table_get_cell_value(table, row, column));
}

void test_table_should_grow_columns_automatically_when_setting_formatted_cell_value(void)
{
    /* Newly created tables have 1 column and 1 row */
    uint16_t original_column_count = lv_table_get_column_count(table);
    TEST_ASSERT_EQUAL_UINT16(1, original_column_count);

    /* Table currently only has a cell at 0,0 (row, column) */
    lv_table_set_cell_value_fmt(table, 0, 1, "LVGL %s", "Rocks!");

    /* Table now should have cells at 0,0 and 0,1, so 2 columns */
    uint16_t expected_column_count = original_column_count + 1;
    TEST_ASSERT_EQUAL_UINT16(expected_column_count, lv_table_get_column_count(table));
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
    int32_t singleline_row_height = table_ptr->row_h[0];

    lv_table_set_cell_value(table, 0, 0, multiline_value);
    int32_t multiline_row_height = table_ptr->row_h[0];

    TEST_ASSERT_GREATER_THAN(singleline_row_height, multiline_row_height);
}

void test_table_should_wrap_long_texts(void)
{
    lv_table_t * table_ptr = (lv_table_t *) table;
    const char * long_text = "Testing automatic text wrap with a very long text";
    const char * small_text = "Hi";

    lv_table_set_column_width(table, 0, 50);

    lv_table_set_cell_value(table, 0, 0, small_text);
    int32_t row_height = table_ptr->row_h[0];

    lv_table_set_cell_value(table, 0, 0, long_text);
    int32_t wrapped_row_height = table_ptr->row_h[0];

    /* Row height on cells with wrapped text is bigger than cells with small texts */
    TEST_ASSERT_GREATER_THAN(row_height, wrapped_row_height);
}

static void draw_part_event_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = draw_task->draw_dsc;
    /*If the cells are drawn...*/
    if(base_dsc->part == LV_PART_ITEMS) {
        uint32_t row = base_dsc->id1;
        uint32_t col = base_dsc->id2;

        /*Make the texts in the first cell center aligned*/
        if(row == 0) {
            lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if(label_draw_dsc) {
                label_draw_dsc->align = LV_TEXT_ALIGN_CENTER;
            }
            lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if(fill_draw_dsc) {
                fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), fill_draw_dsc->color, LV_OPA_20);
                fill_draw_dsc->opa = LV_OPA_COVER;
            }
        }
        /*In the first column align the texts to the right*/
        else if(col == 0) {
            lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if(label_draw_dsc) {
                label_draw_dsc->align = LV_TEXT_ALIGN_RIGHT;
            }
        }

        /*Make every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if(fill_draw_dsc) {
                fill_draw_dsc->color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), fill_draw_dsc->color, LV_OPA_10);
                fill_draw_dsc->opa = LV_OPA_COVER;
            }
        }
    }
}

void test_table_rendering(void)
{
    lv_obj_center(table);
    lv_obj_add_event_cb(table, draw_part_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_set_style_border_side(table, LV_BORDER_SIDE_FULL, LV_PART_ITEMS);
    lv_obj_set_style_pad_all(table, 10, LV_PART_ITEMS);
    lv_obj_set_style_border_width(table, 5, LV_PART_ITEMS);
    lv_table_set_column_count(table, 5);
    lv_table_set_row_count(table, 5);
    lv_table_set_column_width(table, 1, 60);
    lv_table_set_column_width(table, 2, 100);

    lv_table_add_cell_ctrl(table, 0, 1, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(table, 0, 1, "2 cells are merged");

    lv_table_add_cell_ctrl(table, 1, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_add_cell_ctrl(table, 1, 1, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_add_cell_ctrl(table, 1, 2, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_add_cell_ctrl(table, 1, 3, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(table, 1, 0, "5 cells are merged");

    uint32_t i;
    for(i = 0; i < 5; i++) {
        lv_table_set_cell_value_fmt(table, 3, i, "%d", i);
    }

    lv_table_set_cell_value_fmt(table, 2, 3, "Multi\nline text");
    lv_table_set_cell_value_fmt(table, 2, 4, "Very long text wrapped automatically");

    lv_table_add_cell_ctrl(table, 4, 3, LV_TABLE_CELL_CTRL_TEXT_CROP);
    lv_table_set_cell_value_fmt(table, 4, 3, "crop crop crop crop crop crop crop crop ");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/table_1.png");
}

/* See #3120 for context */
void test_table_should_reduce_cells(void)
{
    const uint16_t initial_col_num = 8;
    const uint16_t initial_row_num = 1;
    const uint16_t final_col_num = 4;
    const uint16_t final_row_num = 1;

    lv_obj_center(table);

    lv_table_set_column_count(table, initial_col_num);
    lv_table_set_row_count(table, initial_row_num);

    uint32_t row_idx, col_idx;
    for(row_idx = 0; row_idx < initial_row_num; row_idx++) {
        for(col_idx = 0; col_idx < initial_col_num; col_idx++) {
            lv_table_set_cell_value(table, row_idx, col_idx, "00");
        }
    }

    lv_table_set_column_count(table, final_col_num);
    lv_table_set_row_count(table, final_row_num);

    for(row_idx = 0; row_idx < final_row_num; row_idx++) {
        for(col_idx = 0; col_idx < final_col_num; col_idx++) {
            lv_table_set_cell_value(table, row_idx, col_idx, "00");
        }
    }
}

/* See #3120 for context */
void test_table_should_reduce_cells_with_more_than_one_row(void)
{
    const uint16_t initial_col_num = 8;
    const uint16_t initial_row_num = 2;
    const uint16_t final_col_num = 4;
    const uint16_t final_row_num = 1;

    lv_obj_center(table);

    lv_table_set_column_count(table, initial_col_num);
    lv_table_set_row_count(table, initial_row_num);

    uint32_t row_idx, col_idx;
    for(row_idx = 0; row_idx < initial_row_num; row_idx++) {
        for(col_idx = 0; col_idx < initial_col_num; col_idx++) {
            lv_table_set_cell_value(table, row_idx, col_idx, "00");
        }
    }

    lv_table_set_column_count(table, final_col_num);
    lv_table_set_row_count(table, final_row_num);

    for(row_idx = 0; row_idx < final_row_num; row_idx++) {
        for(col_idx = 0; col_idx < final_col_num; col_idx++) {
            lv_table_set_cell_value(table, row_idx, col_idx, "00");
        }
    }
}

void test_table_should_set_selected_cell(void)
{
    lv_table_set_row_count(table, 2);
    lv_table_set_column_count(table, 2);

    lv_table_set_selected_cell(table, 1, 1);

    uint32_t selected_row = 0;
    uint32_t selected_column = 0;

    lv_table_get_selected_cell(table, &selected_row, &selected_column);

    TEST_ASSERT_EQUAL_UINT32(1, selected_row);
    TEST_ASSERT_EQUAL_UINT32(1, selected_column);
}

void test_table_cell_select_should_not_exceed_table_bounds(void)
{
    lv_table_set_row_count(table, 2);
    lv_table_set_column_count(table, 2);

    lv_table_set_selected_cell(table, 2, 2);

    uint32_t selected_row = 0;
    uint32_t selected_column = 0;

    lv_table_get_selected_cell(table, &selected_row, &selected_column);

    TEST_ASSERT_EQUAL_UINT32(1, selected_row);
    TEST_ASSERT_EQUAL_UINT32(1, selected_column);
}

void test_table_cell_select_should_not_allow_set_on_table_with_no_rows(void)
{
    lv_table_set_row_count(table, 0);
    lv_table_set_column_count(table, 5);

    lv_table_set_selected_cell(table, 4, 4);

    uint32_t selected_row = 0;
    uint32_t selected_column = 0;

    lv_table_get_selected_cell(table, &selected_row, &selected_column);

    TEST_ASSERT_EQUAL_UINT32(0, selected_row);
    TEST_ASSERT_EQUAL_UINT32(0, selected_column);
}

#endif
