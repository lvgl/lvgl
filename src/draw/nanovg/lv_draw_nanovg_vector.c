/**
 * @file lv_draw_nanovg_vector.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG && LV_USE_VECTOR_GRAPHIC

#include "lv_nanovg_utils.h"
#include "lv_nanovg_image_cache.h"
#include "../lv_draw_vector_private.h"
#include "../lv_image_decoder_private.h"
#include <float.h>
#include <math.h>

/*********************
*      DEFINES
*********************/

#define OPA_MIX(opa1, opa2) LV_UDIV255((opa1) * (opa2))

/**********************
*      TYPEDEFS
**********************/

/**********************
*  STATIC PROTOTYPES
**********************/

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_path_ctx_t * dsc);
static void lv_path_to_nvg(NVGcontext * ctx, const lv_vector_path_t * src, lv_fpoint_t * offset);
static enum NVGcompositeOperation lv_blend_to_nvg(lv_vector_blend_t blend);
static enum NVGwinding lv_fill_to_nvg(lv_vector_fill_t fill_rule);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

void lv_draw_nanovg_vector(lv_draw_task_t * t, const lv_draw_vector_dsc_t * dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    if(dsc->task_list == NULL) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_layer_t * layer = dsc->base.layer;
    if(layer->draw_buf == NULL) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    nvgGlobalAlpha(u->vg, t->opa / (float)LV_OPA_COVER);

    lv_vector_for_each_destroy_tasks(dsc->task_list, task_draw_cb, u);
    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

static NVGcolor lv_color32_to_nvg(lv_color32_t color, lv_opa_t opa)
{
    uint8_t a = LV_UDIV255(color.alpha * opa);
    return nvgRGBA(color.red, color.green, color.blue, a);
}

static void draw_fill(lv_draw_nanovg_unit_t * u, const lv_vector_fill_dsc_t * fill_dsc, const lv_fpoint_t * offset,
                      enum NVGcompositeOperation comp_op)
{
    LV_PROFILER_DRAW_BEGIN;

    const enum NVGwinding winding = lv_fill_to_nvg(fill_dsc->fill_rule);

    lv_nanovg_transform(u->vg, &fill_dsc->matrix);

    switch(fill_dsc->style) {
        case LV_VECTOR_DRAW_STYLE_SOLID: {
                lv_nanovg_fill(u->vg, winding, comp_op, lv_color32_to_nvg(fill_dsc->color, fill_dsc->opa));
            }
            break;
        case LV_VECTOR_DRAW_STYLE_PATTERN: {
                const lv_draw_image_dsc_t * img_dsc = &fill_dsc->img_dsc;
                lv_image_header_t header;
                int image_handle = lv_nanovg_image_cache_get_handle(u, img_dsc->src, lv_color_to_32(img_dsc->recolor,
                                                                                                    img_dsc->recolor_opa), 0, &header);
                if(image_handle < 0) {
                    LV_PROFILER_DRAW_END;
                    return;
                }

                float offset_x = 0;
                float offset_y = 0;

                if(fill_dsc->fill_units == LV_VECTOR_FILL_UNITS_OBJECT_BOUNDING_BOX) {
                    offset_x = offset->x;
                    offset_y = offset->y;
                }

                NVGpaint paint = nvgImagePattern(u->vg, offset_x, offset_y, header.w, header.h, 0, image_handle,
                                                 img_dsc->opa / (float)LV_OPA_COVER);

                nvgFillPaint(u->vg, paint);
                nvgFill(u->vg);
            }
            break;
        case LV_VECTOR_DRAW_STYLE_GRADIENT: {
                lv_nanovg_draw_grad(u->vg, &fill_dsc->gradient, winding, comp_op);
            }
            break;
        default:
            LV_LOG_WARN("unsupported style: %d", fill_dsc->style);
            break;
    }

    LV_PROFILER_DRAW_END;
}

static void draw_stroke(lv_draw_nanovg_unit_t * u, const lv_vector_stroke_dsc_t * stroke_dsc)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_nanovg_transform(u->vg, &stroke_dsc->matrix);

    nvgStrokeColor(u->vg, lv_color32_to_nvg(stroke_dsc->color, stroke_dsc->opa));
    nvgStrokeWidth(u->vg, stroke_dsc->width);

    switch(stroke_dsc->style) {
        case LV_VECTOR_DRAW_STYLE_SOLID:
            break;

        case LV_VECTOR_DRAW_STYLE_GRADIENT: {
                NVGpaint paint;
                if(!lv_nanovg_grad_to_paint(u->vg, &stroke_dsc->gradient, &paint)) {
                    LV_PROFILER_DRAW_END;
                    return;
                }
                nvgStrokePaint(u->vg, paint);
            }
            break;

        default:
            LV_LOG_WARN("unsupported style: %d", stroke_dsc->style);
            break;
    }

    nvgStroke(u->vg);

    LV_PROFILER_DRAW_END;
}

static void task_draw_cb(void * ctx, const lv_vector_path_t * path, const lv_vector_path_ctx_t * dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_nanovg_unit_t * u = ctx;

    /* clear area */
    if(!path) {
        NVGcolor c = lv_color32_to_nvg(dsc->fill_dsc.color, dsc->fill_dsc.opa);
        nvgBeginPath(u->vg);
        lv_nanovg_path_append_area(u->vg, &dsc->scissor_area);
        lv_nanovg_fill(u->vg, NVG_CCW, NVG_COPY, c);
        LV_PROFILER_DRAW_END;
        return;
    }

    if(dsc->fill_dsc.opa == LV_OPA_TRANSP && dsc->stroke_dsc.opa == LV_OPA_TRANSP) {
        LV_LOG_TRACE("Full transparent, no need to draw");
        LV_PROFILER_DRAW_END;
        return;
    }

    nvgSave(u->vg);
    lv_nanovg_transform(u->vg, &dsc->matrix);

    lv_fpoint_t offset = {0, 0};
    lv_path_to_nvg(u->vg, path, &offset);

    lv_nanovg_set_clip_area(u->vg, &dsc->scissor_area);

    const enum NVGcompositeOperation comp_op = lv_blend_to_nvg(dsc->blend_mode);
    nvgGlobalCompositeOperation(u->vg, comp_op);

    if(dsc->fill_dsc.opa) {
        draw_fill(u, &dsc->fill_dsc, &offset, comp_op);
    }

    if(dsc->stroke_dsc.opa) {
        draw_stroke(u, &dsc->stroke_dsc);
    }

    nvgRestore(u->vg);

    LV_PROFILER_DRAW_END;
}

static void lv_path_to_nvg(NVGcontext * ctx, const lv_vector_path_t * src, lv_fpoint_t * offset)
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

    nvgBeginPath(ctx);

    for(uint32_t i = 0; i < op_size; i++) {
        switch(ops[i]) {
            case LV_VECTOR_PATH_OP_MOVE_TO: {
                    nvgMoveTo(ctx, point->x, point->y);
                    CMP_BOUNDS(point);
                    point++;
                }
                break;
            case LV_VECTOR_PATH_OP_LINE_TO: {
                    nvgLineTo(ctx, point->x, point->y);
                    CMP_BOUNDS(point);
                    point++;
                }
                break;
            case LV_VECTOR_PATH_OP_QUAD_TO: {
                    nvgQuadTo(ctx, point[0].x, point[0].y, point[1].x, point[1].y);
                    CMP_BOUNDS(&point[0]);
                    CMP_BOUNDS(&point[1]);
                    point += 2;
                }
                break;
            case LV_VECTOR_PATH_OP_CUBIC_TO: {
                    nvgBezierTo(ctx, point[0].x, point[0].y, point[1].x, point[1].y, point[2].x, point[2].y);
                    CMP_BOUNDS(&point[0]);
                    CMP_BOUNDS(&point[1]);
                    CMP_BOUNDS(&point[2]);
                    point += 3;
                }
                break;
            case LV_VECTOR_PATH_OP_CLOSE: {
                    nvgClosePath(ctx);
                }
                break;
            default:
                LV_LOG_WARN("unknown op: %d", ops[i]);
                break;
        }
    }

    offset->x = lroundf(min_x);
    offset->y = lroundf(min_y);
    LV_PROFILER_DRAW_END;
}

static enum NVGcompositeOperation lv_blend_to_nvg(lv_vector_blend_t blend)
{
    switch(blend) {
        case LV_VECTOR_BLEND_SRC_OVER:
            return NVG_SOURCE_OVER;
        case LV_VECTOR_BLEND_SRC_IN:
            return NVG_SOURCE_IN;
        case LV_VECTOR_BLEND_DST_OVER:
            return NVG_DESTINATION_OVER;
        case LV_VECTOR_BLEND_DST_IN:
            return NVG_DESTINATION_IN;
        case LV_VECTOR_BLEND_NONE:
            return NVG_COPY;
        default:
            LV_LOG_INFO("Unknown supported blend mode: %d", blend);
            return NVG_SOURCE_OVER;
    }
}

static enum NVGwinding lv_fill_to_nvg(lv_vector_fill_t fill_rule)
{
    switch(fill_rule) {
        case LV_VECTOR_FILL_NONZERO:
            return NVG_CCW;
        case LV_VECTOR_FILL_EVENODD:
            return NVG_CW;
        default:
            LV_LOG_WARN("Unknown supported fill rule: %d", fill_rule);
            return NVG_CCW;
    }
}

#endif /* LV_USE_DRAW_NANOVG */
