#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/*
 * Border edge cases that apply to every draw unit (SW, VG-Lite, NanoVG, ...).
 *
 * All cases are rendered side by side in one flex layout and compared with a single screenshot.
 * Each cell is 80 x 60, so the largest meaningful border width is min(80, 60) / 2 = 30.
 *
 * Rows:
 *  1. Full border, no radius: widths around the limit and far above it.
 *  2. Full border, radius 16: widths around the radius and around the limit.
 *  3. Full border, circular radius (and a radius larger than half the cell).
 *  4. Partial sides, no radius, with widths up to and above the cell size.
 *  5. Partial sides, radius 16, with widths below, equal to and above the radius.
 *  6. Partial sides, circular and small radius, widths around the radius.
 *  7. Single side with radius 16 and a width equal to or above the cell height.
 */

#define CELL_W      80
#define CELL_H      60

typedef struct {
    int32_t width;
    int32_t radius;
    lv_border_side_t side;
} border_case_t;

#define FULL    LV_BORDER_SIDE_FULL
#define TOP     LV_BORDER_SIDE_TOP
#define BOTTOM  LV_BORDER_SIDE_BOTTOM
#define LEFT    LV_BORDER_SIDE_LEFT
#define RIGHT   LV_BORDER_SIDE_RIGHT
#define CIRCLE  LV_RADIUS_CIRCLE
#define SIDES(a, b)     ((lv_border_side_t)((a) | (b)))
#define SIDES3(a, b, c) ((lv_border_side_t)((a) | (b) | (c)))

static const border_case_t cases[] = {
    /* 1. Full border, no radius */
    {1, 0, FULL}, {4, 0, FULL}, {14, 0, FULL}, {29, 0, FULL}, {30, 0, FULL}, {31, 0, FULL}, {45, 0, FULL}, {100, 0, FULL},
    /* 2. Full border, radius 16 */
    {1, 16, FULL}, {4, 16, FULL}, {15, 16, FULL}, {16, 16, FULL}, {17, 16, FULL}, {29, 16, FULL}, {30, 16, FULL}, {100, 16, FULL},
    /* 3. Full border, circular radius */
    {1, CIRCLE, FULL}, {4, CIRCLE, FULL}, {15, CIRCLE, FULL}, {29, CIRCLE, FULL}, {30, CIRCLE, FULL}, {31, CIRCLE, FULL}, {100, CIRCLE, FULL}, {6, 100, FULL},
    /* 4. Partial sides, no radius */
    {10, 0, TOP}, {35, 0, TOP}, {100, 0, TOP}, {20, 0, SIDES(TOP, BOTTOM)},
    {35, 0, SIDES(TOP, BOTTOM)}, {50, 0, SIDES(LEFT, RIGHT)}, {45, 0, SIDES(TOP, LEFT)}, {70, 0, SIDES(BOTTOM, RIGHT)},
    /* 5. Partial sides, radius 16 */
    {10, 16, TOP}, {16, 16, TOP}, {24, 16, TOP}, {24, 16, LEFT},
    {35, 16, SIDES(TOP, BOTTOM)}, {45, 16, SIDES(LEFT, RIGHT)}, {40, 16, SIDES(TOP, LEFT)}, {100, 16, SIDES(BOTTOM, RIGHT)},
    /* 6. Partial sides, circular and small radius */
    {6, CIRCLE, TOP}, {40, CIRCLE, TOP}, {20, CIRCLE, SIDES(LEFT, RIGHT)}, {8, 8, SIDES(TOP, RIGHT)},
    {9, 8, SIDES(BOTTOM, LEFT)}, {7, 8, SIDES3(TOP, LEFT, RIGHT)}, {31, 12, BOTTOM}, {100, 12, LEFT},
    /* 7. Single side, radius 16, width equal to or above the cell height */
    {60, 16, BOTTOM}, {100, 16, TOP},
};

void setUp(void)
{
    /* Function run before every test */
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(scr, 30, 0); /* 8 cells per row */
    lv_obj_set_style_pad_ver(scr, 5, 0);
    lv_obj_set_style_pad_gap(scr, 5, 0);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_draw_border_edge_cases(void)
{
    lv_obj_t * scr = lv_screen_active();

    for(uint32_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        const border_case_t * c = &cases[i];

        lv_obj_t * obj = lv_obj_create(scr);
        lv_obj_remove_style_all(obj);
        lv_obj_set_size(obj, CELL_W, CELL_H);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xdddddd), 0);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(obj, c->radius, 0);
        lv_obj_set_style_border_color(obj, lv_color_hex(0xff0000), 0);
        lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(obj, c->width, 0);
        lv_obj_set_style_border_side(obj, c->side, 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_border_edge_cases.png");
}

#endif /*LV_BUILD_TEST*/
