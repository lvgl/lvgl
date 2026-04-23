/**
 * test_check_obj_impl.inc
 *
 * Shared log_cb implementation, helper functions, and test function bodies
 * included by each test_check_obj_*.c variant.
 *
 * Each .c wrapper must, in order:
 *   1. #include "../lvgl.h"
 *   2. Apply macro overrides for the desired config variant
 *   3. #include "test_check_obj_vars.inc"  (static vars + log_cb forward decl)
 *   4. Define void setUp(void) and void tearDown(void)
 *   5. Provide void test_check_obj_*() forward declarations
 *   6. #include "test_check_obj_impl.inc"  (this file)
 */


static void log_cb(lv_log_level_t level, const char * buf)
{
    LV_UNUSED(level);
    log_warned = true;
    lv_strncpy(last_log_buf, buf, sizeof(last_log_buf) - 1);
    last_log_buf[sizeof(last_log_buf) - 1] = '\0';
}

/** 1-arg form: NULL check only */
static bool helper_null_check(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, return false);
    LV_UNUSED(obj);
    return true;
}

/** 2-arg form: NULL + optional class check */
static bool helper_class_check(lv_obj_t * obj, const lv_obj_class_t * cls)
{
    LV_CHECK_OBJ(obj, cls, return false);
    LV_UNUSED(obj);
    LV_UNUSED(cls);
    return true;
}

/** 3-arg form: NULL + optional class + optional validity check */
static bool helper_validity_check(lv_obj_t * obj, const lv_obj_class_t * cls)
{
    LV_CHECK_OBJ(obj, cls, obj, return false);
    LV_UNUSED(obj);
    LV_UNUSED(cls);
    return true;
}

/* --- NULL check (1-arg form) --- */

void test_check_obj_null_check_pass(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    log_warned = false; /* ignore logs from object creation */
    bool result = helper_null_check(obj);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
}

void test_check_obj_null_check_fail(void)
{
    bool result = helper_null_check(NULL);
#if LV_USE_CHECK_ARG
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed");
#else
    /* LV_CHECK_OBJ is a no-op: always returns true, never logs */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}

/* --- Class check (2-arg form) --- */

void test_check_obj_class_correct_class_pass(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    log_warned = false; /* ignore logs from object creation */
    bool result = helper_class_check(label, &lv_label_class);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
}

void test_check_obj_class_wrong_class_behavior(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    log_warned = false; /* ignore logs from object creation */
    bool result = helper_class_check(obj, &lv_label_class);
#if LV_USE_CHECK_ARG && LV_USE_CHECK_OBJ_CLASSTYPE
    /* Class check is active: wrong class should fail */
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("lv_obj_has_class");
#else
    /* Class check compiled out or CHECK_ARG disabled: passes silently */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}

void test_check_obj_class_null_still_fails(void)
{
    /* NULL check always precedes class check */
    bool result = helper_class_check(NULL, &lv_label_class);
#if LV_USE_CHECK_ARG
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed");
#else
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}

/* --- Validity check (3-arg form) --- */

void test_check_obj_validity_valid_obj_pass(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    log_warned = false; /* ignore logs from object creation */
    bool result = helper_validity_check(label, &lv_label_class);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
}

void test_check_obj_validity_wrong_class_behavior(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    log_warned = false; /* ignore logs from object creation */
    bool result = helper_validity_check(obj, &lv_label_class);
#if LV_USE_CHECK_ARG && LV_USE_CHECK_OBJ_CLASSTYPE
    /* Class check fires first in the 3-arg form */
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("lv_obj_has_class");
#else
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}

void test_check_obj_validity_null_still_fails(void)
{
    bool result = helper_validity_check(NULL, &lv_label_class);
#if LV_USE_CHECK_ARG
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed");
#else
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}


