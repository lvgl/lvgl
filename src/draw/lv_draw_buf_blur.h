/**
 * @file lv_draw_buf_blur.h
 *
 */

#ifndef LV_DRAW_BUF_BLUR_H
#define LV_DRAW_BUF_BLUR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_BUF_BLUR_TYPE_EXP,
    _LV_DRAW_BUF_BLUR_TYPE_LAST,
} lv_draw_buf_blur_type_t;

typedef struct {
    lv_draw_buf_blur_type_t type; /**< The type of the blur effect*/
    uint16_t radius; /**< The radius of the blur*/
    uint8_t  aprec; /**< The number of bits of the alpha channel (for exp blur only)*/
    uint8_t  zprec; /**< The number of bits of the z-buffer (for exp blur only)*/
} lv_draw_buf_blur_args_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the effect arguments to default values.
 * @param args  the effect arguments to initialize
 */
void lv_draw_buf_blur_args_init(lv_draw_buf_blur_args_t * args);

/**
 * Apply a blur effect to the draw buffer.
 * @param dst_buf  the dst draw buffer to apply the effect
 * @param src_buf  the src draw buffer to apply the effect
 * @param args     the effect arguments
 * @return         LV_RES_OK: the effect was applied successfully; LV_RES_INV: the effect is not supported or failed
 */
lv_result_t lv_draw_buf_blur(lv_draw_buf_t * dst_buf,
                             const lv_draw_buf_t * src_buf,
                             const lv_draw_buf_blur_args_t * args);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_BLUR_H*/
