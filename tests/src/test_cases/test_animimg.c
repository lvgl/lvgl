#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

LV_IMG_DECLARE(animimg001)
LV_IMG_DECLARE(animimg002)
LV_IMG_DECLARE(animimg003)

static lv_obj_t * active_screen = NULL;
static lv_obj_t * animimg = NULL;

void setUp(void);
void tearDown(void);
void test_animimg_successful_create(void);
void test_animimg_set_src(void);
void test_animimg_get_src_count(void);
void test_animimg_set_duration(void);
void test_animimg_set_repeat_count_infinite(void);
void test_animimg_start(void);

void setUp(void)
{
    active_screen = lv_scr_act();
    animimg = lv_animimg_create(active_screen);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_animimg_successful_create(void)
{
    TEST_ASSERT_NOT_NULL(animimg);
}

static lv_img_src_t * animimg_set_src()
{
    lv_img_src_t * anim_imgs[3] = {
        lv_img_src_from_raw(&animimg001),
        lv_img_src_from_raw(&animimg002),
        lv_img_src_from_raw(&animimg003),
    };
    lv_img_src_t * srcs = lv_img_src_from_srcs(anim_imgs, 3);
    lv_animimg_set_src(animimg, srcs);
    return srcs;
}

void test_animimg_set_src(void)
{
    (void)animimg_set_src();
    TEST_ASSERT_NOT_NULL(animimg);
}

void test_animimg_get_src(void)
{
    lv_img_src_t * srcs = animimg_set_src();
    lv_img_src_t * actual_srcs = lv_animimg_get_src(animimg);

    TEST_ASSERT_NOT_NULL(actual_srcs);
    TEST_ASSERT_EQUAL_PTR(actual_srcs, srcs);
}


void test_animimg_get_src_count(void)
{
    (void)animimg_set_src();
    uint8_t actual_count = lv_animimg_get_src_count(animimg);

    TEST_ASSERT_EQUAL_UINT8(actual_count, 3);
}

void test_animimg_set_duration(void)
{
    uint16_t expected_duration = 1000;

    lv_animimg_set_duration(animimg, expected_duration);

    uint16_t actual_duration = lv_animimg_get_duration(animimg);

    TEST_ASSERT_EQUAL_UINT16(actual_duration, expected_duration);
}

void test_animimg_set_repeat_count_infinite(void)
{
    lv_animimg_set_repeat_count(animimg, LV_ANIM_REPEAT_INFINITE);

    uint16_t actual_count = lv_animimg_get_repeat_count(animimg);

    TEST_ASSERT_EQUAL_UINT16(actual_count, LV_ANIM_REPEAT_INFINITE);
}

void test_animimg_start(void)
{
    // for lv_animimg_start() to actually work,
    // we need to properly setup the widget beforehand
    (void)animimg_set_src();
    lv_animimg_set_duration(animimg, 1000);
    lv_animimg_set_repeat_count(animimg, LV_ANIM_REPEAT_INFINITE);

    lv_animimg_start(animimg);

    TEST_ASSERT_NOT_NULL(animimg);
}

#endif

