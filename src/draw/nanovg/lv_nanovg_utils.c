/**
 * @file lv_nanovg_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nanovg_utils.h"

#if LV_USE_DRAW_NANOVG

#include "../../misc/lv_pending.h"
#include "lv_draw_nanovg_private.h"
#include "lv_nanovg_math.h"
#if LV_USE_VECTOR_GRAPHIC
    #include "../lv_draw_vector_private.h"
#endif
#include <float.h>
#include <math.h>

/*********************
*      DEFINES
*********************/

/* Magic number from https://spencermortensen.com/articles/bezier-circle/ */
#define PATH_ARC_MAGIC 0.55191502449351f

#define SIGN(x) (nvg_math_is_zero(x) ? 0 : ((x) > 0 ? 1 : -1))

/**********************
*      TYPEDEFS
**********************/

#if LV_USE_VECTOR_GRAPHIC
/*The number of the segments a curve is split into to get the area of a contour*/
#define CURVE_AREA_STEPS 8

/*Tracks the signed area of the contour that is being appended to the NanoVG path*/
typedef struct {
    lv_fpoint_t first;
    lv_fpoint_t prev;
    float area;
    bool open;
} contour_tracker_t;
#endif /*LV_USE_VECTOR_GRAPHIC*/

/**********************
*  STATIC PROTOTYPES
**********************/

#if LV_USE_VECTOR_GRAPHIC
    static float lv_path_main_area(const lv_vector_path_t * src);
    static void tracker_begin(contour_tracker_t * t, const lv_fpoint_t * p);
    static void tracker_add(contour_tracker_t * t, const lv_fpoint_t * p);
    static void tracker_quad(contour_tracker_t * t, const lv_fpoint_t * c, const lv_fpoint_t * p);
    static void tracker_cubic(contour_tracker_t * t, const lv_fpoint_t * c1, const lv_fpoint_t * c2, const lv_fpoint_t * p);
    static float tracker_end(contour_tracker_t * t);
    static void end_contour(NVGcontext * ctx, contour_tracker_t * t, float main_area);
#endif /*LV_USE_VECTOR_GRAPHIC*/

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_nanovg_utils_init(struct _lv_draw_nanovg_unit_t * u)
{
    LV_UNUSED(u);
}

void lv_nanovg_utils_deinit(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);

    if(u->image_buf) {
        lv_draw_buf_destroy(u->image_buf);
        u->image_buf = NULL;
    }
}

void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(matrix);
    LV_PROFILER_DRAW_BEGIN;

    nvgTransform(ctx,
                 matrix->m[0][0],
                 matrix->m[1][0],
                 matrix->m[0][1],
                 matrix->m[1][1],
                 matrix->m[0][2],
                 matrix->m[1][2]);

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_set_clip_area(NVGcontext * ctx, const lv_area_t * area)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(area);
    LV_PROFILER_DRAW_BEGIN;

    nvgScissor(ctx,
               area->x1, area->y1,
               lv_area_get_width(area), lv_area_get_height(area));

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_rect(NVGcontext * ctx, float x, float y, float w, float h, float r)
{
    LV_ASSERT_NULL(ctx);

    LV_PROFILER_DRAW_BEGIN;

    if(r > 0) {
        const float half_w = w / 2.0f;
        const float half_h = h / 2.0f;

        /*clamping cornerRadius by minimum size*/
        const float r_max = LV_MIN(half_w, half_h);

        nvgRoundedRect(ctx, x, y, w, h, r > r_max ? r_max : r);
        LV_PROFILER_DRAW_END;
        return;
    }

    nvgRect(ctx, x, y, w, h);

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_area(NVGcontext * ctx, const lv_area_t * area)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(area);
    LV_PROFILER_DRAW_BEGIN;

    nvgRect(ctx, area->x1, area->y1, lv_area_get_width(area), lv_area_get_height(area));

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_arc_right_angle(NVGcontext * ctx,
                                           float start_x, float start_y,
                                           float center_x, float center_y,
                                           float end_x, float end_y)
{
    LV_PROFILER_DRAW_BEGIN;
    float dx1 = center_x - start_x;
    float dy1 = center_y - start_y;
    float dx2 = end_x - center_x;
    float dy2 = end_y - center_y;

    float c = SIGN(dx1 * dy2 - dx2 * dy1) * PATH_ARC_MAGIC;

    nvgBezierTo(ctx,
                start_x - c * dy1, start_y + c * dx1,
                end_x - c * dy2, end_y + c * dx2,
                end_x, end_y);
    LV_PROFILER_DRAW_END;
}

void lv_nanovg_path_append_arc(NVGcontext * ctx,
                               float cx, float cy,
                               float radius,
                               float start_angle,
                               float sweep,
                               bool pie)
{
    LV_PROFILER_DRAW_BEGIN;

    if(radius <= 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    /* just circle */
    if(sweep >= 360.0f || sweep <= -360.0f) {
        nvgCircle(ctx, cx, cy, radius);
        LV_PROFILER_DRAW_END;
        return;
    }

    start_angle = NVG_MATH_RADIANS(start_angle);
    sweep = NVG_MATH_RADIANS(sweep);

    int n_curves = (int)ceil(NVG_MATH_FABSF(sweep / NVG_MATH_HALF_PI));
    float sweep_sign = sweep < 0 ? -1.f : 1.f;
    float fract = fmodf(sweep, NVG_MATH_HALF_PI);
    fract = (nvg_math_is_zero(fract)) ? NVG_MATH_HALF_PI * sweep_sign : fract;

    /* Start from here */
    float start_x = radius * NVG_MATH_COSF(start_angle);
    float start_y = radius * NVG_MATH_SINF(start_angle);

    if(pie) {
        nvgMoveTo(ctx, cx, cy);
        nvgLineTo(ctx, start_x + cx, start_y + cy);
    }

    for(int i = 0; i < n_curves; ++i) {
        float end_angle = start_angle + ((i != n_curves - 1) ? NVG_MATH_HALF_PI * sweep_sign : fract);
        float end_x = radius * NVG_MATH_COSF(end_angle);
        float end_y = radius * NVG_MATH_SINF(end_angle);

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
        float k2 = (4.0f / 3.0f) * ((NVG_MATH_SQRTF(2 * q1 * q2) - q2) / (ax * by - ay * bx));

        /* Next start point is the current end point */
        start_x = end_x;
        start_y = end_y;

        end_x += cx;
        end_y += cy;

        float ctrl1_x = ax - k2 * ay + cx;
        float ctrl1_y = ay + k2 * ax + cy;
        float ctrl2_x = bx + k2 * by + cx;
        float ctrl2_y = by - k2 * bx + cy;

        nvgBezierTo(ctx, ctrl1_x, ctrl1_y, ctrl2_x, ctrl2_y, end_x, end_y);
        start_angle = end_angle;
    }

    if(pie) {
        nvgClosePath(ctx);
    }

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_fill(NVGcontext * ctx, enum NVGwinding winding, enum NVGcompositeOperation composite_operation,
                    NVGcolor color)
{
    LV_ASSERT_NULL(ctx);
    LV_PROFILER_DRAW_BEGIN;
    if(winding != LV_NANOVG_WINDING_KEEP) nvgPathWinding(ctx, winding);
    nvgGlobalCompositeOperation(ctx, composite_operation);
    nvgFillColor(ctx, color);
    nvgFill(ctx);
    LV_PROFILER_DRAW_END;
}

void lv_nanovg_end_frame(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_PROFILER_DRAW_BEGIN;

    if(!u->is_started) {
        LV_PROFILER_DRAW_END;
        return;
    }

    LV_PROFILER_DRAW_BEGIN_TAG("nvgEndFrame");
    nvgEndFrame(u->vg);
    LV_PROFILER_DRAW_END_TAG("nvgEndFrame");

    lv_nanovg_clean_up(u);

    LV_PROFILER_DRAW_END;
}

void lv_nanovg_clean_up(struct _lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    LV_PROFILER_DRAW_BEGIN;

    lv_pending_remove_all(u->image_pending);
    lv_pending_remove_all(u->image_uncached_pending);
    lv_pending_remove_all(u->letter_pending);
    u->is_started = false;

    LV_PROFILER_DRAW_END;
}

lv_draw_buf_t * lv_nanovg_reshape_global_image(struct _lv_draw_nanovg_unit_t * u,
                                               lv_color_format_t cf,
                                               uint32_t w,
                                               uint32_t h)
{
    LV_ASSERT_NULL(u);

    uint32_t stride = (w * lv_color_format_get_bpp(cf) + 7) >> 3;
    lv_draw_buf_t * tmp_buf = lv_draw_buf_reshape(u->image_buf, cf, w, h, stride);

    if(!tmp_buf) {
        if(u->image_buf) {
            lv_draw_buf_destroy(u->image_buf);
            u->image_buf = NULL;
        }

        tmp_buf = lv_draw_buf_create(w, h, cf, stride);
        if(!tmp_buf) {
            return NULL;
        }
    }

    u->image_buf = tmp_buf;

    return u->image_buf;
}

#if LV_USE_VECTOR_GRAPHIC

void lv_nanovg_path_append_vector_path(NVGcontext * ctx, const lv_vector_path_t * src, bool nonzero,
                                       lv_fpoint_t * offset)
{
    LV_PROFILER_DRAW_BEGIN;

    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = -FLT_MAX;
    float max_y = -FLT_MAX;

#define CMP_BOUNDS(point)                           \
    do {                                            \
        if((point)->x < min_x) min_x = (point)->x;  \
        if((point)->y < min_y) min_y = (point)->y;  \
        if((point)->x > max_x) max_x = (point)->x;  \
        if((point)->y > max_y) max_y = (point)->y;  \
    } while(0)

    const lv_vector_path_op_t * ops = lv_array_front(&src->ops);
    const lv_fpoint_t * point = lv_array_front(&src->points);
    const uint32_t op_size = lv_array_size(&src->ops);

    /* NanoVG forces every sub-path to the winding set with nvgPathWinding() (counter-clockwise by default)
     * before it fills. A contour that goes in the opposite direction, like the inside of an "O" in a glyph,
     * would be reversed and filled too, but with the non-zero rule it has to be a hole.
     * So set the winding of each contour: the same direction as the biggest contour is solid (counter-clockwise),
     * the opposite direction is a hole (clockwise).
     * The even-odd rule doesn't use it, the caller sets it for the last sub-path. */
    const float main_area = nonzero ? lv_path_main_area(src) : 0.0f;
    contour_tracker_t tracker;
    lv_memzero(&tracker, sizeof(tracker));

    nvgBeginPath(ctx);

    for(uint32_t i = 0; i < op_size; i++) {
        switch(ops[i]) {
            case LV_VECTOR_PATH_OP_MOVE_TO: {
                    if(nonzero) {
                        end_contour(ctx, &tracker, main_area);
                        tracker_begin(&tracker, point);
                    }
                    nvgMoveTo(ctx, point->x, point->y);
                    CMP_BOUNDS(point);
                    point++;
                }
                break;
            case LV_VECTOR_PATH_OP_LINE_TO: {
                    if(nonzero) tracker_add(&tracker, point);
                    nvgLineTo(ctx, point->x, point->y);
                    CMP_BOUNDS(point);
                    point++;
                }
                break;
            case LV_VECTOR_PATH_OP_QUAD_TO: {
                    if(nonzero) tracker_quad(&tracker, &point[0], &point[1]);
                    nvgQuadTo(ctx, point[0].x, point[0].y, point[1].x, point[1].y);
                    CMP_BOUNDS(&point[0]);
                    CMP_BOUNDS(&point[1]);
                    point += 2;
                }
                break;
            case LV_VECTOR_PATH_OP_CUBIC_TO: {
                    if(nonzero) tracker_cubic(&tracker, &point[0], &point[1], &point[2]);
                    nvgBezierTo(ctx, point[0].x, point[0].y, point[1].x, point[1].y, point[2].x, point[2].y);
                    CMP_BOUNDS(&point[0]);
                    CMP_BOUNDS(&point[1]);
                    CMP_BOUNDS(&point[2]);
                    point += 3;
                }
                break;
            case LV_VECTOR_PATH_OP_CLOSE: {
                    nvgClosePath(ctx);
                    if(nonzero) end_contour(ctx, &tracker, main_area);
                }
                break;
            default:
                LV_LOG_WARN("unknown op: %d", ops[i]);
                break;
        }
    }

    if(nonzero) end_contour(ctx, &tracker, main_area);

    if(offset) {
        offset->x = lroundf(min_x);
        offset->y = lroundf(min_y);
    }
    LV_PROFILER_DRAW_END;
}

#endif /*LV_USE_VECTOR_GRAPHIC*/

/**********************
*   STATIC FUNCTIONS
**********************/

#if LV_USE_VECTOR_GRAPHIC

static void tracker_begin(contour_tracker_t * t, const lv_fpoint_t * p)
{
    t->first = *p;
    t->prev = *p;
    t->area = 0.0f;
    t->open = true;
}

static void tracker_add(contour_tracker_t * t, const lv_fpoint_t * p)
{
    if(!t->open) {
        tracker_begin(t, p);
        return;
    }

    t->area += (t->prev.x * p->y - p->x * t->prev.y) * 0.5f;
    t->prev = *p;
}

/*Add a quadratic curve, split into segments so that the area is the one of the curve and not of its control points*/
static void tracker_quad(contour_tracker_t * t, const lv_fpoint_t * c, const lv_fpoint_t * p)
{
    if(!t->open) {
        tracker_begin(t, p);
        return;
    }

    const lv_fpoint_t p0 = t->prev;
    for(int32_t i = 1; i <= CURVE_AREA_STEPS; i++) {
        const float s = (float)i / CURVE_AREA_STEPS;
        const float u = 1.0f - s;
        const lv_fpoint_t pt = {
            u * u * p0.x + 2.0f * u * s * c->x + s * s * p->x,
            u * u * p0.y + 2.0f * u * s * c->y + s * s * p->y,
        };
        tracker_add(t, &pt);
    }
}

/*Add a cubic curve, see tracker_quad()*/
static void tracker_cubic(contour_tracker_t * t, const lv_fpoint_t * c1, const lv_fpoint_t * c2, const lv_fpoint_t * p)
{
    if(!t->open) {
        tracker_begin(t, p);
        return;
    }

    const lv_fpoint_t p0 = t->prev;
    for(int32_t i = 1; i <= CURVE_AREA_STEPS; i++) {
        const float s = (float)i / CURVE_AREA_STEPS;
        const float u = 1.0f - s;
        const float w0 = u * u * u;
        const float w1 = 3.0f * u * u * s;
        const float w2 = 3.0f * u * s * s;
        const float w3 = s * s * s;
        const lv_fpoint_t pt = {
            w0 * p0.x + w1 * c1->x + w2 * c2->x + w3 * p->x,
            w0 * p0.y + w1 * c1->y + w2 * c2->y + w3 * p->y,
        };
        tracker_add(t, &pt);
    }
}

/*Close the contour and return its signed area*/
static float tracker_end(contour_tracker_t * t)
{
    t->area += (t->prev.x * t->first.y - t->first.x * t->prev.y) * 0.5f;
    t->open = false;
    return t->area;
}

/*Set the winding of the contour that was just appended, see lv_path_to_nvg()*/
static void end_contour(NVGcontext * ctx, contour_tracker_t * t, float main_area)
{
    if(!t->open) return;

    const float area = tracker_end(t);
    nvgPathWinding(ctx, area * main_area >= 0.0f ? NVG_CCW : NVG_CW);
}

/*Get the signed area of the contour with the biggest area*/
static float lv_path_main_area(const lv_vector_path_t * src)
{
    const lv_vector_path_op_t * ops = lv_array_front(&src->ops);
    const lv_fpoint_t * point = lv_array_front(&src->points);
    const uint32_t op_size = lv_array_size(&src->ops);

    float main_area = 0.0f;
    contour_tracker_t t;
    lv_memzero(&t, sizeof(t));

    for(uint32_t i = 0; i < op_size; i++) {
        uint32_t point_cnt = 0;
        switch(ops[i]) {
            case LV_VECTOR_PATH_OP_MOVE_TO:
                if(t.open) {
                    const float area = tracker_end(&t);
                    if(LV_ABS(area) > LV_ABS(main_area)) main_area = area;
                }
                tracker_begin(&t, point);
                point_cnt = 1;
                break;
            case LV_VECTOR_PATH_OP_LINE_TO:
                tracker_add(&t, point);
                point_cnt = 1;
                break;
            case LV_VECTOR_PATH_OP_QUAD_TO:
                tracker_quad(&t, &point[0], &point[1]);
                point_cnt = 2;
                break;
            case LV_VECTOR_PATH_OP_CUBIC_TO:
                tracker_cubic(&t, &point[0], &point[1], &point[2]);
                point_cnt = 3;
                break;
            case LV_VECTOR_PATH_OP_CLOSE:
                if(t.open) {
                    const float area = tracker_end(&t);
                    if(LV_ABS(area) > LV_ABS(main_area)) main_area = area;
                }
                break;
            default:
                break;
        }
        point += point_cnt;
    }

    if(t.open) {
        const float area = tracker_end(&t);
        if(LV_ABS(area) > LV_ABS(main_area)) main_area = area;
    }

    return main_area;
}

#endif /*LV_USE_VECTOR_GRAPHIC*/

#endif /* LV_USE_DRAW_NANOVG */
