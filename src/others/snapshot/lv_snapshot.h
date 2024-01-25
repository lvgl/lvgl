/**
 * @file lv_snapshot.h
 *
 */

#ifndef LV_SNAPSHOT_H
#define LV_SNAPSHOT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>

#include "../../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_SNAPSHOT
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Take snapshot for object with its children, create the draw buffer as needed.
 * @param obj   the object to generate snapshot.
 * @param cf    color format for generated image.
 * @return      a pointer to an draw buffer containing snapshot image, or NULL if failed.
 */
lv_draw_buf_t * lv_snapshot_take(lv_obj_t * obj, lv_color_format_t cf);

/**
 * Create a draw buffer to store the snapshot image for object.
 * @param obj   the object to generate snapshot.
 * @param cf    color format for generated image.
 * @return      a pointer to an draw buffer ready for taking snapshot, or NULL if failed.
 */
lv_draw_buf_t * lv_snapshot_create_draw_buf(lv_obj_t * obj, lv_color_format_t cf);

/**
 * Reshape the draw buffer to prepare for taking snapshot for obj.
 * This is usually used to check if the existing draw buffer is enough for
 * obj snapshot. If return LV_RESULT_INVALID, you should create a new one.
 * @param draw_buf  the draw buffer to reshape.
 * @param obj       the object to generate snapshot.
 */
lv_result_t lv_snapshot_reshape_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Take snapshot for object with its children, save image info to provided buffer.
 * @param obj       the object to generate snapshot.
 * @param cf        color format for new snapshot image.
 *                  It could differ with cf of `draw_buf` as long as the new cf will fit in.
 * @param draw_buf  the draw buffer to store the image result. It's reshaped automatically.
 * @return          LV_RESULT_OK on success, LV_RESULT_INVALID on error.
 */
lv_result_t lv_snapshot_take_to_draw_buf(lv_obj_t * obj, lv_color_format_t cf, lv_draw_buf_t * draw_buf);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_SNAPSHOT*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
