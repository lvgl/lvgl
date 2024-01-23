/**
 * @file lv_draw_vglite_layer.c
 *
 */

/**
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE

#include "../../../stdlib/lv_string.h"
#if LV_USE_PARALLEL_DRAW_DEBUG
    #include "../../../core/lv_global.h"
#endif

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

#if LV_USE_PARALLEL_DRAW_DEBUG
    #define _draw_info LV_GLOBAL_DEFAULT()->draw_info
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;
    const lv_draw_buf_t * draw_buf = layer_to_draw->draw_buf;

    /* It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     * In this case just return.
     */
    if(draw_buf == NULL)
        return;

    const lv_area_t area_to_draw = {
        .x1 = 0,
        .y1 = 0,
        .x2 = draw_buf->header.w - 1,
        .y2 = draw_buf->header.h - 1
    };
    lv_draw_buf_invalidate_cache(draw_buf->data, draw_buf->header.stride, draw_buf->header.cf, &area_to_draw);

    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = draw_buf;
    lv_draw_vglite_img(draw_unit, &new_draw_dsc, coords);

#if LV_USE_LAYER_DEBUG || LV_USE_PARALLEL_DRAW_DEBUG
    lv_area_t area_rot;
    lv_area_copy(&area_rot, coords);
    bool has_transform = (draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                          draw_dsc->scale_y != LV_SCALE_NONE);

    if(has_transform) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_image_buf_get_transformed_area(&area_rot, w, h, draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y,
                                           &draw_dsc->pivot);

        area_rot.x1 += coords->x1;
        area_rot.y1 += coords->y1;
        area_rot.x2 += coords->x1;
        area_rot.y2 += coords->y1;
    }
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &area_rot, draw_unit->clip_area)) return;
#endif

#if LV_USE_LAYER_DEBUG
    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_color_hex(layer_to_draw->color_format == LV_COLOR_FORMAT_ARGB8888 ? 0xff0000 : 0x00ff00);
    fill_dsc.opa = LV_OPA_20;
    lv_draw_sw_fill(draw_unit, &fill_dsc, &area_rot);

    lv_draw_border_dsc_t border_dsc;
    lv_draw_border_dsc_init(&border_dsc);
    border_dsc.color = fill_dsc.color;
    border_dsc.opa = LV_OPA_60;
    border_dsc.width = 2;
    lv_draw_sw_border(draw_unit, &border_dsc, &area_rot);

#endif

#if LV_USE_PARALLEL_DRAW_DEBUG
    uint32_t idx = 0;
    lv_draw_unit_t * draw_unit_tmp = _draw_info.unit_head;
    while(draw_unit_tmp != draw_unit) {
        draw_unit_tmp = draw_unit_tmp->next;
        idx++;
    }

    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_rect_dsc_init(&fill_dsc);
    fill_dsc.color = lv_palette_main(idx % _LV_PALETTE_LAST);
    fill_dsc.opa = LV_OPA_10;
    lv_draw_sw_fill(draw_unit, &fill_dsc, &area_rot);

    lv_draw_border_dsc_t border_dsc;
    lv_draw_border_dsc_init(&border_dsc);
    border_dsc.color = lv_palette_main(idx % _LV_PALETTE_LAST);
    border_dsc.opa = LV_OPA_100;
    border_dsc.width = 2;
    lv_draw_sw_border(draw_unit, &border_dsc, &area_rot);

    lv_point_t txt_size;
    lv_text_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

    lv_area_t txt_area;
    txt_area.x1 = draw_area.x1;
    txt_area.x2 = draw_area.x1 + txt_size.x - 1;
    txt_area.y2 = draw_area.y2;
    txt_area.y1 = draw_area.y2 - txt_size.y + 1;

    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_color_black();
    lv_draw_sw_fill(draw_unit, &fill_dsc, &txt_area);

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", idx);
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.text = buf;
    lv_draw_sw_label(draw_unit, &label_dsc, &txt_area);
#endif
}

#endif /*LV_USE_DRAW_VGLITE*/
