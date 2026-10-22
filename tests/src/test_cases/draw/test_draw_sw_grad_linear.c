#if LV_BUILD_TEST
#include "../lvgl.h"

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

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS && !LV_USE_DRAW_VG_LITE && !LV_USE_DRAW_NANOVG
static lv_obj_t * create_linear_grad_obj(lv_grad_dsc_t * grad, int32_t start_x, int32_t start_y,
                                         int32_t end_x, int32_t end_y)
{
    static const lv_color_t colors[2] = { LV_COLOR_MAKE(0xff, 0x00, 0x00), LV_COLOR_MAKE(0x00, 0x00, 0xff) };

    lv_grad_init_stops(grad, colors, NULL, NULL, 2);
    lv_grad_linear_init(grad, start_x, start_y, end_x, end_y, LV_GRAD_EXTEND_PAD);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 380, 220);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_grad(obj, grad, 0);
    return obj;
}
#endif

void test_draw_sw_grad_linear_far_start_point(void)
{
#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS && !LV_USE_DRAW_VG_LITE && !LV_USE_DRAW_NANOVG
    static lv_grad_dsc_t grads[4];

    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(scr, 10, 0);
    lv_obj_set_style_pad_gap(scr, 20, 0);

    /* Start point close to the top left corner */
    create_linear_grad_obj(&grads[0], 60, 60, 160, 110);

    /* Start point far from the corner, end point before it.
     * start * (end - start) doesn't fit in 16 bits, so the whole area was filled with the start color. */
    create_linear_grad_obj(&grads[1], 300, 150, 40, 60);

    /* Start point far from the corner, end point after it. The whole area was filled with the end color. */
    create_linear_grad_obj(&grads[2], 200, 100, 370, 150);

    /* The same with percentage coordinates */
    create_linear_grad_obj(&grads[3], lv_pct(90), lv_pct(80), lv_pct(10), lv_pct(20));

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/sw_grad_linear_far_start_point.png");
#else
    TEST_PASS();
#endif
}

#endif
