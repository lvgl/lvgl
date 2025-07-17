/**
* @file lv_draw_vector.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_vector_private.h"
#include "../misc/lv_area_private.h"
#include "lv_draw_private.h"

#if LV_USE_VECTOR_GRAPHIC

#if !((LV_USE_DRAW_SW && LV_USE_THORVG) || LV_USE_DRAW_VG_LITE)
    #error "LV_USE_VECTOR_GRAPHIC requires either (LV_USE_DRAW_SW and LV_USE_THORVG) or LV_USE_DRAW_VG_LITE"
#endif

#include "../misc/lv_ll.h"
#include "../misc/lv_types.h"
#include "../stdlib/lv_string.h"
#include <math.h>
#include <float.h>

#define MATH_PI  3.14159265358979323846f
#define MATH_HALF_PI 1.57079632679489661923f

#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

#define MATH_RADIANS(deg) ((deg) * DEG_TO_RAD)
#define MATH_DEGREES(rad) ((rad) * RAD_TO_DEG)

/*********************
 *      DEFINES
 *********************/

#ifndef M_PI
    #define M_PI 3.1415926f
#endif

#define CHECK_AND_RESIZE_PATH_CONTAINER(P, N) \
    do { \
        if ((lv_array_size(&(P)->ops) + (N)) > lv_array_capacity(&(P)->ops)) { \
            lv_array_resize(&(P)->ops, ((P)->ops.capacity << 1)); \
        } \
        if ((lv_array_size(&(P)->points) + (N)) > lv_array_capacity(&(P)->points)) { \
            lv_array_resize(&(P)->points, ((P)->points.capacity << 1)); \
        } \
    } while(0)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_vector_path_t * path;
    lv_vector_draw_dsc_t dsc;
} lv_vector_draw_task;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _copy_draw_dsc(lv_vector_draw_dsc_t * dst, const lv_vector_draw_dsc_t * src)
{
    lv_memcpy(&(dst->fill_dsc), &(src->fill_dsc), sizeof(lv_vector_fill_dsc_t));

    dst->stroke_dsc.style = src->stroke_dsc.style;
    dst->stroke_dsc.color = src->stroke_dsc.color;
    dst->stroke_dsc.opa = src->stroke_dsc.opa;
    dst->stroke_dsc.width = src->stroke_dsc.width;
    dst->stroke_dsc.cap = src->stroke_dsc.cap;
    dst->stroke_dsc.join = src->stroke_dsc.join;
    dst->stroke_dsc.miter_limit = src->stroke_dsc.miter_limit;
    lv_array_copy(&(dst->stroke_dsc.dash_pattern), &(src->stroke_dsc.dash_pattern));
    dst->stroke_dsc.gradient.style = src->stroke_dsc.gradient.style;
    dst->stroke_dsc.gradient.cx = src->stroke_dsc.gradient.cx;
    dst->stroke_dsc.gradient.cy = src->stroke_dsc.gradient.cy;
    dst->stroke_dsc.gradient.cr = src->stroke_dsc.gradient.cr;
    dst->stroke_dsc.gradient.spread = src->fill_dsc.gradient.spread;
    lv_memcpy(&(dst->stroke_dsc.gradient), &(src->stroke_dsc.gradient), sizeof(lv_vector_gradient_t));
    lv_memcpy(&(dst->stroke_dsc.matrix), &(src->stroke_dsc.matrix), sizeof(lv_matrix_t));

    dst->blend_mode = src->blend_mode;
    lv_memcpy(&(dst->matrix), &(src->matrix), sizeof(lv_matrix_t));
    lv_area_copy(&(dst->scissor_area), &(src->scissor_area));
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_matrix_transform_point(const lv_matrix_t * matrix, lv_fpoint_t * point)
{
    float x = point->x;
    float y = point->y;

    point->x = x * matrix->m[0][0] + y * matrix->m[1][0] + matrix->m[0][2];
    point->y = x * matrix->m[0][1] + y * matrix->m[1][1] + matrix->m[1][2];
}

void lv_matrix_transform_path(const lv_matrix_t * matrix, lv_vector_path_t * path)
{
    lv_fpoint_t * pt = lv_array_front(&path->points);
    uint32_t size = lv_array_size(&path->points);
    for(uint32_t i = 0; i < size; i++) {
        lv_matrix_transform_point(matrix, &pt[i]);
    }
}

/* path functions */
lv_vector_path_t * lv_vector_path_create(lv_vector_path_quality_t quality)
{
    lv_vector_path_t * path = lv_malloc(sizeof(lv_vector_path_t));
    LV_ASSERT_MALLOC(path);
    lv_memzero(path, sizeof(lv_vector_path_t));
    path->quality = quality;
    lv_array_init(&path->ops, 8, sizeof(lv_vector_path_op_t));
    lv_array_init(&path->points, 8, sizeof(lv_fpoint_t));
    return path;
}

void lv_vector_path_copy(lv_vector_path_t * target_path, const lv_vector_path_t * path)
{
    target_path->quality = path->quality;
    lv_array_copy(&target_path->ops, &path->ops);
    lv_array_copy(&target_path->points, &path->points);
}

void lv_vector_path_clear(lv_vector_path_t * path)
{
    lv_array_clear(&path->ops);
    lv_array_clear(&path->points);
}

void lv_vector_path_delete(lv_vector_path_t * path)
{
    lv_array_deinit(&path->ops);
    lv_array_deinit(&path->points);
    lv_free(path);
}

void lv_vector_path_move_to(lv_vector_path_t * path, const lv_fpoint_t * p)
{
    CHECK_AND_RESIZE_PATH_CONTAINER(path, 1);

    lv_vector_path_op_t op = LV_VECTOR_PATH_OP_MOVE_TO;
    lv_array_push_back(&path->ops, &op);
    lv_array_push_back(&path->points, p);
}

void lv_vector_path_line_to(lv_vector_path_t * path, const lv_fpoint_t * p)
{
    if(lv_array_is_empty(&path->ops)) {
        /*first op must be move_to*/
        return;
    }

    CHECK_AND_RESIZE_PATH_CONTAINER(path, 1);

    lv_vector_path_op_t op = LV_VECTOR_PATH_OP_LINE_TO;
    lv_array_push_back(&path->ops, &op);
    lv_array_push_back(&path->points, p);
}

void lv_vector_path_quad_to(lv_vector_path_t * path, const lv_fpoint_t * p1, const lv_fpoint_t * p2)
{
    if(lv_array_is_empty(&path->ops)) {
        /*first op must be move_to*/
        return;
    }

    CHECK_AND_RESIZE_PATH_CONTAINER(path, 2);

    lv_vector_path_op_t op = LV_VECTOR_PATH_OP_QUAD_TO;
    lv_array_push_back(&path->ops, &op);
    lv_array_push_back(&path->points, p1);
    lv_array_push_back(&path->points, p2);
}

void lv_vector_path_cubic_to(lv_vector_path_t * path, const lv_fpoint_t * p1, const lv_fpoint_t * p2,
                             const lv_fpoint_t * p3)
{
    if(lv_array_is_empty(&path->ops)) {
        /*first op must be move_to*/
        return;
    }

    CHECK_AND_RESIZE_PATH_CONTAINER(path, 3);

    lv_vector_path_op_t op = LV_VECTOR_PATH_OP_CUBIC_TO;
    lv_array_push_back(&path->ops, &op);
    lv_array_push_back(&path->points, p1);
    lv_array_push_back(&path->points, p2);
    lv_array_push_back(&path->points, p3);
}

void lv_vector_path_close(lv_vector_path_t * path)
{
    if(lv_array_is_empty(&path->ops)) {
        /*first op must be move_to*/
        return;
    }

    CHECK_AND_RESIZE_PATH_CONTAINER(path, 1);

    lv_vector_path_op_t op = LV_VECTOR_PATH_OP_CLOSE;
    lv_array_push_back(&path->ops, &op);
}

void lv_vector_path_get_bounding(const lv_vector_path_t * path, lv_area_t * area)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(area);

    uint32_t len = lv_array_size(&path->points);
    if(len == 0) {
        lv_memzero(area, sizeof(lv_area_t));
        return;
    }

    lv_fpoint_t * p = lv_array_front(&path->points);
    float x1 = p[0].x;
    float x2 = p[0].x;
    float y1 = p[0].y;
    float y2 = p[0].y;

    for(uint32_t i = 1; i < len; i++) {
        if(p[i].x < x1) x1 = p[i].x;
        if(p[i].y < y1) y1 = p[i].y;
        if(p[i].x > x2) x2 = p[i].x;
        if(p[i].y > y2) y2 = p[i].y;
    }

    area->x1 = lroundf(x1);
    area->y1 = lroundf(y1);
    area->x2 = lroundf(x2);
    area->y2 = lroundf(y2);
}

void lv_vector_path_append_rect(lv_vector_path_t * path, const lv_area_t * rect, float rx, float ry)
{
    float x = rect->x1;
    float y = rect->y1;
    float w = (float)lv_area_get_width(rect);
    float h = (float)lv_area_get_height(rect);

    float hw = w * 0.5f;
    float hh = h * 0.5f;

    if(rx > hw) rx = hw;
    if(ry > hh) ry = hh;

    if(rx == 0 && ry == 0) {
        lv_fpoint_t pt = {x, y};
        lv_vector_path_move_to(path, &pt);
        pt.x += w;
        lv_vector_path_line_to(path, &pt);
        pt.y += h;
        lv_vector_path_line_to(path, &pt);
        pt.x -= w;
        lv_vector_path_line_to(path, &pt);
        lv_vector_path_close(path);
    }
    else if(rx == hw && ry == hh) {
        lv_fpoint_t pt = {x + w * 0.5f, y + h * 0.5f};
        lv_vector_path_append_circle(path, &pt, rx, ry);
    }
    else {
        float hrx = rx * 0.5f;
        float hry = ry * 0.5f;
        lv_fpoint_t pt, pt2, pt3;

        pt.x = x + rx;
        pt.y = y;
        lv_vector_path_move_to(path, &pt);

        pt.x = x + w - rx;
        pt.y = y;
        lv_vector_path_line_to(path, &pt);

        pt.x = x + w - rx + hrx;
        pt.y = y;
        pt2.x = x + w;
        pt2.y = y + ry - hry;
        pt3.x = x + w;
        pt3.y = y + ry;
        lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);

        pt.x = x + w;
        pt.y = y + h - ry;
        lv_vector_path_line_to(path, &pt);

        pt.x = x + w;
        pt.y = y + h - ry + hry;
        pt2.x = x + w - rx + hrx;
        pt2.y = y + h;
        pt3.x = x + w - rx;
        pt3.y = y + h;
        lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);

        pt.x = x + rx;
        pt.y = y + h;
        lv_vector_path_line_to(path, &pt);

        pt.x = x + rx - hrx;
        pt.y = y + h;
        pt2.x = x;
        pt2.y = y + h - ry + hry;
        pt3.x = x;
        pt3.y = y + h - ry;
        lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);

        pt.x = x;
        pt.y = y + ry;
        lv_vector_path_line_to(path, &pt);

        pt.x = x;
        pt.y = y + ry - hry;
        pt2.x = x + rx - hrx;
        pt2.y = y;
        pt3.x = x + rx;
        pt3.y = y;
        lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);
        lv_vector_path_close(path);
    }
}

void lv_vector_path_append_circle(lv_vector_path_t * path, const lv_fpoint_t * c, float rx, float ry)
{
    float krx = rx * 0.552284f;
    float kry = ry * 0.552284f;
    float cx = c->x;
    float cy = c->y;

    lv_fpoint_t pt, pt2, pt3;
    pt.x = cx;
    pt.y = cy - ry;
    lv_vector_path_move_to(path, &pt);

    pt.x = cx + krx;
    pt.y = cy - ry;
    pt2.x = cx + rx;
    pt2.y = cy - kry;
    pt3.x = cx + rx;
    pt3.y = cy;
    lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);

    pt.x = cx + rx;
    pt.y = cy + kry;
    pt2.x = cx + krx;
    pt2.y = cy + ry;
    pt3.x = cx;
    pt3.y = cy + ry;
    lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);

    pt.x = cx - krx;
    pt.y = cy + ry;
    pt2.x = cx - rx;
    pt2.y = cy + kry;
    pt3.x = cx - rx;
    pt3.y = cy;
    lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);

    pt.x = cx - rx;
    pt.y = cy - kry;
    pt2.x = cx - krx;
    pt2.y = cy - ry;
    pt3.x = cx;
    pt3.y = cy - ry;
    lv_vector_path_cubic_to(path, &pt, &pt2, &pt3);

    lv_vector_path_close(path);
}

/**
 * Add a arc to the path
 * @param path              pointer to a path
 * @param c                 pointer to a `lv_fpoint_t` variable for center of the circle
 * @param radius            the radius for arc
 * @param start_angle       the start angle for arc
 * @param sweep             the sweep angle for arc, could be negative
 * @param pie               true: draw a pie, false: draw a arc
 */
void lv_vector_path_append_arc(lv_vector_path_t * path, const lv_fpoint_t * c, float radius, float start_angle,
                               float sweep, bool pie)
{
    float cx = c->x;
    float cy = c->y;

    /* just circle */
    if(sweep >= 360.0f || sweep <= -360.0f) {
        lv_vector_path_append_circle(path, c, radius, radius);
        return;
    }

    start_angle = MATH_RADIANS(start_angle);
    sweep = MATH_RADIANS(sweep);

    int n_curves = (int)ceil(fabsf(sweep / MATH_HALF_PI));
    float sweep_sign = sweep < 0 ? -1.f : 1.f;
    float fract = fmodf(sweep, MATH_HALF_PI);
    fract = (fabsf(fract) < FLT_EPSILON) ? MATH_HALF_PI * sweep_sign : fract;

    /* Start from here */
    lv_fpoint_t start = {
        .x = radius * cosf(start_angle),
        .y = radius * sinf(start_angle),
    };

    if(pie) {
        lv_vector_path_move_to(path, &(lv_fpoint_t) {
            cx, cy
        });
        lv_vector_path_line_to(path, &(lv_fpoint_t) {
            start.x + cx, start.y + cy
        });
    }
    else {
        lv_vector_path_move_to(path, &(lv_fpoint_t) {
            start.x + cx, start.y + cy
        });
    }

    for(int i = 0; i < n_curves; ++i) {
        float end_angle = start_angle + ((i != n_curves - 1) ? MATH_HALF_PI * sweep_sign : fract);
        float end_x = radius * cosf(end_angle);
        float end_y = radius * sinf(end_angle);

        /* variables needed to calculate bezier control points */

        /** get bezier control points using article:
         * (http://itc.ktu.lt/index.php/ITC/article/view/11812/6479)
         */
        float ax = start.x;
        float ay = start.y;
        float bx = end_x;
        float by = end_y;
        float q1 = ax * ax + ay * ay;
        float q2 = ax * bx + ay * by + q1;
        float k2 = (4.0f / 3.0f) * ((sqrtf(2 * q1 * q2) - q2) / (ax * by - ay * bx));

        /* Next start point is the current end point */
        start.x = end_x;
        start.y = end_y;

        end_x += cx;
        end_y += cy;

        lv_fpoint_t ctrl1 = {ax - k2 * ay + cx, ay + k2 * ax + cy};
        lv_fpoint_t ctrl2 = {bx + k2 * by + cx, by - k2 * bx + cy};
        lv_fpoint_t end = {end_x, end_y};
        lv_vector_path_cubic_to(path, &ctrl1, &ctrl2, &end);
        start_angle = end_angle;
    }

    if(pie) {
        lv_vector_path_close(path);
    }
}

void lv_vector_path_append_path(lv_vector_path_t * path, const lv_vector_path_t * subpath)
{
    uint32_t ops_size = lv_array_size(&path->ops);
    uint32_t nops_size = lv_array_size(&subpath->ops);
    uint32_t point_size = lv_array_size(&path->points);
    uint32_t npoint_size = lv_array_size(&subpath->points);

    lv_array_concat(&path->ops, &subpath->ops);
    path->ops.size = ops_size + nops_size;

    lv_array_concat(&path->points, &subpath->points);
    path->points.size = point_size + npoint_size;
}

/* draw dsc functions */

lv_vector_dsc_t * lv_vector_dsc_create(lv_layer_t * layer)
{
    lv_vector_dsc_t * dsc = lv_malloc(sizeof(lv_vector_dsc_t));
    LV_ASSERT_MALLOC(dsc);
    lv_memzero(dsc, sizeof(lv_vector_dsc_t));

    dsc->layer = layer;

    lv_vector_fill_dsc_t * fill_dsc = &(dsc->current_dsc.fill_dsc);
    fill_dsc->style = LV_VECTOR_DRAW_STYLE_SOLID;
    fill_dsc->color = lv_color_to_32(lv_color_black(), 0xFF);
    fill_dsc->opa = LV_OPA_COVER;
    fill_dsc->fill_rule = LV_VECTOR_FILL_NONZERO;
    lv_matrix_identity(&(fill_dsc->matrix)); /*identity matrix*/

    lv_vector_stroke_dsc_t * stroke_dsc = &(dsc->current_dsc.stroke_dsc);
    stroke_dsc->style = LV_VECTOR_DRAW_STYLE_SOLID;
    stroke_dsc->color = lv_color_to_32(lv_color_black(), 0xFF);
    stroke_dsc->opa = LV_OPA_0; /*default no stroke*/
    stroke_dsc->width = 1.0f;
    stroke_dsc->cap = LV_VECTOR_STROKE_CAP_BUTT;
    stroke_dsc->join = LV_VECTOR_STROKE_JOIN_MITER;
    stroke_dsc->miter_limit = 4.0f;
    lv_matrix_identity(&(stroke_dsc->matrix)); /*identity matrix*/

    dsc->current_dsc.blend_mode = LV_VECTOR_BLEND_SRC_OVER;
    dsc->current_dsc.scissor_area = layer->_clip_area;
    lv_matrix_identity(&(dsc->current_dsc.matrix)); /*identity matrix*/
    dsc->tasks.task_list = NULL;
    return dsc;
}

void lv_vector_dsc_delete(lv_vector_dsc_t * dsc)
{
    if(dsc->tasks.task_list) {
        lv_ll_t * task_list = dsc->tasks.task_list;
        lv_vector_for_each_destroy_tasks(task_list, NULL, NULL);
        dsc->tasks.task_list = NULL;
    }
    lv_array_deinit(&(dsc->current_dsc.stroke_dsc.dash_pattern));
    lv_free(dsc);
}

void lv_vector_dsc_set_blend_mode(lv_vector_dsc_t * dsc, lv_vector_blend_t blend)
{
    dsc->current_dsc.blend_mode = blend;
}

void lv_vector_dsc_set_transform(lv_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_memcpy(&(dsc->current_dsc.matrix), matrix, sizeof(lv_matrix_t));
}

void lv_vector_dsc_set_fill_color(lv_vector_dsc_t * dsc, lv_color_t color)
{
    dsc->current_dsc.fill_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
    dsc->current_dsc.fill_dsc.color = lv_color_to_32(color, 0xFF);
}

void lv_vector_dsc_set_fill_color32(lv_vector_dsc_t * dsc, lv_color32_t color)
{
    dsc->current_dsc.fill_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
    dsc->current_dsc.fill_dsc.color = color;
}

void lv_vector_dsc_set_fill_opa(lv_vector_dsc_t * dsc, lv_opa_t opa)
{
    dsc->current_dsc.fill_dsc.opa = opa;
}

void lv_vector_dsc_set_fill_rule(lv_vector_dsc_t * dsc, lv_vector_fill_t rule)
{
    dsc->current_dsc.fill_dsc.fill_rule = rule;
}

void lv_vector_dsc_set_fill_units(lv_vector_dsc_t * dsc, const lv_vector_fill_units_t units)
{
    dsc->current_dsc.fill_dsc.fill_units = units;
}

void lv_vector_dsc_set_fill_image(lv_vector_dsc_t * dsc, const lv_draw_image_dsc_t * img_dsc)
{
    dsc->current_dsc.fill_dsc.style = LV_VECTOR_DRAW_STYLE_PATTERN;
    lv_memcpy(&(dsc->current_dsc.fill_dsc.img_dsc), img_dsc, sizeof(lv_draw_image_dsc_t));
}

void lv_vector_dsc_set_fill_linear_gradient(lv_vector_dsc_t * dsc, float x1, float y1, float x2, float y2)
{
    dsc->current_dsc.fill_dsc.style = LV_VECTOR_DRAW_STYLE_GRADIENT;
    dsc->current_dsc.fill_dsc.gradient.style = LV_VECTOR_GRADIENT_STYLE_LINEAR;
    dsc->current_dsc.fill_dsc.gradient.x1 = x1;
    dsc->current_dsc.fill_dsc.gradient.y1 = y1;
    dsc->current_dsc.fill_dsc.gradient.x2 = x2;
    dsc->current_dsc.fill_dsc.gradient.y2 = y2;
}

void lv_vector_dsc_set_fill_radial_gradient(lv_vector_dsc_t * dsc, float cx, float cy, float radius)
{
    dsc->current_dsc.fill_dsc.style = LV_VECTOR_DRAW_STYLE_GRADIENT;
    dsc->current_dsc.fill_dsc.gradient.style = LV_VECTOR_GRADIENT_STYLE_RADIAL;
    dsc->current_dsc.fill_dsc.gradient.cx = cx;
    dsc->current_dsc.fill_dsc.gradient.cy = cy;
    dsc->current_dsc.fill_dsc.gradient.cr = radius;
}

void lv_vector_dsc_set_fill_gradient_spread(lv_vector_dsc_t * dsc, lv_vector_gradient_spread_t spread)
{
    dsc->current_dsc.fill_dsc.gradient.spread = spread;
}

void lv_vector_dsc_set_fill_gradient_color_stops(lv_vector_dsc_t * dsc, const lv_grad_stop_t * stops,
                                                 uint16_t count)
{
    if(count > LV_GRADIENT_MAX_STOPS) {
        LV_LOG_WARN("Gradient stops limited: %d, max: %d", count, LV_GRADIENT_MAX_STOPS);
        count = LV_GRADIENT_MAX_STOPS;
    }

    lv_memcpy(&(dsc->current_dsc.fill_dsc.gradient.stops), stops, sizeof(lv_grad_stop_t) * count);
    dsc->current_dsc.fill_dsc.gradient.stops_count = count;
}

void lv_vector_dsc_set_fill_transform(lv_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_memcpy(&(dsc->current_dsc.fill_dsc.matrix), matrix, sizeof(lv_matrix_t));
}

void lv_vector_dsc_set_stroke_transform(lv_vector_dsc_t * dsc, const lv_matrix_t * matrix)
{
    lv_memcpy(&(dsc->current_dsc.stroke_dsc.matrix), matrix, sizeof(lv_matrix_t));
}

void lv_vector_dsc_set_stroke_color32(lv_vector_dsc_t * dsc, lv_color32_t color)
{
    dsc->current_dsc.stroke_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
    dsc->current_dsc.stroke_dsc.color = color;
}

void lv_vector_dsc_set_stroke_color(lv_vector_dsc_t * dsc, lv_color_t color)
{
    dsc->current_dsc.stroke_dsc.style = LV_VECTOR_DRAW_STYLE_SOLID;
    dsc->current_dsc.stroke_dsc.color = lv_color_to_32(color, 0xFF);
}

void lv_vector_dsc_set_stroke_opa(lv_vector_dsc_t * dsc, lv_opa_t opa)
{
    dsc->current_dsc.stroke_dsc.opa = opa;
}

void lv_vector_dsc_set_stroke_width(lv_vector_dsc_t * dsc, float width)
{
    dsc->current_dsc.stroke_dsc.width = width;
}

void lv_vector_dsc_set_stroke_dash(lv_vector_dsc_t * dsc, float * dash_pattern, uint16_t dash_count)
{
    lv_array_t * dash_array = &(dsc->current_dsc.stroke_dsc.dash_pattern);
    if(dash_pattern) {
        lv_array_clear(dash_array);
        if(lv_array_capacity(dash_array) == 0) {
            lv_array_init(dash_array, dash_count, sizeof(float));
        }
        else {
            lv_array_resize(dash_array, dash_count);
        }
        for(uint16_t i = 0; i < dash_count; i++) {
            lv_array_push_back(dash_array, &dash_pattern[i]);
        }
    }
    else {   /*clear dash*/
        lv_array_clear(dash_array);
    }
}

void lv_vector_dsc_set_stroke_cap(lv_vector_dsc_t * dsc, lv_vector_stroke_cap_t cap)
{
    dsc->current_dsc.stroke_dsc.cap = cap;
}

void lv_vector_dsc_set_stroke_join(lv_vector_dsc_t * dsc, lv_vector_stroke_join_t join)
{
    dsc->current_dsc.stroke_dsc.join = join;
}

void lv_vector_dsc_set_stroke_miter_limit(lv_vector_dsc_t * dsc, uint16_t miter_limit)
{
    dsc->current_dsc.stroke_dsc.miter_limit = miter_limit;
}

void lv_vector_dsc_set_stroke_linear_gradient(lv_vector_dsc_t * dsc, float x1, float y1, float x2, float y2)
{
    dsc->current_dsc.stroke_dsc.style = LV_VECTOR_DRAW_STYLE_GRADIENT;
    dsc->current_dsc.stroke_dsc.gradient.style = LV_VECTOR_GRADIENT_STYLE_LINEAR;
    dsc->current_dsc.stroke_dsc.gradient.x1 = x1;
    dsc->current_dsc.stroke_dsc.gradient.y1 = y1;
    dsc->current_dsc.stroke_dsc.gradient.x2 = x2;
    dsc->current_dsc.stroke_dsc.gradient.y2 = y2;
}

void lv_vector_dsc_set_stroke_radial_gradient(lv_vector_dsc_t * dsc, float cx, float cy, float radius)
{
    dsc->current_dsc.stroke_dsc.style = LV_VECTOR_DRAW_STYLE_GRADIENT;
    dsc->current_dsc.stroke_dsc.gradient.style = LV_VECTOR_GRADIENT_STYLE_RADIAL;
    dsc->current_dsc.stroke_dsc.gradient.cx = cx;
    dsc->current_dsc.stroke_dsc.gradient.cy = cy;
    dsc->current_dsc.stroke_dsc.gradient.cr = radius;
}

void lv_vector_dsc_set_stroke_gradient_spread(lv_vector_dsc_t * dsc, lv_vector_gradient_spread_t spread)
{
    dsc->current_dsc.stroke_dsc.gradient.spread = spread;
}

void lv_vector_dsc_set_stroke_gradient_color_stops(lv_vector_dsc_t * dsc, const lv_grad_stop_t * stops,
                                                   uint16_t count)
{
    if(count > LV_GRADIENT_MAX_STOPS) {
        LV_LOG_WARN("Gradient stops limited: %d, max: %d", count, LV_GRADIENT_MAX_STOPS);
        count = LV_GRADIENT_MAX_STOPS;
    }

    lv_memcpy(&(dsc->current_dsc.stroke_dsc.gradient.stops), stops, sizeof(lv_grad_stop_t) * count);
    dsc->current_dsc.stroke_dsc.gradient.stops_count = count;
}

/* draw functions */
void lv_vector_dsc_add_path(lv_vector_dsc_t * dsc, const lv_vector_path_t * path)
{
    lv_area_t rect;
    if(!lv_area_intersect(&rect, &(dsc->layer->_clip_area), &(dsc->current_dsc.scissor_area))) {
        return;
    }

    if(dsc->current_dsc.fill_dsc.opa == 0
       && dsc->current_dsc.stroke_dsc.opa == 0) {
        return;
    }

    if(!dsc->tasks.task_list) {
        dsc->tasks.task_list = lv_malloc(sizeof(lv_ll_t));
        LV_ASSERT_MALLOC(dsc->tasks.task_list);
        lv_ll_init(dsc->tasks.task_list, sizeof(lv_vector_draw_task));
    }

    lv_vector_draw_task * new_task = (lv_vector_draw_task *)lv_ll_ins_tail(dsc->tasks.task_list);
    lv_memset(new_task, 0, sizeof(lv_vector_draw_task));

    new_task->path = lv_vector_path_create(0);

    _copy_draw_dsc(&(new_task->dsc), &(dsc->current_dsc));
    lv_vector_path_copy(new_task->path, path);
    new_task->dsc.scissor_area = rect;
}

void lv_vector_clear_area(lv_vector_dsc_t * dsc, const lv_area_t * rect)
{
    lv_area_t r;
    if(!lv_area_intersect(&r, &(dsc->layer->_clip_area), &(dsc->current_dsc.scissor_area))) {
        return;
    }

    lv_area_t final_rect;
    if(!lv_area_intersect(&final_rect, &r, rect)) {
        return;
    }

    if(!dsc->tasks.task_list) {
        dsc->tasks.task_list = lv_malloc(sizeof(lv_ll_t));
        LV_ASSERT_MALLOC(dsc->tasks.task_list);
        lv_ll_init(dsc->tasks.task_list, sizeof(lv_vector_draw_task));
    }

    lv_vector_draw_task * new_task = (lv_vector_draw_task *)lv_ll_ins_tail(dsc->tasks.task_list);
    lv_memset(new_task, 0, sizeof(lv_vector_draw_task));

    new_task->dsc.fill_dsc.color = dsc->current_dsc.fill_dsc.color;
    new_task->dsc.fill_dsc.opa = dsc->current_dsc.fill_dsc.opa;
    lv_area_copy(&(new_task->dsc.scissor_area), &final_rect);
}

void lv_draw_vector(lv_vector_dsc_t * dsc)
{
    if(!dsc->tasks.task_list) {
        return;
    }

    lv_layer_t * layer = dsc->layer;

    lv_draw_task_t * t = lv_draw_add_task(layer, &(layer->_clip_area), LV_DRAW_TASK_TYPE_VECTOR);
    lv_memcpy(t->draw_dsc, &(dsc->tasks), sizeof(lv_draw_vector_task_dsc_t));
    lv_draw_finalize_task_creation(layer, t);
    dsc->tasks.task_list = NULL;
}

/* draw dsc transform */
void lv_vector_dsc_identity(lv_vector_dsc_t * dsc)
{
    lv_matrix_identity(&(dsc->current_dsc.matrix)); /*identity matrix*/
}

void lv_vector_dsc_scale(lv_vector_dsc_t * dsc, float scale_x, float scale_y)
{
    lv_matrix_scale(&(dsc->current_dsc.matrix), scale_x, scale_y);
}

void lv_vector_dsc_rotate(lv_vector_dsc_t * dsc, float degree)
{
    lv_matrix_rotate(&(dsc->current_dsc.matrix), degree);
}

void lv_vector_dsc_translate(lv_vector_dsc_t * dsc, float tx, float ty)
{
    lv_matrix_translate(&(dsc->current_dsc.matrix), tx, ty);
}

void lv_vector_dsc_skew(lv_vector_dsc_t * dsc, float skew_x, float skew_y)
{
    lv_matrix_skew(&(dsc->current_dsc.matrix), skew_x, skew_y);
}

void lv_vector_for_each_destroy_tasks(lv_ll_t * task_list, vector_draw_task_cb cb, void * data)
{
    if(task_list == NULL) return;

    lv_vector_draw_task * task = lv_ll_get_head(task_list);
    lv_vector_draw_task * next_task = NULL;

    while(task != NULL) {
        next_task = lv_ll_get_next(task_list, task);
        lv_ll_remove(task_list, task);

        if(cb) {
            cb(data, task->path, &(task->dsc));
        }

        if(task->path) {
            lv_vector_path_delete(task->path);
        }
        lv_array_deinit(&(task->dsc.stroke_dsc.dash_pattern));

        lv_free(task);
        task = next_task;
    }
    lv_free(task_list);
}
#endif /* LV_USE_VECTOR_GRAPHIC */
