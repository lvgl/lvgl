#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

lv_obj_t * obj_create(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
    lv_obj_set_width(obj, 150);
    lv_obj_set_height(obj, 150);

    return obj;
}

void obj_set_margin(lv_obj_t * obj, lv_coord_t left, lv_coord_t top, lv_coord_t right, lv_coord_t bottom)
{
    lv_obj_set_style_margin_left(obj, left, LV_PART_MAIN);
    lv_obj_set_style_margin_top(obj, top, LV_PART_MAIN);
    lv_obj_set_style_margin_right(obj, right, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(obj, bottom, LV_PART_MAIN);
}

// takes no effect on postiton but size.
void test_align(void)
{
    lv_obj_t * obj0 = active_screen;

    lv_obj_set_width(obj0, 800);
    lv_obj_set_height(obj0, 400);
    lv_obj_center(obj0);
    lv_obj_set_style_pad_all(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj0, 0, LV_PART_MAIN);

    lv_obj_t * obj0s[] = {
        obj_create(obj0, lv_palette_main(LV_PALETTE_BLUE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_RED)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_DEEP_PURPLE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_GREEN)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_PINK))
    };

    lv_obj_align(obj0s[0], LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align_to(obj0s[1], obj0s[0], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_align_to(obj0s[2], obj0s[1], LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_align_to(obj0s[3], obj0s[2], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_update_layout(obj0);

    obj_set_margin(obj0s[0], 20, 50, 60, 50);
    lv_obj_set_style_width(obj0s[0], LV_PCT(60), LV_PART_MAIN);
    obj_set_margin(obj0s[1], 15, 12, 50, 20);
    obj_set_margin(obj0s[2], 25, 25, 100, 20);
    obj_set_margin(obj0s[3], 12, 50, 100, 100);
    lv_obj_set_style_width(obj0s[3], LV_PCT(100), LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_align_0.png");
    obj_set_margin(obj0s[4], 6, 100, 100, 100);


    lv_obj_set_style_radius(obj0s[0], 0, LV_PART_MAIN);

    lv_obj_align(obj0s[4], LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_update_layout(obj0);

    TEST_ASSERT_EQUAL_SCREENSHOT("margin_align_1.png");
}

void test_flex(void)
{
#define N 5
    lv_obj_t * obj0 = active_screen;

    lv_obj_set_flex_flow(obj0, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj0, 0, LV_PART_MAIN);

    lv_obj_t * obj0s[N] = {
        obj_create(obj0, lv_palette_main(LV_PALETTE_BLUE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_RED)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_PURPLE)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_GREEN)),
        obj_create(obj0, lv_palette_main(LV_PALETTE_PINK))
    };

    lv_obj_t * o;
    for(int i = 0; i < N; i++) {
        o = obj0s[i];
        lv_obj_set_style_radius(o, 0, LV_PART_MAIN);
        lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_border_width(o, 0, LV_PART_MAIN);
    }

    lv_obj_set_width(obj0s[0], LV_PCT(50));
    lv_obj_set_height(obj0s[0], LV_PCT(50));

    obj_set_margin(obj0s[0], 50, 3, 10, 50);
    obj_set_margin(obj0s[1], 25, 6, 20, 50);
    obj_set_margin(obj0s[2], 12, 12, 30, 50);
    obj_set_margin(obj0s[3], 12, 25, 30, 50);
    obj_set_margin(obj0s[4], 24, 50, 50, 50);

    lv_obj_update_layout(obj0);

    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_0.png");

    lv_obj_set_flex_flow(obj0, LV_FLEX_FLOW_ROW_WRAP);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_1.png");

    lv_obj_set_style_flex_main_place(obj0, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_2.png");

    lv_obj_set_style_flex_cross_place(obj0, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_3.png");

    lv_obj_set_style_flex_track_place(obj0, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_4.png");

    lv_obj_set_flex_flow(obj0, LV_FLEX_FLOW_COLUMN_WRAP);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_flex_5.png");

#undef N
}

void test_grid(void)
{
#define M 5
#define N 8

    lv_obj_t * obj0 = active_screen;

    static lv_coord_t row_dsc[M + 1];
    static lv_coord_t col_dsc[N + 1];

    row_dsc[M] = LV_GRID_TEMPLATE_LAST;
    col_dsc[N] = LV_GRID_TEMPLATE_LAST;
    for(int i = 0; i < M; ++i) {
        row_dsc[i] = 80;
    }
    for(int i = 0; i < N; ++i) {
        col_dsc[i] = 80;
    }

    static lv_palette_t colors[] = {
        LV_PALETTE_BLUE,
        LV_PALETTE_PURPLE,
        LV_PALETTE_GREEN,
        LV_PALETTE_PINK,
        LV_PALETTE_AMBER,
        LV_PALETTE_BLUE_GREY,
        LV_PALETTE_INDIGO
    };

    lv_obj_set_layout(obj0, LV_LAYOUT_GRID);
    lv_obj_set_grid_align(obj0, LV_GRID_ALIGN_CENTER, LV_GRID_ALIGN_CENTER);
    lv_obj_set_grid_dsc_array(obj0, col_dsc, row_dsc);

    lv_obj_set_style_pad_gap(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(obj0, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(obj0, 0, LV_PART_MAIN);

    lv_obj_t * obj0s[M][N];

    for(int i = 0; i < M; ++i) {
        for(int j = 0; j < N; ++j) {
            obj0s[i][j] = obj_create(obj0,
                                     lv_palette_main(colors[(i * M + j) % ((sizeof colors) / (sizeof(lv_palette_t)))]));
        }
    }

    lv_obj_t * o;
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < N; ++j) {
            o = obj0s[i][j];
            lv_obj_set_style_radius(o, 0, LV_PART_MAIN);
            lv_obj_set_style_border_width(o, 0, LV_PART_MAIN);
            lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_width(o, 30);

            lv_obj_set_grid_cell(o, LV_GRID_ALIGN_CENTER, j, 1, LV_GRID_ALIGN_STRETCH, i, 1);

            lv_obj_set_style_margin_top(o, i * 5, LV_PART_MAIN);
            lv_obj_set_style_margin_left(o, i * 5 + j * 5, LV_PART_MAIN);
            lv_obj_set_style_margin_right(o, i * 5, LV_PART_MAIN);
            lv_obj_set_style_margin_bottom(o, i * 5 + j * 5, LV_PART_MAIN);
        }
    }

    lv_obj_update_layout(obj0);
    TEST_ASSERT_EQUAL_SCREENSHOT("margin_grid_0.png");

#undef M
#undef N
}
#endif
