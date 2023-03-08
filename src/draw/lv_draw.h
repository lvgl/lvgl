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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_img_dsc_t;

typedef enum {
    LV_DRAW_TASK_TYPE_RECTANGLE,
    LV_DRAW_TASK_TYPE_LABEL,
    LV_DRAW_TASK_TYPE_IMAGE,
    LV_DRAW_TASK_TYPE_LAYER,
    LV_DRAW_TASK_TYPE_LINE,
    LV_DRAW_TASK_TYPE_ARC,
} lv_draw_task_type_t;

typedef enum {
    LV_DRAW_TASK_STATE_WAITING,     /*Waiting for something to be finished. E.g. rendering a layer*/
    LV_DRAW_TASK_STATE_QUEUED,
    LV_DRAW_TASK_STATE_IN_PRGRESS,
    LV_DRAW_TASK_STATE_READY,
} lv_draw_task_state_t;

typedef struct _lv_draw_task_t {
    struct _lv_draw_task_t * next;
    lv_draw_task_type_t type;
    lv_area_t area;
    lv_area_t clip_area;
    int state;              /*int instead of lv_draw_task_state_t to be sure its atomic*/
    void * draw_dsc;
} lv_draw_task_t;

typedef struct {
    void * user_data;
} lv_draw_mask_t;

typedef struct _lv_draw_unit_t {
    struct _lv_draw_unit_t * next;
    struct _lv_draw_ctx_t * draw_ctx;
    const lv_area_t * clip_area;

    int32_t (*dispatch)(struct _lv_draw_unit_t * draw_unit, struct _lv_draw_ctx_t * draw_ctx);
} lv_draw_unit_t;


typedef struct _lv_draw_ctx_t  {
    /**
     *  Pointer to a buffer to draw into
     */
    void * buf;

    /**
     * The position and size of `buf` (absolute coordinates)
     */
    lv_area_t buf_area;

    /**
     * The current clip area with absolute coordinates, always the same or smaller than `buf_area`
     */
    lv_area_t clip_area;

    /**
     * The rendered image in draw_ctx->buf will be converted to this format
     * using draw_ctx->buffer_convert.
     */
    lv_color_format_t color_format;

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
     * Linked list of draw tasks
     */
    lv_draw_task_t * draw_task_head;

    struct _lv_draw_ctx_t * parent;
    struct _lv_draw_ctx_t * next;
    bool all_tasks_added;

#if LV_USE_USER_DATA
    void * user_data;
#endif
} lv_draw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_init(void);

void lv_draw_wait_for_finish(lv_draw_ctx_t * draw_ctx);

lv_draw_task_t * lv_draw_add_task(lv_draw_ctx_t * draw_ctx, const lv_area_t * coords);

void lv_draw_dispatch(void);

void lv_draw_dispatch_if_requested(void);

void lv_draw_dispatch_request(void);

/**
 * Find and available draw task
 * @param draw_ctx      the draw ctx to search in
 * @param t_prev        continue searching from this task
 * @return              tan available draw task or NULL if there is no any
 */
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
#include "lv_draw_rect.h"
#include "lv_draw_label.h"
#include "lv_draw_img.h"
#include "lv_draw_arc.h"
#include "lv_draw_line.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
