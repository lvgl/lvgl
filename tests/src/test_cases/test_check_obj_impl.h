/**
 * test_check_obj_impl.h
 *
 * Shared state, static-inline helpers, and test-body implementations for the
 * test_check_obj_*.c config-variant suite.
 *
 * Why this file can be included directly
 * --------------------------------------
 * The Unity test-runner generator scans each .c file for quoted #include
 * directives and re-emits them into the generated *_Runner.c. This header is
 * safe to include directly because the shared test logic lives in static inline
 * helpers, while each wrapper .c still provides the non-static test entry
 * points that Unity calls.
 *
 * Why test functions are static inline *_impl variants
 * ----------------------------------------------------
 * Unity requires external linkage for test functions so the runner can resolve
 * them via extern declarations. The static inline _impl functions hold the
 * real test logic (compiled once per TU with the correct LV_USE_CHECK_*
 * macros), while each .c wrapper provides thin non-static shims whose names
 * match the void test_*() pattern the runner generator discovers.
 */

#ifndef TEST_CHECK_OBJ_IMPL_H
#define TEST_CHECK_OBJ_IMPL_H

#include "tests/unity/unity.h"
#include <string.h>

/* -------------------------------------------------------------------------
 * Per-translation-unit state
 * ---------------------------------------------------------------------- */

#define TEST_ASSERT_LOG_CONTAINS(substr) \
    do { \
        if(strstr(last_log_buf, substr) == NULL) { \
            TEST_PRINTF("Expected log to contain: '%s'. Actual log: '%s'", substr, last_log_buf); \
            TEST_FAIL(); \
        } \
    } while(0)

static bool log_warned;
static char last_log_buf[512];

/* -------------------------------------------------------------------------
 * Static-inline helpers
 * ---------------------------------------------------------------------- */

static inline void log_cb(lv_log_level_t level, const char * buf)
{
    LV_UNUSED(level);
    log_warned = true;
    lv_strncpy(last_log_buf, buf, sizeof(last_log_buf) - 1);
    last_log_buf[sizeof(last_log_buf) - 1] = '\0';
}

/* Exercises LV_CHECK_OBJ (full chain: NULL + class-type + validity). */
static inline bool helper_null_check(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return false);
    LV_UNUSED(obj);
    return true;
}

/* Exercises LV_CHECK_OBJ_CLASS (NULL + optional class-type check only). */
static inline bool helper_class_check(lv_obj_t * obj, const lv_obj_class_t * cls)
{
    LV_CHECK_OBJ_CLASS(obj, cls, return false);
    LV_UNUSED(obj);
    LV_UNUSED(cls);
    return true;
}

/* Exercises LV_CHECK_OBJ_VALID (NULL + class-type + optional validity check). */
static inline bool helper_validity_check(lv_obj_t * obj, const lv_obj_class_t * cls)
{
    LV_CHECK_OBJ_VALID(obj, cls, return false);
    LV_UNUSED(obj);
    LV_UNUSED(cls);
    return true;
}

/* -------------------------------------------------------------------------
 * Test-body implementations (static inline)
 *
 * Called by the thin non-static shim functions defined in each .c wrapper,
 * so every variant compiles its own copy with the right LV_USE_CHECK_* values.
 * ---------------------------------------------------------------------- */

/* --- NULL check (via LV_CHECK_OBJ with base class) --- */

static inline void test_check_obj_null_check_pass_impl(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    log_warned = false; /* Ignore logs from object creation. */
    bool result = helper_null_check(obj);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
}

static inline void test_check_obj_null_check_fail_impl(void)
{
    bool result = helper_null_check(NULL);
#if LV_USE_CHECK_ARG
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("Check failed");
#else
    /* LV_CHECK_OBJ is a no-op: always returns true, never logs. */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}

/* --- Class check (via LV_CHECK_OBJ_CLASS: NULL + optional class-type) --- */

static inline void test_check_obj_class_correct_class_pass_impl(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    log_warned = false; /* Ignore logs from object creation. */
    bool result = helper_class_check(label, &lv_label_class);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
}

static inline void test_check_obj_class_wrong_class_behavior_impl(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    log_warned = false; /* Ignore logs from object creation. */
    bool result = helper_class_check(obj, &lv_label_class);
#if LV_USE_CHECK_ARG && LV_USE_CHECK_OBJ_CLASSTYPE
    /* Class check is active: wrong class should fail. */
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("lv_obj_has_class");
#else
    /* Class check compiled out or CHECK_ARG disabled: passes silently. */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}

static inline void test_check_obj_class_null_still_fails_impl(void)
{
    /* NULL check always precedes class check. */
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

/* --- Validity check (via LV_CHECK_OBJ_VALID: NULL + class-type + optional validity) --- */

static inline void test_check_obj_validity_valid_obj_pass_impl(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    log_warned = false; /* Ignore logs from object creation. */
    bool result = helper_validity_check(label, &lv_label_class);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
}

static inline void test_check_obj_validity_wrong_class_behavior_impl(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    log_warned = false; /* Ignore logs from object creation. */
    bool result = helper_validity_check(obj, &lv_label_class);
#if LV_USE_CHECK_ARG && LV_USE_CHECK_OBJ_CLASSTYPE
    /* Class check fires first in the 3-arg form. */
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_TRUE(log_warned);
    TEST_ASSERT_LOG_CONTAINS("lv_obj_has_class");
#else
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(log_warned);
#endif
}

static inline void test_check_obj_validity_null_still_fails_impl(void)
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

#endif /* TEST_CHECK_OBJ_IMPL_H */
