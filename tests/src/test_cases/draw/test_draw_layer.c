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

/**
 * Test that rendering completes without deadlock when layer buffer
 * allocation fails due to memory exhaustion.
 *
 * Before the fix, lv_draw_layer_alloc_buf returning NULL left the draw
 * task in WAITING state forever, causing draw_buf_flush to spin in an
 * infinite loop. The fix sets the task to LV_DRAW_TASK_STATE_FAILED so
 * it gets cleaned up and rendering proceeds.
 *
 * Strategy: temporarily replace buf_malloc_cb with a stub that always
 * returns NULL, create a rotated widget to trigger layer allocation,
 * then call lv_refr_now(). If it returns, the fix works.
 */
static void * fake_buf_malloc_fail(size_t size, lv_color_format_t cf)
{
    LV_UNUSED(size);
    LV_UNUSED(cf);
    return NULL;
}

void test_draw_layer_alloc_failed_no_deadlock(void)
{
    /*Create a widget with transform_rotation to force LV_LAYER_TYPE_TRANSFORM path*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 200);
    lv_obj_center(obj);
    lv_obj_set_style_transform_rotation(obj, 450, 0); /*45 degrees*/
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0); /*Force layer even with matrix path*/

    /*Hook buf_malloc_cb to always fail*/
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    lv_draw_buf_malloc_cb_t original_malloc_cb = handlers->buf_malloc_cb;
    handlers->buf_malloc_cb = fake_buf_malloc_fail;

    /*This must return without deadlock - the FAILED state allows the
     *render pipeline to clean up the failed task and proceed.*/
    lv_refr_now(NULL);

    /*Restore original handler*/
    handlers->buf_malloc_cb = original_malloc_cb;
}

#endif
