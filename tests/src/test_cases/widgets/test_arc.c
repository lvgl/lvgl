#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

/* This function runs before each test */
void setUp(void);

void test_arc_creation_successful(void);
void test_arc_should_truncate_to_max_range_when_new_value_exceeds_it(void);
void test_arc_should_truncate_to_min_range_when_new_value_is_inferior(void);
void test_arc_should_update_value_after_updating_range(void);
void test_arc_should_update_angles_when_changing_to_symmetrical_mode(void);
void test_arc_should_update_angles_when_changing_to_symmetrical_mode_value_more_than_middle_range(void);
void test_arc_angles_when_reversed(void);

static lv_obj_t * active_screen = NULL;
static lv_obj_t * arc = NULL;
static lv_obj_t * arc2 = NULL;
static uint32_t event_cnt;
static uint32_t event_cnt2;

static void dummy_event_cb(lv_event_t * e);
static void dummy_event_cb2(lv_event_t * e);

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
    lv_obj_set_style_layout(active_screen, LV_LAYOUT_NONE, 0);
}

void test_arc_creation_successful(void)
{
    arc = lv_arc_create(active_screen);

    TEST_ASSERT_NOT_NULL(arc);
}

void test_arc_basic_render(void)
{
    arc = lv_arc_create(active_screen);
    lv_obj_set_size(arc, 100, 100);
    lv_obj_center(arc);
    lv_arc_set_value(arc, 70);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arc_1.png");
}

void test_arc_rgb565a8_image(void)
{
#if LV_BIN_DECODER_RAM_LOAD
    /*RGB565A8 image rendering requires special handling*/
    arc = lv_arc_create(active_screen);
    lv_obj_set_size(arc, 100, 100);
    lv_obj_center(arc);
    lv_arc_set_value(arc, 70);
    lv_obj_set_style_arc_width(arc, 30, 0);
    lv_obj_set_style_arc_width(arc, 30, LV_PART_INDICATOR);
    lv_obj_set_style_arc_image_src(arc, "A:src/test_files/binimages/cogwheel.RGB565A8.bin", LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(arc, 150, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(arc, 0, LV_PART_KNOB);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arc_2.png");
#endif
}

void test_arc_should_truncate_to_max_range_when_new_value_exceeds_it(void)
{
    /* Default max range is 100 */
    int16_t value_after_truncation = 100;

    arc = lv_arc_create(active_screen);

    lv_arc_set_value(arc, 200);

    TEST_ASSERT_EQUAL_INT16(value_after_truncation, lv_arc_get_value(arc));
}

void test_arc_should_truncate_to_min_range_when_new_value_is_inferior(void)
{
    /* Default min range is 100 */
    int16_t value_after_truncation = 0;

    arc = lv_arc_create(active_screen);

    lv_arc_set_value(arc, 0);

    TEST_ASSERT_EQUAL_INT16(value_after_truncation, lv_arc_get_value(arc));
}

void test_arc_should_update_value_after_updating_range(void)
{
    int16_t value_after_updating_max_range = 50;
    int16_t value_after_updating_min_range = 30;

    arc = lv_arc_create(active_screen);

    lv_arc_set_value(arc, 80);
    lv_arc_set_range(arc, 1, 50);

    TEST_ASSERT_EQUAL_INT16(value_after_updating_max_range, lv_arc_get_value(arc));

    lv_arc_set_value(arc, 10);
    lv_arc_set_range(arc, 30, 50);

    TEST_ASSERT_EQUAL_INT16(value_after_updating_min_range, lv_arc_get_value(arc));
}

void test_arc_should_update_angles_when_changing_to_symmetrical_mode(void)
{
    int16_t expected_angle_start = 135;
    int16_t expected_angle_end = 270;

    /* start angle is 135, end angle is 45 at creation */
    arc = lv_arc_create(active_screen);
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_INT16(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_INT16(expected_angle_end, lv_arc_get_angle_end(arc));
}

void test_arc_should_update_angles_when_changing_to_symmetrical_mode_value_more_than_middle_range(void)
{
    int16_t expected_angle_start = 270;
    int16_t expected_angle_end = 45;

    /* start angle is 135, end angle is 45 at creation */
    arc = lv_arc_create(active_screen);
    lv_arc_set_value(arc, 100);
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_INT16(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_INT16(expected_angle_end, lv_arc_get_angle_end(arc));
}

/* See #2522 for more information */
void test_arc_angles_when_reversed(void)
{
    uint16_t expected_start_angle = 54;
    uint16_t expected_end_angle = 90;
    int16_t expected_value = 40;

    lv_obj_t * arcBlack;
    arcBlack = lv_arc_create(lv_screen_active());

    lv_arc_set_mode(arcBlack, LV_ARC_MODE_REVERSE);

    lv_arc_set_bg_angles(arcBlack, 0, 90);

    lv_arc_set_value(arcBlack, expected_value);

    TEST_ASSERT_EQUAL_UINT16(expected_start_angle, lv_arc_get_angle_start(arcBlack));
    TEST_ASSERT_EQUAL_UINT16(expected_end_angle, lv_arc_get_angle_end(arcBlack));
    TEST_ASSERT_EQUAL_INT16(expected_value, lv_arc_get_value(arcBlack));
}

void test_arc_click_area_with_adv_hittest(void)
{
    arc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(arc, 100, 100);
    lv_obj_set_style_arc_width(arc, 10, 0);
    lv_obj_add_flag(arc, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_event_cb(arc, dummy_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_set_ext_click_area(arc, 5);

    /*No click detected at the middle*/
    event_cnt = 0;
    lv_test_mouse_click_at(50, 50);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);

    /*No click close to the radius - bg_arc - ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(83, 50);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);

    /*Click on the radius - bg_arc - ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(86, 50);
    TEST_ASSERT_GREATER_THAN(0, event_cnt);

    /*Click on the radius + ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(104, 50);
    TEST_ASSERT_GREATER_THAN(0, event_cnt);

    /*No click beyond to the radius + ext_click_area*/
    event_cnt = 0;
    lv_test_mouse_click_at(106, 50);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);
}

/* Check value doesn't go to max when clicking on the other side of the arc */
void test_arc_click_sustained_from_start_to_end_does_not_set_value_to_max(void)
{
    arc = lv_arc_create(lv_screen_active());
    lv_arc_set_value(arc, 0);

    lv_obj_set_size(arc, 100, 100);
    lv_obj_center(arc);
    lv_obj_add_event_cb(arc, dummy_event_cb, LV_EVENT_PRESSED, NULL);
    event_cnt = 0;

    /* Click close to start angle */
    event_cnt = 0;
    lv_test_mouse_release();
    lv_test_indev_wait(50);
    lv_test_mouse_move_to(376, 285);
    lv_test_mouse_press();
    lv_test_indev_wait(500);
    lv_test_mouse_release();
    lv_test_indev_wait(50);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt);
    TEST_ASSERT_EQUAL_INT32(lv_arc_get_min_value(arc), lv_arc_get_value(arc));

    /* Click close to end angle */
    event_cnt = 0;

    lv_test_mouse_release();
    lv_test_indev_wait(50);
    lv_test_mouse_move_to(376, 285);
    lv_test_mouse_press();
    lv_test_indev_wait(500);
    lv_test_mouse_move_to(415, 281);
    lv_test_indev_wait(500);
    lv_test_mouse_release();
    lv_test_indev_wait(50);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt);
    TEST_ASSERT_EQUAL_INT32(lv_arc_get_min_value(arc), lv_arc_get_value(arc));

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arc_3.png");
}

void test_two_overlapping_arcs_can_be_interacted_independently(void)
{
    arc = lv_arc_create(lv_screen_active());
    arc2 = lv_arc_create(lv_screen_active());

    lv_arc_set_value(arc, 0);
    lv_arc_set_value(arc2, 0);
    lv_obj_set_size(arc, 100, 100);
    lv_obj_set_size(arc2, 100, 100);
    lv_arc_set_bg_angles(arc, 20, 160);
    lv_arc_set_bg_angles(arc2, 200, 340);
    lv_obj_add_flag(arc, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_flag(arc2, LV_OBJ_FLAG_ADV_HITTEST);
    lv_arc_set_value(arc, 10);
    lv_arc_set_value(arc2, 10);
    lv_arc_set_rotation(arc, 355);
    lv_arc_set_rotation(arc2, 355);
    lv_obj_center(arc);
    lv_obj_center(arc2);

    // Add event callback to both arcs
    lv_obj_add_event_cb(arc, dummy_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(arc2, dummy_event_cb2, LV_EVENT_PRESSED, NULL);

    // Reset event counters
    event_cnt = 0;
    event_cnt2 = 0;

    // Click on the position of the first arc (center)
    lv_test_mouse_release();
    lv_test_indev_wait(50);
    lv_test_mouse_move_to(400, 195);
    lv_test_mouse_press();
    lv_test_indev_wait(500);
    lv_test_mouse_release();
    lv_test_indev_wait(50);

    // Verify that the event callback was called for the first arc
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt2);

    // click on the position of the second arc (center)
    lv_test_mouse_release();
    lv_test_indev_wait(50);
    lv_test_mouse_move_to(400, 285);
    lv_test_mouse_press();
    lv_test_indev_wait(500);
    lv_test_mouse_release();
    lv_test_indev_wait(50);

    // Verify that the event callback was called for the second arc
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt2);

    // Verify that the screen remains as expected after the interactions
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/overlapping_arcs_test.png");
}


static void dummy_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    event_cnt++;
}

static void dummy_event_cb2(lv_event_t * e)
{
    LV_UNUSED(e);
    event_cnt2++;
}

#endif
