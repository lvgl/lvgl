#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * label;

void setUp(void)
{
    active_screen = lv_scr_act();
    label = lv_label_create(active_screen);
}

void tearDown(void)
{
    lv_obj_del(label);
}

void test_label_creation(void)
{
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), LV_LABEL_DEFAULT_TEXT);
    TEST_ASSERT_EQUAL(lv_label_get_long_mode(label), LV_LABEL_LONG_WRAP);
}

void test_label_recolor(void)
{
    lv_label_set_recolor(label, true);
    TEST_ASSERT(lv_label_get_recolor(label));

    lv_label_set_recolor(label, false);
    TEST_ASSERT_FALSE(lv_label_get_recolor(label));
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

void test_label_long_mode(void)
{
    /* LV_LABEL_LONG_WRAP, LV_LABEL_LONG_DOT, LV_LABEL_LONG_SCROLL, LV_LABEL_LONG_SCROLL_CIRCULAR, LV_LABEL_LONG_CLIP */
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
}

#endif
