#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static bool event_triggered = false;

void setUp(void)
{
    active_screen = lv_screen_active();
    event_triggered = false;
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

static lv_obj_t * create_gif_image(const char * src)
{
    lv_obj_t * gif = lv_gif_create(active_screen);
    lv_gif_set_src(gif, src);
    lv_obj_set_align(gif, LV_ALIGN_CENTER);

    bool opened = lv_gif_is_loaded(gif);
    TEST_ASSERT_TRUE(opened);

    return gif;
}

/* Common event handler for all the consecutive test cases. */
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_READY) {
        event_triggered = true;
    }
}

static void gif_pause_index(int32_t pause_index)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    int32_t frame_count = lv_gif_get_frame_count(gif);
    TEST_ASSERT_LESS_THAN(frame_count, pause_index);

    while(lv_gif_get_current_frame_index(gif) != pause_index) {
        lv_test_wait(10);
    }

    lv_gif_pause(gif);
}

void test_gif_decode_normal(void)
{
    gif_pause_index(1);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_0.png");
    lv_obj_clean(active_screen);

    gif_pause_index(7);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_1.png");
    lv_obj_clean(active_screen);

    gif_pause_index(13);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_2.png");
    lv_obj_clean(active_screen);
}

void test_gif_decode_ignore_invisible(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);
    lv_obj_set_align(gif, LV_ALIGN_CENTER);

    lv_gif_set_auto_pause_invisible(gif, true);

    lv_obj_add_flag(gif, LV_OBJ_FLAG_HIDDEN);

    lv_gif_set_src(gif, "A:src/test_assets/totoro_transparent.gif");

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_invisible.png");

    lv_gif_set_auto_pause_invisible(gif, false);

    lv_obj_remove_flag(gif, LV_OBJ_FLAG_HIDDEN);

    lv_gif_set_src(gif, "A:src/test_assets/totoro_transparent.gif");

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_visible.png");
}

void test_gif_get_size(void)
{
    uint16_t w = 0;
    uint16_t h = 0;
    bool res = lv_gif_get_size("A:src/test_assets/totoro_transparent.gif", &w, &h);
    TEST_ASSERT_TRUE(res);

    TEST_ASSERT_EQUAL(300, w);
    TEST_ASSERT_EQUAL(300, h);

    res = lv_gif_get_size("A:src/test_assets/test_img_lvgl_logo.png", &w, &h);
    TEST_ASSERT_FALSE(res);

    TEST_ASSERT_EQUAL(0, w);
    TEST_ASSERT_EQUAL(0, h);
}

void test_gif_set_get_loop_count(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_set_loop_count(gif, 2);

    TEST_ASSERT_EQUAL(2, lv_gif_get_loop_count(gif));
}

void test_gif_restart(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_restart(gif);

    TEST_ASSERT_EQUAL(0, lv_gif_get_current_frame_index(gif));
}

void test_gif_pause_and_resume(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_pause(gif);

    lv_test_wait(50);

    lv_gif_resume(gif);

    while(lv_gif_get_current_frame_index(gif) != 10) {
        lv_test_wait(10);
    }

    TEST_ASSERT_EQUAL(10, lv_gif_get_current_frame_index(gif));
}

void test_gif_blend_to_xrgb8888(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_XRGB8888);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_xrgb8888.png");
}

void test_gif_blend_to_argb8888(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_ARGB8888);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_argb8888.png");

    gif = create_gif_image("A:src/test_assets/totoro_no_transparency.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_ARGB8888);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_argb8888_no_transparency.png");

    gif = create_gif_image("A:src/test_assets/totoro_no_disposalmethod.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_ARGB8888);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_argb8888_no_disposalmethod.png");
}

void test_gif_blend_to_rgb565(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_RGB565);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_rgb565.png");

    gif = create_gif_image("A:src/test_assets/totoro_no_transparency.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_RGB565);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_rgb565_no_transparency.png");

    gif = create_gif_image("A:src/test_assets/totoro_no_disposalmethod.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_RGB565);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_rgb565_no_disposalmethod.png");
}

void test_gif_blend_to_rgb888(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_RGB888);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_rgb888.png");

    gif = create_gif_image("A:src/test_assets/totoro_no_transparency.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_RGB888);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_rgb888_no_transparency.png");

    gif = create_gif_image("A:src/test_assets/totoro_no_disposalmethod.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_RGB888);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_rgb888_no_disposalmethod.png");
}

void test_gif_blend_to_i8(void)
{
    lv_obj_t * gif = create_gif_image("A:src/test_assets/totoro_transparent.gif");

    lv_gif_set_color_format(gif, LV_COLOR_FORMAT_I8);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_i8.png");
}

void test_gif_loop_limit(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);
    lv_obj_add_event_cb(gif, event_handler, LV_EVENT_READY, NULL);
    lv_obj_set_align(gif, LV_ALIGN_CENTER);

    lv_gif_set_src(gif, "A:src/test_assets/totoro_one_frame.gif");

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_one_frame.png");

    TEST_ASSERT_TRUE(event_triggered);
}

void test_gif_loop_once(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);
    lv_obj_set_align(gif, LV_ALIGN_CENTER);

    lv_gif_set_src(gif, "A:src/test_assets/totoro_one_frame_loop_once.gif");

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_one_frame_loop_once.png");
}

void test_gif_src_type_variable(void)
{
    LV_IMAGE_DECLARE(test_img_bulb_gif);

    lv_obj_t * gif = lv_gif_create(active_screen);
    lv_gif_set_src(gif, &test_img_bulb_gif);

    bool opened = lv_gif_is_loaded(gif);
    TEST_ASSERT_TRUE(opened);
}

void test_gif_restart_not_loaded(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);

    lv_gif_restart(gif);

    TEST_ASSERT_EQUAL(-1, lv_gif_get_current_frame_index(gif));
}

void test_gif_resume_not_loaded(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);

    lv_gif_resume(gif);

    TEST_ASSERT_EQUAL(-1, lv_gif_get_current_frame_index(gif));
}

void test_gif_set_loop_count_not_loaded(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);

    lv_gif_set_loop_count(gif, 5);

    TEST_ASSERT_EQUAL(-1, lv_gif_get_current_frame_index(gif));
}

void test_gif_get_loop_count_not_loaded(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);

    TEST_ASSERT_EQUAL(-1, lv_gif_get_loop_count(gif));
}

void test_gif_get_frame_count_not_loaded(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);

    TEST_ASSERT_EQUAL(-1, lv_gif_get_frame_count(gif));
}

void test_gif_set_src_null(void)
{
    lv_obj_t * gif = lv_gif_create(active_screen);
    lv_obj_set_align(gif, LV_ALIGN_CENTER);

    lv_gif_set_src(gif, NULL);

    bool opened = lv_gif_is_loaded(gif);
    TEST_ASSERT_FALSE(opened);

    lv_gif_set_src(gif, "A:src/test_assets/totoro_transparent.gif");

    opened = lv_gif_is_loaded(gif);
    TEST_ASSERT_TRUE(opened);

    lv_gif_set_src(gif, NULL);

    opened = lv_gif_is_loaded(gif);
    TEST_ASSERT_FALSE(opened);
}

#endif
