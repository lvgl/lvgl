#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

LV_IMAGE_DECLARE(test_animimg001);
LV_IMAGE_DECLARE(test_animimg002);
LV_IMAGE_DECLARE(test_animimg003);

static const lv_image_dsc_t * anim_imgs[3] = {
    &test_animimg001,
    &test_animimg002,
    &test_animimg003,
};

static lv_obj_t * active_screen = NULL;
static lv_obj_t * animimg = NULL;

void setUp(void);
void tearDown(void);
void test_animimg_successful_create(void);
void test_animimg_set_src(void);
void test_animimg_set_src_reverse(void);
void test_animimg_get_src_count(void);
void test_animimg_set_duration(void);
void test_animimg_set_repeat_count_infinite(void);
void test_animimg_start(void);

void setUp(void)
{
    active_screen = lv_screen_active();
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
    lv_animimg_set_src(animimg, (const void **) anim_imgs, 3);

    TEST_ASSERT_NOT_NULL(animimg);
}

void test_animimg_set_src_reverse(void)
{
    lv_animimg_set_src_reverse(animimg, (const void **) anim_imgs, 3);

    TEST_ASSERT_NOT_NULL(animimg);
}

void test_animimg_get_src(void)
{
    lv_animimg_set_src(animimg, (const void **) anim_imgs, 3);

    const void ** actual_dsc = lv_animimg_get_src(animimg);

    TEST_ASSERT_NOT_NULL(actual_dsc);
    TEST_ASSERT_EQUAL_PTR(actual_dsc, anim_imgs);
}

void test_animimg_get_src_count(void)
{
    uint8_t expected_count = 3;

    lv_animimg_set_src(animimg, (const void **) anim_imgs, expected_count);

    uint8_t actual_count = lv_animimg_get_src_count(animimg);

    TEST_ASSERT_EQUAL_UINT8(actual_count, expected_count);
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

    uint32_t actual_count = lv_animimg_get_repeat_count(animimg);

    TEST_ASSERT_EQUAL_UINT32(actual_count, LV_ANIM_REPEAT_INFINITE);
}

void test_animimg_start(void)
{
    // for lv_animimg_start() to actually work,
    // we need to properly setup the widget beforehand
    lv_animimg_set_src(animimg, (const void **) anim_imgs, 3);
    lv_animimg_set_duration(animimg, 1000);
    lv_animimg_set_repeat_count(animimg, LV_ANIM_REPEAT_INFINITE);

    lv_animimg_start(animimg);

    TEST_ASSERT_NOT_NULL(animimg);
}

void test_animimg_property(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_property_t prop;
    lv_obj_t * obj = lv_animimg_create(lv_screen_active());

    prop.id = LV_PROPERTY_ANIMIMAGE_SRC;
    prop.arg1.ptr = anim_imgs;
    prop.arg2.num = 3;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_PTR(lv_obj_get_property(obj, LV_PROPERTY_ANIMIMAGE_SRC).ptr, anim_imgs);
    TEST_ASSERT_EQUAL_PTR(lv_animimg_get_src(obj), anim_imgs);
    TEST_ASSERT_EQUAL_INT(lv_animimg_get_src_count(obj), 3);
    TEST_ASSERT_EQUAL_INT(3, lv_obj_get_property(obj, LV_PROPERTY_ANIMIMAGE_SRC_COUNT).num);

    prop.id = LV_PROPERTY_ANIMIMAGE_DURATION;
    prop.num = 1000;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(1000, lv_animimg_get_duration(obj));
    TEST_ASSERT_EQUAL_INT(1000, lv_obj_get_property(obj, LV_PROPERTY_ANIMIMAGE_DURATION).num);

    prop.id = LV_PROPERTY_ANIMIMAGE_REPEAT_COUNT;
    prop.num = LV_ANIM_REPEAT_INFINITE;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(LV_ANIM_REPEAT_INFINITE, lv_animimg_get_repeat_count(obj));
    TEST_ASSERT_EQUAL_INT(LV_ANIM_REPEAT_INFINITE, lv_obj_get_property(obj, LV_PROPERTY_ANIMIMAGE_REPEAT_COUNT).num_u);
#endif
}

#endif
