#if LV_BUILD_TEST
/* Variant: default — LV_USE_CHECK_ARG=1, LV_USE_CHECK_OBJ_CLASSTYPE=1, LV_USE_CHECK_OBJ_VALIDITY=1 */
/* 1. Include the test conf manually so we can add overrides before lvgl.h */
#include "lv_test_conf.h"
/* 2. Prevent lv_conf_internal.h from re-including the conf */
#define LV_CONF_SKIP
#include "../lvgl.h"
#include "test_check_obj_vars.inc"
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
void test_check_obj_null_check_pass(void);
void test_check_obj_null_check_fail(void);
void test_check_obj_class_correct_class_pass(void);
void test_check_obj_class_wrong_class_behavior(void);
void test_check_obj_class_null_still_fails(void);
void test_check_obj_validity_valid_obj_pass(void);
void test_check_obj_validity_wrong_class_behavior(void);
void test_check_obj_validity_null_still_fails(void);
#include "test_check_obj_impl.inc"
#endif /* LV_BUILD_TEST */
