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

/*********************
 *      DEFINES
 *********************/

#define PATH_KAPPA 0.552284f
#define PATH_MAX_CNT 32

/* Magic number from https://spencermortensen.com/articles/bezier-circle/ */
#define PATH_ARC_MAGIC 0.55191502449351f

#define SIGN(x) (math_zero(x) ? 0 : ((x) > 0 ? 1 : -1))

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_vg_lite_path_t {
    vg_lite_path_t base;
    size_t mem_size;
    uint8_t format_len;
};

typedef struct _lv_vg_lite_path_t * lv_vg_lite_path_ref_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_vg_lite_path_t * path_create(vg_lite_format_t data_format);
static void path_destroy(lv_vg_lite_path_t * path);
static float arc_get_angle(float ux, float uy, float vx, float vy);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_path_init(struct _lv_draw_vg_lite_unit_t * unit)
{
    LV_ASSERT_NULL(unit);
    _lv_ll_init(&unit->path_free_ll, sizeof(lv_vg_lite_path_ref_t));
}

void lv_vg_lite_path_deinit(struct _lv_draw_vg_lite_unit_t * unit)
{
    LV_ASSERT_NULL(unit);

    lv_ll_t * ll_p = &unit->path_free_ll;
    lv_vg_lite_path_ref_t * path_ref;

    _LV_LL_READ(ll_p, path_ref) {
        path_destroy(*path_ref);
    }

    _lv_ll_clear(ll_p);
}

lv_vg_lite_path_t * lv_vg_lite_path_get(struct _lv_draw_vg_lite_unit_t * unit, vg_lite_format_t data_format)
{
    LV_ASSERT_NULL(unit);

    unit->path_max_cnt++;
    LV_ASSERT(unit->path_max_cnt < PATH_MAX_CNT);

    lv_ll_t * ll_p = &unit->path_free_ll;

    lv_vg_lite_path_ref_t * path_ref = _lv_ll_get_head(ll_p);
    if(path_ref) {
        lv_vg_lite_path_t * path = *path_ref;
        lv_vg_lite_path_reset(path, data_format);
        _lv_ll_remove(ll_p, path_ref);
        lv_free(path_ref);

        return path;
    }

    return path_create(data_format);
}

void lv_vg_lite_path_drop(struct _lv_draw_vg_lite_unit_t * unit, lv_vg_lite_path_t * path)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(path);

    unit->path_max_cnt--;
    LV_ASSERT(unit->path_max_cnt >= 0);

    lv_ll_t * ll_p = &unit->path_free_ll;

    uint32_t len = _lv_ll_get_len(ll_p);
    if(len >= PATH_MAX_CNT) {
        lv_vg_lite_path_ref_t * tail = _lv_ll_get_tail(ll_p);
        path_destroy(*tail);
        _lv_ll_remove(ll_p, tail);
        lv_free(tail);
    }

    lv_vg_lite_path_ref_t * head = _lv_ll_ins_head(ll_p);
    LV_ASSERT_MALLOC(head);
    *head = path;
}

void lv_vg_lite_path_reset(lv_vg_lite_path_t * path, vg_lite_format_t data_format)
{
    LV_ASSERT_NULL(path);
    lv_memzero(path->base.path, path->mem_size);
    path->base.path_length = 0;
    path->base.format = data_format;
    path->base.quality = VG_LITE_MEDIUM;
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
}

void lv_vg_lite_path_append_rect(
    lv_vg_lite_path_t * path,
    float x, float y,
    float w, float h,
    float rx, float ry)
{
    const float half_w = w * 0.5f;
    const float half_h = h * 0.5f;

    /*clamping cornerRadius by minimum size*/
    if(rx > half_w)
        rx = half_w;
    if(ry > half_h)
        ry = half_h;

    /*rectangle*/
    if(rx == 0 && ry == 0) {
        lv_vg_lite_path_move_to(path, x, y);
        lv_vg_lite_path_line_to(path, x + w, y);
        lv_vg_lite_path_line_to(path, x + w, y + h);
        lv_vg_lite_path_line_to(path, x, y + h);
        lv_vg_lite_path_close(path);
        return;
    }

    /*circle*/
    if(math_equal(rx, half_w) && math_equal(ry, half_h)) {
        return lv_vg_lite_path_append_circle(path, x + (w * 0.5f), y + (h * 0.5f), rx, ry);
    }

    /*rounded rectangle*/
    float hrx = rx * 0.5f;
    float hry = ry * 0.5f;
    lv_vg_lite_path_move_to(path, x + rx, y);
    lv_vg_lite_path_line_to(path, x + w - rx, y);
    lv_vg_lite_path_cubic_to(path, x + w - rx + hrx, y, x + w, y + ry - hry, x + w, y + ry);
    lv_vg_lite_path_line_to(path, x + w, y + h - ry);
    lv_vg_lite_path_cubic_to(path, x + w, y + h - ry + hry, x + w - rx + hrx, y + h, x + w - rx, y + h);
    lv_vg_lite_path_line_to(path, x + rx, y + h);
    lv_vg_lite_path_cubic_to(path, x + rx - hrx, y + h, x, y + h - ry + hry, x, y + h - ry);
    lv_vg_lite_path_line_to(path, x, y + ry);
    lv_vg_lite_path_cubic_to(path, x, y + ry - hry, x + rx - hrx, y, x + rx, y);
    lv_vg_lite_path_close(path);
}

void lv_vg_lite_path_append_circle(
    lv_vg_lite_path_t * path,
    float cx, float cy,
    float rx, float ry)
{
    /* https://learn.microsoft.com/zh-cn/xamarin/xamarin-forms/user-interface/graphics/skiasharp/curves/beziers */
    float rx_kappa = rx * PATH_KAPPA;
    float ry_kappa = ry * PATH_KAPPA;

    lv_vg_lite_path_move_to(path, cx, cy - ry);
    lv_vg_lite_path_cubic_to(path, cx + rx_kappa, cy - ry, cx + rx, cy - ry_kappa, cx + rx, cy);
    lv_vg_lite_path_cubic_to(path, cx + rx, cy + ry_kappa, cx + rx_kappa, cy + ry, cx, cy + ry);
    lv_vg_lite_path_cubic_to(path, cx - rx_kappa, cy + ry, cx - rx, cy + ry_kappa, cx - rx, cy);
    lv_vg_lite_path_cubic_to(path, cx - rx, cy - ry_kappa, cx - rx_kappa, cy - ry, cx, cy - ry);
    lv_vg_lite_path_close(path);
}

void lv_vg_lite_path_append_arc_right_angle(lv_vg_lite_path_t * path,
                                            float start_x, float start_y,
                                            float center_x, float center_y,
                                            float end_x, float end_y)
{
    float dx1 = center_x - start_x;
    float dy1 = center_y - start_y;
    float dx2 = end_x - center_x;
    float dy2 = end_y - center_y;

    float c = SIGN(dx1 * dy2 - dx2 * dy1) * PATH_ARC_MAGIC;

    lv_vg_lite_path_cubic_to(path,
                             start_x - c * dy1, start_y + c * dx1,
                             end_x - c * dy2, end_y + c * dx2,
                             end_x, end_y);
}

void lv_vg_lite_path_append_arc_acute_angle(lv_vg_lite_path_t * path,
                                            float start_x, float start_y,
                                            float center_x, float center_y,
                                            float end_x, float end_y)
{
    float dx1 = center_x - start_x;
    float dy1 = center_y - start_y;
    float dx2 = end_x - center_x;
    float dy2 = end_y - center_y;

    float theta = arc_get_angle(dx1, dy1, dx2, dy2);
    float c = 1.3333333f * MATH_TANF(theta * 0.25f);
    lv_vg_lite_path_cubic_to(path,
                             start_x - c * dy1, start_y + c * dx1,
                             end_x - c * dy2, end_y + c * dx2,
                             end_x, end_y);

}

void lv_vg_lite_path_append_arc_round(lv_vg_lite_path_t * path,
                                      float cx, float cy,
                                      float start_angle, float end_angle,
                                      float radius,
                                      float width,
                                      bool rounded)
{
    float angle = end_angle - start_angle;

    if(math_zero(angle)) {
        lv_vg_lite_path_append_arc_right_angle(
            path,
            cx + radius, cy,
            cx, cy,
            cx, cy + radius);
        lv_vg_lite_path_append_arc_right_angle(
            path,
            cx, cy + radius,
            cx, cy,
            cx - radius, cy);
        lv_vg_lite_path_append_arc_right_angle(
            path,
            cx - radius, cy,
            cx, cy,
            cx, cy - radius);
        lv_vg_lite_path_append_arc_right_angle(
            path,
            cx, cy - radius,
            cx, cy,
            cx + radius, cy);
        lv_vg_lite_path_close(path);

        if(width - radius < 0) {
            float inner_radius = radius - width;
            lv_vg_lite_path_append_arc_right_angle(
                path,
                cx + inner_radius, cy,
                cx, cy,
                cx, cy + inner_radius);
            lv_vg_lite_path_append_arc_right_angle(
                path,
                cx, cy + inner_radius,
                cx, cy,
                cx - inner_radius, cy);
            lv_vg_lite_path_append_arc_right_angle(
                path,
                cx - inner_radius, cy,
                cx, cy,
                cx, cy - inner_radius);
            lv_vg_lite_path_append_arc_right_angle(
                path,
                cx, cy - inner_radius,
                cx, cy,
                cx + inner_radius, cy);
            lv_vg_lite_path_close(path);
        }
        return;
    }

    float st_sin = MATH_SINF(start_angle);
    float st_cos = MATH_COSF(start_angle);
    float ed_sin = MATH_SINF(end_angle);
    float ed_cos = MATH_COSF(end_angle);

    width = LV_MIN(width, radius);
    // float half_width = width / 2;
    if(rounded) {
        lv_vg_lite_path_append_arc_right_angle(
            path,
            cx + radius * st_cos, cy + radius * st_sin,
            cx, cy,
            cx + radius * ed_cos, cy + radius * ed_sin);
    }
    else {
        lv_vg_lite_path_line_to(path, cx + ed_sin * radius, cy - ed_cos * radius);
    }
}

void lv_vg_lite_path_append_arc(lv_vg_lite_path_t * path,
                                float cx, float cy,
                                float radius,
                                float start_angle,
                                float sweep,
                                bool pie)
{
    /* just circle */
    if(sweep >= 360.0f || sweep <= -360.0f) {
        return lv_vg_lite_path_append_circle(path, cx, cy, radius, radius);
    }

    start_angle = (start_angle * MATH_PI) / 180.0f;
    sweep = sweep * MATH_PI / 180.0f;

    int n_curves = ceil(MATH_FABSF(sweep / MATH_HALF_PI));
    int sweep_sign = (sweep < 0 ? -1 : 1);
    float fract = fmodf(sweep, MATH_HALF_PI);
    fract = (math_zero(fract)) ? MATH_HALF_PI * sweep_sign : fract;

    /* Start from here */
    float start_x = radius * MATH_COSF(start_angle);
    float start_y = radius * MATH_SINF(start_angle);

    if(pie) {
        lv_vg_lite_path_move_to(path, cx, cy);
        lv_vg_lite_path_line_to(path, start_x + cx, start_y + cy);
    }
    else {
        lv_vg_lite_path_move_to(path, start_x + cx, start_y + cy);
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
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_vg_lite_path_t * path_create(vg_lite_format_t data_format)
{
    lv_vg_lite_path_t * path = lv_malloc(sizeof(lv_vg_lite_path_t));
    LV_ASSERT_MALLOC(path);
    lv_memzero(path, sizeof(lv_vg_lite_path_t));
    path->format_len = lv_vg_lite_path_format_len(data_format);
    LV_ASSERT(vg_lite_init_path(
                  &path->base,
                  data_format,
                  VG_LITE_MEDIUM,
                  0,
                  NULL,
                  0, 0, 0, 0)
              == VG_LITE_SUCCESS);
    return path;
}

static void path_destroy(lv_vg_lite_path_t * path)
{
    LV_ASSERT_NULL(path);
    if(path->base.path != NULL) {
        lv_free(path->base.path);
        path->base.path = NULL;
    }
    lv_free(path);
}

static float arc_get_angle(float ux, float uy, float vx, float vy)
{
    float det = ux * vy - uy * vx;
    float norm2 = (ux * ux + uy * uy) * (vx * vx + vy * vy);
    float angle = MATH_ASINF(det * math_fast_inv_sqrtf(norm2));
    return angle;
}

#endif /*LV_USE_DRAW_VG_LITE*/
