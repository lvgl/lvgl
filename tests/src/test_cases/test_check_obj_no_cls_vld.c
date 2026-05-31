#if LV_BUILD_TEST
/* Variant: LV_USE_CHECK_ARG=1, LV_USE_CHECK_OBJ_CLASSTYPE=0, LV_USE_CHECK_OBJ_VALIDITY=1 */
#include "lv_test_conf.h"
#define LV_USE_CHECK_OBJ_CLASSTYPE 0
#define LV_CONF_SKIP
#include "../lvgl.h"
#include "test_cases/test_check_obj_impl.h"

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

void test_check_obj_null_check_pass(void)
{
    test_check_obj_null_check_pass_impl();
}
void test_check_obj_null_check_fail(void)
{
    test_check_obj_null_check_fail_impl();
}
void test_check_obj_class_correct_class_pass(void)
{
    test_check_obj_class_correct_class_pass_impl();
}
void test_check_obj_class_wrong_class_behavior(void)
{
    test_check_obj_class_wrong_class_behavior_impl();
}
void test_check_obj_class_null_still_fails(void)
{
    test_check_obj_class_null_still_fails_impl();
}
void test_check_obj_validity_valid_obj_pass(void)
{
    test_check_obj_validity_valid_obj_pass_impl();
}
void test_check_obj_validity_wrong_class_behavior(void)
{
    test_check_obj_validity_wrong_class_behavior_impl();
}
void test_check_obj_validity_null_still_fails(void)
{
    test_check_obj_validity_null_still_fails_impl();
}
#endif /* LV_BUILD_TEST */
