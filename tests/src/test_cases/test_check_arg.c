#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/** Assert that `last_log_buf` contains the given substring */
#define TEST_ASSERT_LOG_CONTAINS(substr) \
    do { \
        if(strstr(last_log_buf, substr) == NULL) { \
            TEST_PRINTF("Expected log to contain: '%s'. Actual log: '%s'", substr, last_log_buf); \
            TEST_FAIL(); \
        } \
    } while(0)

/**********************
 *  STATIC VARIABLES
 **********************/
static bool log_warned;
static char last_log_buf[512];

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void log_cb(lv_log_level_t level, const char * buf);

static int helper_return_val_on_null(void * ptr);
static void helper_return_void_on_null(void * ptr, bool * was_reached);

/**********************
 *   TEST SETUP
 **********************/

void setUp(void)
{
    log_warned = false;
    last_log_buf[0] = '\0';
    lv_log_register_print_cb(log_cb);
}

void tearDown(void)
{
    lv_log_register_print_cb(NULL);
    lv_obj_clean(lv_screen_active());
}

/**********************
 *   HELPERS
 **********************/

static void log_cb(lv_log_level_t level, const char * buf)
{
    LV_UNUSED(level);
    log_warned = true;
    lv_strncpy(last_log_buf, buf, sizeof(last_log_buf) - 1);
    last_log_buf[sizeof(last_log_buf) - 1] = '\0';
}

static int helper_return_val_on_null(void * ptr)
{
    LV_CHECK_ARG(ptr != NULL, return -1, "ptr is NULL");
    return 42;
}

static void helper_return_void_on_null(void * ptr, bool * was_reached)
{
    LV_CHECK_ARG(ptr != NULL, return, "ptr is NULL");
    *was_reached = true;
}

/**********************
 *   CONDITION TRUE
 **********************/

/* Condition true – no log, no action */
void test_check_arg_condition_true(void)
{
    int x = 0;
    LV_CHECK_ARG(1 == 1, x = -1, ": should not fire");
    TEST_ASSERT_EQUAL_INT(0, x);
    TEST_ASSERT_FALSE(log_warned);
}

/**********************
 *   CONDITION FALSE
 **********************/

/* Condition false – logs and executes action */
void test_check_arg_condition_false(void)
{
    int x = 0;
    LV_CHECK_ARG(1 == 0, x = -1, ": test failure action");
    TEST_ASSERT_EQUAL_INT(-1, x);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed: 1 == 0 : test failure action");
}

/**********************
 *   PRINTF-STYLE ARGS
 **********************/

/* Printf-style formatting in the message */
void test_check_arg_printf_args(void)
{
    int x = 0;
    int val = 7;
    LV_CHECK_ARG(val > 10, x = -1, ": val=%d", val);
    TEST_ASSERT_EQUAL_INT(-1, x);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed: val > 10 : val=7");
}

/**********************
 *   ACTION: BREAK
 **********************/

/* Action is break inside a loop */
void test_check_arg_action_break(void)
{
    int i;
    for(i = 0; i < 5; i++) {
        LV_CHECK_ARG(i < 3, break, ": breaking at i=%d", i);
    }
    TEST_ASSERT_EQUAL_INT(3, i);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed: i < 3 : breaking at i=3");
}

/**********************
 *   ACTION: RETURN
 **********************/

/* return from void function: condition true – function completes */
void test_check_arg_return_void_condition_true(void)
{
    bool reached = false;
    helper_return_void_on_null((void *)1, &reached);
    TEST_ASSERT_TRUE(reached);
    TEST_ASSERT_FALSE(log_warned);
}

/* return from void function: condition false – returns early */
void test_check_arg_return_void_condition_false(void)
{
    bool reached = false;
    helper_return_void_on_null(NULL, &reached);
    TEST_ASSERT_FALSE(reached);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed: ptr != NULL ptr is NULL");
}

/**********************
 *   ACTION: RETURN VAL
 **********************/

/* return value: condition true – returns normal value */
void test_check_arg_return_val_condition_true(void)
{
    int result = helper_return_val_on_null((void *)1);
    TEST_ASSERT_EQUAL_INT(42, result);
    TEST_ASSERT_FALSE(log_warned);
}

/* return value: condition false – returns error value, logs */
void test_check_arg_return_val_condition_false(void)
{
    int result = helper_return_val_on_null(NULL);
    TEST_ASSERT_EQUAL_INT(-1, result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed: ptr != NULL ptr is NULL");
}

/**********************
 *   EDGE CASES
 **********************/

/* Multiple checks in sequence: only the failing one triggers */
void test_check_arg_multiple_in_sequence(void)
{
    int x = 0;
    LV_CHECK_ARG(1 == 1, x = 1, ": first");
    TEST_ASSERT_FALSE(log_warned);

    LV_CHECK_ARG(1 == 0, x = 2, ": second");
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_EQUAL_INT(2, x);
    TEST_ASSERT_LOG_CONTAINS("Check failed: 1 == 0 : second");
}

/* Complex expression as error value in return */
static int helper_return_val_complex_expr(void * ptr)
{
    LV_CHECK_ARG(ptr != NULL, return (1 > 0 ? -100 : -200), "complex expr");
    return 42;
}

void test_check_arg_return_val_complex_expr(void)
{
    int result = helper_return_val_complex_expr(NULL);
    TEST_ASSERT_EQUAL_INT(-100, result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed: ptr != NULL complex expr");
}

/* Zero as error return value is not confused with success */
static int helper_return_val_zero(void * ptr)
{
    LV_CHECK_ARG(ptr != NULL, return 0, "returns zero");
    return 42;
}

void test_check_arg_return_val_zero(void)
{
    int result = helper_return_val_zero(NULL);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed: ptr != NULL returns zero");
}

#endif

