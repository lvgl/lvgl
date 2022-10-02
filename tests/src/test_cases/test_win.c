#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * win = NULL;
static lv_obj_t * header = NULL;
static lv_obj_t * content = NULL;

const char * dummy_text = "Hello LVGL!";

void setUp(void)
{
    active_screen = lv_scr_act();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}


void test_win_should_have_valid_documented_defualt_values(void)
{
    // Header Height must be provided
    int win_header_height = 60;

    // Create the win object and update layout
    win = lv_win_create(active_screen, win_header_height);
    lv_obj_update_layout(win);

    // Check that the window has been created
    TEST_ASSERT_NOT_NULL(win);

    // Check that the correct children have been created
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(win));

    header = lv_win_get_header(win);
    content = lv_win_get_content(win);

    TEST_ASSERT_EQUAL(header, lv_obj_get_child(win, 0));
    TEST_ASSERT_EQUAL(content, lv_obj_get_child(win, 1));

    // Check that the header is correctly sized and empty
    TEST_ASSERT_EQUAL(win_header_height, lv_obj_get_height(header));
    TEST_ASSERT_EQUAL(0, lv_obj_get_child_cnt(header));

    // Check that the content is empty
    TEST_ASSERT_EQUAL(0, lv_obj_get_child_cnt(content));
}

void test_win_add_title_single(void)
{
    // Header Height must be provided
    int win_header_height = 60;

    // Create the win object, get the header and update layout
    win = lv_win_create(active_screen, win_header_height);
    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Add a title to the window
    lv_win_add_title(win, dummy_text);

    // Check that no additional children have been created under win
    // Instead the child should be created under header
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(win));
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(header));

    // Check that the title is a label and has been created properly
    lv_obj_t * title = lv_obj_get_child(header, 0);
    TEST_ASSERT_EQUAL_STRING(dummy_text, lv_label_get_text(title));
    TEST_ASSERT_EQUAL(1, lv_label_get_long_mode(title));
}

void test_win_add_title_multiple(void)
{
    // Header Height must be provided
    int win_header_height = 60;

    // Create the win object, get the header and update layout
    win = lv_win_create(active_screen, win_header_height);
    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Add two titles to the window
    lv_win_add_title(win, dummy_text);
    lv_win_add_title(win, dummy_text);

    // Check that no additional children have been created under win
    // Instead the child should be created under header
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(win));
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(header));
}

void test_win_add_btn(void)
{
    // Header Height must be provided
    int win_header_height = 60;
    int win_btn_width = 50;

    // Create the win object, get the header and update layout
    win = lv_win_create(active_screen, win_header_height);
    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Add a button to the window header
    lv_win_add_btn(win, LV_SYMBOL_OK, win_btn_width);
    lv_obj_update_layout(win);

    // Check that no additional children have been created under win
    // Instead the child should be created under header
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(win));
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(header));

    // Check that the button has been created properly
    lv_obj_t * btn = lv_obj_get_child(header, 0);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(btn));
    TEST_ASSERT_EQUAL(win_btn_width, lv_obj_get_width(btn));

    // Check the output remains visualy consistent
    TEST_ASSERT_EQUAL_SCREENSHOT("win_01.png");
}

void test_win_add_multiple_elements(void)
{
    lv_obj_t * btn;
    lv_obj_t * title;

    // Header Height must be provided
    int win_header_height = 60;
    int win_btn_width = 50;
    int win_btn_close_width = 60;

    // Create the win object, get the header and update layout
    win = lv_win_create(active_screen, win_header_height);
    lv_win_add_btn(win, LV_SYMBOL_LEFT, win_btn_width);
    lv_win_add_title(win, dummy_text);
    lv_win_add_btn(win, LV_SYMBOL_RIGHT, win_btn_width);
    lv_win_add_btn(win, LV_SYMBOL_CLOSE, win_btn_close_width);

    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Check that no additional children have been created under win
    // Instead the child should be created under header
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(win));
    TEST_ASSERT_EQUAL(4, lv_obj_get_child_cnt(header));

    // Check that the left button has been created properly
    btn = lv_obj_get_child(header, 0);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(btn));
    TEST_ASSERT_EQUAL(win_btn_width, lv_obj_get_width(btn));

    // Check that the title is a label and has been created properly
    title = lv_obj_get_child(header, 1);
    TEST_ASSERT_EQUAL_STRING(dummy_text, lv_label_get_text(title));
    TEST_ASSERT_EQUAL(1, lv_label_get_long_mode(title));

    // Check that the right button has been created properly
    btn = lv_obj_get_child(header, 2);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(btn));
    TEST_ASSERT_EQUAL(win_btn_width, lv_obj_get_width(btn));

    // Check that the close button has been created properly
    btn = lv_obj_get_child(header, 3);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(btn));
    TEST_ASSERT_EQUAL(win_btn_close_width, lv_obj_get_width(btn));

    // Check the output remains visualy consistent
    TEST_ASSERT_EQUAL_SCREENSHOT("win_02.png");
}

#endif
