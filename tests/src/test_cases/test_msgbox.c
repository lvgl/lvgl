#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

/* This function runs before each test */
void setUp(void);
/* This function runs after every test */
void tearDown(void);

void test_msgbox_creation_successfull_with_close_btn(void);
void test_msgbox_creation_successfull_no_close_btn(void);
void test_msgbox_creation_successfull_modal(void);
void test_msgbox_get_title(void);
void test_msgbox_get_close_btn(void);
void test_msgbox_get_text(void);
void test_msgbox_get_content(void);
void test_msgbox_get_btns(void);
void test_msgbox_get_active_btn(void);
void test_msgbox_get_active_btn_text(void);
void test_msgbox_close(void);
void test_msgbox_close_modal(void);
void test_msgbox_close_async(void);
void test_msgbox_close_async_modal(void);
void test_msgbox_close_click_event(void);

static lv_obj_t * active_screen = NULL;
static lv_obj_t * msgbox = NULL;
static const char * btns_txts[] = {"Apply", "Cancel", NULL};

void setUp(void)
{
    active_screen = lv_scr_act();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_msgbox_creation_successfull_with_close_btn(void)
{
    bool add_close_btn = true;

    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, add_close_btn);

    TEST_ASSERT_NOT_NULL(msgbox);

    TEST_ASSERT_EQUAL_SCREENSHOT("msgbox_ok_with_close_btn.png");
}

void test_msgbox_creation_successfull_no_close_btn(void)
{
    bool add_close_btn = false;

    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, add_close_btn);

    TEST_ASSERT_NOT_NULL(msgbox);

    TEST_ASSERT_EQUAL_SCREENSHOT("msgbox_ok_no_close_btn.png");
}

void test_msgbox_creation_successfull_modal(void)
{
    // If parent is NULL the message box will be modal
    msgbox = lv_msgbox_create(NULL, "The title", "The text", btns_txts, true);

    TEST_ASSERT_NOT_NULL(msgbox);

    // Since msgbox has no parent, it won´t be clean up at tearDown()
    lv_obj_clean(msgbox);
}

void test_msgbox_get_title(void)
{
    const char * txt_title = "The title";
    lv_obj_t * lbl_title = NULL;

    msgbox = lv_msgbox_create(active_screen, txt_title, "The text", btns_txts, true);

    // Msgbox title is a lv_label widget
    lbl_title = lv_msgbox_get_title(msgbox);

    TEST_ASSERT_EQUAL_STRING(txt_title, lv_label_get_text(lbl_title));
}

void test_msgbox_get_close_btn(void)
{
    lv_obj_t * close_btn = NULL;

    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    // Msgbox close button is a lv_btn widget
    close_btn = lv_msgbox_get_close_btn(msgbox);

    TEST_ASSERT_NOT_NULL(close_btn);
}

void test_msgbox_get_text(void)
{
    const char * txt_text = "The text";
    lv_obj_t * lbl_text = NULL;

    msgbox = lv_msgbox_create(active_screen, "The title", txt_text, btns_txts, true);

    // Msgbox text is a lv_label widget
    lbl_text = lv_msgbox_get_text(msgbox);

    TEST_ASSERT_EQUAL_STRING(txt_text, lv_label_get_text(lbl_text));
}

void test_msgbox_get_content(void)
{
    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    TEST_ASSERT_NOT_NULL(lv_msgbox_get_content(msgbox));
}

void test_msgbox_get_btns(void)
{
    lv_obj_t * btnmatrix = NULL;

    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    // Msgbox buttons is a lv_btnmatrix widget
    btnmatrix = lv_msgbox_get_btns(msgbox);

    for(int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL_STRING(btns_txts[i], lv_btnmatrix_get_btn_text(btnmatrix, i));
    }
}

void test_msgbox_get_active_btn(void)
{
    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    // index of active button is LV_BTNMATRIX_BTN_NONE if unset (no button clicked)
    TEST_ASSERT_EQUAL(LV_BTNMATRIX_BTN_NONE, lv_msgbox_get_active_btn(msgbox));
}

void test_msgbox_get_active_btn_text(void)
{
    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    // active button unset (no button clicked) -> active button text NULL
    TEST_ASSERT_NULL(lv_msgbox_get_active_btn_text(msgbox));
}

void test_msgbox_close(void)
{
    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    lv_msgbox_close(msgbox);

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

void test_msgbox_close_modal(void)
{
    msgbox = lv_msgbox_create(NULL, "The title", "The text", btns_txts, true);

    lv_msgbox_close(msgbox);

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

void test_msgbox_close_async(void)
{
    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    lv_msgbox_close_async(msgbox);

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

void test_msgbox_close_async_modal(void)
{
    msgbox = lv_msgbox_create(NULL, "The title", "The text", btns_txts, true);

    lv_msgbox_close_async(msgbox);

    // lv_msgbox_close deletes the message box
    TEST_ASSERT_NOT_NULL(msgbox);
}

void test_msgbox_close_click_event(void)
{
    lv_obj_t * close_btn = NULL;

    msgbox = lv_msgbox_create(active_screen, "The title", "The text", btns_txts, true);

    // Msgbox close button is a lv_btn widget
    close_btn = lv_msgbox_get_close_btn(msgbox);

    lv_obj_send_event(close_btn, LV_EVENT_CLICKED, NULL);
}

#endif
