/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../lv_img_cache.h"
#include "../../disp/lv_disp.h"
#include "../../disp/lv_disp_private.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MAX_BUF_SIZE (uint32_t) 4 * lv_disp_get_hor_res(_lv_refr_get_disp_refreshing())

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

void lv_draw_sw_layer(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if(layer_to_draw->buf == NULL) return;

    lv_img_dsc_t img_dsc;
    img_dsc.header.w = lv_area_get_width(&layer_to_draw->buf_area);
    img_dsc.header.h = lv_area_get_height(&layer_to_draw->buf_area);
    img_dsc.header.cf = layer_to_draw->color_format;
    img_dsc.header.always_zero = 0;
    img_dsc.data = layer_to_draw->buf;

    lv_draw_img_dsc_t new_draw_dsc;
    lv_memcpy(&new_draw_dsc, draw_dsc, sizeof(lv_draw_img_dsc_t));
    new_draw_dsc.src = &img_dsc;

    lv_draw_sw_img(draw_unit, &new_draw_dsc, coords);

#if LV_USE_LAYER_DEBUG || LV_USE_PARALLEL_DRAW_DEBUG
    lv_area_t area_rot;
    lv_area_copy(&area_rot, coords);
    if(draw_dsc->angle || draw_dsc->zoom != LV_ZOOM_NONE) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_img_buf_get_transformed_area(&area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

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
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_draw_unit_t * draw_unit_tmp = disp->draw_unit_head;
    while(draw_unit_tmp != draw_unit) {
        draw_unit_tmp = draw_unit_tmp->next;
        idx++;
    }

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_palette_main(idx % _LV_PALETTE_LAST);
    rect_dsc.border_color = rect_dsc.bg_color;
    rect_dsc.bg_opa = LV_OPA_10;
    rect_dsc.border_opa = LV_OPA_100;
    rect_dsc.border_width = 2;
    lv_draw_sw_rect(draw_unit, &rect_dsc, &area_rot);

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

    lv_area_t txt_area;
    txt_area.x1 = draw_area.x1;
    txt_area.x2 = draw_area.x1 + txt_size.x - 1;
    txt_area.y2 = draw_area.y2;
    txt_area.y1 = draw_area.y2 - txt_size.y + 1;

    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_black();
    lv_draw_sw_rect(draw_unit, &rect_dsc, &txt_area);

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", idx);
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.text = buf;
    lv_draw_sw_label(draw_unit, &label_dsc, &txt_area);
#endif
}



LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_img(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * draw_dsc,
                                          const lv_area_t * coords)
{
    lv_area_t transformed_area;
    lv_area_copy(&transformed_area, coords);
    if(draw_dsc->angle || draw_dsc->zoom != LV_ZOOM_NONE) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_img_buf_get_transformed_area(&transformed_area, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

        transformed_area.x1 += coords->x1;
        transformed_area.y1 += coords->y1;
        transformed_area.x2 += coords->x1;
        transformed_area.y2 += coords->y1;
    }

    lv_area_t draw_area; /*Common area of cilp_area and coords. The image is visible only here*/
    /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
    if(!_lv_area_intersect(&draw_area, draw_unit->clip_area, &transformed_area)) {
        return;
    }

    bool transformed = draw_dsc->angle != 0 || draw_dsc->zoom != LV_ZOOM_NONE ? true : false;

    lv_img_decoder_dsc_t decoder_dsc;
    lv_img_decoder_open(&decoder_dsc, draw_dsc->src, draw_dsc->recolor, -1);
    const uint8_t * src_buf = decoder_dsc.img_data;

    lv_color_format_t cf = decoder_dsc.header.cf;


    lv_draw_sw_blend_dsc_t blend_dsc;

    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;


    if(!transformed && cf == LV_COLOR_FORMAT_A8) {
        lv_area_t clipped_coords;
        if(!_lv_area_intersect(&clipped_coords, coords, draw_unit->clip_area)) return;

        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_area = coords;
        blend_dsc.src_buf = NULL;
        blend_dsc.color = draw_dsc->recolor;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        blend_dsc.blend_area = coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && cf == LV_COLOR_FORMAT_RGB565A8 && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        lv_coord_t src_w = lv_area_get_width(coords);
        lv_coord_t src_h = lv_area_get_height(coords);
        blend_dsc.src_area = coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_buf += 2 * src_w * src_h;
        blend_dsc.blend_area = coords;
        blend_dsc.mask_area = coords;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*The simplest case just copy the pixels into the draw_buf. Blending will convert the colors if needed*/
    else if(!transformed && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        blend_dsc.src_area = coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.blend_area = coords;
        blend_dsc.src_color_format = cf;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        lv_area_t blend_area = draw_area;
        blend_dsc.blend_area = &blend_area;

        lv_coord_t src_w = lv_area_get_width(coords);
        lv_coord_t src_h = lv_area_get_height(coords);
        lv_coord_t blend_w = lv_area_get_width(&blend_area);
        lv_coord_t blend_h = lv_area_get_height(&blend_area);

        lv_color_format_t cf_final = cf;
        if(transformed) {
            if(cf == LV_COLOR_FORMAT_RGB888 || cf == LV_COLOR_FORMAT_XRGB8888) cf_final = LV_COLOR_FORMAT_ARGB8888;
            if(cf == LV_COLOR_FORMAT_RGB565) cf_final = LV_COLOR_FORMAT_RGB565A8;
        }

        uint32_t px_size = lv_color_format_get_size(cf_final);
        uint32_t max_buf_size = MAX_BUF_SIZE / px_size;
        uint32_t blend_size = lv_area_get_size(&blend_area);
        uint32_t buf_w = blend_w;
        uint32_t buf_h;
        if(blend_size <= max_buf_size) buf_h = blend_h;
        else buf_h = max_buf_size / blend_w;    /*Round to full lines*/

        uint32_t buf_size = buf_w * buf_h;
        uint8_t * tmp_buf = lv_malloc(buf_size * px_size);
        blend_dsc.src_buf = tmp_buf;
        blend_dsc.src_color_format = cf_final;
        lv_coord_t y_last = blend_area.y2;
        blend_area.y2 = blend_area.y1 + buf_h - 1;

        blend_dsc.src_area = &blend_area;

        if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
            blend_dsc.mask_buf =  tmp_buf + buf_w * buf_h * 2;
            blend_dsc.mask_area = &blend_area;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        }

        if(blend_dsc.src_color_format == LV_COLOR_FORMAT_A8) {
            blend_dsc.mask_buf = blend_dsc.src_buf;
            blend_dsc.mask_area = &blend_area;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            blend_dsc.color = draw_dsc->recolor;
            blend_dsc.src_buf = NULL;
        }

        lv_draw_img_sup_t sup;
        sup.alpha_color = draw_dsc->recolor;
        sup.palette = decoder_dsc.palette;
        sup.palette_size = decoder_dsc.palette_size;

        while(blend_area.y1 <= y_last) {
            /*Apply transformations if any or separate the channels*/
            lv_area_t relative_area;
            lv_area_copy(&relative_area, &blend_area);
            lv_area_move(&relative_area, -coords->x1, -coords->y1);
            if(transformed) {
                lv_draw_sw_transform(draw_unit, &relative_area, src_buf, src_w, src_h, src_w,
                                     draw_dsc, &sup, cf, tmp_buf);
            }
            else if(draw_dsc->recolor_opa >= LV_OPA_MIN) {
                lv_coord_t h = lv_area_get_height(&relative_area);
                if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
                    const uint8_t * rgb_src_buf = src_buf + src_w * relative_area.y1 * 2 + relative_area.x1 * 2;
                    const uint8_t * a_src_buf = src_buf + src_w * src_h * 2 + src_w * relative_area.y1 + relative_area.x1;
                    uint8_t * rgb_dest_buf = tmp_buf;
                    uint8_t * a_dest_buf = tmp_buf + blend_w * h * 2;
                    lv_coord_t i;
                    for(i = 0; i < h; i++) {
                        lv_memcpy(rgb_dest_buf, rgb_src_buf, blend_w * 2);
                        lv_memcpy(a_dest_buf, a_src_buf, blend_w);
                        rgb_src_buf += src_w * 2;
                        a_src_buf += src_w;
                        rgb_dest_buf += blend_w * 2;
                        a_dest_buf += blend_w;
                    }
                }
                else if(cf_final != LV_COLOR_FORMAT_A8) {
                    const uint8_t * src_buf_tmp = src_buf + src_w * relative_area.y1 * px_size + relative_area.x1 * px_size;
                    uint8_t * dest_buf_tmp = tmp_buf;
                    lv_coord_t i;
                    for(i = 0; i < h; i++) {
                        lv_memcpy(dest_buf_tmp, src_buf_tmp, blend_w * px_size);
                        dest_buf_tmp += blend_w * px_size;
                        src_buf_tmp += src_w * px_size;
                    }
                }
            }

            /*Apply recolor*/
            if(draw_dsc->recolor_opa > LV_OPA_MIN) {
                lv_color_t color = draw_dsc->recolor;
                lv_opa_t mix = draw_dsc->recolor_opa;
                lv_opa_t mix_inv = 255 - mix;
                if(cf_final == LV_COLOR_FORMAT_RGB565A8 || cf_final == LV_COLOR_FORMAT_RGB565) {
                    lv_coord_t size = lv_area_get_size(&blend_area);
                    lv_coord_t i;
                    uint16_t c_mult[3];
                    c_mult[0] = (color.blue >> 3) * mix;
                    c_mult[1] = (color.green >> 2) * mix;
                    c_mult[2] = (color.red >> 3) * mix;
                    uint16_t * buf16 = (uint16_t *)tmp_buf;
                    for(i = 0; i < size; i++) {
                        buf16[i] = (((c_mult[2] + ((buf16[i] >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
                                   (((c_mult[1] + ((buf16[i] >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
                                   ((c_mult[0] + (buf16[i] & 0x1F) * mix_inv) >> 8);
                    }
                }
                else  if(cf_final != LV_COLOR_FORMAT_A8) {
                    uint32_t size = lv_area_get_size(&blend_area);
                    uint32_t i;
                    uint16_t c_mult[3];
                    c_mult[0] = color.blue * mix;
                    c_mult[1] = color.green * mix;
                    c_mult[2] = color.red * mix;
                    uint8_t * tmp_buf_2 = tmp_buf;
                    for(i = 0; i < size * px_size; i += px_size) {
                        tmp_buf_2[i + 0] = (c_mult[0] + (tmp_buf_2[i + 0] * mix_inv)) >> 8;
                        tmp_buf_2[i + 1] = (c_mult[1] + (tmp_buf_2[i + 1] * mix_inv)) >> 8;
                        tmp_buf_2[i + 2] = (c_mult[2] + (tmp_buf_2[i + 2] * mix_inv)) >> 8;
                    }
                }
            }

            /*Blend*/
            lv_draw_sw_blend(draw_unit, &blend_dsc);

            /*Go to the next lines*/
            blend_area.y1 = blend_area.y2 + 1;
            blend_area.y2 = blend_area.y1 + buf_h - 1;
            if(blend_area.y2 > y_last) {
                blend_area.y2 = y_last;
                if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
                    blend_dsc.mask_buf =  tmp_buf + buf_w * lv_area_get_height(&blend_area) * 2;
                }
            }
        }

        lv_free(tmp_buf);
    }

    lv_img_decoder_close(&decoder_dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SW*/
