#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

/* This function runs before each test */
void setUp(void);
/* This function runs after every test */
void tearDown(void);

void test_msgbox_creation_successful_with_close_button(void);
void test_msgbox_creation_successful_no_close_button(void);
void test_msgbox_creation_successful_modal(void);
void test_msgbox_get_title(void);
void test_msgbox_get_content(void);
void test_msgbox_close(void);
void test_msgbox_close_modal(void);
void test_msgbox_close_async(void);
void test_msgbox_close_async_modal(void);

static lv_obj_t * active_screen = NULL;
static lv_obj_t * msgbox = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_msgbox_creation_successful_with_close_button(void)
{
    msgbox = lv_msgbox_create(active_screen);
    lv_msgbox_add_title(msgbox, "The title");
    lv_msgbox_add_text(msgbox, "The text");
    lv_msgbox_add_footer_button(msgbox, "Apply");
    lv_msgbox_add_footer_button(msgbox, "Close");
    lv_msgbox_add_header_button(msgbox, LV_SYMBOL_AUDIO);
    lv_msgbox_add_close_button(msgbox);

    TEST_ASSERT_NOT_NULL(msgbox);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/msgbox_ok_with_close_btn.png");
}

void test_msgbox_creation_successful_no_close_button(void)
{
    msgbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(msgbox, "The title");
    lv_msgbox_add_text(msgbox, "The text");
    lv_msgbox_add_footer_button(msgbox, "Apply");
    lv_msgbox_add_footer_button(msgbox, "Close");
    lv_msgbox_add_header_button(msgbox, LV_SYMBOL_AUDIO);

    TEST_ASSERT_NOT_NULL(msgbox);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/msgbox_ok_no_close_btn.png");
}

void test_msgbox_creation_successful_modal(void)
{
    // If parent is NULL the message box will be modal
    msgbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(msgbox, "The title");
    lv_msgbox_add_text(msgbox, "The text");
    lv_msgbox_add_footer_button(msgbox, "Apply");
    lv_msgbox_add_footer_button(msgbox, "Close");
    lv_msgbox_add_header_button(msgbox, LV_SYMBOL_AUDIO);
    lv_msgbox_add_close_button(msgbox);

    TEST_ASSERT_NOT_NULL(msgbox);

    // Since msgbox has no parent, it won´t be clean up at tearDown()
    lv_obj_clean(msgbox);
}

void test_msgbox_get_title(void)
{
    const char * txt_title = "The title";
    lv_obj_t * lbl_title = NULL;

    msgbox = lv_msgbox_create(active_screen);
    lv_msgbox_add_title(msgbox, "The title");
    lv_msgbox_add_text(msgbox, "The text");
    lv_msgbox_add_footer_button(msgbox, "Apply");
    lv_msgbox_add_footer_button(msgbox, "Close");
    lv_msgbox_add_header_button(msgbox, LV_SYMBOL_AUDIO);
    lv_msgbox_add_close_button(msgbox);

    // Msgbox title is a lv_label widget
    lbl_title = lv_msgbox_get_title(msgbox);

    TEST_ASSERT_EQUAL_STRING(txt_title, lv_label_get_text(lbl_title));
}

void test_msgbox_get_content(void)
{
    msgbox = lv_msgbox_create(active_screen);

    TEST_ASSERT_NOT_NULL(lv_msgbox_get_content(msgbox));
}

void test_msgbox_close(void)
{
    msgbox = lv_msgbox_create(active_screen);
    lv_msgbox_add_text(msgbox, "The text");

    lv_msgbox_close(msgbox);

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

void test_msgbox_close_modal(void)
{
    msgbox = lv_msgbox_create(NULL);
    lv_msgbox_add_text(msgbox, "The text");

    lv_msgbox_close(msgbox);

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

void test_msgbox_close_async(void)
{
    msgbox = lv_msgbox_create(active_screen);
    lv_msgbox_add_text(msgbox, "The text");

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

void test_msgbox_close_async_modal(void)
{
    msgbox = lv_msgbox_create(NULL);
    lv_msgbox_add_text(msgbox, "The text");

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

#endif
