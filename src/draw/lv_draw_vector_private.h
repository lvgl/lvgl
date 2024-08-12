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

struct lv_vector_path_t {
    lv_vector_path_quality_t quality;
    lv_array_t ops;
    lv_array_t points;
};

struct lv_vector_gradient_t {
    lv_vector_gradient_style_t style;
    lv_gradient_stop_t   stops[LV_GRADIENT_MAX_STOPS];  /**< A gradient stop array */
    uint16_t             stops_count;                   /**< The number of used stops in the array */
    float x1;
    float y1;
    float x2;
    float y2;
    float cx;
    float cy;
    float cr;
    lv_vector_gradient_spread_t spread;
};

struct lv_vector_fill_dsc_t {
    lv_vector_draw_style_t style;
    lv_color32_t color;
    lv_opa_t opa;
    lv_vector_fill_t fill_rule;
    lv_draw_image_dsc_t img_dsc;
    lv_vector_gradient_t gradient;
    lv_matrix_t matrix;
};

struct lv_vector_stroke_dsc_t {
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

struct lv_vector_draw_dsc_t {
    lv_vector_fill_dsc_t fill_dsc;
    lv_vector_stroke_dsc_t stroke_dsc;
    lv_matrix_t matrix;
    lv_vector_blend_t blend_mode;
    lv_area_t scissor_area;
};

struct lv_draw_vector_task_dsc_t {
    lv_draw_dsc_base_t base;
    lv_ll_t * task_list; /*draw task list.*/
};

struct lv_vector_dsc_t {
    lv_layer_t * layer;
    lv_vector_draw_dsc_t current_dsc;
    /* private data */
    lv_draw_vector_task_dsc_t tasks;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_vector_for_each_destroy_tasks(lv_ll_t * task_list, vector_draw_task_cb cb, void * data);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_VECTOR_GRAPHIC */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VECTOR_PRIVATE_H*/
