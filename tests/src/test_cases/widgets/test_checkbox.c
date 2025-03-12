#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void test_checkbox_creation_successful(void);
void test_checkbox_should_call_event_handler_on_click_when_enabled(void);
void test_checkbox_should_have_default_text_when_created(void);
void test_checkbox_should_return_dynamically_allocated_text(void);
void test_checkbox_should_allocate_memory_for_static_text(void);

static lv_obj_t * active_screen = NULL;
static lv_obj_t * checkbox = NULL;

static volatile bool event_called = false;

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(LV_EVENT_VALUE_CHANGED == code) {
        event_called = true;
    }
}

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_checkbox_creation_successful(void)
{
    active_screen = lv_screen_active();
    checkbox = lv_checkbox_create(active_screen);

    TEST_ASSERT_NOT_NULL(checkbox);
}

void test_checkbox_should_call_event_handler_on_click_when_enabled(void)
{
    active_screen = lv_screen_active();
    checkbox = lv_checkbox_create(active_screen);

    lv_obj_add_state(checkbox, LV_STATE_CHECKED);
    lv_obj_add_event_cb(checkbox, event_handler, LV_EVENT_ALL, NULL);

    lv_test_mouse_click_at(checkbox->coords.x1, checkbox->coords.y1);

    TEST_ASSERT_TRUE(event_called);

    event_called = false;
}

void test_checkbox_should_have_default_text_when_created(void)
{
    const char * default_text = "Check box";

    active_screen = lv_screen_active();
    checkbox = lv_checkbox_create(active_screen);

    TEST_ASSERT_EQUAL_STRING(default_text, lv_checkbox_get_text(checkbox));
    TEST_ASSERT_NOT_NULL(lv_checkbox_get_text(checkbox));
}

void test_checkbox_should_return_dynamically_allocated_text(void)
{
    const char * message = "Hello World!";

    active_screen = lv_screen_active();
    checkbox = lv_checkbox_create(active_screen);

    lv_checkbox_set_text(checkbox, message);

    TEST_ASSERT_EQUAL_STRING(message, lv_checkbox_get_text(checkbox));
    TEST_ASSERT_NOT_NULL(lv_checkbox_get_text(checkbox));
}

void test_checkbox_should_allocate_memory_for_static_text(void)
{
    size_t initial_available_memory = 0;
    const char * static_text = "Keep me while you exist";

    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);

    active_screen = lv_screen_active();
    checkbox = lv_checkbox_create(active_screen);

    initial_available_memory = m1.free_size;

    lv_checkbox_set_text_static(checkbox, static_text);

    lv_mem_monitor(&m1);

    LV_UNUSED(initial_available_memory);
    LV_HEAP_CHECK(TEST_ASSERT_LESS_THAN(initial_available_memory, m1.free_size));
}

void test_checkbox_rtl(void)
{
    const char * message =
        "מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).";

    lv_obj_t * screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, 800, 480);
    lv_obj_center(screen);
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_100, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    lv_obj_t * test_checkbox = lv_checkbox_create(active_screen);

    lv_checkbox_set_text(test_checkbox, message);
    lv_obj_set_style_text_font(test_checkbox, &lv_font_dejavu_16_persian_hebrew, 0);
    lv_obj_center(test_checkbox);
    lv_obj_set_style_base_dir(test_checkbox, LV_BASE_DIR_RTL, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/checkbox_rtl_1.png");
}

void test_checkbox_style_opa(void)
{
    lv_obj_t * obj = lv_checkbox_create(lv_screen_active());
    lv_obj_set_style_opa(obj, LV_OPA_0, LV_PART_INDICATOR);
    lv_obj_center(obj);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/checkbox_1.png");
}

#endif
