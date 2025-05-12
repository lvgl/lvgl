#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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


lv_obj_t * test_obj_create(const void * mask_src, int32_t scale, int32_t angle)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 200);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0xf88), 0);
    lv_obj_set_style_bitmap_mask_src(obj, mask_src, 0);
    lv_obj_set_style_transform_scale(obj, scale, 0);
    lv_obj_set_style_transform_rotation(obj, angle, 0);
    lv_obj_center(obj);

    lv_obj_t * label = lv_label_create(obj);
    lv_obj_set_width(label, lv_pct(100));
    lv_label_set_text(label,
                      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque suscipit risus nec pharetra pulvinar. In hac habitasse platea dictumst. Proin placerat congue massa eu luctus. Suspendisse risus nulla, consectetur eget odio ut, mollis sollicitudin magna. Suspendisse volutpat consequat laoreet. Aenean sodales suscipit leo, vitae pulvinar lorem pulvinar eu. Nullam molestie hendrerit est sit amet imperdiet.");
    lv_obj_center(label);

    return obj;
}


void test_draw_layer_bitmap_mask_from_variable(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    test_obj_create(&test_image_cogwheel_a8, LV_SCALE_NONE, 0);
    test_obj_create(&test_image_cogwheel_a8, 300, 0);
    test_obj_create(&test_image_cogwheel_a8, LV_SCALE_NONE, 200);
    test_obj_create(&test_image_cogwheel_a8, 80, 600);

#ifndef NON_AMD64_BUILD
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_layer_bitmap_mask.png");
#endif
}

void test_draw_layer_bitmap_mask_from_image(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    const char * mask_src = "A:src/test_files/binimages/cogwheel.A8.bin";
    test_obj_create(mask_src, LV_SCALE_NONE, 0);
    test_obj_create(mask_src, 300, 0);
    test_obj_create(mask_src, LV_SCALE_NONE, 200);
    test_obj_create(mask_src, 80, 600);

#ifndef NON_AMD64_BUILD
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_layer_bitmap_mask.png");
#endif
}


void test_draw_layer_bitmap_mask_from_image_not_found(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    const char * mask_src = "A:nonexistent/file";
    test_obj_create(mask_src, LV_SCALE_NONE, 0);
    test_obj_create(mask_src, 300, 0);
    test_obj_create(mask_src, LV_SCALE_NONE, 200);
    test_obj_create(mask_src, 80, 600);

#ifndef NON_AMD64_BUILD
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_layer_bitmap_mask_not_masked.png");
#endif
}

void test_draw_layer_dispatch(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);
    lv_obj_set_style_opa(obj, LV_OPA_0, 0);
    lv_obj_center(obj);

    /*Verify whether this scenario will result in a busy loop.*/
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_layer_dispatch.png");
}

#endif
