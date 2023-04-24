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
#include "../../core/lv_disp.h"
#include "../../core/lv_disp_private.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_mem.h"
#include "../../misc/lv_math.h"
#include LV_COLOR_EXTERN_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define MAX_BUF_SIZE (uint32_t) lv_disp_get_hor_res(_lv_refr_get_disp_refreshing())

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void convert_cb(const lv_area_t * dest_area, const void * src_buf, lv_coord_t src_w, lv_coord_t src_h,
                       lv_coord_t src_stride, const lv_draw_img_sup_t * sup, lv_color_format_t cf, lv_color_t * cbuf, lv_opa_t * abuf);

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
    lv_img_dsc_t img_dsc;
    img_dsc.header.w = lv_area_get_width(&layer_to_draw->buf_area);
    img_dsc.header.h = lv_area_get_height(&layer_to_draw->buf_area);
    img_dsc.header.cf = layer_to_draw->color_format;
    img_dsc.header.always_zero = 0;
    img_dsc.header.chroma_keyed = 0;
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
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(layer_to_draw->color_format == LV_COLOR_FORMAT_ARGB8888 ? 0xff0000 : 0x00ff00);
    rect_dsc.border_color = rect_dsc.bg_color;
    rect_dsc.bg_opa = LV_OPA_20;
    rect_dsc.border_opa = LV_OPA_60;
    rect_dsc.border_width = 2;
    lv_draw_sw_rect(draw_unit, &rect_dsc, &area_rot);
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
    lv_img_dsc_t * img_dsc = draw_dsc->src;
    const uint8_t * src_buf = img_dsc->data;

    lv_color_format_t cf = img_dsc->header.cf;
    lv_draw_img_sup_t sup;

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

    const lv_area_t * clip_area_ori = draw_unit->clip_area;
    draw_unit->clip_area = &draw_area;

    bool transformed = draw_dsc->angle != 0 || draw_dsc->zoom != LV_ZOOM_NONE ? true : false;
    bool masked = (draw_dsc->bitmap_mask && _lv_area_is_on(&draw_dsc->bitmap_mask_area, &transformed_area)) ||
                  _lv_area_is_on(&draw_dsc->rectangle_mask_area, &transformed_area);


    lv_draw_sw_blend_dsc_t blend_dsc;

    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;

    /*The simplest case just copy the pixels into the draw_buf*/
    if(!masked && !transformed && cf == LV_COLOR_FORMAT_NATIVE && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        blend_dsc.src_buf = (const lv_color_t *)src_buf;
        blend_dsc.blend_area = coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && cf == LV_COLOR_FORMAT_A8) {
        lv_area_t clipped_coords;
        if(!_lv_area_intersect(&clipped_coords, coords, draw_unit->clip_area)) return;

        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_area = coords;
        blend_dsc.src_buf = NULL;
        blend_dsc.color = sup.alpha_color;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        blend_dsc.blend_area = coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!masked && !transformed && cf == LV_COLOR_FORMAT_RGB565A8 && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        lv_coord_t src_w = lv_area_get_width(coords);
        lv_coord_t src_h = lv_area_get_height(coords);
        blend_dsc.src_buf = (const lv_color_t *)src_buf;
        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_buf += sizeof(lv_color_t) * src_w * src_h;
        blend_dsc.blend_area = coords;
        blend_dsc.mask_area = coords;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        lv_area_t blend_area;
        blend_dsc.blend_area = &blend_area;
        blend_area.x1 = draw_unit->clip_area->x1;
        blend_area.x2 = draw_unit->clip_area->x2;
        blend_area.y1 = draw_unit->clip_area->y1;
        blend_area.y2 = draw_unit->clip_area->y2;

        lv_coord_t src_w = lv_area_get_width(coords);
        lv_coord_t src_h = lv_area_get_height(coords);
        lv_coord_t blend_h = lv_area_get_height(&blend_area);
        lv_coord_t blend_w = lv_area_get_width(&blend_area);

        uint32_t max_buf_size = MAX_BUF_SIZE;
        uint32_t blend_size = lv_area_get_size(&blend_area);
        uint32_t buf_h;
        uint32_t buf_w = blend_w;
        if(blend_size <= max_buf_size) {
            buf_h = blend_h;
        }
        else {
            /*Round to full lines*/
            buf_h = max_buf_size / blend_w;
        }

        /*Create buffers and masks*/
        uint32_t buf_size = buf_w * buf_h;

        lv_color_t * rgb_buf = lv_malloc(buf_size * sizeof(lv_color_t));
        lv_opa_t * mask_buf = lv_malloc(buf_size);
        blend_dsc.mask_buf = mask_buf;
        blend_dsc.mask_area = &blend_area;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.src_buf = rgb_buf;
        lv_coord_t y_last = blend_area.y2;
        blend_area.y2 = blend_area.y1 + buf_h - 1;

        lv_draw_sw_mask_res_t mask_res_def = (cf != LV_COLOR_FORMAT_NATIVE || draw_dsc->angle ||
                                              draw_dsc->zoom != LV_ZOOM_NONE) ?
                                             LV_DRAW_SW_MASK_RES_CHANGED : LV_DRAW_SW_MASK_RES_FULL_COVER;
        blend_dsc.mask_res = mask_res_def;

        if(cf == LV_COLOR_FORMAT_A8) {
            lv_color_buf_fill(rgb_buf, draw_dsc->recolor, buf_size);
        }

        while(blend_area.y1 <= y_last) {
            /*Apply transformations if any or separate the channels*/
            lv_area_t transform_area;
            lv_area_copy(&transform_area, &blend_area);
            lv_area_move(&transform_area, -coords->x1, -coords->y1);
            if(transformed) {
                lv_draw_sw_transform(draw_unit, &transform_area, src_buf, src_w, src_h, src_w,
                                     draw_dsc, &sup, cf, rgb_buf, mask_buf);
            }
            else {
                convert_cb(&transform_area, src_buf, src_w, src_h, src_w, &sup, cf, rgb_buf, mask_buf);
            }

            /*Apply recolor*/
            if(draw_dsc->recolor_opa > LV_OPA_MIN) {
                uint16_t premult_v[3];
                lv_opa_t recolor_opa = draw_dsc->recolor_opa;
                lv_color_t recolor = draw_dsc->recolor;
                LV_COLOR_PREMULT(recolor, recolor_opa, premult_v);
                recolor_opa = 255 - recolor_opa;
                uint32_t i;
                for(i = 0; i < buf_size; i++) {
                    rgb_buf[i] = LV_COLOR_MIX_PREMULT(premult_v, rgb_buf[i], recolor_opa);
                }
            }

            /*Blend*/
            lv_draw_sw_blend(draw_unit, &blend_dsc);

            /*Go the the next lines*/
            blend_area.y1 = blend_area.y2 + 1;
            blend_area.y2 = blend_area.y1 + buf_h - 1;
            if(blend_area.y2 > y_last) blend_area.y2 = y_last;
        }

        lv_free(mask_buf);
        lv_free(rgb_buf);
    }
    draw_unit->clip_area = clip_area_ori;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Separate the image channels to RGB and Alpha to match LV_COLOR_DEPTH settings*/
static void convert_cb(const lv_area_t * dest_area, const void * src_buf, lv_coord_t src_w, lv_coord_t src_h,
                       lv_coord_t src_stride, const lv_draw_img_sup_t * sup, lv_color_format_t cf, lv_color_t * cbuf, lv_opa_t * abuf)
{
    LV_UNUSED(src_h);
    LV_UNUSED(src_w);

    const uint8_t * src_tmp8 = (const uint8_t *)src_buf;
    lv_color_t * cbuf_ori = cbuf;
    lv_opa_t * abuf_ori = abuf;

    lv_coord_t y;
    lv_coord_t x;

    /*Just get the colors from dest_area*/
    if(cf == LV_COLOR_FORMAT_NATIVE || cf == LV_COLOR_FORMAT_NATIVE_CHROMA_KEYED) {
        uint32_t px_cnt = lv_area_get_size(dest_area);
        lv_memset(abuf, 0xff, px_cnt);

        src_tmp8 += (src_stride * dest_area->y1 * sizeof(lv_color_t)) + dest_area->x1 * sizeof(lv_color_t);
        uint32_t dest_w = lv_area_get_width(dest_area);
        uint32_t dest_w_byte = dest_w * sizeof(lv_color_t);

        lv_coord_t src_stride_byte = src_stride * sizeof(lv_color_t);
        lv_color_t * cbuf_tmp = cbuf;
        for(y = dest_area->y1; y <= dest_area->y2; y++) {
            lv_memcpy(cbuf_tmp, src_tmp8, dest_w_byte);
            src_tmp8 += src_stride_byte;
            cbuf_tmp += dest_w;
        }
    }
#if LV_COLOR_DEPTH == 16
    else if(cf == LV_COLOR_FORMAT_RGB565A8) {
        src_tmp8 += (src_stride * dest_area->y1 * sizeof(lv_color_t)) + dest_area->x1 * sizeof(lv_color_t);

        lv_coord_t src_stride_byte = src_stride * sizeof(lv_color_t);

        lv_coord_t dest_h = lv_area_get_height(dest_area);
        lv_coord_t dest_w = lv_area_get_width(dest_area);
        for(y = 0; y < dest_h; y++) {
            lv_memcpy(cbuf, src_tmp8, dest_w * sizeof(lv_color_t));
            cbuf += dest_w;
            src_tmp8 += src_stride_byte;
        }

        src_tmp8 = (const uint8_t *)src_buf;
        src_tmp8 += sizeof(lv_color_t) * src_w * src_h;
        src_tmp8 += src_stride * dest_area->y1 + dest_area->x1;
        for(y = 0; y < dest_h; y++) {
            lv_memcpy(abuf, src_tmp8, dest_w);
            abuf += dest_w;
            src_tmp8 += src_stride;
        }
    }
#endif
    else if(cf >= LV_COLOR_FORMAT_I1 && cf <= LV_COLOR_FORMAT_I8) {
        uint32_t shift = cf - LV_COLOR_FORMAT_I1;   /*0,    1,      2,      3*/
        uint32_t bits = 1 << shift;                 /*1,    2,      4,      8*/
        uint32_t mask = (1 << bits) - 1;            /*0x1,  0x03,   0x0F,   0xFF*/
        uint32_t bit_rev = 8 - (1 << shift);        /*7,    6,      4,      0*/
        uint32_t src_stride_byte = (src_stride + ((1 << (3 - shift)) - 1)) >> (3 - shift);  /*E.g.(src_stride + 7) / 8*/
        lv_coord_t dest_h = lv_area_get_height(dest_area);
        lv_coord_t dest_w = lv_area_get_width(dest_area);
        const lv_color32_t * palette = sup->palette;
        for(y = 0; y < dest_h; y++) {
            if(cf == LV_COLOR_FORMAT_I8) {
                const uint8_t * src_tmp8_line = src_tmp8 + (src_stride_byte * (dest_area->y1 + y)) + dest_area->x1;
                for(x = 0; x < dest_w; x++) {
                    lv_color32_t c = palette[*src_tmp8_line];
                    cbuf[x] = lv_color_make(c.red, c.green, c.blue);
                    abuf[x] = c.alpha;
                    src_tmp8_line++;
                }
            }
            else {
                const uint8_t * src_tmp8_line = src_tmp8 + (src_stride_byte * (dest_area->y1 + y));
                src_tmp8_line += dest_area->x1 >> (3 - shift);
                uint32_t bit_curr = dest_area->x1 & ((1 << (3 - shift)) - 1);
                bit_curr *= bits;
                for(x = 0; x < dest_w; x++) {
                    uint32_t idx = *src_tmp8_line & (mask << (bit_rev - bit_curr)); /*`7 -` because MSB is the left pixel*/
                    idx =  idx >> (bit_rev - bit_curr);
                    lv_color32_t c = palette[idx];
                    cbuf[x] = lv_color_make(c.red, c.green, c.blue);
                    abuf[x] = c.alpha;
                    bit_curr += bits;
                    if(bit_curr == 8) {
                        bit_curr = 0;
                        src_tmp8_line++;
                    }
                }
            }
            cbuf += dest_w;
            abuf += dest_w;
        }
    }
    /*Use the generic color convert functions*/
    else {
        uint8_t px_size = lv_color_format_get_size(cf);
        src_tmp8 += (src_stride * dest_area->y1 * px_size) + dest_area->x1 * px_size;

        lv_coord_t dest_h = lv_area_get_height(dest_area);
        lv_coord_t dest_w = lv_area_get_width(dest_area);
        for(y = 0; y < dest_h; y++) {
            lv_color_buf_to_native(src_tmp8, cf, cbuf, abuf, sup->alpha_color, dest_w);
            cbuf += dest_w;
            abuf += dest_w;
            src_tmp8 += src_stride * px_size;
        }
    }

    bool chroma_keyed = sup->chroma_keyed;
    lv_color_t chroma_key_color = sup->chroma_key_color;
    if(chroma_keyed) {
        uint32_t size = lv_area_get_size(dest_area);
        uint32_t i;
        for(i = 0; i < size; i++) {
            if(lv_color_eq(cbuf_ori[i], chroma_key_color)) abuf_ori[i] = 0x00;
        }
    }
}

#endif /*LV_USE_DRAW_SW*/
