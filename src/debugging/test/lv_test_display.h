/**
 * @file lv_test_display.h
 *
 */

#ifndef LV_TEST_DISPLAY_H
#define LV_TEST_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_TEST

#include "../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/***
 * Create a dummy display for for the tests
 * @param hor_res   the maximal horizontal resolution
 * @param ver_res   the maximal vertical resolution
 * @return          the created display
 *
 * @note            The resolution can be changed to any smaller values later
 *                  using `lv_display_set_resolution`
 *                  The color format can be freely changed later using `lv_display_set_color_format`
 */
lv_display_t * lv_test_display_create(int32_t hor_res, int32_t ver_res);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_DISPLAY_H*/
