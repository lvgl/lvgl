#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void ext_draw_size_event_cb(lv_event_t * e)
{
    lv_event_set_ext_draw_size(e, 100);
}

static void btn_clicked_event_cb(lv_event_t * e)
{
    uint32_t * cnt = lv_event_get_user_data(e);
    (*cnt)++;
}

void test_obj_flag_overflow_visible_1(void)
{
    lv_obj_t * obj_main = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_main, 400, 300);
    lv_obj_set_style_bg_color(obj_main, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_add_flag(obj_main, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_center(obj_main);
    lv_obj_add_event_cb(obj_main, ext_draw_size_event_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);

    lv_obj_t * obj_child_1 = lv_obj_create(obj_main);
    lv_obj_set_size(obj_child_1, 200, 200);
    lv_obj_set_style_bg_color(obj_child_1, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_align(obj_child_1, LV_ALIGN_LEFT_MID, -100, 0);

    lv_obj_t * btn_1 = lv_button_create(obj_child_1);
    lv_obj_set_size(btn_1, 100, 100);
    lv_obj_align(btn_1, LV_ALIGN_LEFT_MID, -75, 0);
    lv_obj_remove_flag(btn_1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    uint32_t cnt_1;
    lv_obj_add_event_cb(btn_1, btn_clicked_event_cb, LV_EVENT_CLICKED, &cnt_1);

    lv_obj_t * label_1 = lv_label_create(btn_1);
    lv_label_set_text(label_1, "Button 1");
    lv_obj_center(label_1);

    lv_obj_t * obj_child_2 = lv_obj_create(obj_main);
    lv_obj_set_size(obj_child_2, 200, 200);
    lv_obj_set_style_bg_color(obj_child_2, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_add_flag(obj_child_2, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_align(obj_child_2, LV_ALIGN_RIGHT_MID, 100, 0);
    lv_obj_add_event_cb(obj_child_2, ext_draw_size_event_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);

    lv_obj_t * btn_2 = lv_button_create(obj_child_2);
    lv_obj_set_size(btn_2, 100, 100);
    lv_obj_align(btn_2, LV_ALIGN_RIGHT_MID, 75, 0);
    lv_obj_remove_flag(btn_2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    uint32_t cnt_2;
    lv_obj_add_event_cb(btn_2, btn_clicked_event_cb, LV_EVENT_CLICKED, &cnt_2);

    lv_obj_t * label_2 = lv_label_create(btn_2);
    lv_label_set_text(label_2, "Button 2");
    lv_obj_center(label_2);

    cnt_1 = 0;
    cnt_2 = 0;

    /*The clipped part of the left button (shouldn't trigger click event)*/
    lv_test_mouse_click_at(100, 220);

    /*The non clipped part of the left button (should trigger click event)*/
    lv_test_mouse_click_at(140, 220);

    /*The left part of the right button (should trigger click event)*/
    lv_test_mouse_click_at(650, 220);

    /*The outer part of the right button (should trigger click event as obj_child_2 has LV_OBJ_FLAG_OVERFLOW_VISIBLE)*/
    lv_test_mouse_click_at(690, 220);

    TEST_ASSERT_EQUAL_UINT32(1, cnt_1);
    TEST_ASSERT_EQUAL_UINT32(2, cnt_2);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_flag_overflow_visible_1_1.png");

    /*Test if the overflowing parts are rendered correctly after scrolling too*/
    lv_obj_scroll_by_bounded(obj_main, -20, 0, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_flag_overflow_visible_1_2.png");

    lv_obj_scroll_by_bounded(obj_child_2, -30, 0, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_flag_overflow_visible_1_3.png");

    /*Test with rotation*/
    lv_obj_set_style_transform_rotation(obj_main, 300, 0);
    lv_obj_set_style_transform_pivot_x(obj_main, 200, 0);
    lv_obj_set_style_transform_pivot_y(obj_main, 150, 0);

    lv_obj_set_style_transform_rotation(obj_child_1, 300, 0);
    lv_obj_set_style_transform_pivot_x(obj_child_1, 100, 0);
    lv_obj_set_style_transform_pivot_y(obj_child_1, 100, 0);

    lv_obj_set_style_transform_rotation(obj_child_2, 300, 0);
    lv_obj_set_style_transform_pivot_x(obj_child_2, 100, 0);
    lv_obj_set_style_transform_pivot_y(obj_child_2, 100, 0);

    lv_obj_set_style_transform_rotation(btn_1, 300, 0);
    lv_obj_set_style_transform_pivot_x(btn_1, 100, 0);
    lv_obj_set_style_transform_pivot_y(btn_1, 100, 0);

    lv_obj_set_style_transform_rotation(btn_2, 300, 0);
    lv_obj_set_style_transform_pivot_x(btn_2, 100, 0);
    lv_obj_set_style_transform_pivot_y(btn_2, 100, 0);

    cnt_1 = 0;
    cnt_2 = 0;

    /*The clipped part of the left button (shouldn't trigger click event)*/
    lv_test_mouse_click_at(185, 40);

    /*The non clipped part of the left button (should trigger click event)*/
    lv_test_mouse_click_at(210, 80);

    /*The left part of the right button (should trigger click event)*/
    lv_test_mouse_click_at(590, 370);

    /*The outer part of the right button (should trigger click event as obj_child_2 has LV_OBJ_FLAG_OVERFLOW_VISIBLE)*/
    lv_test_mouse_click_at(600, 430);

    /*The clipped part of the right button (clipped because it's out of the red panel's ext draw size, shouldn't trigger click event)*/
    lv_test_mouse_click_at(645, 430);

    TEST_ASSERT_EQUAL_UINT32(1, cnt_1);
    TEST_ASSERT_EQUAL_UINT32(2, cnt_2);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_flag_overflow_visible_1_4.png");

}

#endif
