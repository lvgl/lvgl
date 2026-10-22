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


void test_grid_no_crash_on_invalid_settings(void)
{
    /*Should't crash because of these*/

    /*No col/row descriptors on screen*/
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_screen_load(scr);
    lv_obj_set_style_layout(scr, LV_LAYOUT_GRID, 0);
    lv_refr_now(NULL);

    /*No col/row descriptors on a widget*/
    lv_obj_t * cont = lv_obj_create(scr);
    lv_obj_set_style_layout(cont, LV_LAYOUT_GRID, 0);
    lv_refr_now(NULL);

    /*Set a cell without having row/col descriptor on the parent*/
    lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_refr_now(NULL);

    /*Add grid descriptors*/
    const int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    const int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_refr_now(NULL);

    lv_obj_t * label = lv_label_create(cont);

    /*Zero span*/
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 0, 0, LV_GRID_ALIGN_CENTER, 0, 0);
    lv_refr_now(NULL);

    /*Too large span*/
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 0, 20, LV_GRID_ALIGN_CENTER, 0, 30);
    lv_refr_now(NULL);

    /*Negative span*/
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 0, -50, LV_GRID_ALIGN_CENTER, 0, -20);
    lv_refr_now(NULL);

    /*Too large position*/
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 30, 1, LV_GRID_ALIGN_CENTER, 20, 1);
    lv_refr_now(NULL);

    /*Negative position*/
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, -100, 1, LV_GRID_ALIGN_CENTER, -20, 1);
    lv_refr_now(NULL);

    /*Valid settings*/
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 1, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_refr_now(NULL);
    TEST_PASS();
}



/**
 * An item that spans several CONTENT tracks has to fit in them together, so the tracks grow
 * to make room for it. Before this the item was dropped from the track sizing entirely and a
 * CONTENT track only ever saw the items that sit in exactly one track.
 */
void test_grid_content_track_fits_a_spanning_item(void)
{
    static int32_t col_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_column(cont, 10, 0);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    /*One item in the first column only, so that column is 40 wide*/
    lv_obj_t * narrow = lv_obj_create(cont);
    lv_obj_set_size(narrow, 40, 20);
    lv_obj_set_grid_cell(narrow, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    /*And one across both columns, which no column is wide enough for*/
    lv_obj_t * wide = lv_obj_create(cont);
    lv_obj_set_size(wide, 200, 20);
    lv_obj_set_grid_cell(wide, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    /*Shows where the second column starts*/
    lv_obj_t * second_col = lv_obj_create(cont);
    lv_obj_set_size(second_col, 5, 20);
    lv_obj_set_grid_cell(second_col, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_update_layout(cont);

    /*The two columns and the gap make room for the wide item*/
    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_width(cont));

    /*Without the spanning item the columns would be 40 and 5 with a 10 gap, so 55. The
     *missing 145 is shared evenly between the two CONTENT columns, 72 and 73, which makes
     *the first column 112 and starts the second one after it and the gap.*/
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_x(narrow));
    TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_x(wide));
    TEST_ASSERT_EQUAL_INT32(112 + 10, lv_obj_get_x(second_col));

    /*It settles*/
    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_width(cont));
}

/*A spanning item that already fits changes nothing*/
void test_grid_content_track_ignores_a_fitting_spanning_item(void)
{
    static int32_t col_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_column(cont, 10, 0);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    lv_obj_t * a = lv_obj_create(cont);
    lv_obj_set_size(a, 100, 20);
    lv_obj_set_grid_cell(a, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_t * b = lv_obj_create(cont);
    lv_obj_set_size(b, 100, 20);
    lv_obj_set_grid_cell(b, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    /*100 + 10 + 100 = 210, and the spanning item needs only 50, so nothing grows*/
    lv_obj_t * small = lv_obj_create(cont);
    lv_obj_set_size(small, 50, 20);
    lv_obj_set_grid_cell(small, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL_INT32(210, lv_obj_get_width(cont));
}

/*The same down the rows*/
void test_grid_content_row_fits_a_spanning_item(void)
{
    static int32_t col_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_row(cont, 10, 0);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    lv_obj_t * shallow = lv_obj_create(cont);
    lv_obj_set_size(shallow, 20, 40);
    lv_obj_set_grid_cell(shallow, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_t * tall = lv_obj_create(cont);
    lv_obj_set_size(tall, 20, 200);
    lv_obj_set_grid_cell(tall, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 2);

    /*Shows where the second row starts. Asserting the container's height alone would not
     *test anything: it wraps the tall item's coordinates whatever the rows do.*/
    lv_obj_t * second_row = lv_obj_create(cont);
    lv_obj_set_size(second_row, 20, 5);
    lv_obj_set_grid_cell(second_row, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);

    lv_obj_update_layout(cont);

    /*40 + 5 + 10 gap = 55, so 145 is missing and the two rows take 72 and 73*/
    TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_height(cont));
    TEST_ASSERT_EQUAL_INT32(112 + 10, lv_obj_get_y(second_row));
}

/*A fixed track among the spanned ones counts, and only the CONTENT ones take the rest*/
void test_grid_spanning_item_over_a_fixed_track(void)
{
    static int32_t col_dsc[] = {50, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_column(cont, 0, 0);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    /*Spans the 50 px column and the CONTENT one*/
    lv_obj_t * wide = lv_obj_create(cont);
    lv_obj_set_size(wide, 120, 20);
    lv_obj_set_grid_cell(wide, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    /*Aligned to the end of the second column, so its x shows how wide that column is*/
    lv_obj_t * probe = lv_obj_create(cont);
    lv_obj_set_size(probe, 5, 20);
    lv_obj_set_grid_cell(probe, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_update_layout(cont);

    /*The fixed column keeps its 50. Only the CONTENT column grows, from 5 to 70, so the
     *probe's right edge is at 120.*/
    TEST_ASSERT_EQUAL_INT32(120, lv_obj_get_width(cont));
    TEST_ASSERT_EQUAL_INT32(120 - 5, lv_obj_get_x(probe));
}

/*A grid with one 200 x `row` cell, and a STRETCHed item in it with the given min or max sizes*/
static lv_obj_t * stretch_item_create(int32_t y, int32_t cont_w, int32_t row,
                                      lv_style_prop_t w_prop, int32_t w, lv_style_prop_t h_prop, int32_t h)
{
    static int32_t col_dsc[] = {200, LV_GRID_TEMPLATE_LAST};
    /*One template per call, because the grid keeps a pointer to it*/
    static int32_t row_dscs[6][2];
    static uint32_t row_dsc_cnt;
    int32_t * row_dsc = row_dscs[row_dsc_cnt++ % 6];
    row_dsc[0] = row;
    row_dsc[1] = LV_GRID_TEMPLATE_LAST;

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cont, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
    lv_obj_set_pos(cont, 10, y);
    lv_obj_set_size(cont, cont_w, row == LV_GRID_CONTENT ? LV_SIZE_CONTENT : row);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    lv_obj_t * item = lv_obj_create(cont);
    lv_obj_set_grid_cell(item, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_pad_all(item, 0, 0);
    lv_style_value_t v;
    v.num = w;
    lv_obj_set_local_style_prop(item, w_prop, v, 0);
    v.num = h;
    lv_obj_set_local_style_prop(item, h_prop, v, 0);
    return item;
}

/*A STRETCHed grid item is clamped by its min and max size and stays at the start of the cell.
 *A percentage is of the container's content size, and is dropped when the container's size
 *comes from its content, like everywhere else.*/
void test_grid_stretch_min_max(void)
{
    /*An earlier test leaves a grid layout on the active screen*/
    lv_obj_set_layout(lv_screen_active(), LV_LAYOUT_NONE);

    lv_obj_t * max_px = stretch_item_create(10, 400, 60, LV_STYLE_MAX_WIDTH, 150, LV_STYLE_MAX_HEIGHT, 40);
    lv_obj_t * min_px = stretch_item_create(80, 400, 60, LV_STYLE_MIN_WIDTH, 250, LV_STYLE_MIN_HEIGHT, 70);
    lv_obj_t * max_pct = stretch_item_create(160, 400, 60, LV_STYLE_MAX_WIDTH, LV_PCT(25), LV_STYLE_MAX_HEIGHT, 30);
    lv_obj_t * min_pct = stretch_item_create(230, 400, 60, LV_STYLE_MIN_WIDTH, LV_PCT(75), LV_STYLE_MIN_HEIGHT, 50);
    lv_obj_t * pct_in_content = stretch_item_create(300, LV_SIZE_CONTENT, 60,
                                                    LV_STYLE_MAX_WIDTH, LV_PCT(25), LV_STYLE_MIN_WIDTH, LV_PCT(75));
    lv_obj_t * content_row = stretch_item_create(370, 400, LV_GRID_CONTENT, LV_STYLE_MAX_WIDTH, 150, LV_STYLE_MIN_HEIGHT,
                                                 50);

    for(uint32_t i = 0; i < 2; i++) {
        lv_obj_mark_layout_as_dirty(max_px);
        lv_obj_update_layout(lv_screen_active());

        TEST_ASSERT_EQUAL_INT32(150, lv_obj_get_width(max_px));
        TEST_ASSERT_EQUAL_INT32(40, lv_obj_get_height(max_px));
        TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_x(max_px));
        TEST_ASSERT_EQUAL_INT32(0, lv_obj_get_y(max_px));

        /*Larger than the cell, so it overflows it*/
        TEST_ASSERT_EQUAL_INT32(250, lv_obj_get_width(min_px));
        TEST_ASSERT_EQUAL_INT32(70, lv_obj_get_height(min_px));

        /*25% and 75% of the 400 px container*/
        TEST_ASSERT_EQUAL_INT32(100, lv_obj_get_width(max_pct));
        TEST_ASSERT_EQUAL_INT32(300, lv_obj_get_width(min_pct));

        /*The container is as wide as its 200 px column, so the percentages are dropped*/
        TEST_ASSERT_EQUAL_INT32(200, lv_obj_get_width(pct_in_content));

        /*The min height makes the CONTENT row as tall as the item*/
        TEST_ASSERT_EQUAL_INT32(150, lv_obj_get_width(content_row));
        TEST_ASSERT_EQUAL_INT32(50, lv_obj_get_height(content_row));
        TEST_ASSERT_EQUAL_INT32(50, lv_obj_get_height(lv_obj_get_parent(content_row)));
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("grid_stretch_min_max.png");
}

#endif
