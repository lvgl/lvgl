/**
 * @file lv_draw_sw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../../display/lv_display.h"
#include "../../display/lv_display_private.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "arm2d/lv_draw_sw_helium.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/
#define MAX_BUF_SIZE (uint32_t) (4 * lv_display_get_horizontal_resolution(_lv_refr_get_disp_refreshing()) * lv_color_format_get_size(lv_display_get_color_format(_lv_refr_get_disp_refreshing())))

#ifndef LV_DRAW_SW_IMAGE
    #define LV_DRAW_SW_IMAGE(...)   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_RECOLOR
    #define LV_DRAW_SW_RGB565_RECOLOR(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_RECOLOR
    #define LV_DRAW_SW_RGB888_RECOLOR(...)  LV_RESULT_INVALID
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void img_draw_core(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                          const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

/**********************
 *  STATIC VARIABLES
 **********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if(layer_to_draw->draw_buf == NULL) return;

    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;
    lv_draw_sw_image(draw_unit, &new_draw_dsc, coords);
#if LV_USE_LAYER_DEBUG || LV_USE_PARALLEL_DRAW_DEBUG
    lv_area_t area_rot;
    lv_area_copy(&area_rot, coords);
    if(draw_dsc->rotation || draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE) {
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

void lv_draw_sw_image(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                      const lv_area_t * coords)
{
    if(!draw_dsc->tile) {
        _lv_draw_image_normal_helper(draw_unit, draw_dsc, coords, img_draw_core);
    }
    else {
        _lv_draw_image_tiled_helper(draw_unit, draw_dsc, coords, img_draw_core);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void img_draw_core(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                          const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    bool transformed = draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                       draw_dsc->scale_y != LV_SCALE_NONE ? true : false;

    bool masked = draw_dsc->bitmap_mask_src != NULL;

    lv_draw_sw_blend_dsc_t blend_dsc;
    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    const uint8_t * src_buf = decoded->data;
    const lv_image_header_t * header = &decoded->header;
    uint32_t img_stride = decoded->header.stride;
    lv_color_format_t cf = decoded->header.cf;

    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;
    blend_dsc.src_stride = img_stride;

    if(!transformed && !masked && cf == LV_COLOR_FORMAT_A8) {
        lv_area_t clipped_coords;
        if(!_lv_area_intersect(&clipped_coords, img_coords, draw_unit->clip_area)) return;

        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_area = img_coords;
        blend_dsc.mask_stride = img_stride;
        blend_dsc.src_buf = NULL;
        blend_dsc.color = draw_dsc->recolor;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        blend_dsc.blend_area = img_coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && !masked && cf == LV_COLOR_FORMAT_RGB565A8 && draw_dsc->recolor_opa <= LV_OPA_MIN) {
        int32_t src_h = lv_area_get_height(img_coords);
        int32_t src_w = lv_area_get_width(img_coords);
        blend_dsc.src_area = img_coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_buf += img_stride * src_w / header->w * src_h;
        /**
         * Note, for RGB565A8, lacking of stride parameter, we always use
         * always half of RGB map stride as alpha map stride. The image should
         * be generated in this way too.
         */
        blend_dsc.mask_stride = img_stride / 2;
        blend_dsc.blend_area = img_coords;
        blend_dsc.mask_area = img_coords;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*The simplest case just copy the pixels into the draw_buf. Blending will convert the colors if needed*/
    else if(!transformed && !masked && draw_dsc->recolor_opa <= LV_OPA_MIN) {
        blend_dsc.src_area = img_coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.blend_area = img_coords;
        blend_dsc.src_color_format = cf;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*Handle masked RGB565, RGB888, XRGB888, or ARGB8888 images*/
    else if(!transformed && masked && draw_dsc->recolor_opa <= LV_OPA_MIN) {
        blend_dsc.src_area = img_coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.blend_area = img_coords;
        blend_dsc.src_color_format = cf;
        blend_dsc.mask_buf = draw_dsc->bitmap_mask_src->data;
        blend_dsc.mask_stride = draw_dsc->bitmap_mask_src->header.stride;

        const lv_area_t * original_area;
        if(lv_area_get_width(&draw_dsc->original_area) < 0) original_area = img_coords;
        else original_area = &draw_dsc->original_area;

        lv_area_t a = {0, 0, draw_dsc->bitmap_mask_src->header.w - 1, draw_dsc->bitmap_mask_src->header.h - 1};
        lv_area_align(original_area, &a, LV_ALIGN_CENTER, 0, 0);
        blend_dsc.mask_area = &a;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /* check whether it is possible to accelerate the operation in synchronouse mode */
    else if(LV_RESULT_INVALID == LV_DRAW_SW_IMAGE(transformed,      /* whether require transform */
                                                  cf,               /* image format */
                                                  src_buf,          /* image buffer */
                                                  img_coords,       /* src_h, src_w, src_x1, src_y1 */
                                                  img_stride,       /* image stride */
                                                  clipped_img_area, /* blend area */
                                                  draw_unit,        /* target buffer, buffer width, buffer height, buffer stride */
                                                  draw_dsc)) {      /* opa, recolour_opa and colour */
        /*In the other cases every pixel need to be checked one-by-one*/

        lv_area_t blend_area = *clipped_img_area;
        blend_dsc.blend_area = &blend_area;

        int32_t src_w = lv_area_get_width(img_coords);
        int32_t src_h = lv_area_get_height(img_coords);
        int32_t blend_w = lv_area_get_width(&blend_area);
        int32_t blend_h = lv_area_get_height(&blend_area);

        lv_color_format_t cf_final = cf;
        if(transformed) {
            if(cf == LV_COLOR_FORMAT_RGB888 || cf == LV_COLOR_FORMAT_XRGB8888) cf_final = LV_COLOR_FORMAT_ARGB8888;
            else if(cf == LV_COLOR_FORMAT_RGB565) cf_final = LV_COLOR_FORMAT_RGB565A8;
        }

        uint8_t * tmp_buf;
        uint32_t px_size = lv_color_format_get_size(cf_final);
        int32_t buf_h;
        if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
            uint32_t buf_stride = blend_w * 3;
            buf_h = MAX_BUF_SIZE / buf_stride;
            if(buf_h > blend_h) buf_h = blend_h;
            tmp_buf = lv_malloc(buf_stride * buf_h);
        }
        else {
            uint32_t buf_stride = blend_w * lv_color_format_get_size(cf_final);
            buf_h = MAX_BUF_SIZE / buf_stride;
            if(buf_h > blend_h) buf_h = blend_h;
            tmp_buf = lv_malloc(buf_stride * buf_h);
        }
        LV_ASSERT_MALLOC(tmp_buf);

        blend_dsc.src_buf = tmp_buf;
        blend_dsc.src_color_format = cf_final;
        int32_t y_last = blend_area.y2;
        blend_area.y2 = blend_area.y1 + buf_h - 1;

        blend_dsc.src_area = &blend_area;
        if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
            /*RGB565A8 images will blended as RGB565 + mask
             *Therefore the stride can be different. */
            blend_dsc.src_stride = blend_w * 2;
            blend_dsc.mask_buf =  tmp_buf + blend_w * 2 * buf_h;
            blend_dsc.mask_stride = blend_w;
            blend_dsc.mask_area = &blend_area;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        }
        else if(cf_final == LV_COLOR_FORMAT_A8) {
            blend_dsc.mask_buf = blend_dsc.src_buf;
            blend_dsc.mask_stride = blend_w;
            blend_dsc.mask_area = &blend_area;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            blend_dsc.color = draw_dsc->recolor;
            blend_dsc.src_buf = NULL;
        }
        else {
            blend_dsc.src_stride = blend_w * lv_color_format_get_size(cf_final);
        }

        while(blend_area.y1 <= y_last) {
            /*Apply transformations if any or separate the channels*/
            lv_area_t relative_area;
            lv_area_copy(&relative_area, &blend_area);
            lv_area_move(&relative_area, -img_coords->x1, -img_coords->y1);
            if(transformed) {
                lv_draw_sw_transform(draw_unit, &relative_area, src_buf, src_w, src_h, img_stride,
                                     draw_dsc, sup, cf, tmp_buf);
            }
            else if(draw_dsc->recolor_opa >= LV_OPA_MIN) {
                int32_t h = lv_area_get_height(&relative_area);
                if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
                    uint32_t stride_px = img_stride / 2;
                    const uint8_t * rgb_src_buf = src_buf + stride_px * 2 * relative_area.y1 + relative_area.x1 * 2;
                    const uint8_t * a_src_buf = src_buf + stride_px * 2 * src_h + stride_px * relative_area.y1 +
                                                relative_area.x1;
                    uint8_t * rgb_dest_buf = tmp_buf;
                    uint8_t * a_dest_buf = (uint8_t *)blend_dsc.mask_buf;
                    int32_t i;
                    for(i = 0; i < h; i++) {
                        lv_memcpy(rgb_dest_buf, rgb_src_buf, blend_w * 2);
                        lv_memcpy(a_dest_buf, a_src_buf, blend_w);
                        rgb_src_buf += stride_px * 2;
                        a_src_buf += stride_px;
                        rgb_dest_buf +=  blend_w * 2;
                        a_dest_buf += blend_w;
                    }
                }
                else if(cf_final != LV_COLOR_FORMAT_A8) {
                    const uint8_t * src_buf_tmp = src_buf + img_stride * relative_area.y1 + relative_area.x1 * px_size;
                    uint8_t * dest_buf_tmp = tmp_buf;
                    int32_t i;
                    for(i = 0; i < h; i++) {
                        lv_memcpy(dest_buf_tmp, src_buf_tmp, blend_w * px_size);
                        dest_buf_tmp += blend_w * px_size;
                        src_buf_tmp += img_stride;
                    }
                }
            }

            /*Apply recolor*/
            if(draw_dsc->recolor_opa > LV_OPA_MIN) {
                lv_color_t color = draw_dsc->recolor;
                lv_opa_t mix = draw_dsc->recolor_opa;
                lv_opa_t mix_inv = 255 - mix;
                if(cf_final == LV_COLOR_FORMAT_RGB565A8 || cf_final == LV_COLOR_FORMAT_RGB565) {
                    if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_RECOLOR(tmp_buf, blend_area, color, mix)) {
                        uint16_t c_mult[3];
                        c_mult[0] = (color.blue >> 3) * mix;
                        c_mult[1] = (color.green >> 2) * mix;
                        c_mult[2] = (color.red >> 3) * mix;
                        uint16_t * buf16 = (uint16_t *)tmp_buf;
                        int32_t i;
                        int32_t size = lv_area_get_size(&blend_area);
                        for(i = 0; i < size; i++) {
                            buf16[i] = (((c_mult[2] + ((buf16[i] >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
                                       (((c_mult[1] + ((buf16[i] >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
                                       ((c_mult[0] + (buf16[i] & 0x1F) * mix_inv) >> 8);
                        }
                    }
                }
                else  if(cf_final != LV_COLOR_FORMAT_A8) {
                    if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_RECOLOR(tmp_buf, blend_area, color, mix, cf_final)) {
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
            }

            /*Blend*/
            lv_draw_sw_blend(draw_unit, &blend_dsc);

            /*Go to the next area*/
            blend_area.y1 = blend_area.y2 + 1;
            blend_area.y2 = blend_area.y1 + buf_h - 1;
            if(blend_area.y2 > y_last) {
                blend_area.y2 = y_last;
                if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
                    blend_dsc.mask_buf =  tmp_buf + blend_w * 2 * lv_area_get_height(&blend_area);
                }
            }
        }

        lv_free(tmp_buf);
    }
}

#endif /*LV_USE_DRAW_SW*/
