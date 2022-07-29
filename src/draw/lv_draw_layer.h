/**
 * @file lv_draw_layer.h
 *
 */

#ifndef LV_DRAW_LAYER_H
#define LV_DRAW_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_draw_ctx_t;
struct _lv_draw_layer_ctx_t;

typedef enum {
    LV_DRAW_LAYER_FLAG_NONE,
    LV_DRAW_LAYER_FLAG_HAS_ALPHA,
    LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE,
} lv_draw_layer_flags_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a new layer context. It is used to start and independent rendering session
 * with the current draw_ctx
 * @param draw_ctx      pointer to the current draw context
 * @param layer_area    the coordinates of the layer
 * @param flags         OR-ed flags from @lv_draw_layer_flags_t
 * @return              pointer to the layer context, or NULL on error
 */
struct _lv_draw_layer_ctx_t * lv_draw_layer_create(struct _lv_draw_ctx_t * draw_ctx, const lv_area_t * layer_area,
                                                   lv_draw_layer_flags_t flags);

/**
 * Adjust the layer_ctx and/or draw_ctx based on the `layer_ctx->area_act`.
 * It's called only if flags has `LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE`
 * @param draw_ctx      pointer to the current draw context
 * @param layer_ctx     pointer to a layer context
 * @param flags         OR-ed flags from @lv_draw_layer_flags_t
 */
void lv_draw_layer_adjust(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx,
                          lv_draw_layer_flags_t flags);

/**
 * Blend a rendered layer to `layer_ctx->area_act`
 * @param draw_ctx      pointer to the current draw context
 * @param layer_ctx     pointer to a layer context
 * @param draw_dsc      pointer to an image draw descriptor
 */
void lv_draw_layer_blend(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx,
                         lv_draw_img_dsc_t * draw_dsc);

/**
 * Destroy a layer context.
 * @param draw_ctx      pointer to the current draw context
 * @param layer_ctx     pointer to a layer context
 */
void lv_draw_layer_destroy(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_LAYER_H*/
