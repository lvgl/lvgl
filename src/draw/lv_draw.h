/**
 * @file lv_draw.h
 *
 */

/**
 * Modified by NXP in 2024
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

#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#include "../misc/lv_text.h"
#include "../misc/lv_profiler.h"
#include "../misc/lv_matrix.h"
#include "lv_image_decoder.h"
#include "lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_UNIT_NONE  0
#define LV_DRAW_UNIT_IDLE  -1   /**< The draw unit is idle, new dispatching might be requested to try again */

#if LV_DRAW_TRANSFORM_USE_MATRIX
#if !LV_USE_MATRIX
#error "LV_DRAW_TRANSFORM_USE_MATRIX requires LV_USE_MATRIX = 1"
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_TASK_TYPE_NONE = 0,
    LV_DRAW_TASK_TYPE_FILL,
    LV_DRAW_TASK_TYPE_BORDER,
    LV_DRAW_TASK_TYPE_BOX_SHADOW,
    LV_DRAW_TASK_TYPE_LABEL,
    LV_DRAW_TASK_TYPE_IMAGE,
    LV_DRAW_TASK_TYPE_LAYER,
    LV_DRAW_TASK_TYPE_LINE,
    LV_DRAW_TASK_TYPE_ARC,
    LV_DRAW_TASK_TYPE_TRIANGLE,
    LV_DRAW_TASK_TYPE_MASK_RECTANGLE,
    LV_DRAW_TASK_TYPE_MASK_BITMAP,
    LV_DRAW_TASK_TYPE_VECTOR,
} lv_draw_task_type_t;

typedef enum {
    LV_DRAW_TASK_STATE_WAITING,     /*Waiting for something to be finished. E.g. rendering a layer*/
    LV_DRAW_TASK_STATE_QUEUED,
    LV_DRAW_TASK_STATE_IN_PROGRESS,
    LV_DRAW_TASK_STATE_READY,
} lv_draw_task_state_t;

struct _lv_layer_t  {

    /** Target draw buffer of the layer*/
    lv_draw_buf_t * draw_buf;

    /** The absolute coordinates of the buffer */
    lv_area_t buf_area;

    /** The color format of the layer. LV_COLOR_FORMAT_...  */
    lv_color_format_t color_format;

    /**
     * NEVER USE IT DRAW UNITS. USED INTERNALLY DURING DRAW TASK CREATION.
     * The current clip area with absolute coordinates, always the same or smaller than `buf_area`
     * Can be set before new draw tasks are added to indicate the clip area of the draw tasks.
     * Therefore `lv_draw_add_task()` always saves it in the new draw task to know the clip area when the draw task was added.
     * During drawing the draw units also sees the saved clip_area and should use it during drawing.
     * During drawing the layer's clip area shouldn't be used as it might be already changed for other draw tasks.
     */
    lv_area_t _clip_area;

    /**
     * The physical clipping area relative to the display.
     */
    lv_area_t phy_clip_area;

#if LV_DRAW_TRANSFORM_USE_MATRIX
    /** Transform matrix to be applied when rendering the layer */
    lv_matrix_t matrix;
#endif

    /** Linked list of draw tasks */
    lv_draw_task_t * draw_task_head;

    lv_layer_t * parent;
    lv_layer_t * next;
    bool all_tasks_added;
    void * user_data;
};

typedef struct {
    lv_obj_t * obj;
    lv_part_t part;
    uint32_t id1;
    uint32_t id2;
    lv_layer_t * layer;
    size_t dsc_size;
    void * user_data;
} lv_draw_dsc_base_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Used internally to initialize the drawing module
 */
void lv_draw_init(void);

/**
 * Deinitialize the drawing module
 */
void lv_draw_deinit(void);

/**
 * Allocate a new draw unit with the given size and appends it to the list of draw units
 * @param size      the size to allocate. E.g. `sizeof(my_draw_unit_t)`,
 *                  where the first element of `my_draw_unit_t` is `lv_draw_unit_t`.
 */
void * lv_draw_create_unit(size_t size);

/**
 * Add an empty draw task to the draw task list of a layer.
 * @param layer     pointer to a layer
 * @param coords    the coordinates of the draw task
 * @return          the created draw task which needs to be
 *                  further configured e.g. by added a draw descriptor
 */
lv_draw_task_t * lv_draw_add_task(lv_layer_t * layer, const lv_area_t * coords);

/**
 * Needs to be called when a draw task is created and configured.
 * It will send an event about the new draw task to the widget
 * and assign it to a draw unit.
 * @param layer     pointer to a layer
 * @param t         pointer to a draw task
 */
void lv_draw_finalize_task_creation(lv_layer_t * layer, lv_draw_task_t * t);

/**
 * Try dispatching draw tasks to draw units
 */
void lv_draw_dispatch(void);

/**
 * Used internally to try dispatching draw tasks of a specific layer
 * @param disp      pointer to a display on which the dispatching was requested
 * @param layer     pointer to a layer
 * @return          at least one draw task is being rendered (maybe it was taken earlier)
 */
bool lv_draw_dispatch_layer(lv_display_t * disp, lv_layer_t * layer);

/**
 * Wait for a new dispatch request.
 * It's blocking if `LV_USE_OS == 0` else it yields
 */
void lv_draw_dispatch_wait_for_request(void);

/**
 * Wait for draw finish in case of asynchronous task execution.
 * If `LV_USE_OS == 0` it just return.
 */
void lv_draw_wait_for_finish(void);

/**
 * When a draw unit finished a draw task it needs to request dispatching
 * to let LVGL assign a new draw task to it
 */
void lv_draw_dispatch_request(void);

/**
 * Get the total number of draw units.
  */
uint32_t lv_draw_get_unit_count(void);

/**
 * Find and available draw task
 * @param layer             the draw ctx to search in
 * @param t_prev            continue searching from this task
 * @param draw_unit_id      check the task where `preferred_draw_unit_id` equals this value or `LV_DRAW_UNIT_NONE`
 * @return                  tan available draw task or NULL if there is no any
 */
lv_draw_task_t * lv_draw_get_next_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev, uint8_t draw_unit_id);

/**
 * Tell how many draw task are waiting to be drawn on the area of `t_check`.
 * It can be used to determine if a GPU shall combine many draw tasks into one or not.
 * If a lot of tasks are waiting for the current ones it makes sense to draw them one-by-one
 * to not block the dependent tasks' rendering
 * @param t_check   the task whose dependent tasks shall be counted
 * @return          number of tasks depending on `t_check`
 */
uint32_t lv_draw_get_dependent_count(lv_draw_task_t * t_check);

/**
 * Create (allocate) a new layer on a parent layer
 * @param parent_layer      the parent layer to which the layer will be merged when it's rendered
 * @param color_format      the color format of the layer
 * @param area              the areas of the layer (absolute coordinates)
 * @return                  the new target_layer or NULL on error
 */
lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area);

/**
 * Initialize a layer which is allocated by the user
 * @param layer             pointer the layer to initialize (its lifetime needs to be managed by the user)
 * @param parent_layer      the parent layer to which the layer will be merged when it's rendered
 * @param color_format      the color format of the layer
 * @param area              the areas of the layer (absolute coordinates)
 * @return                  the new target_layer or NULL on error
 */
void lv_draw_layer_init(lv_layer_t * layer, lv_layer_t * parent_layer, lv_color_format_t color_format,
                        const lv_area_t * area);

/**
 * Try to allocate a buffer for the layer.
 * @param layer             pointer to a layer
 * @return                  pointer to the allocated aligned buffer or NULL on failure
 */
void * lv_draw_layer_alloc_buf(lv_layer_t * layer);

/**
 * Got to a pixel at X and Y coordinate on a layer
 * @param layer             pointer to a layer
 * @param x                 the target X coordinate
 * @param y                 the target X coordinate
 * @return                  `buf` offset to point to the given X and Y coordinate
 */
void * lv_draw_layer_go_to_xy(lv_layer_t * layer, int32_t x, int32_t y);

/**
 * Get the type of a draw task
 * @param t   the draw task to get the type of
 * @return    the draw task type
*/
lv_draw_task_type_t lv_draw_task_get_type(const lv_draw_task_t * t);

/**
 * Get the draw descriptor of a draw task
 * @param t   the draw task to get the draw descriptor of
 * @return    a void pointer to the draw descriptor
*/
void * lv_draw_task_get_draw_dsc(const lv_draw_task_t * t);

/**
 * Get the draw area of a draw task
 * @param t      the draw task to get the draw area of
 * @param area   the destination where the draw area will be stored
*/
void lv_draw_task_get_area(const lv_draw_task_t * t, lv_area_t * area);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
