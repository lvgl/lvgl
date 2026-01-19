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

static void event_cb(lv_event_t * e)
{
    uint32_t * called = lv_event_get_user_data(e);
    (*called)++;

}

void test_obj_flag_radio_button(void)
{
    lv_group_t * g = lv_group_create();

    lv_indev_set_group(lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD), g);

    lv_obj_t * scr = lv_screen_active();
    lv_obj_t * cb[5];
    uint32_t called[5];
    for(uint32_t i = 0; i < 5; i++) {
        cb[i] = lv_checkbox_create(scr);
        lv_obj_set_y(cb[i], i * 50);
        lv_obj_add_flag(cb[i], LV_OBJ_FLAG_RADIO_BUTTON);
        lv_group_add_obj(g, cb[i]);
        lv_obj_add_event_cb(cb[i], event_cb, LV_EVENT_VALUE_CHANGED, &called[i]);
        called[i] = 0;
    }

    /*Click the first checkbox*/
    lv_test_mouse_click_at(20, 5);
    TEST_ASSERT_TRUE(lv_obj_has_state(cb[0], LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL_UINT32(called[0], 1);
    TEST_ASSERT_EQUAL_UINT32(called[1], 0);
    TEST_ASSERT_EQUAL_UINT32(called[2], 0);
    TEST_ASSERT_EQUAL_UINT32(called[3], 0);
    TEST_ASSERT_EQUAL_UINT32(called[4], 0);

    /*Click the second checkbox*/
    lv_test_mouse_click_at(20, 55);
    TEST_ASSERT_FALSE(lv_obj_has_state(cb[0], LV_STATE_CHECKED));
    TEST_ASSERT_TRUE(lv_obj_has_state(cb[1], LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL_UINT32(called[0], 2);
    TEST_ASSERT_EQUAL_UINT32(called[1], 1);
    TEST_ASSERT_EQUAL_UINT32(called[2], 0);
    TEST_ASSERT_EQUAL_UINT32(called[3], 0);
    TEST_ASSERT_EQUAL_UINT32(called[4], 0);

    /*Clicking the same checkbox shouldn't change anything*/
    lv_test_mouse_click_at(20, 55);
    TEST_ASSERT_FALSE(lv_obj_has_state(cb[0], LV_STATE_CHECKED));
    TEST_ASSERT_TRUE(lv_obj_has_state(cb[1], LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL_UINT32(called[0], 2);
    TEST_ASSERT_EQUAL_UINT32(called[1], 1);
    TEST_ASSERT_EQUAL_UINT32(called[2], 0);
    TEST_ASSERT_EQUAL_UINT32(called[3], 0);
    TEST_ASSERT_EQUAL_UINT32(called[4], 0);

    /*Turn on checkbox 2 with a key*/
    lv_group_focus_obj(cb[2]);
    lv_test_key_hit(LV_KEY_UP);
    TEST_ASSERT_FALSE(lv_obj_has_state(cb[1], LV_STATE_CHECKED));
    TEST_ASSERT_TRUE(lv_obj_has_state(cb[2], LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL_UINT32(called[0], 2);
    TEST_ASSERT_EQUAL_UINT32(called[1], 2);
    TEST_ASSERT_EQUAL_UINT32(called[2], 1);
    TEST_ASSERT_EQUAL_UINT32(called[3], 0);
    TEST_ASSERT_EQUAL_UINT32(called[4], 0);

    /*Nothing happen checking checkbox 2 again*/
    lv_test_key_hit(LV_KEY_RIGHT);
    TEST_ASSERT_FALSE(lv_obj_has_state(cb[1], LV_STATE_CHECKED));
    TEST_ASSERT_TRUE(lv_obj_has_state(cb[2], LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL_UINT32(called[0], 2);
    TEST_ASSERT_EQUAL_UINT32(called[1], 2);
    TEST_ASSERT_EQUAL_UINT32(called[2], 1);
    TEST_ASSERT_EQUAL_UINT32(called[3], 0);
    TEST_ASSERT_EQUAL_UINT32(called[4], 0);

    /*Can't check off*/
    lv_test_key_hit(LV_KEY_DOWN);
    TEST_ASSERT_FALSE(lv_obj_has_state(cb[1], LV_STATE_CHECKED));
    TEST_ASSERT_TRUE(lv_obj_has_state(cb[2], LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL_UINT32(called[0], 2);
    TEST_ASSERT_EQUAL_UINT32(called[1], 2);
    TEST_ASSERT_EQUAL_UINT32(called[2], 1);
    TEST_ASSERT_EQUAL_UINT32(called[3], 0);
    TEST_ASSERT_EQUAL_UINT32(called[4], 0);

    /*Same with left*/
    lv_test_key_hit(LV_KEY_LEFT);
    TEST_ASSERT_FALSE(lv_obj_has_state(cb[1], LV_STATE_CHECKED));
    TEST_ASSERT_TRUE(lv_obj_has_state(cb[2], LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL_UINT32(called[0], 2);
    TEST_ASSERT_EQUAL_UINT32(called[1], 2);
    TEST_ASSERT_EQUAL_UINT32(called[2], 1);
    TEST_ASSERT_EQUAL_UINT32(called[3], 0);
    TEST_ASSERT_EQUAL_UINT32(called[4], 0);

}

#endif
