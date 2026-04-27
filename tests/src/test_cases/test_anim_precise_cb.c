#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

/*********************
 *  STATIC VARIABLES
 *********************/
static lv_anim_value_t g_exec_val;
static lv_anim_value_t g_custom_exec_val;
static bool            g_start_cb_called;
static bool            g_completed_cb_called;
static bool            g_deleted_cb_called;

/*********************
 *  STATIC CALLBACKS
 *********************/
static void exec_cb(void * var, lv_anim_value_t v)
{
    (void)var;
    g_exec_val = v;
}

static void custom_exec_cb(lv_anim_t * a, lv_anim_value_t v)
{
    (void)a;
    g_custom_exec_val = v;
}

static lv_anim_value_t get_value_cb(lv_anim_t * a)
{
    (void)a;
    return (lv_anim_value_t)7;
}

static void start_cb(lv_anim_t * a)
{
    (void)a;
    g_start_cb_called = true;
}

static void completed_cb(lv_anim_t * a)
{
    (void)a;
    g_completed_cb_called = true;
}

static void deleted_cb(lv_anim_t * a)
{
    (void)a;
    g_deleted_cb_called = true;
}

/*********************
 *  SETUP / TEARDOWN
 *********************/
void test_anim_precise_cb_setUp(void)
{
    g_exec_val          = (lv_anim_value_t)0;
    g_custom_exec_val   = (lv_anim_value_t)0;
    g_start_cb_called   = false;
    g_completed_cb_called = false;
    g_deleted_cb_called = false;
}

void test_anim_precise_cb_tearDown(void)
{
    lv_anim_delete_all();
}

void setUp(void)
{
    test_anim_precise_cb_setUp();
}

void tearDown(void)
{
    test_anim_precise_cb_tearDown();
}

/*********************
 *  TEST CASES
 *********************/

/* exec_cb receives lv_anim_value_t and is called with the start value on early_apply */
void test_anim_value_type_exec_cb_early_apply(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, (lv_anim_value_t)10, (lv_anim_value_t)20);
    lv_anim_set_duration(&a, 100);
    /* early_apply=1 by default: exec_cb must be called immediately with start_value */
    lv_anim_start(&a);

#if LV_USE_ANIM_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_anim_value_t)10, g_exec_val);
#else
    TEST_ASSERT_EQUAL_INT32(10, (int32_t)g_exec_val);
#endif
}

/* custom_exec_cb receives lv_anim_t* and lv_anim_value_t */
void test_anim_value_type_custom_exec_cb(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_custom_exec_cb(&a, custom_exec_cb);
    lv_anim_set_values(&a, (lv_anim_value_t)5, (lv_anim_value_t)50);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

#if LV_USE_ANIM_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_anim_value_t)5, g_custom_exec_val);
#else
    TEST_ASSERT_EQUAL_INT32(5, (int32_t)g_custom_exec_val);
#endif
}

/* get_value_cb offset is applied when early_apply=0 and animation actually starts */
void test_anim_get_value_cb_offset_applied(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, (lv_anim_value_t)10, (lv_anim_value_t)20);
    lv_anim_set_duration(&a, 20);
    lv_anim_set_get_value_cb(&a, get_value_cb); /* offset = 7 */
    lv_anim_set_delay(&a, 10);
    a.early_apply = 0;
    lv_anim_start(&a);

    /* Still in delay: exec_cb must not have been called yet */
    lv_test_wait(5);
#if LV_USE_ANIM_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_anim_value_t)0, g_exec_val);
#else
    TEST_ASSERT_EQUAL_INT32(0, (int32_t)g_exec_val);
#endif

    /* After delay: start_value + offset = 10 + 7 = 17, animation is near end => ~27 */
    lv_test_wait(10);
#if LV_USE_ANIM_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_anim_value_t)19.5f, g_exec_val);
#else
    TEST_ASSERT_EQUAL_INT32(19, (int32_t)g_exec_val);
#endif
}

/* start_cb is called once when the animation actually starts (after delay) */
void test_anim_start_cb_called_after_delay(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, (lv_anim_value_t)0, (lv_anim_value_t)100);
    lv_anim_set_duration(&a, 50);
    lv_anim_set_start_cb(&a, start_cb);
    lv_anim_set_delay(&a, 20);
    lv_anim_start(&a);

    TEST_ASSERT_FALSE(g_start_cb_called);
    lv_test_wait(25);
    TEST_ASSERT_TRUE(g_start_cb_called);
}

/* completed_cb and deleted_cb are both called when the animation finishes */
void test_anim_completed_and_deleted_cb_called(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, (lv_anim_value_t)0, (lv_anim_value_t)100);
    lv_anim_set_duration(&a, 20);
    lv_anim_set_completed_cb(&a, completed_cb);
    lv_anim_set_deleted_cb(&a, deleted_cb);
    lv_anim_start(&a);

    TEST_ASSERT_FALSE(g_completed_cb_called);
    TEST_ASSERT_FALSE(g_deleted_cb_called);

    lv_test_wait(30);

    TEST_ASSERT_TRUE(g_completed_cb_called);
    TEST_ASSERT_TRUE(g_deleted_cb_called);
}

/* lv_anim_delete() removes the animation and triggers deleted_cb */
void test_anim_delete_triggers_deleted_cb(void)
{
    lv_anim_value_t var = (lv_anim_value_t)0;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, (lv_anim_value_t)0, (lv_anim_value_t)100);
    lv_anim_set_duration(&a, 200);
    lv_anim_set_deleted_cb(&a, deleted_cb);
    lv_anim_start(&a);

    lv_test_wait(20);
    lv_anim_value_t val_before_delete = g_exec_val;

    bool deleted = lv_anim_delete(&var, exec_cb);
    TEST_ASSERT_TRUE(deleted);
    TEST_ASSERT_TRUE(g_deleted_cb_called);

    /* Value must not change after deletion */
    lv_test_wait(20);
    TEST_ASSERT_EQUAL(val_before_delete, g_exec_val);
}

/* lv_anim_path_linear produces start_value at t=0 and end_value at t=duration */
void test_anim_path_linear_boundary_values(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, (lv_anim_value_t)0, (lv_anim_value_t)100);
    lv_anim_set_duration(&a, 50);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    /* At t=0 (early_apply) value should be start */
#if LV_USE_ANIM_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_anim_value_t)0, g_exec_val);
#else
    TEST_ASSERT_EQUAL_INT32(0, (int32_t)g_exec_val);
#endif

    /* After duration, value should reach end */
    lv_test_wait(60);
#if LV_USE_ANIM_FLOAT
    TEST_ASSERT_EQUAL_FLOAT((lv_anim_value_t)100, g_exec_val);
#else
    TEST_ASSERT_EQUAL_INT32(100, (int32_t)g_exec_val);
#endif
}

/* lv_anim_count_running reflects active animation count */
void test_anim_count_running(void)
{
    TEST_ASSERT_EQUAL_UINT16(0, lv_anim_count_running());

    lv_anim_t a1, a2;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, &a1);
    lv_anim_set_exec_cb(&a1, exec_cb);
    lv_anim_set_values(&a1, (lv_anim_value_t)0, (lv_anim_value_t)100);
    lv_anim_set_duration(&a1, 200);
    lv_anim_start(&a1);

    TEST_ASSERT_EQUAL_UINT16(1, lv_anim_count_running());

    lv_anim_init(&a2);
    lv_anim_set_var(&a2, &a2);
    lv_anim_set_exec_cb(&a2, exec_cb);
    lv_anim_set_values(&a2, (lv_anim_value_t)0, (lv_anim_value_t)50);
    lv_anim_set_duration(&a2, 200);
    lv_anim_start(&a2);

    TEST_ASSERT_EQUAL_UINT16(2, lv_anim_count_running());

    lv_anim_delete_all();
    TEST_ASSERT_EQUAL_UINT16(0, lv_anim_count_running());
}

#endif
