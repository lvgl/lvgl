#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <string.h>

static const char * long_text =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras malesuada ultrices magna in rutrum.";
static const char * long_text_multiline =
    "Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit.\nCras malesuada ultrices magna in rutrum.\n";
static const char * empty_text = "";

static lv_obj_t * active_screen = NULL;
static lv_obj_t * label;
static lv_obj_t * long_label;
static lv_obj_t * long_label_multiline;
static lv_obj_t * empty_label;

void setUp(void)
{
    active_screen = lv_screen_active();
    label = lv_label_create(active_screen);
    long_label = lv_label_create(active_screen);
    long_label_multiline = lv_label_create(active_screen);
    empty_label = lv_label_create(active_screen);

    lv_label_set_text(long_label, long_text);
    lv_label_set_text(long_label_multiline, long_text_multiline);
    lv_label_set_text(empty_label, empty_text);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_label_creation(void)
{
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), LV_LABEL_DEFAULT_TEXT);
    TEST_ASSERT_EQUAL(lv_label_get_long_mode(label), LV_LABEL_LONG_MODE_WRAP);
}

void test_label_set_text(void)
{
    const char * new_text = "Hello world";

    /* Refresh with the current text. */
    lv_label_set_text(label, NULL);
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), LV_LABEL_DEFAULT_TEXT);

    /* '\0' terminated character string to set new text. */
    lv_label_set_text(label, new_text);
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), new_text);
}

void test_label_get_letter_pos_align_left(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = 0,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 29,
        .y = 0
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label_multiline)) - 1; /* char index starts at 0 */

    lv_label_get_letter_pos(label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_get_letter_pos_align_left_on_empty_text(void)
{
    lv_obj_set_style_text_align(empty_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = 0,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 0,
        .y = 0
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(empty_label)) - 1;

    lv_label_get_letter_pos(empty_label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(empty_label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_long_text_multiline_get_letter_pos_align_left(void)
{
    lv_obj_set_style_text_align(long_label_multiline, LV_TEXT_ALIGN_LEFT, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    lv_point_t last_letter_after_new_line_point;
    const lv_point_t expected_first_letter_point = {
        .x = 0,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 305,
        .y = 32
    };
    const lv_point_t expected_last_letter_after_new_line_point = {
        .x = 0,
        .y = 48
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label_multiline)) - 1; /* char index starts at 0 */
    const uint32_t last_letter_after_new_line = strlen(lv_label_get_text(long_label_multiline));

    lv_label_get_letter_pos(long_label_multiline, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(long_label_multiline, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);

    lv_label_get_letter_pos(long_label_multiline, last_letter_after_new_line, &last_letter_after_new_line_point);

    TEST_ASSERT_EQUAL(expected_last_letter_after_new_line_point.x, last_letter_after_new_line_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_after_new_line_point.y, last_letter_after_new_line_point.y);
}

void test_label_long_text_get_letter_pos_align_left(void)
{
    lv_label_set_long_mode(long_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(long_label, 150);
    lv_obj_set_height(long_label, 500);
    lv_obj_set_style_text_align(long_label, LV_TEXT_ALIGN_LEFT, 0);

    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = 0,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 0,
        .y = 1536
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label)) - 1; /* char index starts at 0 */

    lv_label_get_letter_pos(long_label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(long_label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_get_letter_pos_align_right(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = -29,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 0,
        .y = 0
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label_multiline)) - 1; /* char index starts at 0 */

    lv_label_get_letter_pos(label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_get_letter_pos_align_right_on_empty_text(void)
{
    lv_obj_set_style_text_align(empty_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = 0,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 0,
        .y = 0
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(empty_label)) - 1;

    lv_label_get_letter_pos(empty_label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(empty_label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_long_text_multiline_get_letter_pos_align_right(void)
{
    lv_obj_set_style_text_align(long_label_multiline, LV_TEXT_ALIGN_RIGHT, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    lv_point_t last_letter_after_new_line_point;
    const lv_point_t expected_first_letter_point = {
        .x = -205,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 0,
        .y = 32
    };
    const lv_point_t expected_last_letter_after_new_line_point = {
        .x = 0,
        .y = 48
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label_multiline)) - 1; /* char index starts at 0 */
    const uint32_t last_letter_after_new_line = strlen(lv_label_get_text(long_label_multiline));

    lv_label_get_letter_pos(long_label_multiline, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(long_label_multiline, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);

    lv_label_get_letter_pos(long_label_multiline, last_letter_after_new_line, &last_letter_after_new_line_point);

    TEST_ASSERT_EQUAL(expected_last_letter_after_new_line_point.x, last_letter_after_new_line_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_after_new_line_point.y, last_letter_after_new_line_point.y);
}

void test_label_long_text_get_letter_pos_align_right(void)
{
    lv_label_set_long_mode(long_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(long_label, 150);
    lv_obj_set_height(long_label, 500);
    lv_obj_set_style_text_align(long_label, LV_TEXT_ALIGN_RIGHT, 0);

    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = -8,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = -3,
        .y = 1536
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label)) - 1; /* char index starts at 0 */

    lv_label_get_letter_pos(long_label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(long_label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_get_letter_pos_align_center(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = -14,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 15,
        .y = 0
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label_multiline)) - 1; /* char index starts at 0 */

    lv_label_get_letter_pos(label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_get_letter_pos_align_center_on_empty_text(void)
{
    lv_obj_set_style_text_align(empty_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = 0,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 0,
        .y = 0
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(empty_label)) - 1;

    lv_label_get_letter_pos(empty_label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(empty_label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_long_text_multiline_get_letter_pos_align_center(void)
{
    lv_obj_set_style_text_align(long_label_multiline, LV_TEXT_ALIGN_CENTER, 0);
    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    lv_point_t last_letter_after_new_line_point;
    const lv_point_t expected_first_letter_point = {
        .x = -102,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = 153,
        .y = 32
    };
    const lv_point_t expected_last_letter_after_new_line_point = {
        .x = 0,
        .y = 48
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label_multiline)) - 1; /* char index starts at 0 */
    const uint32_t last_letter_after_new_line = strlen(lv_label_get_text(long_label_multiline));

    lv_label_get_letter_pos(long_label_multiline, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(long_label_multiline, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);

    lv_label_get_letter_pos(long_label_multiline, last_letter_after_new_line, &last_letter_after_new_line_point);

    TEST_ASSERT_EQUAL(expected_last_letter_after_new_line_point.x, last_letter_after_new_line_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_after_new_line_point.y, last_letter_after_new_line_point.y);
}

void test_label_long_text_get_letter_pos_align_center(void)
{
    lv_label_set_long_mode(long_label, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(long_label, 150);
    lv_obj_set_height(long_label, 500);
    lv_obj_set_style_text_align(long_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_point_t first_letter_point;
    lv_point_t last_letter_point;
    const lv_point_t expected_first_letter_point = {
        .x = -4,
        .y = 0
    };
    const lv_point_t expected_last_letter_point = {
        .x = -1,
        .y = 1536
    };

    const uint32_t first_letter_idx = 0;
    const uint32_t last_letter_idx = strlen(lv_label_get_text(long_label)) - 1; /* char index starts at 0 */

    lv_label_get_letter_pos(long_label, first_letter_idx, &first_letter_point);

    TEST_ASSERT_EQUAL(expected_first_letter_point.x, first_letter_point.x);
    TEST_ASSERT_EQUAL(expected_first_letter_point.y, first_letter_point.y);

    lv_label_get_letter_pos(long_label, last_letter_idx, &last_letter_point);

    TEST_ASSERT_EQUAL(expected_last_letter_point.x, last_letter_point.x);
    TEST_ASSERT_EQUAL(expected_last_letter_point.y, last_letter_point.y);
}

void test_label_is_char_under_pos_align_left(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    bool expected_result = true;
    bool result = false;
    lv_point_t last_letter_point;

    /* Getting the last letter point */
    const uint32_t last_letter_idx = strlen(lv_label_get_text(label)) - 1;
    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    /* We should get true, since we know the last letter is in this coordinate */
    result = lv_label_is_char_under_pos(label, &last_letter_point);
    TEST_ASSERT_EQUAL(expected_result, result);

    /* If we test a move away from the last letter then we should get false */
    expected_result = false;
    last_letter_point.x += 50;

    result = lv_label_is_char_under_pos(label, &last_letter_point);
    TEST_ASSERT_EQUAL(expected_result, result);
}

void test_label_is_char_under_pos_align_right(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    bool expected_result = true;
    bool result = false;
    lv_point_t last_letter_point;

    /* Getting the last letter point */
    const uint32_t last_letter_idx = strlen(lv_label_get_text(label)) - 1;
    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    /* We should get true, since we know the last letter is in this coordinate */
    result = lv_label_is_char_under_pos(label, &last_letter_point);
    TEST_ASSERT_EQUAL(expected_result, result);

    /* If we test a move away from the last letter then we should get false */
    expected_result = false;
    last_letter_point.x -= 50;

    result = lv_label_is_char_under_pos(label, &last_letter_point);
    TEST_ASSERT_EQUAL(expected_result, result);
}

void test_label_is_char_under_pos_align_center(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    bool expected_result = true;
    bool result = false;
    lv_point_t last_letter_point;

    /* Getting the last letter point */
    const uint32_t last_letter_idx = strlen(lv_label_get_text(label)) - 1;
    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    /* We should get true, since we know the last letter is in this coordinate */
    result = lv_label_is_char_under_pos(label, &last_letter_point);
    TEST_ASSERT_EQUAL(expected_result, result);

    /* If we test a move away from the last letter then we should get false */
    expected_result = false;
    last_letter_point.x -= 50;

    result = lv_label_is_char_under_pos(label, &last_letter_point);
    TEST_ASSERT_EQUAL(expected_result, result);
}

void test_label_cut_text(void)
{
    const char * expected_text = "Te";
    /* label has the default text which is "Text", let's remove "xt" from it */
    lv_label_cut_text(label, 2, 2);

    TEST_ASSERT_EQUAL_STRING(expected_text, lv_label_get_text(label));
}

void test_label_get_letter_on_left(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_point_t last_letter_point;

    const uint32_t last_letter_idx = strlen(lv_label_get_text(label)) - 1;
    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    uint32_t letter_idx_result = lv_label_get_letter_on(label, &last_letter_point, true);

    TEST_ASSERT_EQUAL(last_letter_idx, letter_idx_result);
}

void test_label_get_letter_on_center(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_point_t last_letter_point;

    const uint32_t last_letter_idx = strlen(lv_label_get_text(label)) - 1;
    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    uint32_t letter_idx_result = lv_label_get_letter_on(label, &last_letter_point, true);

    TEST_ASSERT_EQUAL(last_letter_idx, letter_idx_result);
}

void test_label_get_letter_on_right(void)
{
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_point_t last_letter_point;

    const uint32_t last_letter_idx = strlen(lv_label_get_text(label)) - 1;
    lv_label_get_letter_pos(label, last_letter_idx, &last_letter_point);

    uint32_t letter_idx_result = lv_label_get_letter_on(label, &last_letter_point, true);

    TEST_ASSERT_EQUAL(last_letter_idx, letter_idx_result);
}

void test_label_text_selection(void)
{
    const uint32_t selection_start = 0;
    const uint32_t selection_end = strlen(lv_label_get_text(label)) - 1;

    lv_label_set_text_selection_start(label, selection_start);
    lv_label_set_text_selection_end(label, selection_end);

    uint32_t start = lv_label_get_text_selection_start(label);
    uint32_t end = lv_label_get_text_selection_end(label);

    TEST_ASSERT_EQUAL(selection_start, start);
    TEST_ASSERT_EQUAL(selection_end, end);
}

void test_label_rtl_dot_long_mode(void)
{
    const char * message =
        "מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).";

    lv_obj_t * screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, 800, 480);
    lv_obj_center(screen);
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_100, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    lv_obj_t * test_label = lv_label_create(screen);
    lv_obj_set_style_text_font(test_label, &lv_font_dejavu_16_persian_hebrew, 0);
    lv_label_set_long_mode(test_label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_base_dir(test_label, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(test_label, 300, lv_font_dejavu_16_persian_hebrew.line_height);
    lv_label_set_text(test_label, message);
    lv_obj_center(test_label);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/label_rtl_dot_long_mode.png");
}

void test_label_max_width(void)
{
    lv_obj_clean(lv_screen_active());

    lv_obj_t * test_label1 = lv_label_create(lv_screen_active());
    lv_label_set_text(test_label1, long_text);
    lv_obj_set_width(test_label1, 600);
    lv_obj_set_style_max_width(test_label1, 200, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_label1, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_label1, LV_OPA_100, LV_PART_MAIN);

    lv_obj_t * test_label2 = lv_label_create(lv_screen_active());
    lv_label_set_text(test_label2, long_text);
    lv_obj_set_width(test_label2, 600);
    lv_obj_set_height(test_label2, 50);
    lv_obj_set_x(test_label2, 300);
    lv_obj_set_style_max_width(test_label2, 200, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_label2, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_label2, LV_OPA_100, LV_PART_MAIN);
    lv_label_set_long_mode(test_label2, LV_LABEL_LONG_MODE_DOTS);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/label_max_width.png");
}

void test_label_dots(void)
{
    lv_obj_clean(lv_screen_active());

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * label1 = lv_label_create(cont);
    lv_obj_t * label2 = lv_label_create(cont);
    lv_obj_t * label3 = lv_label_create(cont);
    lv_obj_t * label4 = lv_label_create(cont);

    lv_obj_set_size(label1, 150, 30);
    lv_obj_set_size(label2, 150, 30);
    lv_obj_set_size(label3, 150, 30);
    lv_obj_set_size(label4, 150, 30);

    const char * temp_text = "Some other text";

    lv_label_set_text(label1, long_text);
    lv_label_set_text(label2, temp_text);

    lv_label_set_text_fmt(label3, "%.*s", (int)strlen(long_text), long_text);
    lv_label_set_text_fmt(label4, "%.*s", (int)strlen(temp_text), temp_text);

    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_long_mode(label3, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_long_mode(label4, LV_LABEL_LONG_MODE_DOTS);

    /* Setting the text with _fmt when long mode was dots would cause the incorrect text to be displayed */
    lv_label_set_text(label2, long_text);
    lv_label_set_text_fmt(label4, "%.*s", (int)strlen(long_text), long_text);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/label_dots.png");
}

void test_label_with_recolor_cmd(void)
{
    lv_obj_clean(lv_screen_active());

    lv_obj_t * label_recolor = lv_label_create(lv_screen_active());
    lv_label_set_text(label_recolor, "Write a #ff0000 red# word");
    lv_label_set_recolor(label_recolor, true);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/label_recolor.png");
}

void test_label_recolor_with_text_wrap(void)
{
    lv_obj_clean(lv_screen_active());

    /* Create a label with recolor enabled and text wrapping */
    lv_obj_t * label_recolor_wrap = lv_label_create(lv_screen_active());

    /* Configure the label similar to the bug report */
    lv_obj_set_style_text_font(label_recolor_wrap, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(label_recolor_wrap, lv_color_hex(0x8199f7), 0);
    lv_obj_set_width(label_recolor_wrap, 300); /* Fixed width to force wrapping */
    lv_obj_set_style_text_align(label_recolor_wrap, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(label_recolor_wrap, LV_LABEL_LONG_MODE_WRAP);
    lv_label_set_recolor(label_recolor_wrap, true);

    /* Test text with recolor commands that should wrap across multiple lines */
    const char * test_text_with_recolor =
        "Before color. #ff0000 This is a very long red colored text that should maintain its red color even when it wraps across multiple lines due to the label width being too narrow to contain the entire text on a single line# After color.";

    lv_label_set_text(label_recolor_wrap, test_text_with_recolor);
    lv_obj_align(label_recolor_wrap, LV_ALIGN_TOP_MID, 0, 20);

    /* Create a reference label without recolor for comparison */
    lv_obj_t * label_no_recolor = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label_no_recolor, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(label_no_recolor, lv_color_hex(0x8199f7), 0);
    lv_obj_set_width(label_no_recolor, 300);
    lv_obj_set_style_text_align(label_no_recolor, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(label_no_recolor, LV_LABEL_LONG_MODE_WRAP);
    lv_label_set_recolor(label_no_recolor, false); /* Disabled for comparison */

    /* Same text but without recolor commands */
    const char * test_text_without_recolor =
        "Before color. This is a very long red colored text that should maintain its red color even when it wraps across multiple lines due to the label width being too narrow to contain the entire text on a single line After color.";

    lv_label_set_text(label_no_recolor, test_text_without_recolor);
    lv_obj_align(label_no_recolor, LV_ALIGN_BOTTOM_MID, 0, -20);

    /* The bug would manifest as improper recolor handling when text wraps */
    /* This test verifies that recolor commands work correctly with text wrapping */
    /* No assertion needed - the test passes if no crashes occur during rendering */
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/label_recolor_2.png");
}

static void scroll_next_step(lv_obj_t * label1, lv_obj_t * label2, const char * text1, const char * text2, uint32_t idx)
{
    lv_label_set_text(label1, (idx % 2) == 0 ? text1 : text2);
    lv_label_set_text(label2, (idx % 2) == 0 ? text1 : text2);

    char buf[128];
    lv_snprintf(buf, sizeof(buf), "widgets/label_scroll_%d.png", idx);
    TEST_ASSERT_EQUAL_SCREENSHOT(buf);

    lv_test_wait(783); /*Use an odd delay*/
}

void test_label_scroll_mid_update(void)
{
    lv_obj_clean(lv_screen_active());

    const char * text1 = "This is a long text that we will update while scrolling";
    const char * text2 = "THIS IS A LONG TEXT THAT WE WILL UPDATE WHILE SCROLLING";

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_SCROLL);
    lv_label_set_text(label1, text1);
    lv_obj_set_width(label1, 150);
    lv_obj_set_pos(label1, 10, 10);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_label_set_text(label2, text1);
    lv_obj_set_width(label2, 150);
    lv_obj_set_pos(label2, 10, 80);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        scroll_next_step(label1, label2, text1, text2, i);
    }
}

/*
 * For the LONG_MODE_CLIP mode, just re-use the screenshot from the initial
 * screen of the long mode scroll
 */

void test_label_long_mode_clip(void)
{
    lv_obj_clean(lv_screen_active());

    const char * text1 = "This is a long text that we will update while scrolling";

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_CLIP);
    lv_label_set_text(label1, text1);
    lv_obj_set_width(label1, 150);
    lv_obj_set_pos(label1, 10, 10);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_MODE_CLIP);
    lv_label_set_text(label2, text1);
    lv_obj_set_width(label2, 150);
    lv_obj_set_pos(label2, 10, 80);

    char buf[128];
    lv_snprintf(buf, sizeof(buf), "widgets/label_scroll_0.png");
    TEST_ASSERT_EQUAL_SCREENSHOT(buf);
}

void test_label_wrap_mode_clip(void)
{
    lv_obj_clean(lv_screen_active());

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_WRAP);
    lv_label_set_text(label1, "This is a long text that should be clipped vertically");
    lv_obj_set_size(label1, 150, 25);
    lv_obj_set_pos(label1, 10, 10);
    lv_obj_set_style_border_width(label1, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(label1, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);

    char buf[128];
    lv_snprintf(buf, sizeof(buf), "widgets/label_wrap_clip.png");
    TEST_ASSERT_EQUAL_SCREENSHOT(buf);
}
void test_label_translation_tag(void)
{
    static const char * tags[] = {"tiger", NULL};
    static const char * languages[]    = {"en", "de", "es", NULL};
    static const char * translations[] = { "The Tiger", "Der Tiger", "El Tigre" };
    lv_translation_add_static(languages, tags, translations);
    label = lv_label_create(NULL);
    lv_label_set_translation_tag(label, "tiger");

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "The Tiger");

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "El Tigre");

    /* Unknown language translates to the tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "tiger");
}

void test_label_setting_text_disables_translation(void)
{
    static const char * tags[] = {"tiger", NULL};
    static const char * languages[]    = {"en", "de", "es", NULL};
    static const char * translations[] = { "The Tiger", "Der Tiger", "El Tigre" };
    lv_translation_add_static(languages, tags, translations);
    label = lv_label_create(NULL);
    lv_label_set_translation_tag(label, "tiger");

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "The Tiger");

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");

    /* Using set text should unbind the translation tag*/
    lv_label_set_text(label, "Hello world");
    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Hello world");

    lv_label_set_translation_tag(label, "tiger");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");

    /* Using set text static should unbind the translation tag*/
    lv_label_set_text_static(label, "Hello world");
    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Hello world");
    lv_label_set_translation_tag(label, "tiger");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "The Tiger");

    /* Using set text fmt should unbind the translation tag*/
    lv_label_set_text_fmt(label, "Hello world %d", 1);
    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Hello world 1");
    lv_label_set_translation_tag(label, "tiger");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");
}

#endif
