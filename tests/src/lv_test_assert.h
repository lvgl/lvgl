/**
 * @file lv_test_assert.h
 *
 * Custom assert handler for the unit tests. Pulled in by lv_conf_internal.h
 * through LV_ASSERT_CUSTOM_INCLUDE, see tests/configs/common.defconfig.
 */

#ifndef LV_TEST_ASSERT_H
#define LV_TEST_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

void lv_test_assert_fail(void);

#define LV_ASSERT_HANDLER lv_test_assert_fail();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_ASSERT_H*/
