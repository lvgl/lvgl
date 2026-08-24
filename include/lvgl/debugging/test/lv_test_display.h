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
#include "../../config/lv_conf_internal.h"
#if LV_USE_TEST

#include "../../lv_types.h"

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

#if LV_USE_TEST && defined(LV_USE_DRAW_NANOVG) && LV_USE_DRAW_NANOVG && defined(LV_USE_NANOVG_TEST_HEADLESS) && LV_USE_NANOVG_TEST_HEADLESS
/**
 * Create a headless EGL display for EGL headless rendering tests.
 * Uses EGL + pbuffer + FBO for off-screen rendering with glReadPixels readback.
 * @param hor_res   the horizontal resolution
 * @param ver_res   the vertical resolution
 * @return          the created display, or NULL on failure
 */
lv_display_t * lv_test_display_egl_create(int32_t hor_res, int32_t ver_res);

/**
 * Clean up EGL/GL resources after lv_deinit() has destroyed the NanoVG draw unit.
 * Must be called from lv_test_deinit() after lv_deinit(), passing the context that
 * was captured from the display's driver data before lv_deinit() deleted the display.
 * @param egl_ctx   the context returned via lv_display_get_driver_data(), may be NULL
 */
void lv_test_display_egl_cleanup(void * egl_ctx);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_DISPLAY_H*/
