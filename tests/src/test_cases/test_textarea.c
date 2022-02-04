#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * textarea = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    textarea = lv_textarea_create(active_screen);
}

void tearDown(void)
{
    /* Function run after every test */
}

/* When in password mode the lv_textarea_get_text function returns
 * the actual text, not the bullet characters. */
void test_textarea_should_return_actual_text_when_password_mode_is_enabled(void)
{
    const char * text = "Hello LVGL!";

    lv_textarea_add_text(textarea, text);
    lv_textarea_set_password_mode(textarea, true);

    TEST_ASSERT_TRUE(lv_textarea_get_password_mode(textarea));
    TEST_ASSERT_EQUAL_STRING(text, lv_textarea_get_text(textarea));
}

void test_textarea_should_update_label_style_with_one_line_enabled(void)
{
    TEST_ASSERT_EQUAL(0U, lv_textarea_get_one_line(textarea));

    lv_textarea_add_text(textarea, "Hi");
    lv_textarea_set_one_line(textarea, true);

    TEST_ASSERT(lv_textarea_get_one_line(textarea));
    // TEST_ASSERT_EQUAL(LV_SIZE_CONTENT, label_width)
    // TEST_ASSERT_EQUAL(lv_pct(100), label_style_min_width)
}

void test_textarea_cursor_click_pos_field_update(void)
{
    lv_textarea_set_cursor_click_pos(textarea, false);

    TEST_ASSERT_FALSE(lv_textarea_get_cursor_click_pos(textarea));
}

void test_textarea_should_update_placeholder_text(void)
{
    const char * new_placeholder = "LVGL Rocks!!!!!";
    const char * text = "Hello LVGL!";

    /* Allocating memory for placeholder text */
    lv_textarea_set_placeholder_text(textarea, text);
    TEST_ASSERT_EQUAL_STRING(text, lv_textarea_get_placeholder_text(textarea));

    /* Reallocating memory for the new placeholder text */
    lv_textarea_set_placeholder_text(textarea, new_placeholder);
    TEST_ASSERT_EQUAL_STRING(new_placeholder, lv_textarea_get_placeholder_text(textarea));

    /* Freeing allocated memory for placeholder text */
    lv_textarea_set_placeholder_text(textarea, "");
    TEST_ASSERT_EQUAL_STRING("", lv_textarea_get_placeholder_text(textarea));
}

#endif
