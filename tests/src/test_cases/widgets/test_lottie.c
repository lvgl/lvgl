#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"


static uint32_t buf[LV_TEST_WIDTH_TO_STRIDE(100, 4) * 100 + LV_DRAW_BUF_ALIGN];
extern const uint8_t test_lottie_approve[];
extern const size_t test_lottie_approve_size;

/*Due to different floating point precision
 *the rendered images are slightly different on different architectures
 *So compare the screenshots only on AMD64*/
#ifdef NON_AMD64_BUILD
    #undef TEST_ASSERT_EQUAL_SCREENSHOT
    #define TEST_ASSERT_EQUAL_SCREENSHOT(path) (void) path
#endif

void setUp(void)
{

}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_lottie_simple(void)
{
    lv_obj_t * lottie = lv_lottie_create(lv_screen_active());
    lv_lottie_set_buffer(lottie, 100, 100, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));
    lv_lottie_set_src_data(lottie, test_lottie_approve, test_lottie_approve_size);
    lv_obj_center(lottie);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_1.png");

    /*Wait a little*/
    lv_test_fast_forward(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2.png");

    /*Should be the last frame*/
    lv_test_fast_forward(750);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_3.png");

    /*Setting a source should reset the animation*/
    lv_lottie_set_src_data(lottie, test_lottie_approve, test_lottie_approve_size);

    /*Should reset automatically*/
    lv_test_fast_forward(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2.png");
}

void test_lottie_load_from_file(void)
{
    lv_obj_t * lottie = lv_lottie_create(lv_screen_active());
    lv_lottie_set_buffer(lottie, 100, 100, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));
    lv_lottie_set_src_file(lottie, "src/test_assets/test_lottie_approve.json");
    lv_obj_center(lottie);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_1.png");

    /*Wait a little*/
    lv_test_fast_forward(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2.png");

    /*Should be the last frame*/
    lv_test_fast_forward(750);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_3.png");

    /*Setting a source should reset the animation*/
    lv_lottie_set_src_data(lottie, test_lottie_approve, test_lottie_approve_size);

    /*Should reset automatically*/
    lv_test_fast_forward(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2.png");
}

void test_lottie_missing_settings(void)
{
    uint32_t tmp_buf[LV_TEST_WIDTH_TO_STRIDE(100, 4) * 100 + LV_DRAW_BUF_ALIGN];

    lv_obj_t * lottie1 = lv_lottie_create(lv_screen_active());
    lv_lottie_set_buffer(lottie1, 100, 100, lv_draw_buf_align(tmp_buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));

    /*Shouldn't crash without source*/
    lv_timer_handler();

    lv_obj_t * lottie2 = lv_lottie_create(lv_screen_active());
    /*Set the source first*/
    lv_lottie_set_src_data(lottie2, test_lottie_approve, test_lottie_approve_size);

    /*Shouldn't crash without buffer*/
    lv_timer_handler();

    lv_lottie_set_buffer(lottie2, 100, 100, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));

    lv_obj_center(lottie2);
    lv_test_fast_forward(950);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_3.png");
}

void test_lottie_rescale(void)
{
    lv_obj_t * lottie = lv_lottie_create(lv_screen_active());
    lv_lottie_set_buffer(lottie, 100, 100, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));
    lv_lottie_set_src_data(lottie, test_lottie_approve, test_lottie_approve_size);
    lv_obj_center(lottie);

    /*Wait a little*/
    lv_test_fast_forward(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2.png");

    lv_lottie_set_buffer(lottie, 50, 50, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2_small.png");

    /*Should be the last frame*/
    lv_test_fast_forward(750);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_3_small.png");
}


void test_lottie_non_uniform_shape(void)
{
    lv_obj_t * lottie = lv_lottie_create(lv_screen_active());
    lv_lottie_set_buffer(lottie, 50, 200, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));
    lv_lottie_set_src_data(lottie, test_lottie_approve, test_lottie_approve_size);
    lv_obj_center(lottie);

    lv_test_fast_forward(950);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_4.png");
}

void test_lottie_memory_leak(void)
{
    size_t mem_before = lv_test_get_free_mem();

    uint32_t i;
    for(i = 0; i < 32; i++) {
        lv_obj_t * lottie = lv_lottie_create(lv_screen_active());
        lv_lottie_set_buffer(lottie, 100, 100, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));
        lv_lottie_set_src_data(lottie, test_lottie_approve, test_lottie_approve_size);
        lv_obj_center(lottie);
        lv_test_fast_forward(753 * i); /*Render a random frame*/
        lv_timer_handler();
        lv_obj_delete(lottie);
    }
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 16);
}

void test_lottie_no_jump_when_visible_again(void)
{
    lv_obj_t * lottie = lv_lottie_create(lv_screen_active());
    lv_lottie_set_buffer(lottie, 100, 100, lv_draw_buf_align(buf, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED));
    lv_lottie_set_src_data(lottie, test_lottie_approve, test_lottie_approve_size);
    lv_obj_center(lottie);

    /*Wait a little*/
    lv_test_fast_forward(200);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2.png");

    lv_obj_add_flag(lottie, LV_OBJ_FLAG_HIDDEN);
    lv_test_fast_forward(300);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_1.png"); /*Empty screen*/

    /*Should be on the same frame*/
    lv_obj_clear_flag(lottie, LV_OBJ_FLAG_HIDDEN);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_2.png");

    lv_test_fast_forward(750);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/lottie_3.png");

}

#endif
