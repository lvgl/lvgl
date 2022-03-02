#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#include "lv_test_helpers.h"
#include "lv_test_indev.h"

void test_checkbox_creation_successfull(void);
void test_checkbox_should_call_event_handler_on_click_when_enabled(void);
void test_checkbox_should_have_default_text_when_created(void);
void test_checkbox_should_return_dinamically_allocated_text(void);
void test_checkbox_should_allocate_memory_for_static_text(void);

static lv_obj_t *active_screen = NULL;
static lv_obj_t *checkbox = NULL;

static volatile bool event_called = false;

static void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (LV_EVENT_VALUE_CHANGED == code) {
        event_called = true;
    }
}

void test_checkbox_creation_successfull(void)
{
    active_screen = lv_scr_act();
    checkbox = lv_checkbox_create(active_screen);

    TEST_ASSERT_NOT_NULL(checkbox);
}

void test_checkbox_should_call_event_handler_on_click_when_enabled(void)
{
    active_screen = lv_scr_act();
    checkbox = lv_checkbox_create(active_screen);
    
    lv_obj_add_state(checkbox, LV_STATE_CHECKED);
    lv_obj_add_event_cb(checkbox, event_handler, LV_EVENT_ALL, NULL);

    lv_test_mouse_click_at(checkbox->coords.x1, checkbox->coords.y1);

    TEST_ASSERT_TRUE(event_called);
    
    event_called = false;
}

void test_checkbox_should_have_default_text_when_created(void)
{
    const char *default_text = "Check box";

    active_screen = lv_scr_act();
    checkbox = lv_checkbox_create(active_screen);

    TEST_ASSERT_EQUAL_STRING(default_text, lv_checkbox_get_text(checkbox));
    TEST_ASSERT_NOT_NULL(lv_checkbox_get_text(checkbox));
}

void test_checkbox_should_return_dinamically_allocated_text(void)
{
    const char *message = "Hello World!";

    active_screen = lv_scr_act();
    checkbox = lv_checkbox_create(active_screen);

    lv_checkbox_set_text(checkbox, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_checkbox_get_text(checkbox));
    TEST_ASSERT_NOT_NULL(lv_checkbox_get_text(checkbox));
}

void test_checkbox_should_allocate_memory_for_static_text(void)
{
    uint32_t initial_available_memory = 0;
    const char *static_text = "Keep me while you exist";

    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);

    active_screen = lv_scr_act();
    checkbox = lv_checkbox_create(active_screen);

    initial_available_memory = m1.free_size;

    lv_checkbox_set_text_static(checkbox, static_text);

    lv_mem_monitor(&m1);

    LV_HEAP_CHECK(TEST_ASSERT_LESS_THAN(initial_available_memory, m1.free_size));
}

#endif
