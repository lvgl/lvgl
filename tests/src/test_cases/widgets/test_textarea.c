#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * textarea = NULL;

static const char * textarea_default_text = "";

void setUp(void)
{
    active_screen = lv_screen_active();
    textarea = lv_textarea_create(active_screen);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_textarea_should_have_valid_documented_default_values(void)
{
    TEST_ASSERT(lv_textarea_get_cursor_click_pos(textarea));
    TEST_ASSERT_EQUAL(0U, lv_textarea_get_one_line(textarea));
    /* No placeholder text should be set on widget creation */
    TEST_ASSERT_EQUAL_STRING(textarea_default_text, lv_textarea_get_placeholder_text(textarea));
    TEST_ASSERT_EQUAL_STRING(textarea_default_text, lv_textarea_get_text(textarea));
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
    lv_textarea_t * txt_ptr = (lv_textarea_t *) textarea;

    lv_textarea_add_text(textarea, "Hi");
    lv_textarea_set_one_line(textarea, true);

    int32_t left_padding = lv_obj_get_style_pad_left(txt_ptr->label, LV_PART_MAIN);
    int32_t right_padding = lv_obj_get_style_pad_right(txt_ptr->label, LV_PART_MAIN);
    int32_t line_width = lv_obj_get_width(txt_ptr->label);
    int32_t expected_size = left_padding + right_padding + line_width;

    TEST_ASSERT(lv_textarea_get_one_line(textarea));
    TEST_ASSERT_EQUAL_UINT16(expected_size, lv_obj_get_width(txt_ptr->label));
    TEST_ASSERT_EQUAL_UINT16(lv_pct(100), lv_obj_get_style_min_width(txt_ptr->label, LV_PART_MAIN));
}

void test_textarea_cursor_click_pos_field_update(void)
{
    lv_textarea_set_cursor_click_pos(textarea, false);

    TEST_ASSERT_FALSE(lv_textarea_get_cursor_click_pos(textarea));
}

void test_textarea_should_scroll_to_the_end(void)
{
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_add_text(textarea, "Hi this is a long text to test if the textarea scrolls to the end");
    lv_obj_set_width(textarea, LV_DPI_DEF * 3);

    int32_t cur_pos = (int32_t)lv_textarea_get_cursor_pos(textarea);
    const lv_font_t * font = lv_obj_get_style_text_font(textarea, LV_PART_MAIN);
    int32_t font_h = lv_font_get_line_height(font);
    int32_t w = lv_obj_get_content_width(textarea);
    if(cur_pos + font_h - lv_obj_get_scroll_left(textarea) > w) {
        TEST_ASSERT_EQUAL_INT32(lv_obj_get_scroll_x(textarea) + 10, cur_pos - w + font_h);
    }

    TEST_ASSERT(lv_textarea_get_one_line(textarea));
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

void test_textarea_should_keep_only_accepted_chars(void)
{
    const char * accepted_list = "abcd";

    lv_textarea_set_accepted_chars(textarea, accepted_list);
    lv_textarea_set_text(textarea, "abcde");

    TEST_ASSERT_EQUAL_STRING(accepted_list, lv_textarea_get_text(textarea));
}

void test_textarea_in_one_line_mode_should_ignore_line_break_characters(void)
{
    lv_textarea_set_one_line(textarea, true);

    lv_textarea_add_char(textarea, '\n');
    TEST_ASSERT_EQUAL_STRING(textarea_default_text, lv_textarea_get_text(textarea));

    lv_textarea_add_char(textarea, '\r');
    TEST_ASSERT_EQUAL_STRING(textarea_default_text, lv_textarea_get_text(textarea));
}

void test_textarea_should_hide_password_characters(void)
{
    lv_textarea_set_password_mode(textarea, true);
    lv_textarea_set_text(textarea, "12345");

    /* setting bullet hides characters */
    lv_textarea_set_password_bullet(textarea, "O");
    TEST_ASSERT_EQUAL_STRING("OOOOO", lv_label_get_text(lv_textarea_get_label(textarea)));

    /* setting text hides characters */
    lv_textarea_set_text(textarea, "A");
    TEST_ASSERT_EQUAL_STRING("O", lv_label_get_text(lv_textarea_get_label(textarea)));

    lv_textarea_add_char(textarea, 'B');
    TEST_ASSERT_EQUAL_STRING("OB", lv_label_get_text(lv_textarea_get_label(textarea)));

    /* setting show time hides characters */
    /* current behavior is to hide the characters upon setting the show time regardless of the value */
    lv_textarea_set_password_show_time(textarea, lv_textarea_get_password_show_time(textarea));
    TEST_ASSERT_EQUAL_STRING("OO", lv_label_get_text(lv_textarea_get_label(textarea)));

    lv_textarea_set_password_mode(textarea, false);
    TEST_ASSERT_EQUAL_STRING("AB", lv_label_get_text(lv_textarea_get_label(textarea)));

    /* enabling password mode hides characters */
    lv_textarea_set_password_mode(textarea, true);
    TEST_ASSERT_EQUAL_STRING("OO", lv_label_get_text(lv_textarea_get_label(textarea)));
}

void test_textarea_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_property_t prop = { };
    lv_obj_t * obj = lv_textarea_create(lv_screen_active());

    prop.id = LV_PROPERTY_TEXTAREA_TEXT;
    prop.ptr = "Hello World!";
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_STRING("Hello World!", lv_textarea_get_text(obj));
    TEST_ASSERT_EQUAL_STRING("Hello World!", lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_TEXT).ptr);

    prop.id = LV_PROPERTY_TEXTAREA_PLACEHOLDER_TEXT;
    prop.ptr = "Hello!";
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_STRING("Hello!", lv_textarea_get_placeholder_text(obj));
    TEST_ASSERT_EQUAL_STRING("Hello!", lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_PLACEHOLDER_TEXT).ptr);

    prop.id = LV_PROPERTY_TEXTAREA_CURSOR_POS;
    prop.num = 5;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_INT(5, lv_textarea_get_cursor_pos(obj));
    TEST_ASSERT_EQUAL_INT(5, lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_CURSOR_POS).num);

    prop.id = LV_PROPERTY_TEXTAREA_CURSOR_CLICK_POS;
    prop.num = 1;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_INT(1, lv_textarea_get_cursor_click_pos(obj));
    TEST_ASSERT_EQUAL_INT(1, lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_CURSOR_CLICK_POS).num);

    prop.id = LV_PROPERTY_TEXTAREA_PASSWORD_MODE;
    prop.num = true;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_TRUE(lv_textarea_get_password_mode(obj));
    TEST_ASSERT_TRUE(lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_PASSWORD_MODE).num);

    prop.id = LV_PROPERTY_TEXTAREA_PASSWORD_BULLET;
    prop.ptr = "password bullet";
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_STRING("password bullet", lv_textarea_get_password_bullet(obj));
    TEST_ASSERT_EQUAL_STRING("password bullet", lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_PASSWORD_BULLET).ptr);

    prop.id = LV_PROPERTY_TEXTAREA_ONE_LINE;
    prop.enable = true;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_INT(true, lv_textarea_get_one_line(obj));
    TEST_ASSERT_EQUAL_INT(true, lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_ONE_LINE).enable);

    prop.id = LV_PROPERTY_TEXTAREA_ACCEPTED_CHARS;
    prop.ptr = "ABCDEF";
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_STRING("ABCDEF", lv_textarea_get_accepted_chars(obj));
    TEST_ASSERT_EQUAL_STRING("ABCDEF", lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_ACCEPTED_CHARS).ptr);

    prop.id = LV_PROPERTY_TEXTAREA_MAX_LENGTH;
    prop.num = 10;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_INT(10, lv_textarea_get_max_length(obj));
    TEST_ASSERT_EQUAL_INT(10, lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_MAX_LENGTH).num);

    prop.id = LV_PROPERTY_TEXTAREA_INSERT_REPLACE;
    prop.ptr = "abcdef";
    /*No getter function for this property*/
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);

    prop.id = LV_PROPERTY_TEXTAREA_TEXT_SELECTION;
    prop.num = true;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_INT(true, lv_textarea_get_text_selection(obj));
    TEST_ASSERT_EQUAL_INT(true, lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_TEXT_SELECTION).enable);

    prop.id = LV_PROPERTY_TEXTAREA_PASSWORD_SHOW_TIME;
    prop.num = 10;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RES_OK);
    TEST_ASSERT_EQUAL_INT(10, lv_textarea_get_password_show_time(obj));
    TEST_ASSERT_EQUAL_INT(10, lv_obj_get_property(obj, LV_PROPERTY_TEXTAREA_PASSWORD_SHOW_TIME).num);
#endif
}

#endif
