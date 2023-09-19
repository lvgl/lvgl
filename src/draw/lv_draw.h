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
#include "../misc/lv_text.h"
#include "../misc/lv_profiler.h"
#include "../misc/lv_cache.h"
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

struct _lv_draw_image_dsc_t;
struct _lv_display_t;

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

    /**
     * The ID of the draw_unit which should take this task
     */
    uint8_t preferred_draw_unit_id;

    /**
     * Set to which extent `preferred_draw_unit_id` is good at this task.
     * 80: means 20% better (faster) than software rendering
     * 100: the default value
     * 110: means 10% better (faster) than software rendering
     */
    uint8_t preference_score;

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
     * Called to try to assign a draw task to itself.
     * `lv_draw_get_next_available_task` can be used to get an independent draw task.
     * A draw task should be assign only if the draw unit can draw it too
     * @param draw_unit     pointer to the draw unit
     * @param layer         pointer to a layer on which the draw task should be drawn
     * @return              >=0:    The number of taken draw task
     *                      -1:     There where no available draw tasks at all.
     *                              Also means to no call the dispatcher of the other draw units as there is no draw task to take
     */
    int32_t (*dispatch_cb)(struct _lv_draw_unit_t * draw_unit, struct _lv_layer_t * layer);

    /**
     *
     * @param draw_unit
     * @param task
     * @return
     */
    int32_t (*evaluate_cb)(struct _lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
} lv_draw_unit_t;


typedef struct _lv_layer_t  {

    lv_draw_buf_t draw_buf;
    lv_point_t draw_buf_ofs;

    /**
     * The current clip area with absolute coordinates, always the same or smaller than `buf_area`
     * Can be set before new draw tasks are added to indicate the clip area of the draw tasks.
     * Therefore `lv_draw_add_task()` always saves it in the new draw task to know the clip area when the draw task was added.
     * During drawing the draw units also sees the saved clip_area and should use it during drawing.
     * During drawing the layer's clip area shouldn't be used as it might be already changed for other draw tasks.
     */
    lv_area_t clip_area;

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

bool lv_draw_dispatch_layer(struct _lv_display_t * disp, lv_layer_t * layer);

/**
 * Wait for a new dispatch request.
 * It's blocking if `LV_USE_OS == 0` else it yields
 */
void lv_draw_dispatch_wait_for_request(void);

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
 * Create a new layer on a parent layer
 * @param parent_layer      the parent layer to which the layer will be merged when it's rendered
 * @param color_format      the color format of the layer
 * @param area              the areas of the layer (absolute coordinates)
 * @return                  the new target_layer or NULL on error
 */
lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area);


void lv_draw_layer_get_area(lv_layer_t * layer, lv_area_t * area);

/**
 * Try to allocate a buffer for the layer.
 * @param layer             pointer to a layer
 * @return                  pointer to the allocated aligned buffer or NULL on failure
 */
void * lv_draw_layer_alloc_buf(lv_layer_t * layer);

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
