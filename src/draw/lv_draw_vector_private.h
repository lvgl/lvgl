/**
 * @file lv_draw_vector_private.h
 *
 */

#ifndef LV_DRAW_VECTOR_PRIVATE_H
#define LV_DRAW_VECTOR_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vector.h"

#if LV_USE_VECTOR_GRAPHIC

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Stores the shape of the path as arrays of operations and points.
 * For example move to 10;20 then draw a line to 30;40 and draw an
 * arc with 30 radius and 70° sweep.
 *
 * `lv_vector_path_ctx_t` is also required to describe how to fill and stroke the path.
 */
struct _lv_vector_path_t {
    lv_vector_path_quality_t quality;
    lv_array_t ops;
    lv_array_t points;
};

struct _lv_vector_gradient_t {
    lv_vector_gradient_style_t style;
    lv_grad_stop_t   stops[LV_GRADIENT_MAX_STOPS];  /**< A gradient stop array */
    uint16_t         stops_count;                   /**< The number of used stops in the array */
    float x1;
    float y1;
    float x2;
    float y2;
    float cx;
    float cy;
    float cr;
    lv_vector_gradient_spread_t spread;
};

struct _lv_vector_fill_dsc_t {
    lv_vector_draw_style_t style;
    lv_color32_t color;
    lv_opa_t opa;
    lv_vector_fill_t fill_rule;
    lv_vector_fill_units_t fill_units;
    lv_draw_image_dsc_t img_dsc;
    lv_vector_gradient_t gradient;
    lv_matrix_t matrix;
};

struct _lv_vector_stroke_dsc_t {
    lv_vector_draw_style_t style;
    lv_color32_t color;
    lv_opa_t opa;
    float width;
    lv_array_t dash_pattern;
    lv_vector_stroke_cap_t cap;
    lv_vector_stroke_join_t join;
    uint16_t miter_limit;
    lv_vector_gradient_t gradient;
    lv_matrix_t matrix;
};

/**
 * Stores how to fill, stroke, transform etc a given path
 */
struct _lv_vector_path_ctx_t {
    lv_vector_fill_dsc_t fill_dsc;
    lv_vector_stroke_dsc_t stroke_dsc;
    lv_matrix_t matrix;
    lv_vector_blend_t blend_mode;
    lv_area_t scissor_area;
};

struct _lv_draw_vector_dsc_t {
    lv_draw_dsc_base_t base;

    /** The current colors, opacities, matrix, etc for the next task to be added
     * by */
    lv_vector_path_ctx_t * ctx;

    /**
     * Store path shapes and their attributes
     * in a list as `lv_draw_vector_subtask_t`. */
    lv_ll_t * task_list;
};


/**
 * Contains a path shape and its attributes together.
 * It's a task that will be passed to the vector rendering engine.
 * It's used in the `task_list` of `lv_draw_vector_dsc_t`.
 */
typedef struct {
    lv_vector_path_t * path;
    lv_vector_path_ctx_t ctx;
} lv_draw_vector_subtask_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * This is the main function to draw the accumulated vector tasks by passing them
 * to a vector renderer callback.
 * When the callback returns the processed vector task will be destroyed.
 * @param task_list     pointer to the linked list in `lv_draw_vector_dsc_t` that stores
 *                      the path shapes and their attributes.
 * @param cb            the callback used to iterate through the task
 * @param user_data     a custom pointer that will be passed to the callback
 */
void lv_vector_for_each_destroy_tasks(lv_ll_t * task_list, vector_draw_task_cb cb, void * used_data);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_VECTOR_GRAPHIC */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VECTOR_PRIVATE_H*/
