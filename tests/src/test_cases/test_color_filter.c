#if LV_BUILD_TEST || 1
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

static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}

static lv_color_t red(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_mix(lv_color_hex(0xff0000), color, opa);
}

static lv_color_t blue(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_mix(lv_color_hex(0xff), color, opa);
}

static lv_color_t green(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_mix(lv_color_hex(0xff00), color, opa);
}

void test_func_1(void)
{
    static lv_color_filter_dsc_t darken_dsc;
    lv_color_filter_dsc_init(&darken_dsc, darken);

    static lv_color_filter_dsc_t red_dsc;
    lv_color_filter_dsc_init(&red_dsc, red);

    static lv_color_filter_dsc_t blue_dsc;
    lv_color_filter_dsc_init(&blue_dsc, blue);

    static lv_color_filter_dsc_t green_dsc;
    lv_color_filter_dsc_init(&green_dsc, green);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_center(obj);
    lv_obj_set_style_color_filter_dsc(obj, &darken_dsc, 0);
    lv_obj_set_style_color_filter_opa(obj, LV_OPA_50, 0);

    lv_obj_t * obj1 = lv_obj_create(obj);
    lv_obj_set_size(obj1, 200, 200);
    lv_obj_align(obj1, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_style_color_filter_dsc(obj1, &red_dsc, 0);
    lv_obj_set_style_color_filter_opa(obj1, LV_OPA_100, 0);

    lv_obj_t * obj2 = lv_obj_create(obj1);
    lv_obj_set_size(obj2, 100, 100);
    lv_obj_align(obj2, LV_ALIGN_TOP_LEFT, 50, 50);
    lv_obj_set_style_color_filter_dsc(obj2, &blue_dsc, 0);
    lv_obj_set_style_color_filter_opa(obj2, LV_OPA_60, 0);

    lv_obj_t * obj3 = lv_obj_create(obj);
    lv_obj_set_size(obj3, 100, 100);
    lv_obj_align(obj3, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_set_style_border_width(obj3, 10, 0);
    lv_obj_set_style_border_color(obj3, lv_color_black(), 0);
    lv_obj_set_style_color_filter_dsc(obj3, &green_dsc, 0);
    lv_obj_set_style_color_filter_opa(obj3, LV_OPA_0, 0);

    lv_obj_t * obj4 = lv_obj_create(obj);
    lv_obj_set_size(obj4, 100, 100);
    lv_obj_align(obj4, LV_ALIGN_BOTTOM_RIGHT, -20, -50);
    lv_obj_set_style_outline_width(obj4, 10, 0);
    lv_obj_set_style_outline_color(obj4, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_color_filter_dsc(obj4, &green_dsc, 0);
    lv_obj_set_style_color_filter_opa(obj4, LV_OPA_40, 0);

    LV_IMAGE_DECLARE(img_demo_widgets_avatar);
    lv_obj_t * img1 = lv_image_create(obj);
    lv_image_set_src(img1, &img_demo_widgets_avatar);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t * img2 = lv_image_create(obj);
    LV_IMAGE_DECLARE(img_demo_widgets_avatar);
    lv_image_set_src(img2, &img_demo_widgets_avatar);
    lv_obj_set_style_image_recolor(img2, lv_color_white(), 0);
    lv_obj_set_style_image_recolor_opa(img2, LV_OPA_50, 0);
    lv_obj_align(img2, LV_ALIGN_BOTTOM_MID, 0, -10);

#if LV_BIN_DECODER_RAM_LOAD
    TEST_ASSERT_EQUAL_SCREENSHOT("color_filter.png");
#endif
}


#endif
