/**
 * @file lv_draw_sifli_epic_fill.c
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sifli_epic.h"

#if LV_USE_SIFLI_EPIC
#include "lv_sifli_epic_utils.h"
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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

void lv_draw_sifli_epic_fill(lv_draw_task_t * task)
{
    const lv_draw_fill_dsc_t * dsc = (const lv_draw_fill_dsc_t *)task->draw_dsc;
    const lv_area_t * coords = &task->area;

    if(dsc->opa <= LV_OPA_MIN) {
        return;
    }

    lv_layer_t * layer = task->target_layer;

    lv_area_t blend_area;
    if(!lv_area_intersect(&blend_area, coords, &task->clip_area)) {
        return; /* Fully clipped */
    }

    if(!lv_area_intersect(&blend_area, &blend_area, &layer->buf_area)) {
        return; /* Fully clipped */
    }

    lv_grad_dir_t grad_dir = dsc->grad.dir;
    if(grad_dir == LV_GRAD_DIR_NONE) {
        /* Solid color fill */
        EPIC_LayerConfigTypeDef input_layers[1];
        EPIC_LayerConfigTypeDef output_canvas;

        if(lv_epic_setup_layers(&input_layers[0], &output_canvas, task, coords)) {
            return; /* Fully clipped */
        }

        /* Set fill color */
        output_canvas.color_r = dsc->color.red;
        output_canvas.color_g = dsc->color.green;
        output_canvas.color_b = dsc->color.blue;
        output_canvas.color_en = true;
        if(dsc->opa < LV_OPA_MAX) {
            /* Alpha blending with background */
            input_layers[0].alpha = (dsc->opa == 0) ? 255 : (256 - dsc->opa);
            (void)lv_epic_fill(input_layers, 1, &output_canvas);
        }
        else {
            /* Opaque fill */
            (void)lv_epic_fill(NULL, 0, &output_canvas);
        }
    }
    else {
        /* Gradient fill */
        lv_color_format_t dest_cf = layer->color_format;
        uint32_t epic_cf = lv_img_cf_to_epic_cf(dest_cf);

        LV_ASSERT_MSG(EPIC_SUPPROT_OUT_FORMAT(epic_cf),
                      "EPIC: Color format not supported for gradient fill");

        uint8_t * dest_buf = lv_draw_layer_go_to_xy(layer,
                                                    blend_area.x1 - layer->buf_area.x1,
                                                    blend_area.y1 - layer->buf_area.y1);

        EPIC_GradCfgTypeDef param;
        HAL_EPIC_FillGradDataInit(&param);

        param.start = dest_buf;
        param.color_mode = epic_cf;
        param.width = lv_area_get_width(&blend_area);
        param.height = lv_area_get_height(&blend_area);
        param.total_width = layer->draw_buf ? lv_epic_stride_to_width(layer->draw_buf->header.stride,
                                                                       dest_cf) : lv_area_get_width(&layer->buf_area);
        /* Setup gradient colors */
        if(grad_dir == LV_GRAD_DIR_VER) {
            /* Vertical gradient */
            LV_ASSERT(dsc->grad.stops_count == 2);
            EPIC_ColorDef color1 = lv_color_to_epic_color(
                                       dsc->grad.stops[0].color,
                                       LV_OPA_MIX2(dsc->grad.stops[0].opa, dsc->opa));
            EPIC_ColorDef color2 = lv_color_to_epic_color(
                                       dsc->grad.stops[1].color,
                                       LV_OPA_MIX2(dsc->grad.stops[1].opa, dsc->opa));

            param.color[0][0] = color1;
            param.color[0][1] = color1;
            param.color[1][0] = color2;
            param.color[1][1] = color2;
        }
        else if(grad_dir == LV_GRAD_DIR_HOR) {
            /* Horizontal gradient */
            LV_ASSERT(dsc->grad.stops_count == 2);
            EPIC_ColorDef color1 = lv_color_to_epic_color(
                                       dsc->grad.stops[0].color,
                                       LV_OPA_MIX2(dsc->grad.stops[0].opa, dsc->opa));
            EPIC_ColorDef color2 = lv_color_to_epic_color(
                                       dsc->grad.stops[1].color,
                                       LV_OPA_MIX2(dsc->grad.stops[1].opa, dsc->opa));

            param.color[0][0] = color1;
            param.color[0][1] = color2;
            param.color[1][0] = color1;
            param.color[1][1] = color2;
        }

        (void)lv_epic_fill_grad(&param);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_SIFLI_EPIC*/
