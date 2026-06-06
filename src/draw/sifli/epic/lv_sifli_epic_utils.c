/**
 * @file lv_sifli_epic_utils.c
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

#include "lv_sifli_epic_utils.h"

#if LV_USE_SIFLI_EPIC
#include "../../../misc/lv_area_private.h"
#include "string.h"

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

uint32_t lv_img_cf_to_epic_cf(lv_color_format_t cf)
{
    uint32_t color_mode;

    switch(cf) {
        /* 2 byte (+alpha) formats */
        case LV_COLOR_FORMAT_RGB565:
            color_mode = EPIC_INPUT_RGB565;
            break;

#ifdef EPIC_SUPPORT_MASK
        case LV_COLOR_FORMAT_RGB565A8:
            color_mode = EPIC_INPUT_RGB565;
            break;
#endif

        /* 3 byte (+alpha) formats */
        case LV_COLOR_FORMAT_RGB888:
            color_mode = EPIC_INPUT_RGB888;
            break;

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            color_mode = EPIC_INPUT_ARGB8888;
            break;

        case LV_COLOR_FORMAT_ARGB8565:
            color_mode = EPIC_INPUT_ARGB8565;
            break;

#ifdef EPIC_SUPPORT_A8
        case LV_COLOR_FORMAT_A8:
            color_mode = EPIC_INPUT_A8;
            break;
#endif

#ifdef EPIC_SUPPORT_A4
        case LV_COLOR_FORMAT_A4:
            color_mode = EPIC_INPUT_A4;
            break;
#endif

#ifdef EPIC_SUPPORT_A2
        case LV_COLOR_FORMAT_A2:
            color_mode = EPIC_INPUT_A2;
            break;
#endif

#ifdef EPIC_SUPPORT_L8
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_L8:
            color_mode = EPIC_INPUT_L8;
            break;
#endif

#ifdef EPIC_SUPPORT_YUV
        case LV_COLOR_FORMAT_I420:
            color_mode = EPIC_INPUT_YUV420_PLANAR;
            break;

        case LV_COLOR_FORMAT_YUY2:
            color_mode = EPIC_INPUT_YUV422_PACKED_YUYV;
            break;

        case LV_COLOR_FORMAT_UYVY:
            color_mode = EPIC_INPUT_YUV422_PACKED_UYVY;
            break;
#endif

        default:
            LV_ASSERT(false);
            color_mode = EPIC_INPUT_RGB565; /* Fallback */
            break;
    }

    return color_mode;
}

EPIC_ColorDef lv_color_to_epic_color(lv_color_t color, lv_opa_t opa)
{
    EPIC_ColorDef c;
    c.ch.color_r = color.red;
    c.ch.color_g = color.green;
    c.ch.color_b = color.blue;
    c.ch.alpha = opa;
    return c;
}

uint32_t lv_epic_setup_layers(EPIC_LayerConfigTypeDef *epic_bg_layer,
                              EPIC_LayerConfigTypeDef *epic_output_layer,
                              lv_draw_task_t * draw_task,
                              const lv_area_t * coords)
{
    lv_layer_t * layer = draw_task->target_layer;
    uint16_t dest_total_width;

    lv_area_t blend_area;
    if(!lv_area_intersect(&blend_area, coords, &draw_task->clip_area)) {
        return 1; /* Fully clipped, nothing to do */
    }

    if(!lv_area_intersect(&blend_area, &blend_area, &layer->buf_area)) {
        return 1; /* Fully clipped, nothing to do */
    }

    lv_color_format_t dest_cf = layer->color_format;
    dest_total_width = layer->draw_buf ? lv_epic_stride_to_width(layer->draw_buf->header.stride, dest_cf) :
                       lv_area_get_width(&layer->buf_area);

    /* Setup background layer */
    HAL_EPIC_LayerConfigInit(epic_bg_layer);
    epic_bg_layer->color_en = false;
    epic_bg_layer->data = (uint8_t *)lv_draw_layer_go_to_xy(layer, 0, 0);
    epic_bg_layer->color_mode = lv_img_cf_to_epic_cf(dest_cf);
    epic_bg_layer->width = lv_area_get_width(&layer->buf_area);
    epic_bg_layer->total_width = dest_total_width;
    epic_bg_layer->height = lv_area_get_height(&layer->buf_area);
    epic_bg_layer->x_offset = layer->buf_area.x1;
    epic_bg_layer->y_offset = layer->buf_area.y1;

    /* Setup output layer */
    memcpy(epic_output_layer, epic_bg_layer, sizeof(*epic_output_layer));
    epic_output_layer->width = lv_area_get_width(&blend_area);
    epic_output_layer->height = lv_area_get_height(&blend_area);
    epic_output_layer->x_offset = blend_area.x1;
    epic_output_layer->y_offset = blend_area.y1;
    epic_output_layer->data = (uint8_t *)lv_draw_layer_go_to_xy(layer,
                                                                blend_area.x1 - layer->buf_area.x1,
                                                                blend_area.y1 - layer->buf_area.y1);

    LV_ASSERT((epic_output_layer->height * epic_output_layer->width) <= EPIC_MAX_PIXELS);

    return 0; /* Success */
}

bool lv_epic_cf_supported(lv_color_format_t cf, uint32_t flags)
{
    /* EZIP images */
    if((LV_COLOR_FORMAT_RAW == cf || LV_COLOR_FORMAT_RAW_ALPHA == cf) &&
       (0 != (flags & LV_IMAGE_FLAGS_EZIP))) {
        return true;
    }

#if defined(EPIC_INPUT_JPEG)
    if((LV_COLOR_FORMAT_RAW == cf || LV_COLOR_FORMAT_RAW_ALPHA == cf) &&
       (0 != (flags & LV_IMAGE_FLAGS_JPEG))) {
        return true;
    }
#endif

    /* Normal images */
    if(LV_COLOR_FORMAT_RGB565 == cf ||
       LV_COLOR_FORMAT_ARGB8565 == cf ||
       LV_COLOR_FORMAT_RGB888 == cf ||
       LV_COLOR_FORMAT_ARGB8888 == cf ||
       LV_COLOR_FORMAT_XRGB8888 == cf) {
        return true;
    }

#ifdef EPIC_SUPPORT_A8
    if(LV_COLOR_FORMAT_A8 == cf) return true;
#endif

#ifdef EPIC_SUPPORT_MASK
    if(LV_COLOR_FORMAT_RGB565A8 == cf) return true;
#endif

#ifdef EPIC_SUPPORT_A4
    if(LV_COLOR_FORMAT_A4 == cf) return true;
#endif

#ifdef EPIC_SUPPORT_A2
    if(LV_COLOR_FORMAT_A2 == cf) return true;
#endif

#ifdef EPIC_SUPPORT_L8
    if((LV_COLOR_FORMAT_I8 == cf) || (LV_COLOR_FORMAT_L8 == cf)) return true;
#endif

#ifdef EPIC_SUPPORT_YUV
    if(LV_COLOR_FORMAT_I420 == cf ||
       LV_COLOR_FORMAT_YUY2 == cf ||
       LV_COLOR_FORMAT_UYVY == cf) {
        return true;
    }
#endif

    return false;
}

uint16_t lv_epic_stride_to_width(uint32_t stride, lv_color_format_t cf)
{
    uint8_t bpp = lv_color_format_get_bpp(cf);

    if(bpp == 0) {
        return 0;
    }

    return (uint16_t)((stride * 8U) / bpp);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_SIFLI_EPIC*/
