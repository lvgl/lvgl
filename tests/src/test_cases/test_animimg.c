#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

LV_IMG_DECLARE(animimg001)
LV_IMG_DECLARE(animimg002)
LV_IMG_DECLARE(animimg003)

static const lv_img_dsc_t * anim_imgs[3] = {
    &animimg001,
    &animimg002,
    &animimg003,
};

static lv_obj_t * active_screen = NULL;
static lv_obj_t * animimg = NULL;

void setUp(void);
void tearDown(void);
void test_animimg_successful_create(void);
void test_animimg_set_src(void);
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

void test_animimg_set_src(void)
{
    lv_animimg_set_src(animimg, (lv_img_dsc_t **) anim_imgs, 3);

    TEST_ASSERT_NOT_NULL(animimg);
}

void test_animimg_set_duration(void)
{
    lv_animimg_set_duration(animimg, 1000);

    TEST_ASSERT_NOT_NULL(animimg);
}

void test_animimg_set_repeat_count_infinite(void)
{
    lv_animimg_set_repeat_count(animimg, LV_ANIM_REPEAT_INFINITE);

    TEST_ASSERT_NOT_NULL(animimg);
}

void test_animimg_start(void)
{
    // for lv_animimg_start() to actually work,
    // we need to properly setup the widget beforehand
    lv_animimg_set_src(animimg, (lv_img_dsc_t **) anim_imgs, 3);
    lv_animimg_set_duration(animimg, 1000);
    lv_animimg_set_repeat_count(animimg, LV_ANIM_REPEAT_INFINITE);

    lv_animimg_start(animimg);

    TEST_ASSERT_NOT_NULL(animimg);
}

#endif

