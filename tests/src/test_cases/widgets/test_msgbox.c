#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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
    lv_obj_clean(lv_layer_top()); /*Modal message boxes are created on the top layer*/
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

void test_msgbox_content_auto_height(void)
{
    /* If parent is NULL the message box will be modal*/
    msgbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(msgbox, "The title");
    lv_msgbox_add_text(msgbox, "The text");
    lv_msgbox_add_footer_button(msgbox, "Apply");
    lv_msgbox_add_footer_button(msgbox, "Close");
    lv_msgbox_add_header_button(msgbox, LV_SYMBOL_AUDIO);
    lv_msgbox_add_close_button(msgbox);

    /* Test1 : msgbox's height is LV_SIZE_CONTENT by default */
    bool is_height_size_content = (lv_obj_get_style_height(msgbox, 0) == LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL(is_height_size_content, 1);

    lv_obj_update_layout(msgbox);
    lv_obj_t * header = lv_msgbox_get_header(msgbox);
    lv_obj_t * footer = lv_msgbox_get_footer(msgbox);
    lv_obj_t * content = lv_msgbox_get_content(msgbox);

    int32_t h_header = (header == NULL) ? 0 : lv_obj_get_height(header);
    int32_t h_footer = (footer == NULL) ? 0 : lv_obj_get_height(footer);
    int32_t h_content = lv_obj_get_height(content);

    int32_t h_obj_content = lv_obj_get_content_height(msgbox);
    int32_t h_msgbox_element_sum  = h_header + h_footer + h_content;
    /* Default Size : The height of the msgbox's obj-content should be equal to the total height of the msgbox's element. */
    TEST_ASSERT_EQUAL(h_obj_content, h_msgbox_element_sum);

    /* Test2 : Now change size of msgbox manually*/
    lv_obj_set_size(msgbox, lv_pct(80), lv_pct(80));

    is_height_size_content = (lv_obj_get_style_height(msgbox, 0) == LV_SIZE_CONTENT);
    TEST_ASSERT_EQUAL(is_height_size_content, 0);

    lv_obj_update_layout(msgbox);
    h_header = (header == NULL) ? 0 : lv_obj_get_height(header);
    h_footer = (footer == NULL) ? 0 : lv_obj_get_height(footer);
    h_content = lv_obj_get_height(content);

    h_obj_content = lv_obj_get_content_height(msgbox);
    h_msgbox_element_sum  = h_header + h_footer + h_content;
    /* Manual Size : The height of the msgbox's obj-content should also be equal to the total height of the msgbox's element. */
    TEST_ASSERT_EQUAL(h_obj_content, h_msgbox_element_sum);
}

#endif
