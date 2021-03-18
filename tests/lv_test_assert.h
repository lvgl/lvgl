/**
 * @file lv_test_assert.h
 *
 */

#ifndef LV_TEST_ASSERT_H
#define LV_TEST_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <stdint.h>
#include "../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_test_print(const char * s, ...);
void lv_test_exit(const char * s, ...);
void lv_test_error(const char * s, ...);
void lv_test_assert_true(int32_t expression, const char * s);
void lv_test_assert_int_eq(int32_t n1, int32_t n2, const char * s);
void lv_test_assert_int_gt(int32_t n_ref, int32_t n_act, const char * s);
void lv_test_assert_int_lt(int32_t n_ref, int32_t n_act, const char * s);
void lv_test_assert_str_eq(const char * str1, const char * str2, const char * s);
void lv_test_assert_ptr_eq(const void * p_ref, const void * p_act, const char * s);
void lv_test_assert_color_eq(lv_color_t c_ref, lv_color_t c_act, const char * s);
void lv_test_assert_img_eq(const char * ref_img_fn, const char * s);
void lv_test_assert_array_eq(const uint8_t *p_ref, const uint8_t *p_act, int32_t size, const char * s);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_ASSERT_H*/
