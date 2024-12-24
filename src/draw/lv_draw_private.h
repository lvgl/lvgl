/**
 * @file lv_draw_private.h
 *
 */

/**
 * Modified by NXP in 2024
 */

#ifndef LV_DRAW_PRIVATE_H
#define LV_DRAW_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw.h"
#include "../osal/lv_os.h"
#include "../misc/cache/lv_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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

#if LV_DRAW_TRANSFORM_USE_MATRIX
    /** Transform matrix to be applied when rendering the layer */
    lv_matrix_t matrix;
#endif

    volatile int state;              /** int instead of lv_draw_task_state_t to be sure its atomic */

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

struct _lv_draw_mask_t {
    void * user_data;
};

struct _lv_draw_unit_t {
    lv_draw_unit_t * next;

    /**
     * The target_layer on which drawing should happen
     */
    lv_layer_t * target_layer;

    const lv_area_t * clip_area;

    /**
     * Name of the draw unit, for debugging purposes only.
     */
    const char * name;

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
     * Called to signal the unit to complete all tasks in order to return their ready status.
     * This callback can be implemented in case of asynchronous task processing.
     * Below is an example to show the difference between synchronous and asynchronous:
     *
     * Synchronous:
     * LVGL thread              DRAW thread                 HW
     *
     * task1             -->    submit               -->    Receive task1
     *                          wait_for_finish()
     *                   <--    task1->state = READY <--    Complete task1
     * task2             -->    submit               -->    Receive task2
     *                          wait_for_finish()
     *                          task2->state = READY <--    Complete task2
     * task3             -->    submit               -->    Receive task3
     *                          wait_for_finish()
     *                   <--    task3->state = READY <--    Complete task3
     * task4             -->    submit               -->    Receive task4
     *                          wait_for_finish()
     *                   <--    task4->state = READY <--    Complete task4
     * NO MORE TASKS
     *
     *
     * Asynchronous:
     * LVGL thread              DRAW thread                 HW
     *                                                      is IDLE
     * task1             -->    queue task1
     *                          submit               -->    Receive task1
     * task2             -->    queue task2                 is BUSY (with task1)
     * task3             -->    queue task3                 still BUSY (with task1)
     * task4             -->    queue task4                 becomes IDLE
     *                   <--    task1->state = READY <--    Complete task1
     *                          submit               -->    Receive task2, task3, task4
     * NO MORE TASKS
     * wait_for_finish_cb()     wait_for_finish()
     *                                               <--    Complete task2, task3, task4
     *                   <--    task2->state = READY <--
     *                   <--    task3->state = READY <--
     *                   <--    task4->state = READY <--
     *
     * @param draw_unit
     * @return
     */
    int32_t (*wait_for_finish_cb)(lv_draw_unit_t * draw_unit);

    /**
     * Called to delete draw unit.
     * @param draw_unit
     * @return
     */
    int32_t (*delete_cb)(lv_draw_unit_t * draw_unit);
};

typedef struct {
    lv_draw_unit_t * unit_head;
    uint32_t unit_cnt;
    uint32_t used_memory_for_layers; /* measured as bytes */
#if LV_USE_OS
    lv_thread_sync_t sync;
#else
    volatile int dispatch_req;
#endif
    lv_mutex_t circle_cache_mutex;
    bool task_running;
} lv_draw_global_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PRIVATE_H*/
