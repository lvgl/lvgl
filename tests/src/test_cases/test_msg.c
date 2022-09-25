#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}


static uint32_t value_received;

static void msg_cb(lv_msg_t * msg)
{
    const uint32_t * v = lv_msg_get_payload(msg);
    value_received = *v;
}
static void event_cb(lv_event_t * e)
{
    lv_msg_t * msg = lv_event_get_msg(e);
    const uint32_t * v = lv_msg_get_payload(msg);
    lv_label_set_text_fmt(lv_event_get_target(e), "%d", *v);
}

void test_add_entry_and_send_msg(void)
{
    static uint32_t value = 100;
    lv_msg_subscribe((lv_msg_id_t)&value, msg_cb, NULL);

    value_received = 0;
    value = 100;
    lv_msg_update_value(&value);
    TEST_ASSERT_EQUAL_UINT32(100, value_received);

    value = 200;
    lv_msg_update_value(&value);
    TEST_ASSERT_EQUAL_UINT32(200, value_received);

    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_msg_subscribe_obj((lv_msg_id_t)&value, label, NULL);
    lv_obj_add_event_cb(label, event_cb, LV_EVENT_MSG_RECEIVED, NULL);

    value = 300;
    lv_msg_update_value(&value);
    TEST_ASSERT_EQUAL_STRING("300", lv_label_get_text(label));
}


#endif
