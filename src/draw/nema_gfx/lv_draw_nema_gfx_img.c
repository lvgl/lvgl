/**
 * @file lv_draw_nema_gfx_img.c
 *
 */

/**
 * MIT License
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-24 Think Silicon Single Member PC
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_refr.h"

#if LV_USE_NEMA_GFX

#include "lv_draw_nema_gfx.h"
#include "lv_img_cache.h"
#include "../hal/lv_hal_disp.h"
#include "../misc/lv_log.h"
#include "../core/lv_refr.h"
#include "../misc/lv_mem.h"
#include "../misc/lv_math.h"
#include "lv_img_buf.h"
#include "../misc/lv_style.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void show_error(lv_draw_ctx_t * draw_ctx, const lv_area_t * coords, const char * msg);
static void draw_cleanup(_lv_img_cache_entry_t * cache);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_img_cf_alpha_only_format(lv_img_cf_t cf)
{
    bool is_alpha_format = false;

    switch(cf) {
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            is_alpha_format = true;
            break;
        default:
            is_alpha_format = false;
            break;
    }

    return is_alpha_format;
}


bool lv_img_cf_lut_format(lv_img_cf_t cf)
{
    bool is_lut_format = false;

    switch(cf) {
        case LV_IMG_CF_INDEXED_1BIT:
        case LV_IMG_CF_INDEXED_2BIT:
        case LV_IMG_CF_INDEXED_4BIT:
        case LV_IMG_CF_INDEXED_8BIT:
            is_lut_format = true;
            break;
        default:
            is_lut_format = false;
            break;
    }

    return is_lut_format;
}

lv_res_t lv_draw_nema_gfx_img(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                              const lv_area_t * coords, const void * src)
{
    if(draw_dsc->opa <= LV_OPA_MIN) return LV_RES_OK;

    _lv_img_cache_entry_t * cdsc = _lv_img_cache_open(src, draw_dsc->recolor, draw_dsc->frame_id);

    if(cdsc == NULL) return LV_RES_INV;

    lv_img_cf_t cf;
    if(lv_img_cf_is_chroma_keyed(cdsc->dec_dsc.header.cf)) cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;
    else if(LV_IMG_CF_ALPHA_8BIT == cdsc->dec_dsc.header.cf) cf = LV_IMG_CF_ALPHA_8BIT;
    else if(LV_IMG_CF_RGB565A8 == cdsc->dec_dsc.header.cf) cf = LV_IMG_CF_RGB565A8;
    else if(lv_img_cf_has_alpha(cdsc->dec_dsc.header.cf)) cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    else cf = LV_IMG_CF_TRUE_COLOR;

    if(cf == LV_IMG_CF_ALPHA_8BIT) {
        if(draw_dsc->angle || draw_dsc->zoom != LV_IMG_ZOOM_NONE) {
            /* resume normal method */
            cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
            cdsc->dec_dsc.img_data = NULL;
        }
    }

    if(cdsc->dec_dsc.error_msg != NULL) {
        LV_LOG_WARN("Image draw error");

        show_error(draw_ctx, coords, cdsc->dec_dsc.error_msg);
    }
    /*The decoder could open the image and gave the entire uncompressed image.
     *Just draw it!*/
    else if(cdsc->dec_dsc.img_data || (lv_img_cf_alpha_only_format(cdsc->dec_dsc.header.cf)) ||
            lv_img_cf_lut_format(cdsc->dec_dsc.header.cf)) {
        lv_area_t map_area_rot;
        lv_area_copy(&map_area_rot, coords);
        if(draw_dsc->angle || draw_dsc->zoom != LV_IMG_ZOOM_NONE) {
            int32_t w = lv_area_get_width(coords);
            int32_t h = lv_area_get_height(coords);

            _lv_img_buf_get_transformed_area(&map_area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

            map_area_rot.x1 += coords->x1;
            map_area_rot.y1 += coords->y1;
            map_area_rot.x2 += coords->x1;
            map_area_rot.y2 += coords->y1;
        }

        lv_area_t clip_com; /*Common area of mask and coords*/
        bool union_ok;
        union_ok = _lv_area_intersect(&clip_com, draw_ctx->clip_area, &map_area_rot);
        /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
        if(union_ok == false) {
            draw_cleanup(cdsc);
            return LV_RES_OK;
        }

        if(lv_img_cf_alpha_only_format(cdsc->dec_dsc.header.cf)) {
            lv_color32_t tex_col32 = {.full = lv_color_to32(cdsc->dec_dsc.color)};
            uint32_t tex_color = nema_rgba(tex_col32.ch.red, tex_col32.ch.green, tex_col32.ch.blue, 0);
            nema_set_tex_color(tex_color);
            cf = cdsc->dec_dsc.header.cf;
        }

        if(lv_img_cf_lut_format(cdsc->dec_dsc.header.cf)) {
            cf = cdsc->dec_dsc.header.cf;
        }

        const lv_area_t * clip_area_ori = draw_ctx->clip_area;
        draw_ctx->clip_area = &clip_com;
        uint8_t * img_data = (uint8_t *)(cdsc->dec_dsc.img_data) ? (uint8_t *)(cdsc->dec_dsc.img_data) : (uint8_t *)(((
                                                                                                                          lv_img_dsc_t *)src)->data);
        lv_draw_img_decoded(draw_ctx, draw_dsc, coords, img_data, cf);
        draw_ctx->clip_area = clip_area_ori;
    }
    /*The whole uncompressed image is not available. Try to read it line-by-line*/
    else {
        lv_area_t mask_com; /*Common area of mask and coords*/
        bool union_ok;
        union_ok = _lv_area_intersect(&mask_com, draw_ctx->clip_area, coords);
        /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
        if(union_ok == false) {
            return LV_RES_OK;
        }
        int32_t width = lv_area_get_width(&mask_com);
        uint8_t  * buf = lv_mem_buf_get(lv_area_get_width(&mask_com) *
                                        LV_IMG_PX_SIZE_ALPHA_BYTE);  /*+1 because of the possible alpha byte*/
        const lv_area_t * clip_area_ori = draw_ctx->clip_area;
        lv_area_t line;
        lv_area_copy(&line, &mask_com);
        lv_area_set_height(&line, 1);
        int32_t x = mask_com.x1 - coords->x1;
        int32_t y = mask_com.y1 - coords->y1;
        int32_t row;
        lv_res_t read_res;
        for(row = mask_com.y1; row <= mask_com.y2; row++) {
            lv_area_t mask_line;
            union_ok = _lv_area_intersect(&mask_line, clip_area_ori, &line);
            if(union_ok == false) continue;

            read_res = lv_img_decoder_read_line(&cdsc->dec_dsc, x, y, width, buf);
            if(read_res != LV_RES_OK) {
                lv_img_decoder_close(&cdsc->dec_dsc);
                LV_LOG_WARN("Image draw can't read the line");
                lv_mem_buf_release(buf);
                draw_cleanup(cdsc);
                draw_ctx->clip_area = clip_area_ori;
                return LV_RES_INV;
            }
            draw_ctx->clip_area = &mask_line;
            lv_draw_img_decoded(draw_ctx, draw_dsc, &line, buf, cf);
            line.y1++;
            line.y2++;
            y++;
        }
        draw_ctx->clip_area = clip_area_ori;
        lv_mem_buf_release(buf);
    }

    draw_cleanup(cdsc);
    return LV_RES_OK;
}

static void show_error(lv_draw_ctx_t * draw_ctx, const lv_area_t * coords, const char * msg)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_white();
    lv_draw_rect(draw_ctx, &rect_dsc, coords);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_draw_label(draw_ctx, &label_dsc, coords, msg, NULL);
}

static void draw_cleanup(_lv_img_cache_entry_t * cache)
{
    /*Automatically close images with no caching*/
#if LV_IMG_CACHE_DEF_SIZE == 0
    lv_img_decoder_close(&cache->dec_dsc);
#else
    LV_UNUSED(cache);
#endif
}

#endif