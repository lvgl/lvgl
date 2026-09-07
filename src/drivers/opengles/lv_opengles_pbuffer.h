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

#include "../../lvgl_public.h"
#if LV_USE_OPENGLES_PBUFFER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** An off-screen EGL pbuffer context and its render target */
typedef struct _lv_opengles_pbuffer lv_opengles_pbuffer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a display that renders off-screen into an EGL pbuffer and reads the result
 * back into its draw buffer, so no window system is needed.
 *
 * The resolution and color format are fixed at creation; changing either on the
 * returned display is not supported.
 *
 * @param hor_res   horizontal resolution in pixels
 * @param ver_res   vertical resolution in pixels
 * @return          the new display, or NULL on failure
 */
lv_display_t * lv_opengles_pbuffer_create(int32_t hor_res, int32_t ver_res);

/**
 * Read the pbuffer display's current framebuffer contents into a draw buffer.
 * Called automatically when the display is flushed; use it directly only to take an
 * extra copy. The draw buffer must match the display resolution.
 * @param display   a display created by `lv_opengles_pbuffer_create`
 * @param draw_buf  the draw buffer to read the pixels into
 * @return          LV_RESULT_OK on success, LV_RESULT_INVALID on error
 */
lv_result_t lv_opengles_pbuffer_read_to_draw_buf(lv_display_t * display, lv_draw_buf_t * draw_buf);

/**
 * Get the OpenGL ES major version of the context backing a pbuffer display.
 * ES3 is requested first, but drivers that cannot provide it fall back to ES2, where
 * features needing ES3 shaders (e.g. glTF) are unavailable.
 * @param display   a display created by `lv_opengles_pbuffer_create`
 * @return          3 or 2, or 0 if `display` is not a pbuffer display
 */
uint8_t lv_opengles_pbuffer_get_gles_version(lv_display_t * display);

/**
 * Get the pbuffer handle of a display, so it can be torn down after the display itself
 * is gone. See `lv_opengles_pbuffer_destroy`.
 * @param display   a display created by `lv_opengles_pbuffer_create`
 * @return          the handle, or NULL if `display` is not a pbuffer display
 */
lv_opengles_pbuffer_t * lv_opengles_pbuffer_get_handle(lv_display_t * display);

/**
 * Destroy a pbuffer context, releasing its GL objects and the EGL context.
 *
 * This is deliberately not done when the display is deleted. Draw units may still run
 * GL calls while they are being destroyed, and `lv_deinit()` deletes displays before
 * draw units, so tearing the EGL context down from the display's delete event would
 * pull the context out from under them. Fetch the handle with
 * `lv_opengles_pbuffer_get_handle()` beforehand and call this afterwards.
 *
 * @param pbuffer   handle from `lv_opengles_pbuffer_get_handle`, NULL is ignored
 */
void lv_opengles_pbuffer_destroy(lv_opengles_pbuffer_t * pbuffer);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES_PBUFFER */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_PBUFFER_H*/
