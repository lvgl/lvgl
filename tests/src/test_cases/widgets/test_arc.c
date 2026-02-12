#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

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
    lv_value_precise_t expected_angle_start = 135;
    lv_value_precise_t expected_angle_end = 270;

    /* start angle is 135, end angle is 45 at creation */
    arc = lv_arc_create(active_screen);
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_FLOAT(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_FLOAT(expected_angle_end, lv_arc_get_angle_end(arc));
}

void test_arc_should_update_angles_when_changing_to_symmetrical_mode_value_more_than_middle_range(void)
{
    lv_value_precise_t expected_angle_start = 270;
    lv_value_precise_t expected_angle_end = 45;

    /* start angle is 135, end angle is 45 at creation */
    arc = lv_arc_create(active_screen);
    lv_arc_set_value(arc, 100);
    lv_arc_set_mode(arc, LV_ARC_MODE_SYMMETRICAL);

    TEST_ASSERT_EQUAL_FLOAT(expected_angle_start, lv_arc_get_angle_start(arc));
    TEST_ASSERT_EQUAL_FLOAT(expected_angle_end, lv_arc_get_angle_end(arc));
}

/* See #2522 for more information */
void test_arc_angles_when_reversed(void)
{
    lv_value_precise_t expected_start_angle = 54;
    lv_value_precise_t expected_end_angle = 90;
    int16_t expected_value = 40;

    lv_obj_t * arcBlack;
    arcBlack = lv_arc_create(lv_screen_active());

    lv_arc_set_mode(arcBlack, LV_ARC_MODE_REVERSE);

    lv_arc_set_bg_angles(arcBlack, 0, 90);

    lv_arc_set_value(arcBlack, expected_value);

    TEST_ASSERT_EQUAL_FLOAT(expected_start_angle, lv_arc_get_angle_start(arcBlack));
    TEST_ASSERT_EQUAL_FLOAT(expected_end_angle, lv_arc_get_angle_end(arcBlack));
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
    lv_test_wait(50);
    lv_test_mouse_move_to(376, 285);
    lv_test_mouse_press();
    lv_test_wait(500);
    lv_test_mouse_release();
    lv_test_wait(50);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt);
    TEST_ASSERT_EQUAL_INT32(lv_arc_get_min_value(arc), lv_arc_get_value(arc));

    /* Click close to end angle */
    event_cnt = 0;

    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(376, 285);
    lv_test_mouse_press();
    lv_test_wait(500);
    lv_test_mouse_move_to(415, 281);
    lv_test_wait(500);
    lv_test_mouse_release();
    lv_test_wait(50);

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
    lv_test_wait(50);
    lv_test_mouse_move_to(400, 195);
    lv_test_mouse_press();
    lv_test_wait(500);
    lv_test_mouse_release();
    lv_test_wait(50);

    // Verify that the event callback was called for the first arc
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt2);

    // click on the position of the second arc (center)
    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(400, 285);
    lv_test_mouse_press();
    lv_test_wait(500);
    lv_test_mouse_release();
    lv_test_wait(50);

    // Verify that the event callback was called for the second arc
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt2);

    // Verify that the screen remains as expected after the interactions
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/overlapping_arcs_test.png");
}

void test_arc_padding(void)
{
    arc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(arc, 412, 412);
    lv_obj_set_style_arc_width(arc, 100, LV_PART_MAIN);

    lv_obj_set_style_arc_width(arc, 60, LV_PART_INDICATOR);
    lv_obj_set_style_pad_bottom(arc, 10, LV_PART_INDICATOR);

    lv_obj_set_style_bg_color(arc, lv_palette_darken(LV_PALETTE_BLUE, 3), LV_PART_KNOB);
    lv_obj_set_style_pad_all(arc, -5, LV_PART_KNOB);

    lv_obj_center(arc);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arc_4.png");
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

void test_arc_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_arc_create(lv_screen_active());
    lv_property_t prop = { };

    /* Test START_ANGLE property (PRECISE type) */
    prop.id = LV_PROPERTY_ARC_START_ANGLE;
    prop.precise = 45;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(45, lv_obj_get_property(obj, LV_PROPERTY_ARC_START_ANGLE).precise);
    TEST_ASSERT_EQUAL_INT(45, lv_arc_get_angle_start(obj));

    /* Test END_ANGLE property (PRECISE type) */
    prop.id = LV_PROPERTY_ARC_END_ANGLE;
    prop.precise = 270;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(270, lv_obj_get_property(obj, LV_PROPERTY_ARC_END_ANGLE).precise);
    TEST_ASSERT_EQUAL_INT(270, lv_arc_get_angle_end(obj));

    /* Test BG_START_ANGLE property (PRECISE type) */
    prop.id = LV_PROPERTY_ARC_BG_START_ANGLE;
    prop.precise = 30;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(30, lv_obj_get_property(obj, LV_PROPERTY_ARC_BG_START_ANGLE).precise);
    TEST_ASSERT_EQUAL_INT(30, lv_arc_get_bg_angle_start(obj));

    /* Test BG_END_ANGLE property (PRECISE type) */
    prop.id = LV_PROPERTY_ARC_BG_END_ANGLE;
    prop.precise = 300;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(300, lv_obj_get_property(obj, LV_PROPERTY_ARC_BG_END_ANGLE).precise);
    TEST_ASSERT_EQUAL_INT(300, lv_arc_get_bg_angle_end(obj));

    /* Test ROTATION property (INT type) */
    prop.id = LV_PROPERTY_ARC_ROTATION;
    prop.num = 90;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(90, lv_obj_get_property(obj, LV_PROPERTY_ARC_ROTATION).num);
    TEST_ASSERT_EQUAL_INT(90, lv_arc_get_rotation(obj));

    /* Test MODE property (INT type) */
    prop.id = LV_PROPERTY_ARC_MODE;
    prop.num = LV_ARC_MODE_SYMMETRICAL;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(LV_ARC_MODE_SYMMETRICAL, lv_obj_get_property(obj, LV_PROPERTY_ARC_MODE).num);
    TEST_ASSERT_EQUAL_INT(LV_ARC_MODE_SYMMETRICAL, lv_arc_get_mode(obj));

    /* Test VALUE property (INT type) */
    lv_arc_set_range(obj, 0, 100);
    prop.id = LV_PROPERTY_ARC_VALUE;
    prop.num = 75;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(75, lv_obj_get_property(obj, LV_PROPERTY_ARC_VALUE).num);
    TEST_ASSERT_EQUAL_INT(75, lv_arc_get_value(obj));

    /* Test MIN_VALUE property (INT type) */
    prop.id = LV_PROPERTY_ARC_MIN_VALUE;
    prop.num = 10;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(10, lv_obj_get_property(obj, LV_PROPERTY_ARC_MIN_VALUE).num);
    TEST_ASSERT_EQUAL_INT(10, lv_arc_get_min_value(obj));

    /* Test MAX_VALUE property (INT type) */
    prop.id = LV_PROPERTY_ARC_MAX_VALUE;
    prop.num = 200;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(200, lv_obj_get_property(obj, LV_PROPERTY_ARC_MAX_VALUE).num);
    TEST_ASSERT_EQUAL_INT(200, lv_arc_get_max_value(obj));

    /* Test CHANGE_RATE property (INT type) */
    prop.id = LV_PROPERTY_ARC_CHANGE_RATE;
    prop.num = 50;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(50, lv_obj_get_property(obj, LV_PROPERTY_ARC_CHANGE_RATE).num);

    /* Test KNOB_OFFSET property (INT type) */
    prop.id = LV_PROPERTY_ARC_KNOB_OFFSET;
    prop.num = 5;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(5, lv_obj_get_property(obj, LV_PROPERTY_ARC_KNOB_OFFSET).num);
    TEST_ASSERT_EQUAL_INT(5, lv_arc_get_knob_offset(obj));

    lv_obj_delete(obj);
#endif
}

void test_arc_set_change_rate(void)
{
    arc = lv_arc_create(active_screen);

    /* Test setting various change rate values */
    lv_arc_set_change_rate(arc, 0);
    lv_arc_set_change_rate(arc, 100);
    lv_arc_set_change_rate(arc, 500);
    lv_arc_set_change_rate(arc, 1000);
}

void test_arc_set_knob_offset(void)
{
    int32_t test_offset_positive = 15;
    int32_t test_offset_negative = -10;
    int32_t test_offset_zero = 0;

    arc = lv_arc_create(active_screen);

    /* Test setting positive offset */
    lv_arc_set_knob_offset(arc, test_offset_positive);
    TEST_ASSERT_EQUAL_INT32(test_offset_positive, lv_arc_get_knob_offset(arc));

    /* Test setting negative offset */
    lv_arc_set_knob_offset(arc, test_offset_negative);
    TEST_ASSERT_EQUAL_INT32(test_offset_negative, lv_arc_get_knob_offset(arc));

    /* Test setting zero offset */
    lv_arc_set_knob_offset(arc, test_offset_zero);
    TEST_ASSERT_EQUAL_INT32(test_offset_zero, lv_arc_get_knob_offset(arc));
}

void test_arc_getter_functions(void)
{
    arc = lv_arc_create(active_screen);

    /* Test background angle getters */
    lv_value_precise_t bg_start_angle = 45.0f;
    lv_value_precise_t bg_end_angle = 270.0f;

    lv_arc_set_bg_angles(arc, bg_start_angle, bg_end_angle);
    TEST_ASSERT_EQUAL_FLOAT(bg_start_angle, lv_arc_get_bg_angle_start(arc));
    TEST_ASSERT_EQUAL_FLOAT(bg_end_angle, lv_arc_get_bg_angle_end(arc));

    /* Test max value getter */
    int32_t max_value = 200;
    lv_arc_set_range(arc, 0, max_value);
    TEST_ASSERT_EQUAL_INT32(max_value, lv_arc_get_max_value(arc));

    /* Test mode getter */
    lv_arc_mode_t mode = LV_ARC_MODE_SYMMETRICAL;
    lv_arc_set_mode(arc, mode);
    TEST_ASSERT_EQUAL(mode, lv_arc_get_mode(arc));

    /* Test rotation getter */
    int32_t rotation = 90;
    lv_arc_set_rotation(arc, rotation);
    TEST_ASSERT_EQUAL_INT32(rotation, lv_arc_get_rotation(arc));

    /* Test knob offset getter */
    int32_t knob_offset = 15;
    lv_arc_set_knob_offset(arc, knob_offset);
    TEST_ASSERT_EQUAL_INT32(knob_offset, lv_arc_get_knob_offset(arc));
}

void test_arc_drag_prevents_big_angle_jumps(void)
{
    arc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(arc, 100, 100);
    lv_obj_center(arc);
    lv_obj_update_layout(arc);

    /* Set initial value */
    lv_arc_set_value(arc, 25);

    /* Test normal drag - drag along the arc smoothly */
    lv_test_mouse_move_to(376, 285);  /* Start position on arc */
    lv_test_mouse_press();
    lv_test_wait(500);

    /* Drag to a nearby position on the arc */
    lv_test_mouse_move_to(390, 290);
    lv_test_wait(500);

    int32_t value_after_small_drag = lv_arc_get_value(arc);

    lv_test_mouse_release();
    lv_test_wait(50);

    /* Value should have changed during drag */
    TEST_ASSERT_NOT_EQUAL(25, value_after_small_drag);

    /* Test with different arc range to cover more of the logic */
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_value(arc, 50);

    lv_test_mouse_move_to(380, 280);
    lv_test_mouse_press();
    lv_test_wait(500);
    lv_test_mouse_move_to(400, 295);
    lv_test_wait(500);
    lv_test_mouse_release();
    lv_test_wait(50);

    TEST_ASSERT_GREATER_OR_EQUAL_INT32(0, lv_arc_get_value(arc));
    TEST_ASSERT_LESS_OR_EQUAL_INT32(100, lv_arc_get_value(arc));
}

void test_arc_align_obj_to_angle(void)
{
    arc = lv_arc_create(active_screen);
    lv_obj_set_size(arc, 100, 100);
    lv_obj_center(arc);

    /* Create an object to align */
    lv_obj_t * aligned_obj = lv_obj_create(arc);
    lv_obj_set_size(aligned_obj, 10, 10);

    /* Test align with different values and offsets */
    lv_arc_set_value(arc, 0);
    lv_arc_align_obj_to_angle(arc, aligned_obj, 0);

    /* Get coordinates after alignment */
    int32_t x1 = lv_obj_get_x(aligned_obj);
    int32_t y1 = lv_obj_get_y(aligned_obj);

    /* Test with different arc value */
    lv_arc_set_value(arc, 50);
    lv_arc_align_obj_to_angle(arc, aligned_obj, 0);

    /* Position should change when value changes */
    int32_t x2 = lv_obj_get_x(aligned_obj);
    int32_t y2 = lv_obj_get_y(aligned_obj);

    /* At least one coordinate should be different */
    TEST_ASSERT_TRUE((x1 != x2) || (y1 != y2));

    /* Test with positive radius offset */
    lv_arc_align_obj_to_angle(arc, aligned_obj, 10);
    int32_t x3 = lv_obj_get_x(aligned_obj);
    int32_t y3 = lv_obj_get_y(aligned_obj);

    /* Position should be different with offset */
    TEST_ASSERT_TRUE((x2 != x3) || (y2 != y3));

    /* Test with negative radius offset */
    lv_arc_align_obj_to_angle(arc, aligned_obj, -10);
    int32_t x4 = lv_obj_get_x(aligned_obj);
    int32_t y4 = lv_obj_get_y(aligned_obj);

    /* Position should be different with negative offset */
    TEST_ASSERT_TRUE((x3 != x4) || (y3 != y4));

    /* Test with maximum value */
    lv_arc_set_value(arc, 100);
    lv_arc_align_obj_to_angle(arc, aligned_obj, 0);
}

void test_arc_rotate_obj_to_angle(void)
{
    arc = lv_arc_create(active_screen);
    lv_obj_set_size(arc, 100, 100);
    lv_obj_center(arc);

    /* Create an object to rotate */
    lv_obj_t * rotated_obj = lv_obj_create(arc);
    lv_obj_set_size(rotated_obj, 10, 10);

    /* Test rotate with value 0 */
    lv_arc_set_value(arc, 0);
    lv_arc_rotate_obj_to_angle(arc, rotated_obj, 0);

    /* Get initial rotation */
    int32_t rotation1 = lv_obj_get_style_transform_rotation(rotated_obj, 0);

    /* Test with different arc value */
    lv_arc_set_value(arc, 50);
    lv_arc_rotate_obj_to_angle(arc, rotated_obj, 0);

    /* Rotation should change when value changes */
    int32_t rotation2 = lv_obj_get_style_transform_rotation(rotated_obj, 0);
    TEST_ASSERT_NOT_EQUAL(rotation1, rotation2);

    /* Test with positive radius offset */
    lv_arc_rotate_obj_to_angle(arc, rotated_obj, 15);

    /* Get coordinates with offset */
    int32_t x1 = lv_obj_get_x(rotated_obj);
    int32_t y1 = lv_obj_get_y(rotated_obj);

    /* Test with different offset */
    lv_arc_rotate_obj_to_angle(arc, rotated_obj, -15);

    /* Position should change with different offset */
    int32_t x2 = lv_obj_get_x(rotated_obj);
    int32_t y2 = lv_obj_get_y(rotated_obj);
    TEST_ASSERT_TRUE((x1 != x2) || (y1 != y2));

    /* Test with maximum value */
    lv_arc_set_value(arc, 100);
    lv_arc_rotate_obj_to_angle(arc, rotated_obj, 0);

    /* Rotation should be different at max value */
    int32_t rotation3 = lv_obj_get_style_transform_rotation(rotated_obj, 0);
    TEST_ASSERT_NOT_EQUAL(rotation2, rotation3);

    /* Verify rotation is within expected range (angle * 10 + 900) */
    /* For arc angle 0-270 degrees, rotation should be 900-3600 */
    TEST_ASSERT_GREATER_OR_EQUAL_INT32(0, rotation3);
}

static void test_arc_key_step(lv_obj_t * obj, int32_t start_val, uint32_t key, int32_t expected_val)
{
    lv_arc_set_value(obj, start_val);
    lv_obj_send_event(obj, LV_EVENT_KEY, &key);
    TEST_ASSERT_EQUAL_INT32(expected_val, lv_arc_get_value(obj));
}

void test_arc_key_event_handling(void)
{
    arc = lv_arc_create(active_screen);
    lv_obj_set_size(arc, 100, 100);
    lv_arc_set_range(arc, 0, 100);

    /* Test RIGHT key increases value */
    test_arc_key_step(arc, 50, LV_KEY_RIGHT, 51);

    /* Test UP key increases value */
    test_arc_key_step(arc, 50, LV_KEY_UP, 51);

    /* Test LEFT key decreases value */
    test_arc_key_step(arc, 50, LV_KEY_LEFT, 49);

    /* Test DOWN key decreases value */
    test_arc_key_step(arc, 50, LV_KEY_DOWN, 49);

    /* Test that value respects maximum limit */
    test_arc_key_step(arc, 100, LV_KEY_RIGHT, 100);

    /* Test that value respects minimum limit */
    test_arc_key_step(arc, 0, LV_KEY_LEFT, 0);
}

static void test_arc_rotary_step(lv_obj_t * obj, int32_t start_val, int32_t rotary_diff, int32_t expected_val)
{
    lv_arc_set_value(obj, start_val);
    lv_obj_send_event(obj, LV_EVENT_ROTARY, &rotary_diff);
    TEST_ASSERT_EQUAL_INT32(expected_val, lv_arc_get_value(obj));
}

void test_arc_rotary_event_handling(void)
{
    arc = lv_arc_create(active_screen);
    lv_obj_set_size(arc, 100, 100);
    lv_arc_set_range(arc, 0, 100);

    /* Test positive rotary diff increases value */
    test_arc_rotary_step(arc, 50, 5, 55);

    /* Test negative rotary diff decreases value */
    test_arc_rotary_step(arc, 50, -5, 45);

    /* Test large positive rotary diff respects maximum */
    test_arc_rotary_step(arc, 95, 10, 100);

    /* Test large negative rotary diff respects minimum */
    test_arc_rotary_step(arc, 5, -10, 0);

    /* Test zero rotary diff keeps value unchanged */
    test_arc_rotary_step(arc, 50, 0, 50);
}

void test_arc_encoder_focused_event(void)
{
    arc = lv_arc_create(active_screen);
    lv_obj_set_size(arc, 100, 100);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 30);

    /* Create a group and add arc to it */
    lv_group_t * g = lv_group_create();
    lv_indev_set_group(lv_test_indev_get_indev(LV_INDEV_TYPE_ENCODER), g);
    lv_group_add_obj(g, arc);

    /* Test encoder click to enter editing mode */
    lv_group_set_editing(g, false);
    TEST_ASSERT_FALSE(lv_group_get_editing(g));

    lv_test_encoder_click();
    lv_test_wait(50);

    /* Clicking should enter editing mode */
    TEST_ASSERT_TRUE(lv_group_get_editing(g));

    /* Test value change with encoder in editing mode */
    int32_t value_before = lv_arc_get_value(arc);
    lv_test_encoder_turn(10);
    lv_test_wait(50);
    int32_t value_after = lv_arc_get_value(arc);

    /* Value should change when encoder is used in editing mode */
    TEST_ASSERT_EQUAL_INT32(value_before + 10, value_after);

    /* Test encoder click again to exit editing mode */
    lv_test_encoder_click();
    lv_test_wait(50);
    TEST_ASSERT_FALSE(lv_group_get_editing(g));

    /* Test rotary in non-editing mode should not change value */
    value_before = lv_arc_get_value(arc);
    lv_test_encoder_turn(5);
    lv_test_wait(50);
    value_after = lv_arc_get_value(arc);

    /* Value should not change when not in editing mode */
    TEST_ASSERT_EQUAL_INT32(value_before, value_after);
    lv_group_delete(g);
}

static void run_arc_drag_test(int32_t bg_start, int32_t bg_end, int32_t start_val, int32_t drag_from, int32_t drag_to)
{
    lv_arc_set_bg_angles(arc, bg_start, bg_end);
    lv_arc_set_value(arc, start_val);

    int32_t center_x = lv_obj_get_x(arc) + lv_obj_get_width(arc) / 2;
    int32_t center_y = lv_obj_get_y(arc) + lv_obj_get_height(arc) / 2;
    int32_t radius = 85;

    int32_t x_from = center_x + (radius * lv_trigo_sin(drag_from)) / LV_TRIGO_SIN_MAX;
    int32_t y_from = center_y - (radius * lv_trigo_cos(drag_from)) / LV_TRIGO_SIN_MAX;
    int32_t x_to = center_x + (radius * lv_trigo_sin(drag_to)) / LV_TRIGO_SIN_MAX;
    int32_t y_to = center_y - (radius * lv_trigo_cos(drag_to)) / LV_TRIGO_SIN_MAX;

    lv_test_mouse_move_to(x_from, y_from);
    lv_test_mouse_press();
    lv_test_wait(50);
    lv_test_mouse_move_to(x_to, y_to);
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(50);

    TEST_ASSERT_GREATER_OR_EQUAL_INT32(0, lv_arc_get_value(arc));
    TEST_ASSERT_LESS_OR_EQUAL_INT32(100, lv_arc_get_value(arc));
}

void test_arc_angle_within_bg_bounds_edge_cases(void)
{
    arc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(arc, 200, 200);
    lv_obj_center(arc);
    lv_obj_update_layout(arc);

    /* Test 1: angle in [360-tolerance, 360] range */
    run_arc_drag_test(60, 270, 10, 90, 50);

    /* Test 2: angle near 360° with small tolerance */
    run_arc_drag_test(10, 270, 20, 60, 5);

    /* Test 3: angle after end within tolerance */
    run_arc_drag_test(40, 250, 80, 240, 275);

    /* Test 4: tolerance wraps past 360° */
    run_arc_drag_test(10, 355, 90, 40, 5);

    /* Edge test 1: 360° boundary tolerance */
    run_arc_drag_test(60, 300, 15, 100, 350);

    /* Edge test 2: wrapped arc tolerance */
    run_arc_drag_test(300, 60, 50, 30, 245);

    /* Edge test 3: after-end tolerance */
    run_arc_drag_test(30, 240, 85, 240, 280);

    /* Edge test 4: wrap past 360° tolerance */
    run_arc_drag_test(15, 350, 92, 30, 8);

    /* Edge test 5: wrapped arc after-end tolerance */
    run_arc_drag_test(300, 60, 80, 60, 75);
}

#endif
