
#ifndef LV_UNITY_SUPPORT_H
#define LV_UNITY_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "../../lvgl.h"

#if LV_COLOR_DEPTH != 32
#  define TEST_ASSERT_EQUAL_SCREENSHOT(path)                TEST_IGNORE_MESSAGE("Requires LV_COLOR_DEPTH 32");
#  define TEST_ASSERT_EQUAL_SCREENSHOT_MESSAGE(path, msg)   TEST_PRINTF(msg); TEST_IGNORE_MESSAGE("Requires LV_COLOR_DEPTH 32");
#else

#  define TEST_ASSERT_EQUAL_SCREENSHOT(path)                if(LV_HOR_RES != 800 || LV_VER_RES != 480) {          \
        TEST_IGNORE_MESSAGE("Requires 800x480 resolution"); \
    } else {                                              \
        TEST_ASSERT_MESSAGE(lv_test_screenshot_compare(path), path);            \
    }

#  define TEST_ASSERT_EQUAL_SCREENSHOT_MESSAGE(path, msg)    if(LV_HOR_RES != 800 || LV_VER_RES != 480) {             \
        TEST_PRINTF(msg);                                       \
        TEST_IGNORE_MESSAGE("Requires 800x480 resolution");     \
    } else {                                                  \
        TEST_ASSERT_MESSAGE(lv_test_screenshot_compare(path), msg);  \
    }
#endif

#  define TEST_ASSERT_EQUAL_COLOR(c1, c2)                   TEST_ASSERT_TRUE(lv_color_eq(c1, c2))
#  define TEST_ASSERT_EQUAL_COLOR_MESSAGE(c1, c2, msg)      TEST_ASSERT_TRUE(lv_color_eq(c1, c2), msg)

#  define TEST_ASSERT_EQUAL_COLOR32(c1, c2)                   TEST_ASSERT_TRUE(lv_color32_eq(c1, c2))
#  define TEST_ASSERT_EQUAL_COLOR32_MESSAGE(c1, c2, msg)      TEST_ASSERT_TRUE(lv_color32_eq(c1, c2), msg)


#  define TEST_ASSERT_MEM_LEAK_LESS_THAN(prev_usage, threshold)  TEST_ASSERT_LESS_OR_EQUAL(threshold, LV_ABS((int64_t)(prev_usage) - (int64_t)lv_test_get_free_mem()));

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_UNITY_SUPPORT_H*/
