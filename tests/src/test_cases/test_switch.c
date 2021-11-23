#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#include "lv_test_indev.h"

uint8_t value_changed = 0;
lv_obj_t *scr = NULL;
lv_obj_t *sw = NULL;

void setUp(void)
{
    /* Function run before every test */
    scr = lv_scr_act();
    sw = lv_switch_create(scr);
}

void tearDown(void)
{
    /* Function run after every test */
    value_changed = 0;
}

static void event_handler(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (LV_EVENT_VALUE_CHANGED == event) {
        value_changed++;
    }

}

void test_switch_should_have_default_state_after_being_created(void)
{
    lv_state_t state = lv_obj_get_state(sw);
    TEST_ASSERT_EQUAL(state, LV_STATE_DEFAULT);
}

#define SWITCHES_CNT    10

void test_switch_should_not_leak_memory_after_deletion(void)
{
    size_t idx = 0;
    uint32_t initial_available_memory = 0;
    uint32_t final_available_memory = 0;
    lv_mem_monitor_t monitor;
    lv_obj_t *switches[SWITCHES_CNT] = {NULL};

    lv_mem_monitor(&monitor);
    initial_available_memory = monitor.free_size;
    
    for (idx = 0; idx < SWITCHES_CNT; idx++) {
        switches[idx] = lv_switch_create(scr);
    }
    
    for (idx = 0; idx < SWITCHES_CNT; idx++) {
        lv_obj_del(switches[idx]);
    }
    
    lv_mem_monitor(&monitor);
    final_available_memory = monitor.free_size;

    TEST_ASSERT_LESS_THAN(initial_available_memory, final_available_memory);
}

void test_switch_animation(void)
{
    uint32_t target_time = 0;
    uint32_t time = 0;
    lv_switch_t * anim_sw = (lv_switch_t *) sw;
    int32_t initial_anim_state = anim_sw->anim_state;

    /* Trigger animation */
    lv_test_mouse_click_at(sw->coords.x1, sw->coords.y1);

    /* Let 50 ticks pass so the assert doesn't get executed right away */
    time = custom_tick_get();
    target_time = time + 50;

    while (time < target_time) {
        time = custom_tick_get();
    }

    /* The anim_state value got bigger after pressing the switch */
    TEST_ASSERT_GREATER_THAN(initial_anim_state, anim_sw->anim_state);
}

/* See #2330 for context */
void test_switch_should_trigger_value_changed_event_only_once(void)
{
    lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);
    lv_test_mouse_click_at(sw->coords.x1, sw->coords.y1);

    TEST_ASSERT_EQUAL(1, value_changed);
}

/* See #2785 for context */
void test_switch_should_state_change_when_event_bubbling_is_enabled(void)
{
    lv_obj_add_flag(sw, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_test_mouse_click_at(sw->coords.x1, sw->coords.y1);

    TEST_ASSERT(lv_obj_has_state(sw, LV_STATE_CHECKED));
}

#endif
