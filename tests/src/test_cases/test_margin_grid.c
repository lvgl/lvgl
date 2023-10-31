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

lv_obj_t * obj_create(lv_obj_t * parent, lv_color_t color)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
    lv_obj_set_width(obj, 150);
    lv_obj_set_height(obj, 150);

    return obj;
}

void obj_set_margin(lv_obj_t * obj, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    lv_obj_set_style_margin_left(obj, left, LV_PART_MAIN);
    lv_obj_set_style_margin_top(obj, top, LV_PART_MAIN);
    lv_obj_set_style_margin_right(obj, right, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(obj, bottom, LV_PART_MAIN);
}

void test_grid(void)
{
#define M 5
#define N 8

    lv_obj_t * obj0 = active_screen;

    static int32_t row_dsc[M + 1];
    static int32_t col_dsc[N + 1];

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
