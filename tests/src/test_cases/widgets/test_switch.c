#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define SWITCHES_CNT    10

uint8_t value_changed_event_cnt = 0;
lv_obj_t * scr = NULL;
lv_obj_t * sw = NULL;

void setUp(void)
{
    /* Function run before every test */
    scr = lv_screen_active();
    sw = lv_switch_create(scr);
}

void tearDown(void)
{
    /* Function run after every test */
    value_changed_event_cnt = 0;
}

static void mouse_click_on_switch(void)
{
    lv_test_mouse_click_at(sw->coords.x1, sw->coords.y1);
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(LV_EVENT_VALUE_CHANGED == event) {
        value_changed_event_cnt++;
    }

}

void test_switch_should_have_default_state_after_being_created(void)
{
    lv_state_t state = lv_obj_get_state(sw);
    TEST_ASSERT_EQUAL(state, LV_STATE_DEFAULT);
}

void test_switch_should_not_leak_memory_after_deletion(void)
{
    size_t idx = 0;
    size_t initial_available_memory = 0;
    lv_obj_t * switches[SWITCHES_CNT] = {NULL};

    initial_available_memory = lv_test_get_free_mem();

    for(idx = 0; idx < SWITCHES_CNT; idx++) {
        switches[idx] = lv_switch_create(scr);
    }

    for(idx = 0; idx < SWITCHES_CNT; idx++) {
        lv_obj_delete(switches[idx]);
    }

    LV_UNUSED(initial_available_memory);
    LV_HEAP_CHECK(TEST_ASSERT_MEM_LEAK_LESS_THAN(initial_available_memory, 24));
}

void test_switch_animation(void)
{
    lv_switch_t * anim_sw = (lv_switch_t *) sw;
    int32_t initial_anim_state = anim_sw->anim_state;

    /* Trigger animation */
    mouse_click_on_switch();
    /* Wait some time  */
    lv_test_wait(50);

    int32_t checked_anim_state = anim_sw->anim_state;
    TEST_ASSERT_GREATER_THAN(initial_anim_state, checked_anim_state);
    TEST_ASSERT(lv_obj_has_state(sw, LV_STATE_CHECKED));

    mouse_click_on_switch();
    lv_test_wait(50);

    TEST_ASSERT_LESS_THAN(checked_anim_state, anim_sw->anim_state);
    TEST_ASSERT_FALSE(lv_obj_has_state(sw, LV_STATE_CHECKED));
}

void test_switch_should_not_have_extra_draw_size_at_creation(void)
{
    int32_t extra_size = lv_obj_get_ext_draw_size(sw);

    TEST_ASSERT_EQUAL(0, extra_size);
}

void test_switch_should_update_extra_draw_size_after_editing_padding(void)
{
    int32_t pad = 6;
    int32_t actual = 0;
    int32_t expected = pad + LV_SWITCH_KNOB_EXT_AREA_CORRECTION;

    static lv_style_t style_knob;
    lv_style_init(&style_knob);
    lv_style_set_pad_all(&style_knob, pad);

    lv_obj_remove_style_all(sw);
    lv_obj_add_style(sw, &style_knob, LV_PART_KNOB);
    lv_obj_center(sw);

    /* Get extra draw size */
    actual = lv_obj_get_ext_draw_size(sw);

    TEST_ASSERT_EQUAL(expected, actual);
}

/* See #2330 for context */
void test_switch_should_trigger_value_changed_event_only_once(void)
{
    lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);
    mouse_click_on_switch();

    TEST_ASSERT_EQUAL(1, value_changed_event_cnt);
}

/* See #2785 for context */
void test_switch_should_state_change_when_event_bubbling_is_enabled(void)
{
    lv_obj_add_flag(sw, LV_OBJ_FLAG_EVENT_BUBBLE);
    mouse_click_on_switch();

    TEST_ASSERT(lv_obj_has_state(sw, LV_STATE_CHECKED));
}


void test_screeshots(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clean(scr);

    sw = lv_switch_create(lv_screen_active());

    sw = lv_switch_create(lv_screen_active());
    lv_obj_add_state(sw, LV_STATE_CHECKED);

    sw = lv_switch_create(lv_screen_active());

    sw = lv_switch_create(lv_screen_active());
    lv_obj_add_state(sw, LV_STATE_CHECKED | LV_STATE_DISABLED);

    sw = lv_switch_create(lv_screen_active());
    lv_switch_set_orientation(sw, LV_SWITCH_ORIENTATION_VERTICAL);
    lv_obj_set_size(sw, 50, 100);

    sw = lv_switch_create(lv_screen_active());
    lv_switch_set_orientation(sw, LV_SWITCH_ORIENTATION_VERTICAL);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_set_size(sw, 50, 100);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/switch_1.png");
}

#endif
