#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_layer_t * g_layer;
static lv_draw_buf_malloc_cb_t g_original_malloc_cb;
static uint32_t g_draw_cnt;
static uint32_t g_layer_memory_while_drawing;
static uint32_t g_tile_cnt;

static uint32_t layer_memory(void)
{
    return LV_GLOBAL_DEFAULT()->draw_info.used_memory_for_layers;
}

void setUp(void)
{
    /* Function run before every test */
    g_draw_cnt = 0;
    g_layer_memory_while_drawing = 0;
    g_tile_cnt = lv_display_get_tile_cnt(lv_display_get_default());
}

void tearDown(void)
{
    /* Function run after every test */
    if(g_original_malloc_cb) {
        lv_draw_buf_get_handlers()->buf_malloc_cb = g_original_malloc_cb;
        g_original_malloc_cb = NULL;
    }
    if(g_layer) {
        lv_draw_layer_delete(g_layer);
        g_layer = NULL;
    }
    lv_display_set_tile_cnt(lv_display_get_default(), g_tile_cnt);
    lv_obj_clean(lv_screen_active());
}

static void draw_main_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    g_draw_cnt++;
    g_layer_memory_while_drawing = LV_MAX(g_layer_memory_while_drawing, layer_memory());
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
 * allocation fails due to memory exhaustion, and that the rest of the
 * screen is still drawn.
 *
 * Strategy: replace buf_malloc_cb with a stub that always returns NULL,
 * create a rotated widget to trigger layer allocation, then call
 * lv_refr_now(). The layer can't be created, so that widget is skipped.
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

    /*A widget next to it that needs no layer*/
    lv_obj_t * sibling = lv_obj_create(lv_screen_active());
    lv_obj_set_size(sibling, 50, 50);
    lv_obj_add_event_cb(sibling, draw_main_cb, LV_EVENT_DRAW_MAIN, NULL);

    /*Hook buf_malloc_cb to always fail*/
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    g_original_malloc_cb = handlers->buf_malloc_cb;
    handlers->buf_malloc_cb = fake_buf_malloc_fail;

    /*This must return without deadlock: the layer isn't created and the widget is skipped*/
    lv_refr_now(NULL);

    /*The rest of the screen is still drawn, and no layer memory is left counted*/
    TEST_ASSERT_GREATER_THAN_UINT32(0, g_draw_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, layer_memory());
}

void test_draw_layer_display_and_tiles_allocate_no_layer_memory(void)
{
    lv_display_set_tile_cnt(lv_display_get_default(), 4);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_center(obj);
    lv_obj_add_event_cb(obj, draw_main_cb, LV_EVENT_DRAW_MAIN, NULL);

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    /*The display's layer and its tiles draw into the display's buffer*/
    TEST_ASSERT_GREATER_THAN_UINT32(0, g_draw_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, g_layer_memory_while_drawing);
    TEST_ASSERT_EQUAL_UINT32(0, layer_memory());
}

void test_draw_layer_create_allocates_the_buffer(void)
{
    lv_layer_t parent;
    lv_layer_init(&parent);
    parent.display = lv_display_get_default();

    lv_area_t area = {10, 20, 29, 34};
    g_layer = lv_draw_layer_create(&parent, LV_COLOR_FORMAT_ARGB8888, &area);
    TEST_ASSERT_NOT_NULL(g_layer);

    /*The buffer exists right away, covers the area, is cleared and is counted*/
    TEST_ASSERT_NOT_NULL(g_layer->draw_buf);
    TEST_ASSERT_TRUE(g_layer->buffer_owned);
    TEST_ASSERT_EQUAL_INT32(20, g_layer->draw_buf->header.w);
    TEST_ASSERT_EQUAL_INT32(15, g_layer->draw_buf->header.h);
    TEST_ASSERT_EQUAL_UINT32(15 * g_layer->draw_buf->header.stride, layer_memory());

    const uint8_t * row = g_layer->draw_buf->data;
    uint32_t i;
    for(i = 0; i < 20 * 4; i++) TEST_ASSERT_EQUAL_UINT8(0, row[i]);

    /*Deleting it gives the memory back*/
    lv_draw_layer_delete(g_layer);
    g_layer = NULL;
    TEST_ASSERT_EQUAL_UINT32(0, layer_memory());
}

#endif
