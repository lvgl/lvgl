/**
 * @file lv_draw_m2d_private.h
 *
 */

#ifndef LV_DRAW_M2D_PRIVATE_H
#define LV_DRAW_M2D_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
*      INCLUDES
*********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_M2D

#include "../../display/lv_display_private.h"
#include "../lv_draw_private.h"
#include "../lv_image_dsc.h"
#include "../lv_draw_rect.h"
#include <m2d/m2d.h>

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/**
 * Draw unit for the M2D renderer
 *
 * Created from @ref lv_draw_m2d_init().
 */
typedef struct _lv_draw_m2d_unit_t {
    lv_draw_unit_t base_unit;   /**< Draw init used internally by LVGL. */
    lv_draw_task_t * task_act;  /**< Active task for this draw unit. */
} lv_draw_m2d_unit_t;

/**
 * Draw buffer suited for M2D renderer
 *
 * Allocated or imported by the M2D draw buffer handlers.
 */
typedef struct _lv_draw_m2d_buf_t {
    /**
     * Draw buffer used internally by LVGL
     */
    lv_draw_buf_t base_buf;

    /**
     * Main M2D buffer
     */
    struct m2d_buffer * m2d_buf;

    /**
     * Optional M2D buffer, allocated dynamically when needed only once for all,
     * used when the task requires multiple rendering passes
     */
    struct m2d_buffer * m2d_tmp_buf;

    /**
     * Used as a key for the internal @ref lv_map_t that associates a @ref lv_image_dsc_t
     * instance with a this @ref lv_draw_m2d_buf_t instance
     */
    void * alias_key;

    /**
     * Tell whether the draw buffer is currently owned either by the GPU or the CPU,
     * to decide whether some data cache operations should be done
     */
    bool owned_by_gpu;
} lv_draw_m2d_buf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize draw buffer handlers to create, import or destroy M2D draw buffers
 *
 * Called from @ref lv_draw_m2d_init().
 */
void lv_draw_buf_m2d_init_handlers(void);

/**
 * Initialize the @ref lv_map_t `m2d_buf_map` instance
 *
 * The `m2d_buf_map` map associates @ref lv_image_dsc_t with @ref lv_draw_m2d_buf_t.
 * Called from @ref lv_draw_m2d_init()
 */
void lv_draw_m2d_init_buf_map(void);

/**
 * Clean up the @ref lv_map_t `m2d_buf_map` instance
 *
 * Called from @ref lv_draw_m2d_deinit().
 */
void lv_draw_m2d_cleanup_buf_map(void);

/**
 * Search for the @ref lv_draw_m2d_buf_t associted to some @ref lv_image_dsc_t
 *
 * Search for an entry in the `m2d_buf_map` map; if this entry exists then just
 * return its value, otherwise create a new `draw_m2d_buf` M2D buffer with m2d_alloc(),
 * copy the source image data into the data of `draw_m2d_buf`, insert the
 * {`img_dsc->data`, `draw_m2d_buf`} pair into the `m2d_buf_map` map, finally return
 * `m2d_buf_map`.
 *
 * @param img_dsc pointer to an image descriptor.
 * @return the M2D draw buffer associated to the @p img_dsc image descriptor.
 */
lv_draw_m2d_buf_t * lv_draw_m2d_search_buf_map(const lv_image_dsc_t * img_dsc);

/**
 * Synchronize the M2D buffer before being accessed by the CPU
 *
 * This function waits for all GPU operations involving @p draw_m2d_buf to complete.
 * @param draw_m2d_buf M2D draw buffer to synchronize.
 * @return 0 if successfull, otherwise a negative number.
 */
int32_t lv_draw_m2d_buf_sync_for_cpu(lv_draw_m2d_buf_t * draw_m2d_buf);

/**
 * Synchronize the M2D buffer before being accessed by the GPU
 *
 * This function flushes the CPU data cache if needed.
 * @param draw_m2d_buf M2D draw buffer to synchronize.
 * @return 0 if successful, otherwise a negative number.
 */
int32_t lv_draw_m2d_buf_sync_for_gpu(lv_draw_m2d_buf_t * draw_m2d_buf);

/**
 * Get the M2D buffer used as the target of intermediate rendering passes.
 *
 * @param draw_m2d_buf M2D draw buffer.
 * @return the M2D buffer, allocated once for all, to be used with task requiring
 *         multiple rendering passes.
 */
struct m2d_buffer * lv_draw_m2d_buf_get_tmp_buffer(lv_draw_m2d_buf_t * draw_m2d_buf);

/**
 * Check whether the M2D draw unit supports some FILL task
 *
 * @param task pointer to the task to validate.
 * @param draw_dsc pointer to the draw fill descriptor of the FILL task.
 * @return true if the FILL task is supported, otherwise false.
 */
bool lv_draw_m2d_fill_is_supported(const lv_draw_task_t * task, const lv_draw_fill_dsc_t * draw_dsc);

/**
 * Execute some FILL task with the M2D draw unit
 *
 * @param t pointer to the FILL task to execute.
 * @param draw_dsc pointer to the draw fill descriptor of the FILL task.
 * @param coords coordinates relative to the target layer of the FILL task.
 */
void lv_draw_m2d_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * draw_dsc, const lv_area_t * coords);

/**
 * Check whether the M2D draw unit supports some IMAGE task
 *
 * @param task pointer to the IMAGE task to validate.
 * @param draw_dsc pointer to the draw image descriptor of the IMAGE task.
 * @return true if the IMAGE task is supported, otherwise false.
 */
bool lv_draw_m2d_image_is_supported(const lv_draw_task_t * task, const lv_draw_image_dsc_t * draw_dsc);

/**
 * Execute some IMAGE task with the M2D draw unit
 *
 * @param t pointer to the IMAGE task to execute.
 * @param draw_dsc pointer to the draw image descriptor of the IMAGE task.
 * @param coords coordinates relative to the target layer of the IMAGE task.
 */
void lv_draw_m2d_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

/**
 * Check whether the M2D draw unit supports some LAYER task
 *
 * @param task pointer to the LAYER task to validate.
 * @param draw_dsc pointer to the draw image descriptor of the LAYER task.
 * @return true if the LAYER task is supported, otherwise false.
 */
bool lv_draw_m2d_layer_is_supported(const lv_draw_task_t * task, const lv_draw_image_dsc_t * draw_dsc);

/**
 * Execute some LAYER task with the M2D draw unit
 *
 * @param t pointer to the LAYER task to execute.
 * @param draw_dsc pointer to the draw image descriptor of the LAYER task.
 * @param coords coordinates relative to the target layer of the LAYER task.
 */
void lv_draw_m2d_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords);

#endif /*LV_USE_DRAW_M2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_M2D_PRIVATE_H*/
