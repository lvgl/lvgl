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

void test_xml_canvas_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * canvas1_attrs[] = {
        "name", "c1",
        "x", "10",
        "y", "10",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_canvas", canvas1_attrs);

    lv_obj_t * c1 = lv_obj_find_by_name(NULL, "c1");
    TEST_ASSERT_NOT_NULL(c1);
    lv_draw_buf_t  * draw_buf = lv_draw_buf_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_STRIDE_AUTO);
    lv_canvas_set_draw_buf(c1, draw_buf);
    lv_canvas_fill_bg(c1, lv_color_hex3(0x234), LV_OPA_COVER);
    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_canvas.png");
    lv_draw_buf_destroy(draw_buf);
}

#endif
