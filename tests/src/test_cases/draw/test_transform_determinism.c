#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* This test targets the SW transform's area-independent partial rendering.
 * With VG-Lite the image is drawn by the GPU which doesn't provide this
 * guarantee, so the whole test is skipped in that case. */
#if LV_USE_DRAW_VG_LITE

void setUp(void)
{
}

void tearDown(void)
{
}

void test_transform_partial_render_deterministic(void)
{
    TEST_PASS();
}

void test_transform_partial_render_deterministic_rgb565a8(void)
{
    TEST_PASS();
}

#else

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/**
 * Re-render only the middle of `img` and verify that the result
 * is bit-identical to the full render.
 * The transformation must not depend on the rendered area, else
 * partial rendering gives slightly different pixels (rounding errors).
 */
static void invalidate_middle_and_compare(lv_obj_t * img)
{
    lv_refr_now(NULL);

    lv_display_t * disp = lv_display_get_default();
    lv_draw_buf_t * front = lv_display_get_buf_active(disp);
    uint32_t size = front->header.stride * front->header.h;
    uint8_t * full_render = lv_malloc(size);
    TEST_ASSERT_NOT_NULL(full_render);
    lv_memcpy(full_render, front->data, size);

    /*Invalidate only the middle part of the image and render again*/
    lv_area_t area;
    lv_obj_get_coords(img, &area);
    int32_t w = lv_area_get_width(&area);
    int32_t h = lv_area_get_height(&area);
    area.x1 += w / 4;
    area.x2 -= w / 4;
    area.y1 += h / 4;
    area.y2 -= h / 4;
    lv_inv_area(disp, &area);
    lv_refr_now(NULL);

    TEST_ASSERT_EQUAL_MEMORY(full_render, front->data, size);
    lv_free(full_render);
}

void test_transform_partial_render_deterministic(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_image_cogwheel_argb8888);
    lv_obj_center(img);

    /*Rotation only*/
    lv_image_set_rotation(img, 300);
    invalidate_middle_and_compare(img);

    /*Scale only (also test the source edge clamping)*/
    lv_image_set_rotation(img, 0);
    lv_image_set_scale(img, 400);
    invalidate_middle_and_compare(img);

    /*Rotation and asymmetric scale*/
    lv_image_set_rotation(img, 1234);
    lv_image_set_scale_x(img, 180);
    lv_image_set_scale_y(img, 310);
    invalidate_middle_and_compare(img);
}

void test_transform_partial_render_deterministic_rgb565a8(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_image_cogwheel_rgb565a8);
    lv_obj_center(img);

    lv_image_set_rotation(img, 567);
    lv_image_set_scale(img, 333);
    invalidate_middle_and_compare(img);
}

#endif /*LV_USE_DRAW_VG_LITE*/

#endif
