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

#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#include "../misc/lv_text.h"
#include "../misc/lv_profiler.h"
#include "lv_image_decoder.h"
#include "../osal/lv_os.h"
#include "lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_UNIT_ID_ANY  0

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
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

struct _lv_draw_task_t {
    lv_draw_task_t * next;

    lv_draw_task_type_t type;

    /**
     * The area where to draw
     */
    lv_area_t area;

    /**
     * The real draw area. E.g. for shadow, outline, or transformed images it's different from `area`
     */
    lv_area_t _real_area;

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

    /**
     * The ID of the draw_unit which should take this task
     */
    uint8_t preferred_draw_unit_id;

    /**
     * Set to which extent `preferred_draw_unit_id` is good at this task.
     * 80: means 20% better (faster) than software rendering
     * 100: the default value
     * 110: means 10% worse (slower) than software rendering
     */
    uint8_t preference_score;

};

typedef struct {
    void * user_data;
} lv_draw_mask_t;

struct _lv_draw_unit_t {
    lv_draw_unit_t * next;

    /**
     * The target_layer on which drawing should happen
     */
    lv_layer_t * target_layer;

    const lv_area_t * clip_area;

    /**
     * Called to try to assign a draw task to itself.
     * `lv_draw_get_next_available_task` can be used to get an independent draw task.
     * A draw task should be assign only if the draw unit can draw it too
     * @param draw_unit     pointer to the draw unit
     * @param layer         pointer to a layer on which the draw task should be drawn
     * @return              >=0:    The number of taken draw task:
     *                                  0 means the task has not yet been completed.
     *                                  1 means a new task has been accepted.
     *                      -1:     The draw unit wanted to work on a task but couldn't do that
     *                              due to some errors (e.g. out of memory).
     *                              It signals that LVGL should call the dispatcher later again
     *                              to let draw unit try to start the rendering again.
     */
    int32_t (*dispatch_cb)(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

    /**
     *
     * @param draw_unit
     * @param task
     * @return
     */
    int32_t (*evaluate_cb)(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

    /**
     * Called to delete draw unit.
     * @param draw_unit
     * @return
     */
    int32_t (*delete_cb)(lv_draw_unit_t * draw_unit);
};

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

    /** Linked list of draw tasks */
    lv_draw_task_t * draw_task_head;

    lv_layer_t * parent;
    lv_layer_t * next;
    bool all_tasks_added;
    void * user_data;
};

typedef struct {
    lv_obj_t * obj;
    uint32_t part;
    uint32_t id1;
    uint32_t id2;
    lv_layer_t * layer;
    size_t dsc_size;
    void * user_data;
} lv_draw_dsc_base_t;

typedef struct {
    lv_draw_unit_t * unit_head;
    uint32_t used_memory_for_layers_kb;
#if LV_USE_OS
    lv_thread_sync_t sync;
#else
    int dispatch_req;
#endif
    lv_mutex_t circle_cache_mutex;
    bool task_running;
} lv_draw_global_info_t;

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
 * @param t         poinr to a draw task
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
 * When a draw unit finished a draw task it needs to request dispatching
 * to let LVGL assign a new draw task to it
 */
void lv_draw_dispatch_request(void);

/**
 * Find and available draw task
 * @param layer             the draw ctx to search in
 * @param t_prev            continue searching from this task
 * @param draw_unit_id      check the task where `preferred_draw_unit_id` equals this value or `LV_DRAW_UNIT_ID_ANY`
 * @return                  tan available draw task or NULL if there is no any
 */
lv_draw_task_t * lv_draw_get_next_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev, uint8_t draw_unit_id);

/**
 * Tell how many draw task are waiting to be drawn on the area of `t_check`.
 * It can be used to determine if a GPU shall combine many draw tasks in to one or not.
 * If a lot of tasks are waiting for the current ones it makes sense to draw them one-by-one
 * to not block the dependent tasks' rendering
 * @param t_check   the task whose dependent tasks shall be counted
 * @return          number of tasks depending on `t_check`
 */
uint32_t lv_draw_get_dependent_count(lv_draw_task_t * t_check);

/**
 * Create a new layer on a parent layer
 * @param parent_layer      the parent layer to which the layer will be merged when it's rendered
 * @param color_format      the color format of the layer
 * @param area              the areas of the layer (absolute coordinates)
 * @return                  the new target_layer or NULL on error
 */
lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area);

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
#include "lv_draw_image.h"
#include "lv_draw_arc.h"
#include "lv_draw_line.h"
#include "lv_draw_triangle.h"
#include "lv_draw_mask.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
