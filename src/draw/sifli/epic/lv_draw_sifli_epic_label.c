/**
 * @file lv_draw_sifli_epic_label.c
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
#include "../../../draw/lv_draw_label_private.h"
#include "../../../draw/lv_draw_rect.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_letter_cb(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);
static void draw_placeholder_border(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc);
static void draw_bitmap_glyph(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc);
static void draw_glyph_as_image(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc);
static void draw_fill_part(lv_draw_task_t * task, lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sifli_epic_label(lv_draw_task_t * task)
{
    const lv_draw_label_dsc_t * dsc = (const lv_draw_label_dsc_t *)task->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN || dsc->text == NULL || dsc->text[0] == '\0') {
        return;
    }

#ifndef EPIC_SUPPORT_A8
    LV_UNUSED(task);
#else
    lv_draw_label_iterate_characters(task, dsc, &task->area, draw_letter_cb);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_letter_cb(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                           lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {
        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_NONE: {
#if LV_USE_FONT_PLACEHOLDER
                    draw_placeholder_border(task, glyph_draw_dsc);
#endif
                    break;
                }

            case LV_FONT_GLYPH_FORMAT_A1:
            case LV_FONT_GLYPH_FORMAT_A2:
            case LV_FONT_GLYPH_FORMAT_A3:
            case LV_FONT_GLYPH_FORMAT_A4:
            case LV_FONT_GLYPH_FORMAT_A8:
                if(glyph_draw_dsc->rotation % 3600 == 0) {
                    draw_bitmap_glyph(task, glyph_draw_dsc);
                }
                else {
                    draw_glyph_as_image(task, glyph_draw_dsc);
                }
                break;

            case LV_FONT_GLYPH_FORMAT_IMAGE:
                draw_glyph_as_image(task, glyph_draw_dsc);
                break;

            default:
                break;
        }
    }

    if(fill_draw_dsc && fill_area) {
        draw_fill_part(task, fill_draw_dsc, fill_area);
    }
}

static void draw_placeholder_border(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc)
{
    if(glyph_draw_dsc->bg_coords == NULL) {
        return;
    }

    lv_draw_border_dsc_t border_draw_dsc;
    lv_draw_border_dsc_init(&border_draw_dsc);
    border_draw_dsc.opa = glyph_draw_dsc->opa;
    border_draw_dsc.color = glyph_draw_dsc->color;
    border_draw_dsc.width = 1;

    lv_draw_task_t border_task = *task;
    border_task.draw_dsc = &border_draw_dsc;
    border_task.area = *glyph_draw_dsc->bg_coords;
    lv_draw_sifli_epic_border(&border_task);
}

static void draw_bitmap_glyph(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc)
{
    if(glyph_draw_dsc->opa <= LV_OPA_MIN || glyph_draw_dsc->g == NULL || glyph_draw_dsc->letter_coords == NULL) {
        return;
    }

    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
    if(glyph_draw_dsc->glyph_data == NULL) {
        return;
    }

    const lv_draw_buf_t * glyph_buf = (const lv_draw_buf_t *)glyph_draw_dsc->glyph_data;
    if(glyph_buf->data == NULL) {
        EPIC_ASSERT_MSG(false, "EPIC: Bitmap glyph buffer has no data");
        return;
    }

    EPIC_LayerConfigTypeDef bg_layer;
    EPIC_LayerConfigTypeDef output_layer;

    if(lv_epic_setup_layers(&bg_layer, &output_layer, task, glyph_draw_dsc->letter_coords)) {
        return;
    }

    EPIC_LayerConfigTypeDef input_layers[2];
    input_layers[0] = bg_layer;
    HAL_EPIC_LayerConfigInit(&input_layers[1]);

    input_layers[1].data = (uint8_t *)glyph_buf->data;
    input_layers[1].color_mode = lv_img_cf_to_epic_cf(glyph_buf->header.cf);
    input_layers[1].width = glyph_buf->header.w;
    input_layers[1].height = glyph_buf->header.h;
    input_layers[1].total_width = lv_epic_stride_to_width(glyph_buf->header.stride, glyph_buf->header.cf);
    input_layers[1].x_offset = glyph_draw_dsc->letter_coords->x1;
    input_layers[1].y_offset = glyph_draw_dsc->letter_coords->y1;
    input_layers[1].alpha = glyph_draw_dsc->opa;
    input_layers[1].color_en = true;
    input_layers[1].color_r = glyph_draw_dsc->color.red;
    input_layers[1].color_g = glyph_draw_dsc->color.green;
    input_layers[1].color_b = glyph_draw_dsc->color.blue;
    input_layers[1].ax_mode = ALPHA_BLEND_RGBCOLOR;

    (void)lv_epic_blend(input_layers, 2, &output_layer);
}

static void draw_glyph_as_image(lv_draw_task_t * task, lv_draw_glyph_dsc_t * glyph_draw_dsc)
{
    if(glyph_draw_dsc->opa <= LV_OPA_MIN || glyph_draw_dsc->g == NULL || glyph_draw_dsc->letter_coords == NULL) {
        return;
    }

    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
    if(glyph_draw_dsc->glyph_data == NULL) {
        return;
    }

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    img_dsc.rotation = glyph_draw_dsc->rotation;
    img_dsc.scale_x = LV_SCALE_NONE;
    img_dsc.scale_y = LV_SCALE_NONE;
    img_dsc.opa = glyph_draw_dsc->opa;
    img_dsc.src = glyph_draw_dsc->glyph_data;
    img_dsc.recolor = glyph_draw_dsc->color;
    img_dsc.pivot.x = glyph_draw_dsc->pivot.x;
    img_dsc.pivot.y = glyph_draw_dsc->g ? (glyph_draw_dsc->g->box_h + glyph_draw_dsc->g->ofs_y) : 0;

    lv_draw_task_t img_task = *task;
    img_task.draw_dsc = &img_dsc;
    img_task.area = *glyph_draw_dsc->letter_coords;
    lv_draw_sifli_epic_img(&img_task);
}

static void draw_fill_part(lv_draw_task_t * task, lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    lv_draw_task_t fill_task = *task;
    fill_task.draw_dsc = fill_draw_dsc;
    fill_task.area = *fill_area;
    lv_draw_sifli_epic_fill(&fill_task);
}

#endif /*LV_USE_SIFLI_EPIC*/
