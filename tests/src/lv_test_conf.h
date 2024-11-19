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

/***********************
 * PLATFORM CONFIGS
 ***********************/

#ifdef LVGL_CI_USING_SYS_HEAP
#define LV_USE_STDLIB_MALLOC        LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING        LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF       LV_STDLIB_CLIB
#define LV_USE_OS                   LV_OS_PTHREAD
#define LV_OBJ_STYLE_CACHE          0
#define LV_BIN_DECODER_RAM_LOAD     1   /* Run test with bin image loaded to RAM */
#endif

#ifdef LVGL_CI_USING_DEF_HEAP
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN
#define LV_OBJ_STYLE_CACHE      1
#define LV_BIN_DECODER_RAM_LOAD 0
#endif

#ifdef MICROPYTHON
#define LV_USE_BUILTIN_MALLOC   0
#define LV_USE_BUILTIN_MEMCPY   1
#define LV_USE_BUILTIN_SNPRINTF 1
#define LV_STDLIB_INCLUDE "include/lv_mp_mem_custom_include.h"
#define LV_MALLOC       m_malloc
#define LV_REALLOC      m_realloc
#define LV_FREE         m_free
#define LV_MEMSET       lv_memset_builtin
#define LV_MEMCPY       lv_memcpy_builtin
#define LV_SNPRINTF     lv_snprintf_builtin
#define LV_VSNPRINTF    lv_vsnprintf_builtin
#define LV_STRLEN       lv_strlen_builtin
#define LV_STRNCPY      lv_strncpy_builtin

#define LV_ENABLE_GC 1
#define LV_GC_INCLUDE "py/mpstate.h"
#define LV_GC_ROOT(x) MP_STATE_PORT(x)
#endif

#ifndef __ASSEMBLY__
void lv_test_assert_fail(void);
#define LV_ASSERT_HANDLER lv_test_assert_fail();

typedef void * lv_user_data_t;
#endif
/***********************
 * TEST CONFIGS
 ***********************/

#if !(defined(LV_TEST_OPTION)) || LV_TEST_OPTION == 5
#define  LV_COLOR_DEPTH     32
#define  LV_DPI_DEF         160
#include "lv_test_conf_full.h"
#elif LV_TEST_OPTION == 6
#define  LV_COLOR_DEPTH     32
#define  LV_DPI_DEF         160

#define  LV_DRAW_BUF_ALIGN  64
#ifdef _MSC_VER
#define  LV_ATTRIBUTE_MEM_ALIGN __declspec(align(LV_DRAW_BUF_ALIGN))
#else
#define  LV_ATTRIBUTE_MEM_ALIGN __attribute__((aligned(LV_DRAW_BUF_ALIGN)))
#endif

#include "lv_test_conf_vg_lite.h"
#include "lv_test_conf_full.h"
#elif LV_TEST_OPTION == 7
#define  LV_COLOR_DEPTH     32
#define  LV_DPI_DEF         160
#define  LV_USE_DRAW_SDL    1
#define  LV_USE_SDL         1
#include "lv_test_conf_full.h"
#elif LV_TEST_OPTION == 4
#define  LV_COLOR_DEPTH     24
#define  LV_DPI_DEF         120
#elif LV_TEST_OPTION == 3
#define  LV_COLOR_DEPTH     16
#define  LV_DPI_DEF         90
#include "lv_test_conf_minimal.h"
#elif LV_TEST_OPTION == 2
#define  LV_COLOR_DEPTH     8
#define  LV_DPI_DEF         60
#include "lv_test_conf_minimal.h"
#elif LV_TEST_OPTION == 1
#define  LV_COLOR_DEPTH     1
#define  LV_DPI_DEF         30
#define  LV_DRAW_SW_COMPLEX 0
#include "lv_test_conf_minimal.h"
#endif

#if defined(LVGL_CI_USING_SYS_HEAP) || defined(LVGL_CI_USING_DEF_HEAP)
#undef LV_LOG_PRINTF

/*Use a large value be sure any issues will cause crash*/
#define LV_DRAW_BUF_STRIDE_ALIGN                64

#ifndef LV_DRAW_BUF_ALIGN
/*Use non power of 2 to avoid the case when `malloc` returns aligned pointer by default, and use a large value be sure any issues will cause crash*/
#define LV_DRAW_BUF_ALIGN                       852
#endif

/*For screenshots*/
#undef LV_DPI_DEF
#define  LV_DPI_DEF         130
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_CONF_H*/
