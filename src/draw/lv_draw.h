/**
 * @file lv_draw.h
 *
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_style.h"
#include "../misc/lv_txt.h"
#include "lv_img_decoder.h"
#include "lv_img_cache.h"

#include "lv_draw_rect.h"
#include "lv_draw_label.h"
#include "lv_draw_img.h"
#include "lv_draw_line.h"
#include "lv_draw_triangle.h"
#include "lv_draw_arc.h"
#include "lv_draw_mask.h"
#include "lv_draw_transform.h"
#include "lv_draw_layer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_TASK_TYPE_RECTANGLE,
    LV_DRAW_TASK_TYPE_LABEL,
    LV_DRAW_TASK_TYPE_LINE,
    LV_DRAW_TASK_TYPE_ARC,
} lv_draw_task_type_t;

typedef enum {
    LV_DRAW_TASK_STATE_QUEUED,
    LV_DRAW_TASK_STATE_IN_PRGRESS,
    LV_DRAW_TASK_STATE_READY,
} lv_draw_task_state_t;

typedef struct _lv_draw_task_t {
    struct _lv_draw_task_t * next;
    lv_draw_task_type_t type;
    lv_area_t area;
    lv_area_t clip_area;
    lv_draw_task_state_t state;
    void * draw_dsc;
} lv_draw_task_t;

typedef struct {
    void * user_data;
} lv_draw_mask_t;

typedef struct _lv_draw_layer_ctx_t {
    lv_area_t area_full;
    lv_area_t area_act;
    lv_coord_t max_row_with_alpha;
    lv_coord_t max_row_with_no_alpha;
    void * buf;
    struct {
        const lv_area_t * clip_area;
        lv_area_t * buf_area;
        void * buf;
        lv_color_format_t color_format;
    } original;
} lv_draw_layer_ctx_t;

typedef struct _lv_draw_unit_t {
    struct _lv_draw_unit_t * next;
    struct _lv_draw_ctx_t * draw_ctx;
    const lv_area_t * clip_area;

    int32_t (*dispatch)(struct _lv_draw_unit_t * draw_unit, struct _lv_draw_ctx_t * draw_ctx);

    /**
     * Wait until all background operations are finished. (E.g. GPU operations)
     */
    void (*wait_for_finish)(struct _lv_draw_ctx_t * draw_ctx);
} lv_draw_unit_t;


typedef struct _lv_draw_ctx_t  {
    /**
     *  Pointer to a buffer to draw into
     */
    void * buf;

    /**
     * The position and size of `buf` (absolute coordinates)
     */
    lv_area_t * buf_area;

    /**
     * The current clip area with absolute coordinates, always the same or smaller than `buf_area`
     */
    const lv_area_t * clip_area;

    /**
     * The rendered image in draw_ctx->buf will be converted to this format
     * using draw_ctx->buffer_convert.
     */
    lv_color_format_t color_format;

    lv_draw_unit_t * draw_unit_head;
    int dispatch_req;       /*`int` type to be sure it's atomic write/read*/

    /**
     * Copy an area from buffer to an other
     * @param draw_ctx      pointer to a draw context
     * @param dest_buf      copy the buffer into this buffer
     * @param dest_stride   the width of the dest_buf in pixels
     * @param dest_area     the destination area
     * @param src_buf       copy from this buffer
     * @param src_stride    the width of src_buf in pixels
     * @param src_area      the source area.
     *
     * @note dest_area and src_area must have the same width and height
     *       but can have different x and y position.
     * @note dest_area and src_area must be clipped to the real dimensions of the buffers
     */
    void (*buffer_copy)(struct _lv_draw_ctx_t * draw_ctx, void * dest_buf, lv_coord_t dest_stride,
                        const lv_area_t * dest_area,
                        void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

    /**
     * Convert the content of `draw_ctx->buf` to `draw_ctx->color_format`
     * @param draw_ctx
     */
    void (*buffer_convert)(struct _lv_draw_ctx_t * draw_ctx);

    void (*buffer_clear)(struct _lv_draw_ctx_t * draw_ctx);

    /**
     * Initialize a new layer context.
     * The original buffer and area data are already saved from `draw_ctx` to `layer_ctx`
     * @param draw_ctx      pointer to the current draw context
     * @param layer_area    the coordinates of the layer
     * @param flags         OR-ed flags from @lv_draw_layer_flags_t
     * @return              pointer to the layer context, or NULL on error
     */
    struct _lv_draw_layer_ctx_t * (*layer_init)(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx,
                                                lv_draw_layer_flags_t flags);

    /**
     * Adjust the layer_ctx and/or draw_ctx based on the `layer_ctx->area_act`.
     * It's called only if flags has `LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE`
     * @param draw_ctx      pointer to the current draw context
     * @param layer_ctx     pointer to a layer context
     * @param flags         OR-ed flags from @lv_draw_layer_flags_t
     */
    void (*layer_adjust)(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx,
                         lv_draw_layer_flags_t flags);

    /**
     * Blend a rendered layer to `layer_ctx->area_act`
     * @param draw_ctx      pointer to the current draw context
     * @param layer_ctx     pointer to a layer context
     * @param draw_dsc      pointer to an image draw descriptor
     */
    void (*layer_blend)(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx,
                        const lv_draw_img_dsc_t * draw_dsc);

    /**
     * Destroy a layer context. The original buffer and area data of the `draw_ctx` will be restored
     * and the `layer_ctx` itself will be freed automatically.
     * @param draw_ctx      pointer to the current draw context
     * @param layer_ctx     pointer to a layer context
     */
    void (*layer_destroy)(struct _lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx);

    /**
     * Size of a layer context in bytes.
     */
    size_t layer_instance_size;

    /**
     * Linked list of draw tasks
     */
    lv_draw_task_t * draw_task_head;

#if LV_USE_USER_DATA
    void * user_data;
#endif
} lv_draw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_init(void);

void lv_draw_wait_for_finish(lv_draw_ctx_t * draw_ctx);

void lv_draw_dispatch(lv_draw_ctx_t * draw_ctx);

void lv_draw_dispatch_if_requested(lv_draw_ctx_t * draw_ctx);

void lv_draw_dispatch_request(lv_draw_ctx_t * draw_ctx);

lv_draw_task_t * lv_draw_get_next_available_task(lv_draw_ctx_t * draw_ctx, lv_draw_task_t * t_prev);


/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
