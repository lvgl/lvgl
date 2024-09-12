#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../lv_test_indev.h"
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

struct click_counts {
    uint32_t num_clicked;
    uint32_t num_short_clicked;
    uint32_t num_single_clicked;
    uint32_t num_double_clicked;
    uint32_t num_triple_clicked;
    uint32_t num_long_pressed;
    uint8_t short_click_streak;
};

static void click_event_cb(lv_event_t * e)
{
    struct click_counts * counts = lv_event_get_user_data(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_CLICKED:
            counts->num_clicked++;
            break;
        case LV_EVENT_SHORT_CLICKED:
            counts->num_short_clicked++;
            break;
        case LV_EVENT_SINGLE_CLICKED:
            counts->num_single_clicked++;
            break;
        case LV_EVENT_DOUBLE_CLICKED:
            counts->num_double_clicked++;
            break;
        case LV_EVENT_TRIPLE_CLICKED:
            counts->num_triple_clicked++;
            break;
        case LV_EVENT_LONG_PRESSED:
            counts->num_long_pressed++;
            break;
        default:
            break;
    }

    lv_indev_t * indev = lv_event_get_param(e);
    counts->short_click_streak = lv_indev_get_short_click_streak(indev);
}

void test_click(void)
{
    /*Setup button that counts events.*/
    struct click_counts counts;
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, click_event_cb, LV_EVENT_CLICKED, &counts);
    lv_obj_add_event_cb(btn, click_event_cb, LV_EVENT_SHORT_CLICKED, &counts);
    lv_obj_add_event_cb(btn, click_event_cb, LV_EVENT_SINGLE_CLICKED, &counts);
    lv_obj_add_event_cb(btn, click_event_cb, LV_EVENT_DOUBLE_CLICKED, &counts);
    lv_obj_add_event_cb(btn, click_event_cb, LV_EVENT_TRIPLE_CLICKED, &counts);
    lv_obj_add_event_cb(btn, click_event_cb, LV_EVENT_LONG_PRESSED, &counts);

    /*Simple click.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_mouse_click_at(50, 50);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(1, counts.short_click_streak);

    /*Second click nearby.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_mouse_click_at(47, 52);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(2, counts.short_click_streak);

    /*Third click nearby.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_mouse_click_at(49, 55);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(3, counts.short_click_streak);

    /*Fourth click nearby.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_mouse_click_at(50, 50);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(4, counts.short_click_streak);

    /*Resetting the click streak due to distance.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_mouse_click_at(10, 10);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(1, counts.short_click_streak);

    /*Second click nearby.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_mouse_click_at(12, 14);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(2, counts.short_click_streak);

    /*Resetting the click streak due to time.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_indev_wait(1000);
    lv_test_mouse_click_at(12, 14);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(1, counts.short_click_streak);

    /*Long press does not continue (or start) click streak.*/
    lv_memzero(&counts, sizeof(counts));
    lv_test_mouse_press();
    lv_test_indev_wait(1000);
    lv_test_mouse_release();
    lv_test_indev_wait(50);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_single_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_double_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, counts.num_triple_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, counts.num_long_pressed);
    TEST_ASSERT_EQUAL_UINT8(1, counts.short_click_streak);
}

#endif
