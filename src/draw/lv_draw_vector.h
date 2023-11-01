/**
 * @file lv_draw_vector.h
 *
 */

#ifndef LV_DRAW_VECTOR_H
#define LV_DRAW_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "../misc/lv_array.h"


#if LV_USE_VECTOR_GRAPHIC

/**********************
 *      TYPEDEFS
 **********************/
enum {
    LV_VECTOR_FILL_NONZERO = 0,
    LV_VECTOR_FILL_EVENODD,
};
typedef uint8_t lv_vector_fill_t;

enum {
    LV_VECTOR_STROKE_CAP_BUTT = 0,
    LV_VECTOR_STROKE_CAP_SQUARE,
    LV_VECTOR_STROKE_CAP_ROUND,
};
typedef uint8_t lv_vector_stroke_cap_t;

enum {
    LV_VECTOR_STROKE_JOIN_MITER = 0,
    LV_VECTOR_STROKE_JOIN_BEVEL,
    LV_VECTOR_STROKE_JOIN_ROUND,
};
typedef uint8_t lv_vector_stroke_join_t;

enum {
    LV_VECTOR_PATH_QUALITY_MEDIUM = 0, /* default*/
    LV_VECTOR_PATH_QUALITY_HIGH,
    LV_VECTOR_PATH_QUALITY_LOW,
};
typedef uint8_t lv_vector_path_quality_t;

enum {
    LV_VECTOR_BLEND_SRC_OVER = 0,
    LV_VECTOR_BLEND_SRC_IN,
    LV_VECTOR_BLEND_DST_OVER,
    LV_VECTOR_BLEND_DST_IN,
    LV_VECTOR_BLEND_SCREEN,
    LV_VECTOR_BLEND_MULTIPLY,
    LV_VECTOR_BLEND_NONE,
    LV_VECTOR_BLEND_ADDITIVE,
    LV_VECTOR_BLEND_SUBTRACTIVE,
};
typedef uint8_t lv_vector_blend_t;

enum {
    LV_VECTOR_PATH_OP_MOVE_TO = 0,
    LV_VECTOR_PATH_OP_LINE_TO,
    LV_VECTOR_PATH_OP_QUAD_TO,
    LV_VECTOR_PATH_OP_CUBIC_TO,
    LV_VECTOR_PATH_OP_CLOSE,
};
typedef uint8_t lv_vector_path_op_t;

enum {
    LV_VECTOR_DRAW_STYLE_SOLID = 0,
    LV_VECTOR_DRAW_STYLE_PATTERN,
    LV_VECTOR_DRAW_STYLE_GRADIENT,
};
typedef uint8_t lv_vector_draw_style_t;

enum {
    LV_VECTOR_GRADIENT_SPREAD_PAD = 0,
    LV_VECTOR_GRADIENT_SPREAD_REPEAT,
    LV_VECTOR_GRADIENT_SPREAD_REFLECT,
};
typedef uint8_t lv_vector_gradient_spread_t;

enum {
    LV_VECTOR_GRADIENT_STYLE_LINEAR = 0,
    LV_VECTOR_GRADIENT_STYLE_RADIAL,
};
typedef uint8_t lv_vector_gradient_style_t;

typedef struct {
    float x;
    float y;
} lv_fpoint_t;


typedef struct {
    float m[3][3];
} lv_matrix_t;


typedef struct {
    lv_vector_path_quality_t quality;
    lv_array_t ops;
    lv_array_t points;
} lv_vector_path_t;

typedef struct {
    lv_vector_gradient_style_t style;
    lv_grad_dsc_t grad;
    float cx;
    float cy;
    float cr;
    lv_vector_gradient_spread_t spread;
} lv_vector_gradient_t;

typedef struct {
    lv_vector_draw_style_t style;
    lv_color32_t color;
    lv_opa_t opa;
    lv_vector_fill_t fill_rule;
    lv_draw_image_dsc_t img_dsc;
    lv_vector_gradient_t gradient;
    lv_matrix_t matrix;
} lv_vector_fill_dsc_t;

typedef struct {
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
} lv_vector_stroke_dsc_t;

typedef struct {
    lv_vector_fill_dsc_t fill_dsc;
    lv_vector_stroke_dsc_t stroke_dsc;
    lv_matrix_t matrix;
    lv_vector_blend_t blend_mode;
    lv_area_t scissor_area;
} lv_vector_draw_dsc_t;


typedef struct {
    lv_draw_dsc_base_t base;
    lv_ll_t * task_list; // draw task list.
} lv_draw_vector_task_dsc_t;

typedef struct {
    struct _lv_layer_t * layer;
    lv_vector_draw_dsc_t current_dsc;
    /* private data */
    lv_draw_vector_task_dsc_t tasks;
} lv_vector_dsc_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* matrix functions */
void lv_matrix_identity(lv_matrix_t * matrix);
void lv_matrix_translate(lv_matrix_t * matrix, float tx, float ty);
void lv_matrix_scale(lv_matrix_t * matrix, float scale_x, float scale_y);
void lv_matrix_rotate(lv_matrix_t * matrix, float degree);
void lv_matrix_skew(lv_matrix_t * matrix, float skew_x, float skew_y);
void lv_matrix_multiply(lv_matrix_t * matrix, const lv_matrix_t * matrix2);

/* path functions */
lv_vector_path_t * lv_vector_path_create(lv_vector_path_quality_t quality);
void lv_vector_path_copy(lv_vector_path_t * target_path, const lv_vector_path_t * path);
void lv_vector_path_clear(lv_vector_path_t * path);
void lv_vector_path_destroy(lv_vector_path_t * path);

void lv_vector_path_move_to(lv_vector_path_t * path, const lv_fpoint_t * p);
void lv_vector_path_line_to(lv_vector_path_t * path, const lv_fpoint_t * p);
void lv_vector_path_quad_to(lv_vector_path_t * path, const lv_fpoint_t * p1, const lv_fpoint_t * p2);
void lv_vector_path_cubic_to(lv_vector_path_t * path, const lv_fpoint_t * p1, const lv_fpoint_t * p2,
                             const lv_fpoint_t * p3);
void lv_vector_path_close(lv_vector_path_t * path);

/* complex shape functions */
void lv_vector_path_append_rect(lv_vector_path_t * path, const lv_area_t * rect, lv_coord_t rx, lv_coord_t ry);
void lv_vector_path_append_circle(lv_vector_path_t * path, const lv_fpoint_t * c, lv_coord_t rx, lv_coord_t ry);
void lv_vector_path_append_path(lv_vector_path_t * path, const lv_vector_path_t * subpath);

/* draw dsc property functions */
lv_vector_dsc_t * lv_vector_dsc_create(lv_layer_t * layer);
void lv_vector_dsc_destroy(lv_vector_dsc_t * dsc);

void lv_vector_dsc_set_transform(lv_vector_dsc_t * dsc, const lv_matrix_t * matrix);
void lv_vector_dsc_set_blend_mode(lv_vector_dsc_t * dsc, lv_vector_blend_t blend);
void lv_vector_dsc_set_fill_color32(lv_vector_dsc_t * dsc, lv_color32_t color);
void lv_vector_dsc_set_fill_color(lv_vector_dsc_t * dsc, lv_color_t color);
void lv_vector_dsc_set_fill_opa(lv_vector_dsc_t * dsc, lv_opa_t opa);
void lv_vector_dsc_set_fill_rule(lv_vector_dsc_t * dsc, lv_vector_fill_t rule);
void lv_vector_dsc_set_fill_image(lv_vector_dsc_t * dsc, const lv_draw_image_dsc_t * img_dsc);
void lv_vector_dsc_set_fill_linear_gradient(lv_vector_dsc_t * dsc, const lv_grad_dsc_t * grad,
                                            lv_vector_gradient_spread_t spread);
void lv_vector_dsc_set_fill_radial_gradient(lv_vector_dsc_t * dsc, const lv_grad_dsc_t * grad, float cx, float cy,
                                            float radius, lv_vector_gradient_spread_t spread);
void lv_vector_dsc_set_fill_transform(lv_vector_dsc_t * dsc, const lv_matrix_t * matrix);
void lv_vector_dsc_set_stroke_color32(lv_vector_dsc_t * dsc, lv_color32_t color);
void lv_vector_dsc_set_stroke_color(lv_vector_dsc_t * dsc, lv_color_t color);
void lv_vector_dsc_set_stroke_opa(lv_vector_dsc_t * dsc, lv_opa_t opa);
void lv_vector_dsc_set_stroke_width(lv_vector_dsc_t * dsc, float width);
void lv_vector_dsc_set_stroke_dash(lv_vector_dsc_t * dsc, float * dash_pattern, uint16_t dash_count);
void lv_vector_dsc_set_stroke_cap(lv_vector_dsc_t * dsc, lv_vector_stroke_cap_t cap);
void lv_vector_dsc_set_stroke_join(lv_vector_dsc_t * dsc, lv_vector_stroke_join_t join);
void lv_vector_dsc_set_stroke_miter_limit(lv_vector_dsc_t * dsc, uint16_t miter_limit);
void lv_vector_dsc_set_stroke_linear_gradient(lv_vector_dsc_t * dsc, const lv_grad_dsc_t * grad,
                                              lv_vector_gradient_spread_t spread);
void lv_vector_dsc_set_stroke_radial_gradient(lv_vector_dsc_t * dsc, const lv_grad_dsc_t * grad, float cx, float cy,
                                              float radius, lv_vector_gradient_spread_t spread);
void lv_vector_dsc_set_stroke_transform(lv_vector_dsc_t * dsc, const lv_matrix_t * matrix);

/* draw dsc transform functions */
void lv_vector_dsc_identity(lv_vector_dsc_t * dsc);
void lv_vector_dsc_scale(lv_vector_dsc_t * dsc, float scale_x, float scale_y);
void lv_vector_dsc_rotate(lv_vector_dsc_t * dsc, float degree);
void lv_vector_dsc_translate(lv_vector_dsc_t * dsc, float tx, float ty);
void lv_vector_dsc_skew(lv_vector_dsc_t * dsc, float skew_x, float skew_y);

/* draw functions */
void lv_vector_add(lv_vector_dsc_t * dsc, const lv_vector_path_t * path);
void lv_vector_clear(lv_vector_dsc_t * dsc, const lv_area_t * rect);
void lv_vector_draw(lv_vector_dsc_t * dsc);

/* Traverser for task list */
typedef void (*vector_draw_task_cb)(void * ctx, const lv_vector_path_t * path, const lv_vector_draw_dsc_t * dsc);
void _lv_vector_for_each_destroy_tasks(lv_ll_t * task_list, vector_draw_task_cb cb, void * data);
#endif /* LV_USE_VECTOR_GRAPHIC */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRAW_VECTOR_H */
