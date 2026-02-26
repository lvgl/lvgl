#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_value_precise_t g_last_v;
static int32_t g_last_i32;

static void exec_cb_i32(void * var, int32_t v)
{
    (void)var;
    g_last_i32 = v;
}

static void exec_cb_precise(void * var, lv_value_precise_t v)
{
    (void)var;
    g_last_v = v;
}

static lv_value_precise_t get_value_precise_cb(lv_anim_t * a)
{
    (void)a;
    return (lv_value_precise_t)7;
}

void test_anim_precise_cb_setUp(void)
{
}

void test_anim_precise_cb_tearDown(void)
{
    lv_anim_delete_all();
}

/* Unity runner will call global `setUp`/`tearDown` around each test.
 * `Makefile_Tests` also renames these symbols to `<testname>_setUp/_tearDown`.
 * Provide wrappers to avoid the runner treating our helpers as test cases.
 */
void setUp(void)
{
    test_anim_precise_cb_setUp();
}

void tearDown(void)
{
    test_anim_precise_cb_tearDown();
}

void test_anim_precise_callbacks_run_and_offset_applied(void)
{
    lv_anim_t a;
    lv_anim_init(&a);

    g_last_v = (lv_value_precise_t)0;
    g_last_i32 = 0;

    lv_anim_set_var(&a, &a);
    lv_anim_set_duration(&a, 20);
    lv_anim_set_exec_cb(&a, exec_cb_i32);
    lv_anim_set_exec_cb_precise(&a, exec_cb_precise);

    /* Use relative/offset mode by setting early_apply=0.
     * Offset should be applied when animation actually starts (act_time becomes >= 0).
     */
    lv_anim_set_early_apply(&a, false);
    lv_anim_set_get_value_cb_precise(&a, get_value_precise_cb);

    lv_anim_set_values(&a, (lv_value_precise_t)10, (lv_value_precise_t)20);

    /* Add a delay so we can assert that offset is not applied before start */
    lv_anim_set_delay(&a, 10);
    lv_anim_start(&a);

    /* Still in delay: start_cb not called, no exec yet */
    lv_test_wait(5);
    TEST_ASSERT_EQUAL_INT32(0, g_last_i32);
#if LV_USE_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_value_precise_t)0, g_last_v);
#else
    TEST_ASSERT_EQUAL_INT32(0, g_last_v);
#endif

    /* Pass the delay: first step should apply start_value + offset => 10 + 7 = 17 */
    lv_test_wait(10);
    TEST_ASSERT_EQUAL_INT32(19, g_last_i32);

#if LV_USE_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_value_precise_t)19.5f, g_last_v);
#else
    TEST_ASSERT_EQUAL_INT32(19, g_last_v);
#endif
}

#endif
