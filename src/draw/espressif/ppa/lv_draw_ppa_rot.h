/**
 * @file lv_draw_ppa_rot.h
 *
 */

#ifndef LV_DRAW_PPA_ROT_H
#define LV_DRAW_PPA_ROT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Result of mapping a visible render tile back onto a PPA source block for a
 * 90/180/270-degree rotation (no scaling).
 *
 * Like lv_draw_ppa_srm_calc_block(), this is pure geometry: it depends only on
 * coordinates and the rotation step, never on the PPA hardware or the ESP-IDF,
 * so it can be exercised by the host test suite even though the rotation itself
 * only runs on an ESP32-P4.
 */
typedef struct {
    bool draw;              /**< false: nothing is visible / off-screen, skip the operation */
    lv_area_t dest_area;    /**< visible window relative to the layer buffer origin (PPA writes here) */
    int32_t block_x;        /**< source block top-left X (un-rotated source image space) */
    int32_t block_y;        /**< source block top-left Y */
    int32_t block_w;        /**< source block width  */
    int32_t block_h;        /**< source block height */
} lv_draw_ppa_rot_block_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Map a visible render tile back into the PPA source block for a 90/180/270
 * rotation step (scale fixed at 1:1).
 *
 * Clips the rotated image to the render tile, computes a non-negative
 * buffer-local destination window (clamped to the destination buffer so the
 * PPA can never write out of bounds), and inverts the rotation to find the
 * source sub-block that maps onto that window.
 *
 * Rotation convention: @p angle is the LVGL clockwise angle in 1/10-degree
 * units, already normalised to one of 900 / 1800 / 2700. @p coords is the
 * on-screen bounding box of the rotated image (LVGL's task area): for 90/270
 * its size is img_h x img_w, for 180 it is img_w x img_h. The block geometry
 * for the 90/270 steps assumes screen-space clockwise rotation; a maintainer
 * with P4 hardware can swap the 900/2700 cases if the rendered result is
 * mirrored, without affecting the clamping/safety guarantees.
 *
 * @param coords     on-screen bounding box of the rotated image (screen coords)
 * @param buf_area   the layer buffer (render tile) area, in screen coordinates
 * @param buf_w      destination buffer width  in pixels
 * @param buf_h      destination buffer height in pixels
 * @param img_w      decoded source image width  in pixels
 * @param img_h      decoded source image height in pixels
 * @param angle      normalised rotation, one of 900 / 1800 / 2700
 * @return           the computed block; check `.draw` before using the rest
 */
static inline lv_draw_ppa_rot_block_t lv_draw_ppa_rot_calc_block(
    const lv_area_t * coords, const lv_area_t * buf_area,
    int32_t buf_w, int32_t buf_h, int32_t img_w, int32_t img_h,
    int32_t angle)
{
    lv_draw_ppa_rot_block_t r;
    lv_memzero(&r, sizeof(r));

    /* Clip the rotated image to the render tile. */
    lv_area_t vis;
    if(!lv_area_intersect(&vis, coords, buf_area)) return r;

    /* Offset of the visible window inside the full rotated output. */
    int32_t out_dx = vis.x1 - coords->x1;
    int32_t out_dy = vis.y1 - coords->y1;
    int32_t vw = lv_area_get_width(&vis);
    int32_t vh = lv_area_get_height(&vis);

    /* Buffer-local destination (non-negative: vis is inside buf_area). */
    lv_area_copy(&r.dest_area, &vis);
    lv_area_move(&r.dest_area, -buf_area->x1, -buf_area->y1);

    /* Clamp the destination window to the buffer so the PPA never writes out
     * of bounds, even if coords and buf_area disagree. */
    if(r.dest_area.x1 < 0 || r.dest_area.y1 < 0) return r;
    if(r.dest_area.x1 >= buf_w || r.dest_area.y1 >= buf_h) return r;
    if(r.dest_area.x1 + vw > buf_w) vw = buf_w - r.dest_area.x1;
    if(r.dest_area.y1 + vh > buf_h) vh = buf_h - r.dest_area.y1;
    if(vw <= 0 || vh <= 0) return r;

    /* Invert the rotation: find the source sub-block that maps onto the visible
     * output window. For 90/270 the source axes are swapped, so the source
     * block dimensions are (vh, vw); for 180 they stay (vw, vh). */
    int32_t sx, sy, sw, sh;
    switch(angle) {
        case 1800: /* 180 */
            sw = vw;
            sh = vh;
            sx = img_w - out_dx - vw;
            sy = img_h - out_dy - vh;
            break;
        case 900:  /* 90 clockwise */
            sw = vh;
            sh = vw;
            sx = out_dy;
            sy = img_h - out_dx - vw;
            break;
        case 2700: /* 270 clockwise */
            sw = vh;
            sh = vw;
            sx = img_w - out_dy - vh;
            sy = out_dx;
            break;
        default:
            return r;
    }

    /* Reject any block that falls outside the source image; clamp the far edge. */
    if(sx < 0 || sy < 0 || sx >= img_w || sy >= img_h) return r;
    if(sx + sw > img_w) sw = img_w - sx;
    if(sy + sh > img_h) sh = img_h - sy;
    if(sw <= 0 || sh <= 0) return r;

    r.block_x = sx;
    r.block_y = sy;
    r.block_w = sw;
    r.block_h = sh;
    r.draw = true;
    return r;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_PPA_ROT_H */
