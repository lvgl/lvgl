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

#endif
