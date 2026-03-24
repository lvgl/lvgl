#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/** Assert that `last_log_buf` contains the given substring */
#define TEST_ASSERT_LOG_CONTAINS(substr) \
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(last_log_buf, substr), "Expected log to contain: " substr)

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
static int helper_verify_msg_return(void * ptr);
static int helper_return_val_complex_expr(void * ptr);
static int helper_return_val_zero(void * ptr);

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
    LV_VERIFY_OR_RETURN_VAL(ptr != NULL, -1, "ptr is NULL");
    return 42;
}

static void helper_return_void_on_null(void * ptr, bool * was_reached)
{
    LV_VERIFY_OR_RETURN(ptr != NULL, "ptr is NULL");
    *was_reached = true;
}

static int helper_verify_msg_return(void * ptr)
{
    LV_VERIFY_MSG(ptr != NULL, return -1, "ptr is NULL via MSG");
    return 42;
}

/**********************
 *   LV_VERIFY
 **********************/

/* Base macro: condition true – no log, no action */
void test_verify_base_condition_true(void)
{
    int x = 0;
    LV_VERIFY(1 == 1, x = -1, ": should not fire");
    TEST_ASSERT_EQUAL_INT(0, x);
    TEST_ASSERT_FALSE(log_warned);
}

/* Base macro: condition false – logs and executes action */
void test_verify_base_condition_false(void)
{
    int x = 0;
    LV_VERIFY(1 == 0, x = -1, ": test failure action");
    TEST_ASSERT_EQUAL_INT(-1, x);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: 1 == 0: test failure action");
}

/* Base macro with printf-style formatting */
void test_verify_base_printf_args(void)
{
    int x = 0;
    int val = 7;
    LV_VERIFY(val > 10, x = -1, ": val=%d", val);
    TEST_ASSERT_EQUAL_INT(-1, x);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: val > 10: val=7");
}

/* Base macro: action is break inside a loop */
void test_verify_base_action_break(void)
{
    int i;
    for(i = 0; i < 5; i++) {
        LV_VERIFY(i < 3, break, ": breaking at i=%d", i);
    }
    TEST_ASSERT_EQUAL_INT(3, i);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: i < 3: breaking at i=3");
}

/**********************
 *   LV_VERIFY_OR_LOG
 **********************/

/* OR_LOG: condition true – no log, execution continues */
void test_verify_or_log_condition_true(void)
{
    int x = 0;
    LV_VERIFY_OR_LOG(1 == 1, ": should not log");
    x = 1;
    TEST_ASSERT_EQUAL_INT(1, x);
    TEST_ASSERT_FALSE(log_warned);
}

/* OR_LOG: condition false – logs but execution continues */
void test_verify_or_log_condition_false(void)
{
    int x = 0;
    LV_VERIFY_OR_LOG(1 == 0, ": logged warning");
    x = 1; /* execution must continue past the macro */
    TEST_ASSERT_EQUAL_INT(1, x);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: 1 == 0: logged warning");
}

/* OR_LOG: printf-style varargs */
void test_verify_or_log_printf_args(void)
{
    int val = 99;
    LV_VERIFY_OR_LOG(val < 0, ": unexpected val=%d", val);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: val < 0: unexpected val=99");
}

/**********************
 *   LV_VERIFY_OR_LOG_MSG
 **********************/

/* OR_LOG_MSG: condition true – no log */
void test_verify_or_log_msg_condition_true(void)
{
    LV_VERIFY_OR_LOG_MSG(1 == 1, "should not appear");
    TEST_ASSERT_FALSE(log_warned);
}

/* OR_LOG_MSG: condition false – logs the message, continues */
void test_verify_or_log_msg_condition_false(void)
{
    int x = 0;
    LV_VERIFY_OR_LOG_MSG(1 == 0, "plain message logged");
    x = 1;
    TEST_ASSERT_EQUAL_INT(1, x);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: 1 == 0: plain message logged");
}

/**********************
 *   LV_VERIFY_MSG
 **********************/

/* MSG: condition true – no log, no action */
void test_verify_msg_condition_true(void)
{
    int result = helper_verify_msg_return((void *)1);
    TEST_ASSERT_EQUAL_INT(42, result);
    TEST_ASSERT_FALSE(log_warned);
}

/* MSG: condition false – logs and executes action */
void test_verify_msg_condition_false(void)
{
    int result = helper_verify_msg_return(NULL);
    TEST_ASSERT_EQUAL_INT(-1, result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: ptr != NULL: ptr is NULL via MSG");
}

/**********************
 *   LV_VERIFY_OR_RETURN
 **********************/

/* OR_RETURN: condition true – function completes normally */
void test_verify_or_return_condition_true(void)
{
    bool reached = false;
    helper_return_void_on_null((void *)1, &reached);
    TEST_ASSERT_TRUE(reached);
    TEST_ASSERT_FALSE(log_warned);
}

/* OR_RETURN: condition false – returns early, logs warning */
void test_verify_or_return_condition_false(void)
{
    bool reached = false;
    helper_return_void_on_null(NULL, &reached);
    TEST_ASSERT_FALSE(reached);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: ptr != NULL: ptr is NULL");
}

/**********************
 *   LV_VERIFY_OR_RETURN_VAL
 **********************/

/* OR_RETURN_VAL: condition true – returns normal value */
void test_verify_or_return_val_condition_true(void)
{
    int result = helper_return_val_on_null((void *)1);
    TEST_ASSERT_EQUAL_INT(42, result);
    TEST_ASSERT_FALSE(log_warned);
}

/* OR_RETURN_VAL: condition false – returns error value, logs */
void test_verify_or_return_val_condition_false(void)
{
    int result = helper_return_val_on_null(NULL);
    TEST_ASSERT_EQUAL_INT(-1, result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: ptr != NULL: ptr is NULL");
}

/**********************
 *   LV_VERIFY_OR_ASSERT
 **********************/

/* OR_ASSERT: condition true – no log, no assert */
void test_verify_or_assert_condition_true(void)
{
    LV_VERIFY_OR_ASSERT(1 == 1, ": should not assert");
    TEST_ASSERT_FALSE(log_warned);
}

/* OR_ASSERT: condition true with printf-style args – no log */
void test_verify_or_assert_printf_condition_true(void)
{
    int val = 5;
    LV_VERIFY_OR_ASSERT(val > 0, ": val=%d", val);
    TEST_ASSERT_FALSE(log_warned);
}

/**********************
 *   LV_VERIFY_OR_ASSERT_MSG
 **********************/

/* OR_ASSERT_MSG: condition true – no log, no assert */
void test_verify_or_assert_msg_condition_true(void)
{
    LV_VERIFY_OR_ASSERT_MSG(1 == 1, "should not assert");
    TEST_ASSERT_FALSE(log_warned);
}

/*
 * Note: LV_VERIFY_OR_ASSERT / LV_VERIFY_OR_ASSERT_MSG with a false condition
 * invokes LV_ASSERT_HANDLER which aborts the process in the test environment,
 * so the failing path cannot be tested in-process.
 */

/**********************
 *   EDGE CASES
 **********************/

/* Multiple verifications in sequence: only the failing one triggers */
void test_verify_multiple_in_sequence(void)
{
    int x = 0;
    LV_VERIFY(1 == 1, x = 1, ": first");
    TEST_ASSERT_FALSE(log_warned);

    LV_VERIFY(1 == 0, x = 2, ": second");
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_EQUAL_INT(2, x);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: 1 == 0: second");
}

/* Helper: returns a ternary expression via error_val to test parenthesization */
static int helper_return_val_complex_expr(void * ptr)
{
    LV_VERIFY_OR_RETURN_VAL(ptr != NULL, 1 > 0 ? -100 : -200, "complex expr");
    return 42;
}

/* OR_RETURN_VAL with complex expression as error_val */
void test_verify_or_return_val_complex_expr(void)
{
    int result = helper_return_val_complex_expr(NULL);
    TEST_ASSERT_EQUAL_INT(-100, result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: ptr != NULL: complex expr");
}

/* Helper: returns 0 on failure to verify zero is not confused with success */
static int helper_return_val_zero(void * ptr)
{
    LV_VERIFY_OR_RETURN_VAL(ptr != NULL, 0, "returns zero");
    return 42;
}

/* Verify that zero as an error_val is returned correctly */
void test_verify_or_return_val_zero(void)
{
    int result = helper_return_val_zero(NULL);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Verification failed: ptr != NULL: returns zero");
}

#endif

