/**
 * @file lv_draw_vg_lite_vector.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_pending.h"
#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_grad.h"
#include "lv_vg_lite_stroke.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_vector_private.h"
#include <float.h>
#include <math.h>

/*********************
 *      DEFINES
 *********************/

#define OPA_MIX(opa1, opa2) LV_UDIV255((opa1) * (opa2))

/**********************
 *      TYPEDEFS
 **********************/

typedef void * path_drop_data_t;
typedef void (*path_drop_func_t)(struct _lv_draw_vg_lite_unit_t *, path_drop_data_t);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_draw_dsc_t * dsc);
static void lv_path_to_vg(lv_vg_lite_path_t * dest, const lv_vector_path_t * src, lv_fpoint_t * offset,
                          float expand_bound);
static vg_lite_blend_t lv_blend_to_vg(lv_vector_blend_t blend);
static vg_lite_fill_t lv_fill_to_vg(lv_vector_fill_t fill_rule);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vg_lite_vector(lv_draw_task_t * t, const lv_draw_vector_task_dsc_t * dsc)
{
    if(dsc->task_list == NULL)
        return;

    lv_layer_t * layer = dsc->base.layer;
    if(layer->draw_buf == NULL)
        return;

    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    LV_PROFILER_DRAW_BEGIN;
    lv_vector_for_each_destroy_tasks(dsc->task_list, task_draw_cb, u);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static vg_lite_color_t lv_color32_to_vg(lv_color32_t color, lv_opa_t opa)
{
    uint8_t a = LV_OPA_MIX2(color.alpha, opa);
    if(a < LV_OPA_COVER) {
        color.red = LV_UDIV255(color.red * a);
        color.green = LV_UDIV255(color.green * a);
        color.blue = LV_UDIV255(color.blue * a);
    }
    return (uint32_t)a << 24 | (uint32_t)color.blue << 16 | (uint32_t)color.green << 8 | color.red;
}

static void draw_fill(lv_draw_vg_lite_unit_t * u,
                      lv_vg_lite_path_t * lv_vg_path,
                      const lv_vector_draw_dsc_t * dsc,
                      vg_lite_matrix_t * matrix,
                      const lv_fpoint_t * offset,
                      const lv_opa_t opa)
{
    LV_PROFILER_DRAW_BEGIN;

    const vg_lite_color_t vg_color = lv_color32_to_vg(dsc->fill_dsc.color, OPA_MIX(dsc->fill_dsc.opa, opa));
    const vg_lite_blend_t blend = lv_blend_to_vg(dsc->blend_mode);
    const vg_lite_fill_t fill = lv_fill_to_vg(dsc->fill_dsc.fill_rule);

    /* If it is fill mode, the end op code should be added */
    lv_vg_lite_path_end(lv_vg_path);

    vg_lite_path_t * vg_path = lv_vg_lite_path_get_path(lv_vg_path);
    LV_VG_LITE_ASSERT_PATH(vg_path);

    switch(dsc->fill_dsc.style) {
        case LV_VECTOR_DRAW_STYLE_SOLID: {
                /* normal draw shape */
                lv_vg_lite_draw(
                    &u->target_buffer,
                    vg_path,
                    fill,
                    matrix,
                    blend,
                    vg_color);
            }
            break;
        case LV_VECTOR_DRAW_STYLE_PATTERN: {
                /* draw image */
                vg_lite_buffer_t image_buffer;
                lv_image_decoder_dsc_t decoder_dsc;
                if(lv_vg_lite_buffer_open_image(&image_buffer, &decoder_dsc, dsc->fill_dsc.img_dsc.src, false, true)) {
                    /* Calculate pattern matrix. Should start from path bond box, and also apply fill matrix. */
                    vg_lite_matrix_t pattern_matrix = *matrix;

                    if(dsc->fill_dsc.fill_units == LV_VECTOR_FILL_UNITS_OBJECT_BOUNDING_BOX) {
                        /* Convert to object bounding box coordinates */
                        vg_lite_translate(offset->x, offset->y, &pattern_matrix);
                    }

                    vg_lite_matrix_t fill_matrix;
                    lv_vg_lite_matrix(&fill_matrix, &dsc->fill_dsc.matrix);
                    lv_vg_lite_matrix_multiply(&pattern_matrix, &fill_matrix);

                    const lv_draw_image_dsc_t * img_dsc = &dsc->fill_dsc.img_dsc;
                    lv_draw_image_dsc_t tmp_dsc;
                    if(opa < LV_OPA_COVER) {
                        tmp_dsc = dsc->fill_dsc.img_dsc;
                        tmp_dsc.opa = OPA_MIX(tmp_dsc.opa, opa);
                        img_dsc = &tmp_dsc;
                    }

                    vg_lite_color_t recolor = lv_vg_lite_image_recolor(&image_buffer, img_dsc);

                    lv_vg_lite_draw_pattern(
                        &u->target_buffer,
                        vg_path,
                        fill,
                        matrix,
                        &image_buffer,
                        &pattern_matrix,
                        blend,
                        VG_LITE_PATTERN_COLOR,
                        0,
                        recolor,
                        VG_LITE_FILTER_BI_LINEAR);

                    lv_vg_lite_pending_add(u->image_dsc_pending, &decoder_dsc);
                }
            }
            break;
        case LV_VECTOR_DRAW_STYLE_GRADIENT: {
                vg_lite_matrix_t grad_matrix = *matrix;

#if LV_USE_VG_LITE_THORVG
                /* Workaround inconsistent radial gradient matrix behavior between device and ThorVG */
                if(dsc->fill_dsc.gradient.style == LV_VECTOR_GRADIENT_STYLE_RADIAL) {
                    /* Restore matrix to identity */
                    vg_lite_identity(&grad_matrix);
                }
#endif

                vg_lite_matrix_t fill_matrix;
                lv_vg_lite_matrix(&fill_matrix, &dsc->fill_dsc.matrix);
                lv_vg_lite_matrix_multiply(&grad_matrix, &fill_matrix);

                const lv_vector_gradient_t * gradient = &dsc->fill_dsc.gradient;
                lv_vector_gradient_t tmp_gradient;
                if(opa < LV_OPA_COVER) {
                    tmp_gradient = dsc->fill_dsc.gradient;
                    for(uint16_t i = 0; i < tmp_gradient.stops_count; i++) {
                        tmp_gradient.stops[i].opa = OPA_MIX(tmp_gradient.stops[i].opa, opa);
                    }

                    gradient = &tmp_gradient;
                }

                lv_vg_lite_draw_grad(
                    u->grad_ctx,
                    &u->target_buffer,
                    vg_path,
                    gradient,
                    &grad_matrix,
                    matrix,
                    fill,
                    blend);
            }
            break;
        default:
            LV_LOG_WARN("unsupported style: %d", dsc->fill_dsc.style);
            break;
    }

    LV_PROFILER_DRAW_END;
}

static void draw_stroke(lv_draw_vg_lite_unit_t * u,
                        const lv_vector_path_t * path,
                        lv_vg_lite_path_t * lv_vg_path,
                        const lv_vector_draw_dsc_t * dsc,
                        vg_lite_matrix_t * matrix,
                        const lv_opa_t opa)
{
    LV_PROFILER_DRAW_BEGIN;

    vg_lite_path_t * vg_path = lv_vg_lite_path_get_path(lv_vg_path);

    LV_UNUSED(path);
    lv_cache_entry_t * stroke_cache_entey = lv_vg_lite_stroke_get(u, lv_vg_path, &dsc->stroke_dsc);
    if(!stroke_cache_entey) {
        LV_LOG_ERROR("convert stroke failed");
        LV_PROFILER_DRAW_END;
        return;
    }

    vg_lite_path_t * vg_stroke_path = lv_vg_lite_path_get_path(lv_vg_lite_stroke_get_path(stroke_cache_entey));

    /* set stroke params */
    vg_stroke_path->quality = vg_path->quality;
    vg_stroke_path->stroke_color = lv_color32_to_vg(dsc->stroke_dsc.color, OPA_MIX(dsc->stroke_dsc.opa, opa));
    const vg_lite_color_t vg_color = 0;

    /* set stroke path bounding box */
    lv_memcpy(vg_stroke_path->bounding_box, vg_path->bounding_box, sizeof(vg_path->bounding_box));
    LV_VG_LITE_ASSERT_PATH(vg_stroke_path);

    const vg_lite_blend_t blend = lv_blend_to_vg(dsc->blend_mode);

    switch(dsc->stroke_dsc.style) {
        case LV_VECTOR_DRAW_STYLE_SOLID: {
                /* normal draw shape */
                lv_vg_lite_draw(
                    &u->target_buffer,
                    vg_stroke_path,
                    VG_LITE_FILL_NON_ZERO,
                    matrix,
                    blend,
                    vg_color);
            }
            break;
        default:
            LV_LOG_WARN("unsupported style: %d", dsc->stroke_dsc.style);
            break;
    }

    lv_vg_lite_stroke_drop(u, stroke_cache_entey);
    LV_PROFILER_DRAW_END;
}

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_draw_dsc_t * dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_vg_lite_unit_t * u = ctx;
    LV_VG_LITE_ASSERT_DEST_BUFFER(&u->target_buffer);

    vg_lite_matrix_t matrix = u->global_matrix;

    const lv_area_t scissor_area = lv_matrix_is_identity((lv_matrix_t *)&matrix)
                                   ? dsc->scissor_area
                                   : lv_matrix_transform_area((lv_matrix_t *)&matrix, &dsc->scissor_area);

    /* clear area */
    if(!path) {
        vg_lite_color_t c = lv_color32_to_vg(dsc->fill_dsc.color, OPA_MIX(dsc->fill_dsc.opa, u->task_act->opa));
        vg_lite_rectangle_t rect;
        lv_vg_lite_rect(&rect, &scissor_area);
        LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_clear");
        LV_VG_LITE_CHECK_ERROR(vg_lite_clear(&u->target_buffer, &rect, c), {
            lv_vg_lite_buffer_dump_info(&u->target_buffer);
            LV_LOG_ERROR("rect: X%d Y%d W%d H%d", rect.x, rect.y, rect.width, rect.height);
            lv_vg_lite_color_dump_info(c);
        });
        LV_PROFILER_DRAW_END_TAG("vg_lite_clear");
        LV_PROFILER_DRAW_END;
        return;
    }

    if(dsc->fill_dsc.opa == LV_OPA_TRANSP && dsc->stroke_dsc.opa == LV_OPA_TRANSP) {
        LV_LOG_TRACE("Full transparent, no need to draw");
        LV_PROFILER_DRAW_END;
        return;
    }

    /* transform matrix */
    vg_lite_matrix_t dsc_matrix;
    lv_vg_lite_matrix(&dsc_matrix, &dsc->matrix);
    lv_vg_lite_matrix_multiply(&matrix, &dsc_matrix);
    LV_VG_LITE_ASSERT_MATRIX(&matrix);

    /* convert path */
    lv_vg_lite_path_t * lv_vg_path = lv_vg_lite_path_get(u, VG_LITE_FP32);

    lv_fpoint_t offset = {0, 0};
    lv_path_to_vg(lv_vg_path, path, &offset, dsc->stroke_dsc.opa ? dsc->stroke_dsc.width : 0);

    if(vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR)) {
        /* set scissor area */
        lv_vg_lite_set_scissor_area(&scissor_area);
        LV_LOG_TRACE("Set scissor area: X1:%" LV_PRId32 ", Y1:%" LV_PRId32 ", X2:%" LV_PRId32 ", Y2:%" LV_PRId32,
                     scissor_area.x1, scissor_area.y1, scissor_area.x2, scissor_area.y2);
    }
    else {
        /* calc inverse matrix */
        vg_lite_matrix_t result;
        if(!lv_vg_lite_matrix_inverse(&result, &matrix)) {
            LV_LOG_ERROR("no inverse matrix");
            lv_vg_lite_matrix_dump_info(&matrix);
            lv_vg_lite_path_drop(u, lv_vg_path);
            LV_PROFILER_DRAW_END;
            return;
        }

        /* Reverse the clip area on the source */
        lv_point_precise_t p1 = { scissor_area.x1, scissor_area.y1 };
        lv_point_precise_t p1_res = lv_vg_lite_matrix_transform_point(&result, &p1);

        /* vg-lite bounding_box will crop the pixels on the edge, so +1px is needed here */
        lv_point_precise_t p2 = { scissor_area.x2 + 1, scissor_area.y2 + 1 };
        lv_point_precise_t p2_res = lv_vg_lite_matrix_transform_point(&result, &p2);

        lv_vg_lite_path_set_bounding_box(lv_vg_path, p1_res.x, p1_res.y, p2_res.x, p2_res.y);
    }

    const lv_opa_t layer_opa = u->task_act->opa;

    if(dsc->fill_dsc.opa) {
        draw_fill(u, lv_vg_path, dsc, &matrix, &offset, layer_opa);
    }

    if(dsc->stroke_dsc.opa) {
        draw_stroke(u, path, lv_vg_path, dsc, &matrix, layer_opa);
    }

    /* drop path */
    lv_vg_lite_path_drop(u, lv_vg_path);

    /* Flush in time to avoid accumulation of drawing commands */
    lv_vg_lite_flush(u);

    LV_PROFILER_DRAW_END;
}

static vg_lite_quality_t lv_quality_to_vg(lv_vector_path_quality_t quality)
{
    switch(quality) {
        case LV_VECTOR_PATH_QUALITY_LOW:
            return VG_LITE_LOW;
        case LV_VECTOR_PATH_QUALITY_MEDIUM:
            return VG_LITE_MEDIUM;
        case LV_VECTOR_PATH_QUALITY_HIGH:
            return VG_LITE_HIGH;
        default:
            return VG_LITE_MEDIUM;
    }
}

static void lv_path_to_vg(lv_vg_lite_path_t * dest, const lv_vector_path_t * src, lv_fpoint_t * offset,
                          float expand_bound)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_vg_lite_path_set_quality(dest, lv_quality_to_vg(src->quality));

    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = FLT_MIN;
    float max_y = FLT_MIN;

#define CMP_BOUNDS(point)                           \
    do {                                            \
        if((point)->x < min_x) min_x = (point)->x;  \
        if((point)->y < min_y) min_y = (point)->y;  \
        if((point)->x > max_x) max_x = (point)->x;  \
        if((point)->y > max_y) max_y = (point)->y;  \
    } while(0)

#define COPY_POINT_NEXT()        \
    do {                         \
        CMP_BOUNDS(point);       \
        *path_data++ = point->x; \
        *path_data++ = point->y; \
        point++;                 \
    } while(0)

    const lv_vector_path_op_t * ops = lv_array_front(&src->ops);
    const lv_fpoint_t * point = lv_array_front(&src->points);
    const uint32_t op_size = lv_array_size(&src->ops);
    const uint32_t point_size = lv_array_size(&src->points);
    const uint32_t path_length = (op_size + point_size * 2) * sizeof(float);

    /* Reserved memory for path data */
    lv_vg_lite_path_reserve_space(dest, path_length);
    vg_lite_path_t * vg_path = lv_vg_lite_path_get_path(dest);
    vg_path->path_length = path_length;
    float * path_data = vg_path->path;

    for(uint32_t i = 0; i < op_size; i++) {
        switch(ops[i]) {
            case LV_VECTOR_PATH_OP_MOVE_TO: {
                    LV_VG_LITE_PATH_SET_OP_CODE(path_data++, uint32_t, VLC_OP_MOVE);
                    COPY_POINT_NEXT();
                }
                break;
            case LV_VECTOR_PATH_OP_LINE_TO: {
                    LV_VG_LITE_PATH_SET_OP_CODE(path_data++, uint32_t, VLC_OP_LINE);
                    COPY_POINT_NEXT();
                }
                break;
            case LV_VECTOR_PATH_OP_QUAD_TO: {
                    LV_VG_LITE_PATH_SET_OP_CODE(path_data++, uint32_t, VLC_OP_QUAD);
                    COPY_POINT_NEXT();
                    COPY_POINT_NEXT();
                }
                break;
            case LV_VECTOR_PATH_OP_CUBIC_TO: {
                    LV_VG_LITE_PATH_SET_OP_CODE(path_data++, uint32_t, VLC_OP_CUBIC);
                    COPY_POINT_NEXT();
                    COPY_POINT_NEXT();
                    COPY_POINT_NEXT();
                }
                break;
            case LV_VECTOR_PATH_OP_CLOSE: {
                    LV_VG_LITE_PATH_SET_OP_CODE(path_data++, uint32_t, VLC_OP_CLOSE);
                }
                break;
            default:
                LV_LOG_WARN("unknown op: %d", ops[i]);
                break;
        }
    }

    LV_ASSERT_MSG((lv_uintptr_t)path_data - (lv_uintptr_t)vg_path->path == path_length, "path length overflow");

    lv_vg_lite_path_set_bounding_box(dest,
                                     min_x - expand_bound,
                                     min_y - expand_bound,
                                     max_x + expand_bound + 1,
                                     max_y + expand_bound + 1);

    offset->x = lroundf(min_x);
    offset->y = lroundf(min_y);
    LV_PROFILER_DRAW_END;
}

static vg_lite_blend_t lv_blend_to_vg(lv_vector_blend_t blend)
{
    switch(blend) {
        case LV_VECTOR_BLEND_SRC_OVER:
            return VG_LITE_BLEND_SRC_OVER;
        case LV_VECTOR_BLEND_SCREEN:
            return VG_LITE_BLEND_SCREEN;
        case LV_VECTOR_BLEND_MULTIPLY:
            return VG_LITE_BLEND_MULTIPLY;
        case LV_VECTOR_BLEND_NONE:
            return VG_LITE_BLEND_NONE;
        case LV_VECTOR_BLEND_ADDITIVE:
            return VG_LITE_BLEND_ADDITIVE;
        case LV_VECTOR_BLEND_SRC_IN:
            return VG_LITE_BLEND_SRC_IN;
        case LV_VECTOR_BLEND_DST_OVER:
            return VG_LITE_BLEND_DST_OVER;
        case LV_VECTOR_BLEND_DST_IN:
            return VG_LITE_BLEND_DST_IN;
        case LV_VECTOR_BLEND_SUBTRACTIVE:
            return VG_LITE_BLEND_SUBTRACT;
        default:
            return VG_LITE_BLEND_SRC_OVER;
    }
}

static vg_lite_fill_t lv_fill_to_vg(lv_vector_fill_t fill_rule)
{
    switch(fill_rule) {
        case LV_VECTOR_FILL_NONZERO:
            return VG_LITE_FILL_NON_ZERO;
        case LV_VECTOR_FILL_EVENODD:
            return VG_LITE_FILL_EVEN_ODD;
        default:
            return VG_LITE_FILL_NON_ZERO;
    }
}

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/

