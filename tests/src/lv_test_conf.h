/**
 * @file lv_test_conf.h
 *
 */

#ifndef LV_TEST_CONF_H
#define LV_TEST_CONF_H

#define LV_CONF_SUPPRESS_DEFINE_CHECK 1

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

#ifdef LVGL_CI_USING_SYS_HEAP
#define LV_STDLIB_INCLUDE <stdlib.h>
#define LV_MALLOC       malloc
#define LV_REALLOC      realloc
#define LV_FREE         free
#define LV_MEMSET       memset
#define LV_MEMCPY       memcpy
#endif

#ifdef LVGL_CI_USING_DEF_HEAP
#define LV_MALLOC       lv_malloc_builtin
#define LV_REALLOC      lv_realloc_builtin
#define LV_FREE         lv_free_builtin
#define LV_MEMSET       lv_memset_builtin
#define LV_MEMCPY       lv_memcpy_builtin
#endif


void lv_test_assert_fail(void);
#define LV_ASSERT_HANDLER lv_test_assert_fail();

uint32_t custom_tick_get(void);
#define LV_TICK_CUSTOM_SYS_TIME_EXPR custom_tick_get()

typedef void * lv_user_data_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_CONF_H*/
