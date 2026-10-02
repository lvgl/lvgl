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

/**
 * Regression test: lv_draw_layer() must always mark its source layer's
 * all_tasks_added, even at a zero/negative scale where nothing will
 * actually be drawn.
 *
 * Before the fix, lv_draw_layer() returned immediately for scale_x/scale_y
 * <= 0, before lv_draw_add_task() ever ran and before all_tasks_added was
 * touched. dsc->src is a layer the caller already created and rendered
 * *before* calling this function (see lv_refr.c), so skipping that flag
 * left it permanently unfinished: lv_draw_dispatch_layer() only notices a
 * child layer is done, and reaps the LV_DRAW_TASK_TYPE_LAYER task that's
 * waiting on it, once all_tasks_added is true and the child's own task
 * list is empty (src/draw/lv_draw.c). Miss that and both the lv_layer_t
 * and its draw buffer are orphaned - leaked - forever.
 *
 * This calls lv_draw_layer() directly rather than through a widget or
 * animation. That's deliberate: as of this writing, lv_refr.c's own
 * layer_get_area() already refuses to render an object whose transformed
 * area collapses to nothing at scale 0, so a plain widget can no longer
 * drive lv_draw_layer() itself down to a degenerate scale (confirmed by
 * tracing - every direct/animated/gradual attempt through the widget and
 * style system bails out before reaching this function at all). That
 * upstream guard is a good thing, but it doesn't make this function's own
 * contract optional: if that guard is ever loosened, or some other caller
 * (a custom widget, a future draw pipeline) is less careful, lv_draw_layer()
 * must still not orphan its source layer - and if no caller can ever reach
 * it with a degenerate scale, this guard is dead code and the fix should be
 * reverted rather than carried as an unreachable landmine.
 */
void test_draw_layer_scale_zero_marks_all_tasks_added(void)
{
    lv_area_t coords = {0, 0, 59, 74};

    lv_layer_t parent;
    lv_memzero(&parent, sizeof(parent));
    parent.opa = LV_OPA_COVER;
    parent._clip_area = coords;

    lv_layer_t layer_to_draw;
    lv_memzero(&layer_to_draw, sizeof(layer_to_draw));
    layer_to_draw.parent = &parent;

    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);
    dsc.src = &layer_to_draw;
    dsc.scale_x = 0;
    dsc.scale_y = 0;

    lv_draw_layer(&parent, &dsc, &coords);

    TEST_ASSERT_TRUE(layer_to_draw.all_tasks_added);
    TEST_ASSERT_NOT_NULL(parent.draw_task_head);

    /*Nothing should actually be drawn: the task's own clip area must be empty.*/
    lv_area_t dummy;
    TEST_ASSERT_FALSE(lv_area_intersect(&dummy, &parent.draw_task_head->clip_area, &coords));

    /*With all_tasks_added set and no pending tasks of its own, the child layer is
     *"ready" - lv_draw_dispatch_layer() must find and unblock the waiting task in
     *the parent rather than leaving it (and the child layer) stuck forever.*/
    lv_draw_dispatch_layer(NULL, &layer_to_draw);
    TEST_ASSERT_NOT_EQUAL(LV_DRAW_TASK_STATE_BLOCKED, parent.draw_task_head->state);

    /*Not lv_draw_cleanup_task(): for a TYPE_LAYER task that calls
     *lv_draw_layer_delete(dsc->src), which lv_free()s it - fine for a real,
     *heap-allocated child layer, but layer_to_draw here is a stack variable.*/
    lv_free(parent.draw_task_head);
}

/*Same as above, but with a negative scale rather than exactly zero.*/
void test_draw_layer_scale_negative_marks_all_tasks_added(void)
{
    lv_area_t coords = {0, 0, 59, 74};

    lv_layer_t parent;
    lv_memzero(&parent, sizeof(parent));
    parent.opa = LV_OPA_COVER;
    parent._clip_area = coords;

    lv_layer_t layer_to_draw;
    lv_memzero(&layer_to_draw, sizeof(layer_to_draw));
    layer_to_draw.parent = &parent;

    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);
    dsc.src = &layer_to_draw;
    dsc.scale_x = LV_SCALE_NONE;
    dsc.scale_y = -10;

    lv_draw_layer(&parent, &dsc, &coords);

    TEST_ASSERT_TRUE(layer_to_draw.all_tasks_added);

    lv_draw_dispatch_layer(NULL, &layer_to_draw);
    TEST_ASSERT_NOT_EQUAL(LV_DRAW_TASK_STATE_BLOCKED, parent.draw_task_head->state);

    lv_free(parent.draw_task_head);
}

#endif
