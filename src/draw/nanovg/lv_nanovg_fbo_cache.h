/**
 * @file lv_nanovg_fbo_cache.h
 *
 */

#ifndef LV_NANOVG_FBO_CACHE_H
#define LV_NANOVG_FBO_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_DRAW_NANOVG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_nanovg_unit_t;
struct NVGLUframebuffer;

/** A framebuffer checked out of the pool. Owned by the pool, not the caller. */
typedef struct _lv_nanovg_fbo_t lv_nanovg_fbo_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize the FBO cache
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_fbo_cache_init(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief Deinitialize the FBO cache
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_fbo_cache_deinit(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief Check a framebuffer out of the pool, creating one if none is free
 * @param u pointer to the nanovg unit
 * @param width the width of the FBO
 * @param height the height of the FBO
 * @param flags the FBO flags
 * @param format the texture format
 * @return the framebuffer, or NULL on failure
 */
lv_nanovg_fbo_t * lv_nanovg_fbo_cache_get(struct _lv_draw_nanovg_unit_t * u, int width, int height, int flags,
                                          int format);

/**
 * @brief Return a framebuffer to the pool
 * @param u pointer to the nanovg unit
 * @param fbo the framebuffer to return
 */
void lv_nanovg_fbo_cache_release(struct _lv_draw_nanovg_unit_t * u, lv_nanovg_fbo_t * fbo);

/**
 * @brief Convert a cache entry to a framebuffer
 * @return the framebuffer pointer
 */
struct NVGLUframebuffer * lv_nanovg_fbo_cache_entry_to_fb(lv_nanovg_fbo_t * fbo);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_FBO_CACHE_H*/
