#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

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
}

void test_switch_should_have_default_state_after_being_created(void)
{
    lv_state_t state = lv_obj_get_state(sw);
    TEST_ASSERT_EQUAL(state, LV_STATE_DEFAULT);
}

#endif
