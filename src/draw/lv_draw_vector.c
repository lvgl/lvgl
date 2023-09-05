/**
 * @file lv_draw_vector.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_vector.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"
#include <math.h>

#if LV_USE_VECTOR

/*********************
 *      DEFINES
 *********************/

#ifndef M_PI
    #define M_PI 3.1415926f
#endif

#define RAD(d) (d * (float)M_PI / 180.0f)
#define lv_vector_scale(ctx, x, y) lv_vector_scale_with_pivot(ctx, x, y, 0, 0)
#define lv_vector_rotate(ctx, degree) lv_vector_rotate_with_pivot(ctx, degree, 0, 0)
#define lv_vector_rotate_with_pivot(ctx, degree, x, y) lv_vector_transformation(ctx, degree, 1, x, y)

/**********************
 *      TYPEDEFS
 **********************/

struct lv_vector_context_s {
    lv_vector_path_t * path;
    lv_vector_fill_dsc_t fill_dsc;
    lv_vector_stroke_dsc_t stroke_dsc;
    lv_matrix_t matrix;
    lv_vector_blend_t blend;
    lv_area_t scissor_area;
    bool stroke_first;
    struct lv_vector_context_s * next;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void _matrix_rotate(lv_matrix_t * matrix, float c, float s);
static inline void _matrix_skew_x(lv_matrix_t * matrix, float cx);
static inline void _matrix_skew_y(lv_matrix_t * matrix, float cy);
static inline void _matrix_rotate3d(lv_matrix_t * matrix, float c1, float s1, float c2, float s2, float c3, float s3);
static inline void _matrix_rotate3d_internal(lv_matrix_t * matrix, float r11, float r12, float r21, float r22);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_vector_context_t * lv_vector_context_create(void)
{
    LV_PROFILER_BEGIN;

    lv_vector_context_t * ctx = lv_malloc(sizeof(lv_vector_context_t));
    LV_ASSERT_MALLOC(ctx);
    lv_memzero(ctx, sizeof(lv_vector_context_t));
    ctx->fill_dsc.opa = LV_OPA_COVER;
    ctx->fill_dsc.img_dsc.zoom = LV_SCALE_NONE;
#if LV_COLOR_DEPTH > 8
    ctx->fill_dsc.img_dsc.antialias = 1;
#endif
    ctx->fill_dsc.matrix.m[0][0] = 1.0f;
    ctx->fill_dsc.matrix.m[1][1] = 1.0f;
    ctx->fill_dsc.matrix.m[2][2] = 1.0f;
    ctx->matrix.m[0][0] = 1.0f;
    ctx->matrix.m[1][1] = 1.0f;
    ctx->matrix.m[2][2] = 1.0f;
    ctx->stroke_dsc.miter_limit = 4;
    ctx->scissor_area.x2 = LV_COORD_MAX;
    ctx->scissor_area.y2 = LV_COORD_MAX;

    LV_PROFILER_END;
    return ctx;
}

void lv_vector_context_destroy(lv_vector_context_t * ctx)
{
    lv_vector_context_t * next;
    while(ctx) {
        lv_vector_path_destroy(ctx->path);
        if(ctx->stroke_dsc.dash_pattern) {
            lv_free(ctx->stroke_dsc.dash_pattern);
        }
        next = ctx->next;
        lv_free(ctx);
        ctx = next;
    }
}

void lv_vector_add(struct _lv_layer_t * layer, lv_vector_context_t * ctx, lv_vector_path_t * path)
{
    lv_area_t a;
    if(!_lv_area_intersect(&a, &layer->clip_area, &ctx->scissor_area)) return;
    if(ctx->fill_dsc.opa == 0 && ctx->stroke_dsc.opa == 0) return;

    LV_PROFILER_BEGIN;

    lv_vector_context_t * new_ctx = lv_malloc(sizeof(lv_vector_context_t));
    LV_ASSERT_MALLOC(new_ctx);
    lv_memcpy(new_ctx, ctx, sizeof(lv_vector_context_t));
    /*Copy ctx path content as well*/
    new_ctx->path = lv_vector_path_copy(path);
    new_ctx->next = NULL;
    /*Append new_ctx to the end of the list*/
    while(ctx->next) ctx = ctx->next;
    ctx->next = new_ctx;

    LV_PROFILER_END;
}

void lv_vector_clear(struct _lv_layer_t * layer, lv_vector_context_t * ctx, lv_area_t * rect, lv_color_t color)
{
    lv_area_t a;
    if(!_lv_area_intersect(&a, &layer->clip_area, rect)) return;

    LV_PROFILER_BEGIN;

    lv_vector_context_t * new_ctx = lv_malloc(sizeof(lv_vector_context_t));
    LV_ASSERT_MALLOC(new_ctx);
    lv_memzero(new_ctx, sizeof(lv_vector_context_t));
    new_ctx->fill_dsc.color = color;
    new_ctx->scissor_area = a;
    new_ctx->path = NULL; /*This indicates a clear operation*/
    new_ctx->next = NULL;
    while(ctx->next) ctx = ctx->next;
    ctx->next = new_ctx;

    LV_PROFILER_END;
}

void lv_vector_draw(struct _lv_layer_t * layer, lv_vector_context_t * ctx)
{
    if(!ctx->next) return;  /*Empty list, return*/

    LV_PROFILER_BEGIN;

    lv_area_t a;
    lv_vector_context_t * tail = ctx->next;
    while(tail) {
        _lv_area_join(&a, &layer->clip_area, &tail->scissor_area);
        tail = tail->next;
    }
    lv_draw_task_t * t = lv_draw_add_task(layer, &a);
    t->type = LV_DRAW_TASK_TYPE_VECTOR;
    t->draw_dsc = ctx->next;
    lv_draw_finalize_task_creation(layer, t);

    LV_PROFILER_END;
}

void lv_vector_set_scissor(lv_vector_context_t * ctx, lv_area_t * area)
{
    lv_area_copy(&ctx->scissor_area, area);
}

void lv_vector_set_path_transform(lv_vector_context_t * ctx, lv_matrix_t * matrix)
{
    lv_matrix_copy(&ctx->matrix, matrix);
}

void lv_vector_set_fill_color(lv_vector_context_t * ctx, lv_color_t color)
{
    ctx->fill_dsc.color = color;
}

void lv_vector_set_fill_opa(lv_vector_context_t * ctx, lv_opa_t opa)
{
    ctx->fill_dsc.opa = opa;
}

void lv_vector_set_fill_rule(lv_vector_context_t * ctx, lv_vector_fill_t rule)
{
    ctx->fill_dsc.fill_rule = rule;
}

void lv_vector_set_fill_pattern_mode(lv_vector_context_t * ctx, lv_vector_pattern_t mode)
{
    ctx->fill_dsc.pattern_mode = mode;
}

void lv_vector_set_fill_image(lv_vector_context_t * ctx, lv_draw_image_dsc_t * img_dsc)
{
    ctx->fill_dsc.img_dsc = *img_dsc;
}

void lv_vector_set_fill_grad(lv_vector_context_t * ctx, lv_grad_dsc_t * grad)
{
    ctx->fill_dsc.grad = *grad;
}

void lv_vector_set_fill_transform(lv_vector_context_t * ctx, lv_matrix_t * matrix)
{
    lv_matrix_copy(&ctx->matrix, matrix);
}

void lv_vector_set_stroke_color(lv_vector_context_t * ctx, lv_color_t color)
{
    ctx->stroke_dsc.color = color;
}

void lv_vector_set_stroke_opa(lv_vector_context_t * ctx, lv_opa_t opa)
{
    ctx->stroke_dsc.opa = opa;
}

void lv_vector_set_stroke_width(lv_vector_context_t * ctx, float width)
{
    ctx->stroke_dsc.width = width > 0 ? width : 0;
}

void lv_vector_set_stroke_dash(lv_vector_context_t * ctx, float * dash_pattern, uint16_t dash_pattern_count)
{
    if(dash_pattern_count > 0) {
        ctx->stroke_dsc.dash_pattern = lv_realloc(ctx->stroke_dsc.dash_pattern, dash_pattern_count * sizeof(dash_pattern[0]));
        LV_ASSERT_MALLOC(ctx->stroke_dsc.dash_pattern);
        for(uint16_t i = 0; i < dash_pattern_count; i++) {
            ctx->stroke_dsc.dash_pattern[i] = dash_pattern[i];
        }

        ctx->stroke_dsc.dash_pattern_count = dash_pattern_count;
    }
}

void lv_vector_set_stroke_cap(lv_vector_context_t * ctx, lv_vector_stroke_cap_t cap)
{
    ctx->stroke_dsc.cap = cap;
}
void lv_vector_set_stroke_join(lv_vector_context_t * ctx, lv_vector_stroke_join_t join)
{
    ctx->stroke_dsc.join = join;
}

void lv_vector_set_stroke_miter_limit(lv_vector_context_t * ctx, uint16_t miter_limit)
{
    ctx->stroke_dsc.miter_limit = miter_limit;
}

void lv_vector_set_stroke_grad(lv_vector_context_t * ctx, lv_grad_dsc_t * grad)
{
    ctx->stroke_dsc.grad = *grad;
}

void lv_vector_set_order(lv_vector_context_t * ctx, bool stroke_first)
{
    ctx->stroke_first = stroke_first;
}

void lv_matrix_translate(lv_matrix_t * matrix, float x, float y)
{
    float (*m)[3] = matrix->m;
    m[0][2] += m[0][0] * x + m[0][1] * y;
    m[1][2] += m[1][0] * x + m[1][1] * y;
    m[2][2] += m[2][0] * x + m[2][1] * y;
}

void lv_matrix_scale(lv_matrix_t * matrix, float a, float b)
{
    float (*m)[3] = matrix->m;
    m[0][0] *= a;
    m[1][0] *= a;
    m[2][0] *= a;
    m[0][1] *= b;
    m[1][1] *= b;
    m[2][1] *= b;
}

void lv_matrix_rotate(lv_matrix_t * matrix, float degree)
{
    float rad = RAD(degree);
    float c = cosf(rad);
    float s = sinf(rad);
    _matrix_rotate(matrix, c, s);
}

void lv_matrix_skew_x(lv_matrix_t * matrix, float skew_x)
{
    float cx = tanf(skew_x);
    _matrix_skew_x(matrix, cx);
}

void lv_matrix_skew_y(lv_matrix_t * matrix, float skew_y)
{
    float cy = tanf(skew_y);
    _matrix_skew_y(matrix, cy);
}

void lv_matrix_rotate_p3d(lv_matrix_t * matrix, float rx, float ry, float rz)
{
    float c1 = cosf(RAD(rx));
    float s1 = sinf(RAD(rx));
    float c2 = cosf(RAD(ry));
    float s2 = sinf(RAD(ry));
    float c3 = cosf(RAD(rz));
    float s3 = sinf(RAD(rz));
    _matrix_rotate3d(matrix, c1, s1, c2, s2, c3, s3);
}

void lv_matrix_rotate_quaternion(lv_matrix_t * matrix, float q0, float q1, float q2, float q3)
{
    float q11 = q1 * q1;
    float q22 = q2 * q2;
    float q33 = q3 * q3;
    float q12 = q1 * q2;
    float q03 = q0 * q3;
    float r11 = 1 - 2 * (q22 + q33);
    float r12 = 2 * (q12 - q03);
    float r21 = 2 * (q12 + q03);
    float r22 = 1 - 2 * (q11 + q33);
    _matrix_rotate3d_internal(matrix, r11, r12, r21, r22);
}

void lv_vector_translate(lv_vector_context_t * ctx, float dx, float dy)
{
    /* right multiply by [1 0 x; 0 1 y; 0 0 1] */
    lv_matrix_translate(&ctx->matrix, dx, dy);
    lv_matrix_translate(&ctx->fill_dsc.matrix, dx, dy);
}

void lv_vector_scale_with_pivot(lv_vector_context_t * ctx, float a, float b, float x, float y)
{
    /* right multiply by [a 0 (1-a)x; 0 b (1-b)y; 0 0 1] */
    lv_matrix_scale(&ctx->matrix, a, b);
    lv_matrix_scale(&ctx->fill_dsc.matrix, a, b);
    lv_matrix_translate(&ctx->matrix, (1 - a) * x, (1 - b) * y);
    lv_matrix_translate(&ctx->fill_dsc.matrix, (1 - a) * x, (1 - b) * y);
}

void lv_vector_transformation(lv_vector_context_t * ctx, float degree, float scale, float x, float y)
{
    /* right multiply by [zc -zs (1-zc)x+zs*y; zs zc (1-zc)y-zs*x; 0 0 1] */
    float rad = RAD(degree);
    float c = scale * cosf(rad);
    float s = scale * sinf(rad);
    _matrix_rotate(&ctx->matrix, c, s);
    _matrix_rotate(&ctx->fill_dsc.matrix, c, s);
    lv_matrix_translate(&ctx->matrix, (1 - c) * x + s * y, (1 - c) * y - s * x);
    lv_matrix_translate(&ctx->fill_dsc.matrix, (1 - c) * x + s * y, (1 - c) * y - s * x);
}

void lv_vector_skew_x(lv_vector_context_t * ctx, float skew_x)
{
    /* right multiply by [1 cx 0; 0 1 0; 0 0 1] */
    float cx = tanf(skew_x);
    _matrix_skew_x(&ctx->matrix, cx);
    _matrix_skew_x(&ctx->fill_dsc.matrix, cx);
}

void lv_vector_skew_y(lv_vector_context_t * ctx, float skew_y)
{
    /* right multiply by [1 0 0; cy 1 0; 0 0 1] */
    float cy = tanf(skew_y);
    _matrix_skew_y(&ctx->matrix, cy);
    _matrix_skew_y(&ctx->fill_dsc.matrix, cy);
}

void lv_vector_rotate_p3d(lv_vector_context_t * ctx, float rx, float ry, float rz)
{
    /* Using affine transformations to simulate 3d rotations cast to xy plane
     * right multiply by [r11 r12 0; r21 r22 0; 0 0 1],
     * where r11=c2*c3 r12=s1*s2*c3-c1*s3 r21=c2*s3 r22=c1*c3-s1*s2*s3
     */
    float c1 = cosf(RAD(rx));
    float s1 = sinf(RAD(rx));
    float c2 = cosf(RAD(ry));
    float s2 = sinf(RAD(ry));
    float c3 = cosf(RAD(rz));
    float s3 = sinf(RAD(rz));
    _matrix_rotate3d(&ctx->matrix, c1, s1, c2, s2, c3, s3);
    _matrix_rotate3d(&ctx->fill_dsc.matrix, c1, s1, c2, s2, c3, s3);
}

void lv_vector_rotate_quaternion(lv_vector_context_t * ctx, float q0, float q1, float q2, float q3)
{
    float q11 = q1 * q1;
    float q22 = q2 * q2;
    float q33 = q3 * q3;
    float q12 = q1 * q2;
    float q03 = q0 * q3;
    float r11 = 1 - 2 * (q22 + q33);
    float r12 = 2 * (q12 - q03);
    float r21 = 2 * (q12 + q03);
    float r22 = 1 - 2 * (q11 + q33);
    _matrix_rotate3d_internal(&ctx->matrix, r11, r12, r21, r22);
    _matrix_rotate3d_internal(&ctx->fill_dsc.matrix, r11, r12, r21, r22);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void _matrix_rotate(lv_matrix_t * matrix, float c, float s)
{
    float (*m)[3] = matrix->m;
    m[0][0] = c * m[0][0] + s * m[0][1];
    m[1][0] = c * m[1][0] + s * m[1][1];
    m[2][0] = c * m[2][0] + s * m[2][1];
    m[0][1] = c * m[0][1] - s * m[0][0];
    m[1][1] = c * m[1][1] - s * m[1][0];
    m[2][1] = c * m[2][1] - s * m[2][0];
}

static inline void _matrix_skew_x(lv_matrix_t * matrix, float cx)
{
    float (*m)[3] = matrix->m;
    m[0][1] += cx * m[0][0];
    m[1][1] += cx * m[1][0];
}

static inline void _matrix_skew_y(lv_matrix_t * matrix, float cy)
{
    float (*m)[3] = matrix->m;
    m[0][0] += cy * m[0][1];
    m[1][0] += cy * m[1][1];
}

static inline void _matrix_rotate3d(lv_matrix_t * matrix, float c1, float s1, float c2, float s2, float c3, float s3)
{
    float r11 = c2 * c3;
    float r12 = s2 * s2 * c3 - c1 * s3;
    float r21 = c2 * s3;
    float r22 = c1 * c3 - s1 * s2 * s3;
    _matrix_rotate3d_internal(matrix, r11, r12, r21, r22);
}

static inline void _matrix_rotate3d_internal(lv_matrix_t * matrix, float r11, float r12, float r21, float r22)
{
    float (*m)[3] = matrix->m;
    m[0][0] = r11 * m[0][0] + r21 * m[0][1];
    m[1][0] = r11 * m[1][0] + r21 * m[1][1];
    m[2][0] = r11 * m[2][0] + r21 * m[2][1];
    m[0][1] = r12 * m[0][1] - r22 * m[0][0];
    m[1][1] = r12 * m[1][1] - r22 * m[1][0];
    m[2][1] = r12 * m[2][1] - r22 * m[2][0];
}

#endif /*LV_USE_VECTOR*/
