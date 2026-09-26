#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_no_residual_border_on_scale_down(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_center(obj);
    lv_obj_set_style_border_color(obj, lv_color_hex3(0xf00), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0x0f0), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_transform_1.png");

    int32_t pivot_x[] = {lv_pct(50), lv_pct(8),  lv_pct(0), lv_pct(106), lv_pct(-16)};
    int32_t pivot_y[] = {lv_pct(50), lv_pct(12), lv_pct(0), lv_pct(113), lv_pct(-27)};
    int32_t scale[] = {256 * 4,      412,        569,       288,         711};

    uint32_t i;
    for(i = 0; i < 5; i++) {
        lv_obj_set_style_transform_pivot_x(obj, pivot_x[i], 0);
        lv_obj_set_style_transform_pivot_y(obj, pivot_y[i], 0);
        lv_obj_set_style_transform_scale(obj, scale[i], 0);
        lv_refr_now(NULL);

        /*Should be the same as the original without any artifacts*/
        lv_obj_set_style_transform_scale(obj, 256, 0);
        TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_transform_1.png");
    }

}

void test_update_layer_type_on_state_change(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_transform_rotation(&style, 90);
    lv_style_set_transform_scale_x(&style, 100);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_center(obj);
    lv_obj_add_style(obj, &style, LV_STATE_CHECKED);
    lv_refr_now(NULL);

    lv_obj_add_state(obj, LV_STATE_CHECKED);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_transform_2.png");

}

/**
 * Regression test: lv_draw_sw_transform() must not crash at a zero (or
 * negative) scale.
 *
 * transform_point_upscaled() maps destination pixels back into source
 * space by dividing by draw_dsc->scale_x/scale_y. Before the fix that
 * division had no zero-guard, so a scale of exactly 0 crashed with a
 * divide-by-zero the instant a fold/scale-down animation finished.
 *
 * Higher-level callers are expected to skip drawing entirely at a
 * non-positive scale (e.g. lv_draw_layer() forces an empty clip area
 * instead of ever reaching this function - see test_draw_layer_scale_zero_no_leak
 * in test_draw_layer.c), but this exercises the guard directly so the
 * function is safe regardless of caller, and confirms it's a true no-op
 * (the destination buffer is left untouched).
 */
void test_transform_zero_scale_no_div_by_zero(void)
{
    uint8_t src_buf[10 * 10 * 4];
    lv_memset(src_buf, 0xAA, sizeof(src_buf));

    uint8_t dest_buf[10 * 10 * 4];
    uint8_t sentinel[10 * 10 * 4];
    lv_memset(dest_buf, 0x55, sizeof(dest_buf));
    lv_memset(sentinel, 0x55, sizeof(sentinel));

    lv_area_t dest_area = { 0, 0, 9, 9 };

    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);

    lv_draw_image_sup_t sup;
    lv_memzero(&sup, sizeof(sup));

    static const int32_t scale_x[] = { 0,   0, -5, 256 };
    static const int32_t scale_y[] = { 0, -20,  0,  -1 };
    uint32_t i;
    for(i = 0; i < sizeof(scale_x) / sizeof(scale_x[0]); i++) {
        dsc.scale_x = scale_x[i];
        dsc.scale_y = scale_y[i];

        /*Must not crash - that alone is the regression check.*/
        lv_draw_sw_transform(&dest_area, src_buf, 10, 10, 10 * 4, &dsc, &sup,
                             LV_COLOR_FORMAT_ARGB8888, dest_buf);

        /*And it must be a true no-op: nothing visible at this scale, so nothing should be drawn.*/
        TEST_ASSERT_EQUAL_UINT8_ARRAY(sentinel, dest_buf, sizeof(dest_buf));
    }
}

#endif
