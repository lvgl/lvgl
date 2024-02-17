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

void test_draw_layer_bitmap_mask(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_a8);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 200);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0xf88), 0);
    lv_obj_set_style_bitmap_mask_src(obj, &test_image_cogwheel_a8, 0);
    lv_obj_center(obj);

    lv_obj_t * label = lv_label_create(obj);
    lv_obj_set_width(label, lv_pct(100));
    lv_label_set_text(label,
                      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque suscipit risus nec pharetra pulvinar. In hac habitasse platea dictumst. Proin placerat congue massa eu luctus. Suspendisse risus nulla, consectetur eget odio ut, mollis sollicitudin magna. Suspendisse volutpat consequat laoreet. Aenean sodales suscipit leo, vitae pulvinar lorem pulvinar eu. Nullam molestie hendrerit est sit amet imperdiet.");
    lv_obj_center(label);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_layer_bitmap_mask.png");

}

#endif
