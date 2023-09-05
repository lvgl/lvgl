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
#include "../stdlib/lv_string.h"

#if LV_USE_VECTOR

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_VECTOR_PATH_FILL_NONZERO = 0,
    LV_VECTOR_PATH_FILL_EVENODD
};
typedef uint8_t lv_vector_fill_t;

enum {
    LV_VECTOR_PATH_PATTERN_MODE_COLOR = 0,
    LV_VECTOR_PATH_PATTERN_MODE_PAD,
};
typedef uint8_t lv_vector_pattern_t;

enum {
    LV_VECTOR_PATH_STROKE_CAP_SQUARE = 0,
    LV_VECTOR_PATH_STROKE_CAP_ROUND,
    LV_VECTOR_PATH_STROKE_CAP_BUTT,
};
typedef uint8_t lv_vector_stroke_cap_t;

enum {
    LV_VECTOR_PATH_STROKE_JOIN_BEVEL = 0,
    LV_VECTOR_PATH_STROKE_JOIN_ROUND,
    LV_VECTOR_PATH_STROKE_JOIN_MITER,
};
typedef uint8_t lv_vector_stroke_join_t;

enum {
    LV_VECTOR_PATH_BLEND_CLEAR = 0,
    LV_VECTOR_PATH_BLEND_SRC,
    LV_VECTOR_PATH_BLEND_SRC_ATOP,
    LV_VECTOR_PATH_BLEND_SRC_OVER,
    LV_VECTOR_PATH_BLEND_SRC_IN,
    LV_VECTOR_PATH_BLEND_SRC_OUT,
    LV_VECTOR_PATH_BLEND_DST,
    LV_VECTOR_PATH_BLEND_DST_ATOP,
    LV_VECTOR_PATH_BLEND_DST_OVER,
    LV_VECTOR_PATH_BLEND_DST_IN,
    LV_VECTOR_PATH_BLEND_DST_OUT,
    LV_VECTOR_PATH_BLEND_XOR,
};
typedef uint8_t lv_vector_blend_t;

typedef struct {

} lv_vector_grad_t;
typedef struct {
    float m[3][3];
} lv_matrix_t;

typedef struct {
    lv_color_t color;
    lv_opa_t opa;
    lv_vector_fill_t fill_rule;
    lv_vector_pattern_t pattern_mode;
    lv_draw_image_dsc_t img_dsc;
    lv_grad_dsc_t grad;
    lv_matrix_t matrix;
} lv_vector_fill_dsc_t;

typedef struct {
    lv_color_t color;
    lv_opa_t opa;
    float width;
    float * dash_pattern;            /* The array of consecutive pair values of the dash length and the gap length */
    uint16_t dash_pattern_count;     /* The length of the dash_pattern array */
    float dash_offset;               /* The shift of the starting point within the repeating dash pattern */
    lv_vector_stroke_cap_t cap;       /* The shape to be used at the end of open subpaths */
    lv_vector_stroke_join_t join;     /* The shape to be used at the corners of paths when they are joined */
    uint16_t miter_limit;            /* A limit on the extent of the stroke join, the default value is 4 */
    lv_grad_dsc_t grad;
} lv_vector_stroke_dsc_t;

typedef struct {
    uint8_t * path;
    uint32_t path_length;
    uint32_t memory_size;
} lv_vector_path_t;

typedef struct {
    float x;
    float y;
} lv_vector_point_t;

typedef struct lv_vector_context_s lv_vector_context_t;

static inline void lv_matrix_copy(lv_matrix_t * dst, lv_matrix_t * src)
{
    lv_memcpy(dst, src, sizeof(lv_matrix_t));
}

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*Path operations, implemented by different engine*/
lv_vector_path_t * lv_vector_path_create(void);
lv_vector_path_t * lv_vector_path_copy(lv_vector_path_t * path);
void lv_vector_path_destroy(lv_vector_path_t * path);
void lv_vector_path_move_to(lv_vector_path_t * path, lv_vector_point_t p);
void lv_vector_path_line_to(lv_vector_path_t * path, lv_vector_point_t p);
void lv_vector_path_quad_to(lv_vector_path_t * path, lv_vector_point_t p1, lv_vector_point_t p2);
void lv_vector_path_cubic_to(lv_vector_path_t * path, lv_vector_point_t p1, lv_vector_point_t p2, lv_vector_point_t p3);
void lv_vector_path_arc_to(lv_vector_path_t * path, lv_vector_point_t p1, lv_vector_point_t p2, float r);
void lv_vector_path_close(lv_vector_path_t * path);
void lv_vector_path_append_rect(lv_vector_path_t * path, lv_area_t * rect, float r);
void lv_vector_path_append_circle(lv_vector_path_t * path, lv_vector_point_t c, float r);
/**
 * @param[in] start_angle The start angle of the arc given in degrees, measured counter-clockwise from the horizontal line.
 * @param[in] sweep The central angle of the arc given in degrees, measured counter-clockwise from @p start_angle.
 * @param[in] pie Specifies whether to draw radii from the arc's center to both of its end-point - drawn if @c true.
 */
void lv_vector_path_append_arc(lv_vector_path_t * path, lv_vector_point_t c, float r, uint16_t start_angle,
                               uint16_t sweep,
                               bool pie);
void lv_vector_path_append_path(lv_vector_path_t * path, lv_vector_path_t * subpath);

/*Context operations*/
lv_vector_context_t * lv_vector_context_create(void);
void lv_vector_context_destroy(lv_vector_context_t * ctx);
void lv_vector_set_scissor(lv_vector_context_t * ctx, lv_area_t * area);
void lv_vector_set_path_transform(lv_vector_context_t * ctx, lv_matrix_t * matrix);
void lv_vector_set_fill_color(lv_vector_context_t * ctx, lv_color_t color);
void lv_vector_set_fill_opa(lv_vector_context_t * ctx, lv_opa_t opa);
void lv_vector_set_fill_rule(lv_vector_context_t * ctx, lv_vector_fill_t rule);
void lv_vector_set_fill_pattern_mode(lv_vector_context_t * ctx, lv_vector_pattern_t mode);
void lv_vector_set_fill_image(lv_vector_context_t * ctx, lv_draw_image_dsc_t * img_dsc);
void lv_vector_set_fill_grad(lv_vector_context_t * ctx, lv_grad_dsc_t * grad);
void lv_vector_set_fill_transform(lv_vector_context_t * ctx, lv_matrix_t * matrix);
void lv_vector_set_stroke_color(lv_vector_context_t * ctx, lv_color_t color);
void lv_vector_set_stroke_opa(lv_vector_context_t * ctx, lv_opa_t opa);
void lv_vector_set_stroke_width(lv_vector_context_t * ctx, float width);
/**
 * @param[in] dash_pattern The array of consecutive pair values of the dash length and the gap length.
 * @param[in] dash_pattern_count The length of the @p dash_pattern array.
 */
void lv_vector_set_stroke_dash(lv_vector_context_t * ctx, float * dash_pattern, uint16_t dash_pattern_count);
/**
 * @param[in] cap The cap style value. The default value is @c LV_VECTOR_PATH_STROKE_CAP_SQUARE.
 */
void lv_vector_set_stroke_cap(lv_vector_context_t * ctx, lv_vector_stroke_cap_t cap);
/**
 * The join style is used for joining the two line segment while stroking the path.
 * @param[in] join The join style value. The default value is @c LV_VECTOR_PATH_STROKE_JOIN_BEVEL.
 */
void lv_vector_set_stroke_join(lv_vector_context_t * ctx, lv_vector_stroke_join_t join);
/**
 * @param[in] miterlimit The miterlimit imposes a limit on the extent of the stroke join, when the @c LV_VECTOR_PATH_STROKE_JOIN_MITER is set. The default value is 4.
 */
void lv_vector_set_stroke_miter_limit(lv_vector_context_t * ctx, uint16_t miter_limit);
void lv_vector_set_stroke_grad(lv_vector_context_t * ctx, lv_grad_dsc_t * grad);
void lv_vector_set_order(lv_vector_context_t * ctx, bool stroke_first);

/*Draw path commands*/
void lv_vector_add(struct _lv_layer_t * layer, lv_vector_context_t * ctx, lv_vector_path_t * path);
void lv_vector_clear(struct _lv_layer_t * layer, lv_vector_context_t * ctx, lv_area_t * rect, lv_color_t color);
void lv_vector_draw(struct _lv_layer_t * layer, lv_vector_context_t * ctx);

/*Helper functions*/
void lv_matrix_translate(lv_matrix_t * matrix, float dx, float dy);
void lv_matrix_scale(lv_matrix_t * matrix, float scale_x, float scale_y);
void lv_matrix_rotate(lv_matrix_t * matrix, float degree);
void lv_matrix_skew_x(lv_matrix_t * matrix, float skew_x);
void lv_matrix_skew_y(lv_matrix_t * matrix, float skew_y);
/*Pseudo 3D rotation (using affine transformations to simulate 3d rotations cast to xy plane)*/
void lv_matrix_rotate_p3d(lv_matrix_t * matrix, float rx, float ry, float rz);
void lv_matrix_rotate_quaternion(lv_matrix_t * matrix, float q0, float q1, float q2, float q3);
void lv_vector_translate(lv_vector_context_t * ctx, float dx, float dy);
void lv_vector_scale(lv_vector_context_t * ctx, float scale_x, float scale_y);
void lv_vector_rotate(lv_vector_context_t * ctx, float degree);
void lv_vector_skew_x(lv_vector_context_t * ctx, float skew_x);
void lv_vector_skew_y(lv_vector_context_t * ctx, float skew_y);
void lv_vector_rotate_p3d(lv_vector_context_t * ctx, float rx, float ry, float rz);
void lv_vector_rotate_quaternion(lv_vector_context_t * ctx, float q0, float q1, float q2, float q3);
void lv_vector_scale_with_pivot(lv_vector_context_t * ctx, float scale_x, float scale_y, float pivot_x, float pivot_y);
void lv_vector_rotate_with_pivot(lv_vector_context_t * ctx, float degree, float pivot_x, float pivot_y);
void lv_vector_transformation(lv_vector_context_t * ctx, float degree, float scale_xy, float pivot_x, float pivot_y);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_VECTOR*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VECTOR_H*/
