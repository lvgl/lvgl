/**
 * @file lv_vg_lite_path.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_path.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_math.h"
#include <float.h>

/*********************
 *      DEFINES
 *********************/

#define PATH_KAPPA 0.552284f

/* Magic number from https://spencermortensen.com/articles/bezier-circle/ */
#define PATH_ARC_MAGIC 0.55191502449351f

#define SIGN(x) (math_zero(x) ? 0 : ((x) > 0 ? 1 : -1))

#define VLC_OP_ARG_LEN(OP, LEN) \
    case VLC_OP_##OP:           \
    return (LEN)

/**********************
 *      TYPEDEFS
 **********************/

struct lv_vg_lite_path_t {
    vg_lite_path_t base;
    size_t mem_size;
    uint8_t format_len;
};

typedef struct {
    float min_x;
    float min_y;
    float max_x;
    float max_y;
} lv_vg_lite_path_bounds_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_path_init(struct lv_draw_vg_lite_unit_t * unit)
{
    LV_ASSERT_NULL(unit);
    unit->global_path = lv_vg_lite_path_create(VG_LITE_FP32);
    unit->path_in_use = false;
}

void lv_vg_lite_path_deinit(struct lv_draw_vg_lite_unit_t * unit)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT(!unit->path_in_use);
    lv_vg_lite_path_destroy(unit->global_path);
    unit->global_path = NULL;
}

lv_vg_lite_path_t * lv_vg_lite_path_create(vg_lite_format_t data_format)
{
    LV_PROFILER_BEGIN;
    lv_vg_lite_path_t * path = lv_malloc_zeroed(sizeof(lv_vg_lite_path_t));
    LV_ASSERT_MALLOC(path);
    path->format_len = lv_vg_lite_path_format_len(data_format);
    LV_ASSERT(vg_lite_init_path(
                  &path->base,
                  data_format,
                  VG_LITE_MEDIUM,
                  0,
                  NULL,
                  0, 0, 0, 0)
              == VG_LITE_SUCCESS);
    LV_PROFILER_END;
    return path;
}

void lv_vg_lite_path_destroy(lv_vg_lite_path_t * path)
{
    LV_PROFILER_BEGIN;
    LV_ASSERT_NULL(path);
    if(path->base.path != NULL) {
        lv_free(path->base.path);
        path->base.path = NULL;

        /* clear remaining path data */
        LV_VG_LITE_CHECK_ERROR(vg_lite_clear_path(&path->base));
    }
    lv_free(path);
    LV_PROFILER_END;
}

lv_vg_lite_path_t * lv_vg_lite_path_get(struct lv_draw_vg_lite_unit_t * unit, vg_lite_format_t data_format)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(unit->global_path);
    LV_ASSERT(!unit->path_in_use);
    lv_vg_lite_path_reset(unit->global_path, data_format);
    unit->path_in_use = true;
    return unit->global_path;
}

void lv_vg_lite_path_drop(struct lv_draw_vg_lite_unit_t * unit, lv_vg_lite_path_t * path)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(path);
    LV_ASSERT(unit->global_path == path);
    LV_ASSERT(unit->path_in_use);
    unit->path_in_use = false;
}

void lv_vg_lite_path_reset(lv_vg_lite_path_t * path, vg_lite_format_t data_format)
{
    LV_ASSERT_NULL(path);
    path->base.path_length = 0;
    path->base.format = data_format;
    path->base.quality = VG_LITE_MEDIUM;
    path->base.path_type = VG_LITE_DRAW_ZERO;
    path->format_len = lv_vg_lite_path_format_len(data_format);
}

vg_lite_path_t * lv_vg_lite_path_get_path(lv_vg_lite_path_t * path)
{
    LV_ASSERT_NULL(path);
    return &path->base;
}

void lv_vg_lite_path_set_bonding_box(lv_vg_lite_path_t * path,
                                     float min_x, float min_y,
                                     float max_x, float max_y)
{
    LV_ASSERT_NULL(path);
    path->base.bounding_box[0] = min_x;
    path->base.bounding_box[1] = min_y;
    path->base.bounding_box[2] = max_x;
    path->base.bounding_box[3] = max_y;
}

void lv_vg_lite_path_set_bonding_box_area(lv_vg_lite_path_t * path, const lv_area_t * area)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(area);
    lv_vg_lite_path_set_bonding_box(path, area->x1, area->y1, area->x2 + 1, area->y2 + 1);
}

void lv_vg_lite_path_get_bonding_box(lv_vg_lite_path_t * path,
                                     float * min_x, float * min_y,
                                     float * max_x, float * max_y)
{
    LV_ASSERT_NULL(path);
    if(min_x) *min_x = path->base.bounding_box[0];
    if(min_y) *min_y = path->base.bounding_box[1];
    if(max_x) *max_x = path->base.bounding_box[2];
    if(max_y) *max_y = path->base.bounding_box[3];
}

static void path_bounds_iter_cb(void * user_data, uint8_t op_code, const float * data, uint32_t len)
{
    LV_UNUSED(op_code);

    if(len == 0) {
        return;
    }

    typedef struct {
        float x;
        float y;
    } point_t;

    const int pt_len = sizeof(point_t) / sizeof(float);

    LV_ASSERT(len % pt_len == 0);

    const point_t * pt = (point_t *)data;
    len /= pt_len;

    lv_vg_lite_path_bounds_t * bounds = user_data;

    for(uint32_t i = 0; i < len; i++) {
        if(pt[i].x < bounds->min_x) bounds->min_x = pt[i].x;
        if(pt[i].y < bounds->min_y) bounds->min_y = pt[i].y;
        if(pt[i].x > bounds->max_x) bounds->max_x = pt[i].x;
        if(pt[i].y > bounds->max_y) bounds->max_y = pt[i].y;
    }
}

bool lv_vg_lite_path_update_bonding_box(lv_vg_lite_path_t * path)
{
    LV_ASSERT_NULL(path);

    if(!path->format_len) {
        return false;
    }

    LV_PROFILER_BEGIN;

    lv_vg_lite_path_bounds_t bounds;

    /* init bounds */
    bounds.min_x = FLT_MAX;
    bounds.min_y = FLT_MAX;
    bounds.max_x = FLT_MIN;
    bounds.max_y = FLT_MIN;

    /* calc bounds */
    lv_vg_lite_path_for_each_data(lv_vg_lite_path_get_path(path), path_bounds_iter_cb, &bounds);

    /* set bounds */
    lv_vg_lite_path_set_bonding_box(path, bounds.min_x, bounds.min_y, bounds.max_x, bounds.max_y);

    LV_PROFILER_END;

    return true;
}

void lv_vg_lite_path_set_quality(lv_vg_lite_path_t * path, vg_lite_quality_t quality)
{
    LV_ASSERT_NULL(path);
    path->base.quality = quality;
}

static void lv_vg_lite_path_append_data(lv_vg_lite_path_t * path, const void * data, size_t len)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(data);

    if(path->base.path_length + len > path->mem_size) {
        if(path->mem_size == 0) {
            path->mem_size = len;
        }
        else {
            path->mem_size *= 2;
        }
        path->base.path = lv_realloc(path->base.path, path->mem_size);
        LV_ASSERT_MALLOC(path->base.path);
    }

    lv_memcpy((uint8_t *)path->base.path + path->base.path_length, data, len);
    path->base.path_length += len;
}

static void lv_vg_lite_path_append_op(lv_vg_lite_path_t * path, uint32_t op)
{
    lv_vg_lite_path_append_data(path, &op, path->format_len);
}

static void lv_vg_lite_path_append_point(lv_vg_lite_path_t * path, float x, float y)
{
    if(path->base.format == VG_LITE_FP32) {
        lv_vg_lite_path_append_data(path, &x, sizeof(x));
        lv_vg_lite_path_append_data(path, &y, sizeof(y));
        return;
    }

    int32_t ix = (int32_t)(x);
    int32_t iy = (int32_t)(y);
    lv_vg_lite_path_append_data(path, &ix, path->format_len);
    lv_vg_lite_path_append_data(path, &iy, path->format_len);
}

void lv_vg_lite_path_move_to(lv_vg_lite_path_t * path,
                             float x, float y)
{
    LV_ASSERT_NULL(path);
    lv_vg_lite_path_append_op(path, VLC_OP_MOVE);
    lv_vg_lite_path_append_point(path, x, y);
}

void lv_vg_lite_path_line_to(lv_vg_lite_path_t * path,
                             float x, float y)
{
    LV_ASSERT_NULL(path);
    lv_vg_lite_path_append_op(path, VLC_OP_LINE);
    lv_vg_lite_path_append_point(path, x, y);
}

void lv_vg_lite_path_quad_to(lv_vg_lite_path_t * path,
                             float cx, float cy,
                             float x, float y)
{
    LV_ASSERT_NULL(path);
    lv_vg_lite_path_append_op(path, VLC_OP_QUAD);
    lv_vg_lite_path_append_point(path, cx, cy);
    lv_vg_lite_path_append_point(path, x, y);
}

void lv_vg_lite_path_cubic_to(lv_vg_lite_path_t * path,
                              float cx1, float cy1,
                              float cx2, float cy2,
                              float x, float y)
{
    LV_ASSERT_NULL(path);
    lv_vg_lite_path_append_op(path, VLC_OP_CUBIC);
    lv_vg_lite_path_append_point(path, cx1, cy1);
    lv_vg_lite_path_append_point(path, cx2, cy2);
    lv_vg_lite_path_append_point(path, x, y);
}

void lv_vg_lite_path_close(lv_vg_lite_path_t * path)
{
    LV_ASSERT_NULL(path);
    lv_vg_lite_path_append_op(path, VLC_OP_CLOSE);
}

void lv_vg_lite_path_end(lv_vg_lite_path_t * path)
{
    LV_ASSERT_NULL(path);
    lv_vg_lite_path_append_op(path, VLC_OP_END);
    path->base.add_end = 1;
}

void lv_vg_lite_path_append_rect(
    lv_vg_lite_path_t * path,
    float x, float y,
    float w, float h,
    float r)
{
    LV_PROFILER_BEGIN;
    const float half_w = w / 2.0f;
    const float half_h = h / 2.0f;

    /*clamping cornerRadius by minimum size*/
    const float r_max = LV_MIN(half_w, half_h);
    if(r > r_max)
        r = r_max;

    /*rectangle*/
    if(r <= 0) {
        lv_vg_lite_path_move_to(path, x, y);
        lv_vg_lite_path_line_to(path, x + w, y);
        lv_vg_lite_path_line_to(path, x + w, y + h);
        lv_vg_lite_path_line_to(path, x, y + h);
        lv_vg_lite_path_close(path);
        LV_PROFILER_END;
        return;
    }

    /*circle*/
    if(math_equal(r, half_w) && math_equal(r, half_h)) {
        lv_vg_lite_path_append_circle(path, x + half_w, y + half_h, r, r);
        LV_PROFILER_END;
        return;
    }

    /* Get the control point offset for rounded cases */
    const float offset = r * PATH_ARC_MAGIC;

    /* Rounded rectangle case */
    /* Starting point */
    lv_vg_lite_path_move_to(path, x + r, y);

    /* Top side */
    lv_vg_lite_path_line_to(path, x + w - r, y);

    /* Top-right corner */
    lv_vg_lite_path_cubic_to(path, x + w - r + offset, y, x + w, y + r - offset, x + w, y + r);

    /* Right side */
    lv_vg_lite_path_line_to(path, x + w, y + h - r);

    /* Bottom-right corner*/
    lv_vg_lite_path_cubic_to(path, x + w, y + h - r + offset, x + w - r + offset, y + h, x + w - r, y + h);

    /* Bottom side */
    lv_vg_lite_path_line_to(path, x + r, y + h);

    /* Bottom-left corner */
    lv_vg_lite_path_cubic_to(path, x + r - offset, y + h, x, y + h - r + offset, x, y + h - r);

    /* Left side*/
    lv_vg_lite_path_line_to(path, x, y + r);

    /* Top-left corner */
    lv_vg_lite_path_cubic_to(path, x, y + r - offset, x + r - offset, y, x + r, y);

    /* Ending point */
    lv_vg_lite_path_close(path);
    LV_PROFILER_END;
}

void lv_vg_lite_path_append_circle(
    lv_vg_lite_path_t * path,
    float cx, float cy,
    float rx, float ry)
{
    LV_PROFILER_BEGIN;
    /* https://learn.microsoft.com/zh-cn/xamarin/xamarin-forms/user-interface/graphics/skiasharp/curves/beziers */
    float rx_kappa = rx * PATH_KAPPA;
    float ry_kappa = ry * PATH_KAPPA;

    lv_vg_lite_path_move_to(path, cx, cy - ry);
    lv_vg_lite_path_cubic_to(path, cx + rx_kappa, cy - ry, cx + rx, cy - ry_kappa, cx + rx, cy);
    lv_vg_lite_path_cubic_to(path, cx + rx, cy + ry_kappa, cx + rx_kappa, cy + ry, cx, cy + ry);
    lv_vg_lite_path_cubic_to(path, cx - rx_kappa, cy + ry, cx - rx, cy + ry_kappa, cx - rx, cy);
    lv_vg_lite_path_cubic_to(path, cx - rx, cy - ry_kappa, cx - rx_kappa, cy - ry, cx, cy - ry);
    lv_vg_lite_path_close(path);
    LV_PROFILER_END;
}

void lv_vg_lite_path_append_arc_right_angle(lv_vg_lite_path_t * path,
                                            float start_x, float start_y,
                                            float center_x, float center_y,
                                            float end_x, float end_y)
{
    LV_PROFILER_BEGIN;
    float dx1 = center_x - start_x;
    float dy1 = center_y - start_y;
    float dx2 = end_x - center_x;
    float dy2 = end_y - center_y;

    float c = SIGN(dx1 * dy2 - dx2 * dy1) * PATH_ARC_MAGIC;

    lv_vg_lite_path_cubic_to(path,
                             start_x - c * dy1, start_y + c * dx1,
                             end_x - c * dy2, end_y + c * dx2,
                             end_x, end_y);
    LV_PROFILER_END;
}

void lv_vg_lite_path_append_arc(lv_vg_lite_path_t * path,
                                float cx, float cy,
                                float radius,
                                float start_angle,
                                float sweep,
                                bool pie)
{
    LV_PROFILER_BEGIN;
    /* just circle */
    if(sweep >= 360.0f || sweep <= -360.0f) {
        lv_vg_lite_path_append_circle(path, cx, cy, radius, radius);
        LV_PROFILER_END;
        return;
    }

    start_angle = MATH_RADIANS(start_angle);
    sweep = MATH_RADIANS(sweep);

    int n_curves = (int)ceil(MATH_FABSF(sweep / MATH_HALF_PI));
    float sweep_sign = sweep < 0 ? -1.f : 1.f;
    float fract = fmodf(sweep, MATH_HALF_PI);
    fract = (math_zero(fract)) ? MATH_HALF_PI * sweep_sign : fract;

    /* Start from here */
    float start_x = radius * MATH_COSF(start_angle);
    float start_y = radius * MATH_SINF(start_angle);

    if(pie) {
        lv_vg_lite_path_move_to(path, cx, cy);
        lv_vg_lite_path_line_to(path, start_x + cx, start_y + cy);
    }

    for(int i = 0; i < n_curves; ++i) {
        float end_angle = start_angle + ((i != n_curves - 1) ? MATH_HALF_PI * sweep_sign : fract);
        float end_x = radius * MATH_COSF(end_angle);
        float end_y = radius * MATH_SINF(end_angle);

        /* variables needed to calculate bezier control points */

        /** get bezier control points using article:
         * (http://itc.ktu.lt/index.php/ITC/article/view/11812/6479)
         */
        float ax = start_x;
        float ay = start_y;
        float bx = end_x;
        float by = end_y;
        float q1 = ax * ax + ay * ay;
        float q2 = ax * bx + ay * by + q1;
        float k2 = (4.0f / 3.0f) * ((MATH_SQRTF(2 * q1 * q2) - q2) / (ax * by - ay * bx));

        /* Next start point is the current end point */
        start_x = end_x;
        start_y = end_y;

        end_x += cx;
        end_y += cy;

        float ctrl1_x = ax - k2 * ay + cx;
        float ctrl1_y = ay + k2 * ax + cy;
        float ctrl2_x = bx + k2 * by + cx;
        float ctrl2_y = by - k2 * bx + cy;

        lv_vg_lite_path_cubic_to(path, ctrl1_x, ctrl1_y, ctrl2_x, ctrl2_y, end_x, end_y);
        start_angle = end_angle;
    }

    if(pie) {
        lv_vg_lite_path_close(path);
    }

    LV_PROFILER_END;
}

uint8_t lv_vg_lite_vlc_op_arg_len(uint8_t vlc_op)
{
    switch(vlc_op) {
            VLC_OP_ARG_LEN(END, 0);
            VLC_OP_ARG_LEN(CLOSE, 0);
            VLC_OP_ARG_LEN(MOVE, 2);
            VLC_OP_ARG_LEN(MOVE_REL, 2);
            VLC_OP_ARG_LEN(LINE, 2);
            VLC_OP_ARG_LEN(LINE_REL, 2);
            VLC_OP_ARG_LEN(QUAD, 4);
            VLC_OP_ARG_LEN(QUAD_REL, 4);
            VLC_OP_ARG_LEN(CUBIC, 6);
            VLC_OP_ARG_LEN(CUBIC_REL, 6);
            VLC_OP_ARG_LEN(SCCWARC, 5);
            VLC_OP_ARG_LEN(SCCWARC_REL, 5);
            VLC_OP_ARG_LEN(SCWARC, 5);
            VLC_OP_ARG_LEN(SCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCCWARC, 5);
            VLC_OP_ARG_LEN(LCCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCWARC, 5);
            VLC_OP_ARG_LEN(LCWARC_REL, 5);
        default:
            break;
    }

    LV_LOG_ERROR("UNKNOW_VLC_OP: 0x%x", vlc_op);
    LV_ASSERT(false);
    return 0;
}

uint8_t lv_vg_lite_path_format_len(vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return 1;
        case VG_LITE_S16:
            return 2;
        case VG_LITE_S32:
            return 4;
        case VG_LITE_FP32:
            return 4;
        default:
            break;
    }

    LV_LOG_ERROR("UNKNOW_FORMAT: %d", format);
    LV_ASSERT(false);
    return 0;
}

void lv_vg_lite_path_for_each_data(const vg_lite_path_t * path, lv_vg_lite_path_iter_cb_t cb, void * user_data)
{
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(cb);

    uint8_t fmt_len = lv_vg_lite_path_format_len(path->format);
    uint8_t * cur = path->path;
    uint8_t * end = cur + path->path_length;
    float tmp_data[8];

    while(cur < end) {
        /* get op code */
        uint8_t op_code = VLC_GET_OP_CODE(cur);

        /* get arguments length */
        uint8_t arg_len = lv_vg_lite_vlc_op_arg_len(op_code);

        /* skip op code */
        cur += fmt_len;

        /* print arguments */
        for(uint8_t i = 0; i < arg_len; i++) {
            switch(path->format) {
                case VG_LITE_S8:
                    tmp_data[i] = *((int8_t *)cur);
                    break;
                case VG_LITE_S16:
                    tmp_data[i] = *((int16_t *)cur);
                    break;
                case VG_LITE_S32:
                    tmp_data[i] = *((int32_t *)cur);
                    break;
                case VG_LITE_FP32:
                    tmp_data[i] = *((float *)cur);
                    break;
                default:
                    LV_LOG_ERROR("UNKNOW_FORMAT(%d)", path->format);
                    LV_ASSERT(false);
                    break;
            }

            cur += fmt_len;
        }

        cb(user_data, op_code, tmp_data, arg_len);
    }
}

void lv_vg_lite_path_append_path(lv_vg_lite_path_t * dest, const lv_vg_lite_path_t * src)
{
    LV_ASSERT_NULL(dest);
    LV_ASSERT_NULL(src);

    LV_ASSERT(dest->base.format == dest->base.format);
    lv_vg_lite_path_append_data(dest, src->base.path, src->base.path_length);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
