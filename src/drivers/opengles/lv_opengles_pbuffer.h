/**
 * @file lv_opengles_pbuffer.h
 *
 */

#ifndef LV_OPENGLES_PBUFFER_H
#define LV_OPENGLES_PBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_EGL

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

/**
 * Create a display with PBuffer-based offscreen rendering
 * @param width The width of the framebuffer
 * @param height The height of the framebuffer
 * @return The created display or NULL on error
 */
lv_display_t * lv_opengles_pbuffer_create(int32_t width, int32_t height);

/**
 * Read the current framebuffer into a buffer
 * @param display The display to read from
 * @param buffer The buffer to store pixels (must be allocated by caller)
 * @param buffer_size Size of the buffer in bytes
 * @return LV_RESULT_OK on success, LV_RESULT_INVALID on error
 */
lv_result_t lv_opengles_pbuffer_read_to_draw_buf(lv_display_t * display, lv_draw_buf_t * draw_buf);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_EGL */
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_PBUFFER_H*/

