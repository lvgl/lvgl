#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"
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

void test_recolor_1(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 150, 150);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_recolor(obj, lv_color_black(), 0);
    lv_obj_set_style_recolor_opa(obj, LV_OPA_0, 0);

    lv_obj_t * obj1 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj1, 150, 150);
    lv_obj_set_style_border_width(obj1, 0, 0);
    lv_obj_set_style_bg_color(obj1, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(obj1, LV_OPA_50, 0);

    lv_obj_t * obj2 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj2, 150, 150);
    lv_obj_set_style_border_width(obj2, 0, 0);
    lv_obj_set_style_recolor(obj2, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(obj2, LV_OPA_50, 0);

    lv_obj_t * obj3 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj3, 150, 150);
    lv_obj_set_style_border_width(obj3, 0, 0);
    lv_obj_set_style_bg_color(obj3, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(obj3, LV_OPA_50, 0);
    lv_obj_set_style_recolor(obj3, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(obj3, LV_OPA_50, 0);

    lv_obj_t * obj4 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj4, 150, 150);
    lv_obj_set_style_border_width(obj4, 0, 0);
    lv_obj_set_style_recolor(obj4, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(obj4, LV_OPA_50, 0);

    lv_obj_t * obj5 = lv_obj_create(obj4);
    lv_obj_set_size(obj5, 50, 50);
    lv_obj_set_style_border_width(obj5, 0, 0);
    lv_obj_set_style_bg_color(obj5, lv_color_hex(0xff00), 0);
    lv_obj_set_style_bg_opa(obj5, LV_OPA_100, 0);
    lv_obj_align(obj5, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t * obj6 = lv_obj_create(obj4);
    lv_obj_set_size(obj6, 50, 50);
    lv_obj_set_style_border_width(obj6, 0, 0);
    lv_obj_set_style_bg_color(obj6, lv_color_hex(0xff00), 0);
    lv_obj_set_style_bg_opa(obj6, LV_OPA_50, 0);
    lv_obj_align(obj6, LV_ALIGN_RIGHT_MID, 0, 0);

    LV_IMAGE_DECLARE(img_demo_widgets_avatar);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &img_demo_widgets_avatar);

    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &img_demo_widgets_avatar);
    lv_obj_set_style_image_recolor(img1, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_image_recolor_opa(img1, LV_OPA_50, 0);

    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    lv_image_set_src(img2, &img_demo_widgets_avatar);
    lv_obj_set_style_recolor(img2, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(img2, LV_OPA_50, 0);

    lv_obj_t * img3 = lv_image_create(lv_screen_active());
    lv_image_set_src(img3, &img_demo_widgets_avatar);
    lv_obj_set_style_image_recolor(img3, lv_color_hex(0xff00), 0);
    lv_obj_set_style_image_recolor_opa(img3, LV_OPA_50, 0);
    lv_obj_set_style_recolor(img3, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(img3, LV_OPA_50, 0);

    lv_obj_t * lable = lv_label_create(lv_screen_active());
    lv_label_set_text(lable, "Hello World Hello World");

    lv_obj_t * lable1 = lv_label_create(lv_screen_active());
    lv_label_set_text(lable1, "Hello World Hello World");
    lv_obj_set_style_text_color(lable1, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_text_opa(lable1, LV_OPA_50, 0);

    lv_obj_t * lable2 = lv_label_create(lv_screen_active());
    lv_label_set_text(lable2, "Hello World Hello World");
    lv_obj_set_style_recolor(lable2, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(lable2, LV_OPA_50, 0);

    lv_obj_t * lable3 = lv_label_create(lv_screen_active());
    lv_label_set_text(lable3, "Hello World Hello World");
    lv_obj_set_style_recolor(lable3, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(lable3, LV_OPA_50, 0);
    lv_obj_set_style_text_color(lable3, lv_color_hex(0xff00), 0);
    lv_obj_set_style_text_opa(lable3, LV_OPA_50, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("recolor_1.png");
}

#endif
