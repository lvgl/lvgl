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
#include "../../image/lv_image_decoder_private.h"

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

void lv_draw_nanovg_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_vector_dsc_t * dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    if(dsc->task_list == NULL) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_layer_t * dsc_layer = dsc->base.layer;
    if(dsc_layer->draw_buf == NULL) {
        lv_vector_for_each_destroy_tasks(dsc->task_list, NULL, NULL);
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)draw_unit;

    nvgGlobalAlpha(u->vg, dsc->opa / (float)LV_OPA_COVER);

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
                int image_handle = lv_nanovg_image_cache_get_handle(u, img_dsc->src, 0, &header);
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
    /*Set the scissor before the transform, nvgScissor() is transformed by the current matrix*/
    lv_nanovg_set_clip_area(u->vg, &dsc->scissor_area);
    lv_nanovg_transform(u->vg, &dsc->matrix);

    lv_fpoint_t offset = {0, 0};
    lv_nanovg_path_append_vector_path(u->vg, path, dsc->fill_dsc.fill_rule == LV_VECTOR_FILL_NONZERO, &offset);

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
            /*The winding of each contour was set by lv_path_to_nvg(), don't override the last one*/
            return LV_NANOVG_WINDING_KEEP;
        case LV_VECTOR_FILL_EVENODD:
            return NVG_CW;
        default:
            LV_LOG_WARN("Unknown supported fill rule: %d", fill_rule);
            return LV_NANOVG_WINDING_KEEP;
    }
}

#endif /* LV_USE_DRAW_NANOVG */
