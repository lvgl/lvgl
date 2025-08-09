#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#if LV_USE_SNAPSHOT

#include "unity/unity.h"

#define NUM_SNAPSHOTS 10

void test_snapshot_should_not_leak_memory(void)
{
    uint32_t idx = 0;
    size_t initial_available_memory = 0;
    size_t final_available_memory = 0;
    lv_mem_monitor_t monitor;

    lv_draw_buf_t * snapshots[NUM_SNAPSHOTS] = {NULL};

    lv_mem_monitor(&monitor);
    initial_available_memory = monitor.free_size;

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        snapshots[idx] = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
        TEST_ASSERT_NOT_NULL(snapshots[idx]);
    }

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        lv_draw_buf_destroy(snapshots[idx]);
    }

    lv_mem_monitor(&monitor);
    final_available_memory = monitor.free_size;

    TEST_ASSERT_EQUAL(initial_available_memory, final_available_memory);
}

void test_snapshot_with_transform_should_not_leak_memory(void)
{
    uint32_t idx = 0;
    size_t initial_available_memory = 0;
    size_t final_available_memory = 0;
    lv_mem_monitor_t monitor;

    lv_draw_buf_t * snapshots[NUM_SNAPSHOTS] = {NULL};
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Wubba lubba dub dub!");
    lv_obj_set_style_transform_rotation(label, 450, 0);

    lv_mem_monitor(&monitor);
    initial_available_memory = monitor.free_size;

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        snapshots[idx] = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
        TEST_ASSERT_NOT_NULL(snapshots[idx]);
    }

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        lv_draw_buf_destroy(snapshots[idx]);
    }

    lv_mem_monitor(&monitor);
    final_available_memory = monitor.free_size;
    lv_obj_delete(label);

    TEST_ASSERT_EQUAL(initial_available_memory, final_available_memory);
}

void test_snapshot_take_snapshot_immediately_after_obj_create(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Wubba lubba dub dub!");

    lv_draw_buf_t * draw_dsc = lv_snapshot_take(label, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_t * img_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(img_obj, draw_dsc);

    lv_obj_delete(label);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_0.png");

    lv_obj_center(img_obj);
    lv_image_set_inner_align(img_obj, LV_IMAGE_ALIGN_CENTER);
    lv_image_set_rotation(img_obj, 450);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_1.png");

    lv_obj_delete(img_obj);
    lv_draw_buf_destroy(draw_dsc);
}

void test_snapshot_take_snapshot_with_transform(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Wubba lubba dub dub!");
    lv_obj_set_style_transform_rotation(label, 450, 0);

    lv_draw_buf_t * draw_dsc = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);

    lv_obj_delete(label);

    lv_obj_t * img_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(img_obj, draw_dsc);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_2.png");

    lv_obj_delete(img_obj);
    lv_draw_buf_destroy(draw_dsc);
}

void test_snapshot_take_snapshot_multiple_widgets(void)
{
    lv_obj_t * obj_1 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_1, 200, 200);
    lv_obj_set_style_border_width(obj_1, 0, 0);
    lv_obj_set_style_bg_color(obj_1, lv_color_hex3(0xff0), 0);
    lv_obj_center(obj_1);

    lv_obj_t * top_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(top_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(top_obj, 0, 0);
    lv_obj_set_style_radius(top_obj, 0, 0);
    lv_obj_set_style_bg_opa(top_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(top_obj, 0, 0);
    lv_obj_center(top_obj);

    lv_obj_t * obj_2 = lv_obj_create(top_obj);
    lv_obj_set_size(obj_2, 100, 100);
    lv_obj_set_style_border_width(obj_2, 0, 0);
    lv_obj_set_style_bg_color(obj_2, lv_color_hex3(0xf00), 0);
    lv_obj_center(obj_2);

    lv_obj_t * obj_3 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_3, 100, 100);
    lv_obj_set_style_border_width(obj_3, 0, 0);
    lv_obj_set_style_bg_color(obj_3, lv_color_hex3(0x00f), 0);
    lv_obj_align(obj_3, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * obj_4 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_4, 100, 100);
    lv_obj_set_style_border_width(obj_4, 0, 0);
    lv_obj_set_style_bg_color(obj_4, lv_color_hex3(0x0ff), 0);
    lv_obj_align(obj_4, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(obj_4, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t * obj_5 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_5, 100, 100);
    lv_obj_set_style_border_width(obj_5, 0, 0);
    lv_obj_set_style_bg_color(obj_5, lv_color_hex3(0xff0), 0);
    lv_obj_align(obj_5, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_opa(obj_5, LV_OPA_TRANSP, 0);

    lv_obj_t * obj_6 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_6, 100, 100);
    lv_obj_set_style_border_width(obj_6, 0, 0);
    lv_obj_set_style_bg_color(obj_6, lv_color_hex3(0xf0f), 0);
    lv_obj_align(obj_6, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_opa_layered(obj_6, LV_OPA_TRANSP, 0);

    lv_draw_buf_t * draw_buf = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, draw_buf);
    lv_obj_center(img);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_3.png");

    lv_obj_clean(lv_screen_active());
    lv_draw_buf_destroy(draw_buf);
}

#else /*LV_USE_SNAPSHOT*/

void test_snapshot_should_not_leak_memory(void)
{

}

void test_snapshot_with_transform_should_not_leak_memory(void)
{

}

void test_snapshot_take_snapshot_immediately_after_obj_create(void)
{

}

void test_snapshot_take_snapshot_with_transform(void)
{

}

#endif

#endif
