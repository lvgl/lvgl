
#ifndef LV_UNITY_SUPPORT_H
#define LV_UNITY_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h> 
#include "../../lvgl.h"

bool lv_test_assert_img_eq(const char * fn_ref);


#if LV_COLOR_DEPTH != 32
#  define TEST_ASSERT_EQUAL_SCREENSHOT(path)                TEST_IGNORE_MESSAGE("Requires LV_COLOR_DEPTH 32");
#  define TEST_ASSERT_EQUAL_SCREENSHOT_MESSAGE(path, msg)   TEST_PRINTF(msg); TEST_IGNORE_MESSAGE("Requires LV_COLOR_DEPTH 32");
#else

#  define TEST_ASSERT_EQUAL_SCREENSHOT(path)                if(LV_HOR_RES != 800 || LV_VER_RES != 480) {          \
                                                              TEST_IGNORE_MESSAGE("Requires 800x480 resolution"); \
                                                            } else {                                              \
                                                              TEST_ASSERT(lv_test_assert_img_eq(path));            \
                                                            }                                                      
                                                      
#  define TEST_ASSERT_EQUAL_SCREENSHOT_MESSAGE(path, msg)    if(LV_HOR_RES != 800 || LV_VER_RES != 480) {             \
                                                              TEST_PRINTF(msg);                                       \
                                                              TEST_IGNORE_MESSAGE("Requires 800x480 resolution");     \
                                                            } else {                                                  \
                                                              TEST_ASSERT_MESSAGE(lv_test_assert_img_eq(path), msg);  \
                                                            }   
#endif

#  define TEST_ASSERT_EQUAL_COLOR(c1, c2)                   TEST_ASSERT_EQUAL_UINT32(c1.full, c2.full)
#  define TEST_ASSERT_EQUAL_COLOR_MESSAGE(c1, c2, msg)      TEST_ASSERT_EQUAL_UINT32_MESSAGE(c1.full, c2.full, msg)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_UNITY_SUPPORT_H*/

