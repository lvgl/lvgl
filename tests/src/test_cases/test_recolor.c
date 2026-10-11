#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"
#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_color32_t top_layer_recolor;
static uint32_t top_layer_draw_count;

static void capture_layer_recolor_cb(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    top_layer_recolor = layer->recolor;
    top_layer_draw_count++;
}

void setUp(void)
{
    active_screen = lv_screen_active();
    top_layer_recolor = lv_color32_make(0, 0, 0, 0);
    top_layer_draw_count = 0;
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
    lv_obj_clean(lv_layer_top());
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

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello World Hello World");

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_text(label1, "Hello World Hello World");
    lv_obj_set_style_text_color(label1, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_text_opa(label1, LV_OPA_50, 0);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_text(label2, "Hello World Hello World");
    lv_obj_set_style_recolor(label2, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(label2, LV_OPA_50, 0);

    lv_obj_t * label3 = lv_label_create(lv_screen_active());
    lv_label_set_text(label3, "Hello World Hello World");
    lv_obj_set_style_recolor(label3, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(label3, LV_OPA_50, 0);
    lv_obj_set_style_text_color(label3, lv_color_hex(0xff00), 0);
    lv_obj_set_style_text_opa(label3, LV_OPA_50, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("recolor_1.png");
}

void test_recolor_does_not_leak_to_top_layer(void)
{
    lv_obj_t * parent = lv_obj_create(active_screen);
    lv_obj_set_pos(parent, 10, 10);
    lv_obj_set_size(parent, 40, 40);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_set_style_border_width(parent, 0, 0);
    lv_obj_set_style_radius(parent, 0, 0);
    lv_obj_set_style_recolor(parent, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_recolor_opa(parent, LV_OPA_COVER, 0);

    lv_obj_t * child = lv_obj_create(parent);
    lv_obj_set_size(child, 20, 20);
    lv_obj_set_style_border_width(child, 0, 0);
    lv_obj_set_style_radius(child, 0, 0);
    lv_obj_set_style_bg_opa(child, LV_OPA_COVER, 0);

    lv_obj_t * top_obj = lv_obj_create(lv_layer_top());
    lv_obj_set_pos(top_obj, 10, 10);
    lv_obj_set_size(top_obj, 20, 20);
    lv_obj_set_style_border_width(top_obj, 0, 0);
    lv_obj_set_style_radius(top_obj, 0, 0);
    lv_obj_set_style_bg_opa(top_obj, LV_OPA_COVER, 0);
    lv_obj_add_event_cb(top_obj, capture_layer_recolor_cb, LV_EVENT_DRAW_MAIN, NULL);

    /* Clear the initial full-screen invalidation, then exercise the optimized
     * refresh path which starts at the opaque child instead of the screen. */
    lv_refr_now(NULL);

    lv_area_t child_area;
    lv_obj_get_coords(child, &child_area);
    TEST_ASSERT_EQUAL_PTR(child, lv_refr_get_top_obj(&child_area, active_screen));

    top_layer_draw_count = 0;
    lv_obj_invalidate(child);
    lv_refr_now(NULL);

    TEST_ASSERT_GREATER_THAN_UINT32(0, top_layer_draw_count);
    TEST_ASSERT_EQUAL_UINT8(LV_OPA_TRANSP, top_layer_recolor.alpha);
}

#endif
