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
#include "../misc/lv_profiler.h"
#include "lv_img_decoder.h"
#include "lv_img_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_img_dsc_t;
struct _lv_disp_t;

typedef enum {
    LV_DRAW_TASK_TYPE_FILL,
    LV_DRAW_TASK_TYPE_BORDER,
    LV_DRAW_TASK_TYPE_BOX_SHADOW,
    LV_DRAW_TASK_TYPE_BG_IMG,
    LV_DRAW_TASK_TYPE_LABEL,
    LV_DRAW_TASK_TYPE_IMAGE,
    LV_DRAW_TASK_TYPE_LAYER,
    LV_DRAW_TASK_TYPE_LINE,
    LV_DRAW_TASK_TYPE_ARC,
    LV_DRAW_TASK_TYPE_TRIANGLE,
    LV_DRAW_TASK_TYPE_MASK_RECTANGLE,
    LV_DRAW_TASK_TYPE_MASK_BITMAP,
} lv_draw_task_type_t;

typedef enum {
    LV_DRAW_TASK_STATE_WAITING,     /*Waiting for something to be finished. E.g. rendering a layer*/
    LV_DRAW_TASK_STATE_QUEUED,
    LV_DRAW_TASK_STATE_IN_PROGRESS,
    LV_DRAW_TASK_STATE_READY,
} lv_draw_task_state_t;

typedef struct _lv_draw_task_t {
    struct _lv_draw_task_t * next;

    lv_draw_task_type_t type;

    /**
     * The bounding box of the thing to draw
     */
    lv_area_t area;

    /** The original area which is updated*/
    lv_area_t clip_area_original;

    /**
     * The clip area of the layer is saved here when the draw task is created.
     * As the clip area of the layer can be changed as new draw tasks are added its current value needs to be saved.
     * Therefore during drawing the layer's clip area shouldn't be used as it might be already changed for other draw tasks.
     */
    lv_area_t clip_area;

    volatile int state;              /*int instead of lv_draw_task_state_t to be sure its atomic*/

    void * draw_dsc;
} lv_draw_task_t;

typedef struct {
    void * user_data;
} lv_draw_mask_t;

typedef struct _lv_draw_unit_t {
    struct _lv_draw_unit_t * next;

    /**
     * The target_layer on which drawing should happen
     */
    struct _lv_layer_t * target_layer;

    const lv_area_t * clip_area;

    /**
     * Try to assign a draw task to itself.
     * `lv_draw_get_next_available_task` can be used to get an independent draw task.
     * A draw task should be assign only if the draw unit can draw it too
     * @param draw_unit     pointer to the draw unit
     * @param layer         pointer to a layer on which the draw task should be drawn
     * @return              >=0:    The number of taken draw task
     *                      -1:     There where no available draw tasks at all.
     *                              Also means to no call the dispatcher of the other draw units as there is no dra task to tkae
     */
    int32_t (*dispatch)(struct _lv_draw_unit_t * draw_unit, struct _lv_layer_t * layer);
} lv_draw_unit_t;


typedef struct _lv_layer_t  {
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
     * Can be set before new draw tasks are added to indicate the clip area of the draw tasks.
     * Therefore `lv_draw_add_task()` always saves it in the new draw task to know the clip area when the draw task was added.
     * During drawing the draw units also sees the saved clip_area and should use it during drawing.
     * During drawing the layer's clip area shouldn't be used as it might be already changed for other draw tasks.
     */
    lv_area_t clip_area;

    /**
     * The rendered image in layer->buf will be converted to this format
     * using target_layer->buffer_convert.
     */
    lv_color_format_t color_format;

    /**
     * Copy an area from buffer to an other
     * @param layer      pointer to a draw context
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
    void (*buffer_copy)(struct _lv_layer_t * target_layer, void * dest_buf, lv_coord_t dest_stride,
                        const lv_area_t * dest_area,
                        void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

    /**
     * Convert the content of `target_layer->buf` to `target_layer->color_format`
     * @param target_layer
     */
    void (*buffer_convert)(struct _lv_layer_t * layer);

    void (*buffer_clear)(struct _lv_layer_t * target_layer, const lv_area_t * a);

    /**
     * Linked list of draw tasks
     */
    lv_draw_task_t * draw_task_head;

    struct _lv_layer_t * parent;
    struct _lv_layer_t * next;
    bool all_tasks_added;
    void * user_data;
} lv_layer_t;

typedef struct {
    struct _lv_obj_t * obj;
    uint32_t part;
    uint32_t id1;
    uint32_t id2;
    lv_layer_t * layer;
} lv_draw_dsc_base_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_init(void);

/**
 * Allocate a new draw unit with the given size and appends it to the list of draw units
 * @param size      the size to allocate. E.g. `sizeof(my_draw_unit_t)`,
 *                  where the first element of `my_draw_unit_t` is `lv_draw_unit_t`.
 */
void * lv_draw_create_unit(size_t size);

lv_draw_task_t * lv_draw_add_task(lv_layer_t * layer, const lv_area_t * coords);

void lv_draw_finalize_task_creation(lv_layer_t * layer, lv_draw_task_t * t);

void lv_draw_dispatch(void);

bool lv_draw_dispatch_layer(struct _lv_disp_t * disp, lv_layer_t * layer);

/**
 * Wait for a new dispatch request.
 * It's blocking if `LV_USE_OS == 0` else it yields
 */
void lv_draw_dispatch_wait_for_request(void);

void lv_draw_dispatch_request(void);

/**
 * Find and available draw task
 * @param layer      the draw ctx to search in
 * @param t_prev        continue searching from this task
 * @return              tan available draw task or NULL if there is no any
 */
lv_draw_task_t * lv_draw_get_next_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev);

/**
 * Create a new layer on a parent layer
 * @param parent_layer      the parent layer to which the layer will be merged when it's rendered
 * @param color_format      the color format of the layer
 * @param area              the areas of the layer (absolute coordinates)
 * @return                  the new target_layer or NULL on error
 */
lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area);


/**
 * Call to tell that a layer buffer with X kB size was allocated
 * @param kb        size of the layer buffer in kB (if < 1024 use 1)
 */
void lv_draw_add_used_layer_size(uint32_t kb);

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
#include "lv_draw_triangle.h"
#include "lv_draw_mask.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
