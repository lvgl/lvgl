#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

static void create_gif_image(bool decode_invisible, int32_t pause_index)
{
    lv_obj_t * gif = lv_gif_create(active_screen);
    lv_gif_set_src(gif, "A:src/test_assets/totoro-transparent.gif");
    lv_obj_center(gif);

    lv_gif_set_auto_pause_invisible(gif, decode_invisible);

    bool opened = lv_gif_is_loaded(gif);
    TEST_ASSERT_TRUE(opened);

    while(lv_gif_get_current_frame_index(gif) != pause_index) {
        lv_test_fast_forward(5);
    }

    lv_gif_pause(gif);
}

void test_gif_decode_normal(void)
{
    create_gif_image(true, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_0.png");
    lv_obj_clean(active_screen);

    create_gif_image(true, 7);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_1.png");
    lv_obj_clean(active_screen);

    create_gif_image(true, 13);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_2.png");
    lv_obj_clean(active_screen);
}

void test_gif_decode_ignore_invisible(void)
{
    create_gif_image(false, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_0.png");
    lv_obj_clean(active_screen);

    create_gif_image(false, 7);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_1.png");
    lv_obj_clean(active_screen);

    create_gif_image(false, 13);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/gif_frame_2.png");
    lv_obj_clean(active_screen);
}

#endif
