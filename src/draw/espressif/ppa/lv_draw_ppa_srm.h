/**
 * @file lv_draw_ppa_srm.h
 *
 */

#ifndef LV_DRAW_PPA_SRM_H
#define LV_DRAW_PPA_SRM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"
#include "../../../misc/lv_area_private.h"

#include <math.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Result of mapping a visible render tile back onto a PPA SRM source block.
 *
 * This is pure geometry: it depends only on coordinates and scale, never on
 * the PPA hardware or the ESP-IDF, so it can be exercised by the host test
 * suite even though the SRM operation itself only runs on an ESP32-P4.
 */
typedef struct {
    bool draw;                /**< false: nothing is visible, skip the operation */
    lv_area_t visible_area;   /**< image area clipped to the render tile (screen coords) */
    lv_area_t dest_area;      /**< visible area relative to the layer buffer origin */
    int32_t clip_w;           /**< visible width  (= width of visible_area) */
    int32_t clip_h;           /**< visible height (= height of visible_area) */
    int32_t block_x;          /**< source block top-left X */
    int32_t block_y;          /**< source block top-left Y */
    int32_t block_w;          /**< source block width  */
    int32_t block_h;          /**< source block height */
    bool gap_right;           /**< PPA floor-rounding leaves a 1px gap on the right edge */
    bool gap_bottom;          /**< ... and/or on the bottom edge */
    float scale_x;            /**< horizontal scale factor (1.0 = no scaling) */
    float scale_y;            /**< vertical scale factor */
} lv_draw_ppa_srm_block_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Map a visible render tile back into PPA SRM source-block coordinates.
 *
 * Keeps the transform pivot fixed on screen as the scale changes, clamps the
 * source block to the image and to the destination buffer, and flags the
 * single-pixel right/bottom gap left by the PPA's floor-rounding so the caller
 * can patch it.
 *
 * @param coords     image rectangle at 1:1 scale, in screen coordinates
 * @param buf_area   the layer buffer (render tile) area, in screen coordinates
 * @param buf_w      destination buffer width  in pixels
 * @param buf_h      destination buffer height in pixels
 * @param img_w      decoded source image width  in pixels
 * @param img_h      decoded source image height in pixels
 * @param scale_x    horizontal scale, 256 (LV_SCALE_NONE) = 1:1
 * @param scale_y    vertical scale,   256 (LV_SCALE_NONE) = 1:1
 * @param pivot_x    transform pivot X relative to the image top-left
 * @param pivot_y    transform pivot Y relative to the image top-left
 * @return           the computed block; check `.draw` before using the rest
 */
static inline lv_draw_ppa_srm_block_t lv_draw_ppa_srm_calc_block(
    const lv_area_t * coords, const lv_area_t * buf_area,
    int32_t buf_w, int32_t buf_h, int32_t img_w, int32_t img_h,
    int32_t scale_x, int32_t scale_y, int32_t pivot_x, int32_t pivot_y)
{
    lv_draw_ppa_srm_block_t r;
    lv_memzero(&r, sizeof(r));

    /* coords = image rect at 1:1 scale (may extend off-screen).
     * Intersect with the render tile to get the actual visible clip. */
    if(!lv_area_intersect(&r.visible_area, coords, buf_area)) return r;

    float sx = (scale_x != LV_SCALE_NONE) ? ((float)scale_x / 256.0f) : 1.0f;
    float sy = (scale_y != LV_SCALE_NONE) ? ((float)scale_y / 256.0f) : 1.0f;
    r.scale_x = sx;
    r.scale_y = sy;

    /* Virtual image origin: pivot stays fixed on screen as scale changes. */
    float virt_x = (float)coords->x1 + (float)pivot_x * (1.0f - sx);
    float virt_y = (float)coords->y1 + (float)pivot_y * (1.0f - sy);

    /* Visible clip dimensions and buffer-local destination (always non-negative) */
    r.clip_w = lv_area_get_width(&r.visible_area);
    r.clip_h = lv_area_get_height(&r.visible_area);

    lv_area_copy(&r.dest_area, &r.visible_area);
    lv_area_move(&r.dest_area, -buf_area->x1, -buf_area->y1);

    /* Map visible tile top-left back into source image space */
    int32_t src_bx = (int32_t)(((float)r.visible_area.x1 - virt_x) / sx);
    int32_t src_by = (int32_t)(((float)r.visible_area.y1 - virt_y) / sy);

    /* ceilf gives the ideal source block; floorf clamp keeps PPA happy.
     * The PPA may render 1 pixel short — the caller fixes that afterwards. */
    uint32_t src_bw = (uint32_t)ceilf((float)r.clip_w / sx);
    uint32_t src_bh = (uint32_t)ceilf((float)r.clip_h / sy);

    uint32_t avail_w = (uint32_t)(buf_w - r.dest_area.x1);
    uint32_t avail_h = (uint32_t)(buf_h - r.dest_area.y1);
    uint32_t max_src_bw = (uint32_t)floorf((float)avail_w / sx);
    uint32_t max_src_bh = (uint32_t)floorf((float)avail_h / sy);
    r.gap_right  = (src_bw > max_src_bw);
    r.gap_bottom = (src_bh > max_src_bh);
    if(src_bw > max_src_bw) src_bw = max_src_bw;
    if(src_bh > max_src_bh) src_bh = max_src_bh;

    if(src_bx < 0 || src_by < 0 ||
       (uint32_t)src_bx >= (uint32_t)img_w || (uint32_t)src_by >= (uint32_t)img_h) {
        return r;
    }
    if((uint32_t)src_bx + src_bw > (uint32_t)img_w) src_bw = (uint32_t)img_w - (uint32_t)src_bx;
    if((uint32_t)src_by + src_bh > (uint32_t)img_h) src_bh = (uint32_t)img_h - (uint32_t)src_by;
    if(src_bw == 0 || src_bh == 0) return r;

    r.block_x = src_bx;
    r.block_y = src_by;
    r.block_w = (int32_t)src_bw;
    r.block_h = (int32_t)src_bh;
    r.draw = true;
    return r;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_PPA_SRM_H */
