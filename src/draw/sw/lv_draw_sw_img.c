/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#include "../lv_img_cache.h"
#include "../../hal/lv_hal_disp.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_mem.h"
#include "../../misc/lv_math.h"

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

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_img(const lv_area_t * map_area, const lv_area_t * clip_area,
                                          const uint8_t * map_p,
                                          const lv_draw_img_dsc_t * draw_dsc,
                                          bool chroma_key, bool alpha_byte)
{
//    /*Use the clip area as draw area*/
//    lv_area_t draw_area;
//    lv_area_copy(&draw_area, clip_area);
//
//    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
//    lv_disp_draw_buf_t * draw_buf = lv_disp_get_draw_buf(disp);
//    const lv_area_t * disp_area = &draw_buf->area;
//
//    /*Now `draw_area` has absolute coordinates.
//     *Make it relative to `disp_area` to simplify draw to `disp_buf`*/
//    draw_area.x1 -= disp_area->x1;
//    draw_area.y1 -= disp_area->y1;
//    draw_area.x2 -= disp_area->x1;
//    draw_area.y2 -= disp_area->y1;
//
//    bool mask_any = lv_draw_mask_is_any(clip_area);
//
//    /*The simplest case just copy the pixels into the draw_buf*/
//    if(!mask_any && draw_dsc->angle == 0 && draw_dsc->zoom == LV_IMG_ZOOM_NONE &&
//       chroma_key == false && alpha_byte == false && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
//        lv_draw_blend_map(clip_area, map_area, (lv_color_t *)map_p, NULL, LV_DRAW_MASK_RES_FULL_COVER, draw_dsc->opa,
//                          draw_dsc->blend_mode);
//    }
//    /*In the other cases every pixel need to be checked one-by-one*/
//    else {
//        //#if LV_DRAW_COMPLEX
//        /*The pixel size in byte is different if an alpha byte is added too*/
//        uint8_t px_size_byte = alpha_byte ? LV_IMG_PX_SIZE_ALPHA_BYTE : sizeof(lv_color_t);
//
//        /*Go to the first displayed pixel of the map*/
//        int32_t map_w = lv_area_get_width(map_area);
//        const uint8_t * map_buf_tmp = map_p;
//        map_buf_tmp += map_w * (draw_area.y1 - (map_area->y1 - disp_area->y1)) * px_size_byte;
//        map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1)) * px_size_byte;
//
//        lv_color_t c;
//        lv_color_t chroma_keyed_color = LV_COLOR_CHROMA_KEY;
//        uint32_t px_i = 0;
//
//        const uint8_t * map_px;
//
//        lv_coord_t draw_area_h = lv_area_get_height(&draw_area);
//        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);
//
//        lv_area_t blend_area;
//        blend_area.x1 = draw_area.x1 + disp_area->x1;
//        blend_area.x2 = blend_area.x1 + draw_area_w - 1;
//        blend_area.y1 = disp_area->y1 + draw_area.y1;
//        blend_area.y2 = blend_area.y1;
//
//        bool transform = draw_dsc->angle != 0 || draw_dsc->zoom != LV_IMG_ZOOM_NONE ? true : false;
//        /*Simple ARGB image. Handle it as special case because it's very common*/
//        if(!mask_any && !transform && !chroma_key && draw_dsc->recolor_opa == LV_OPA_TRANSP && alpha_byte) {
//            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(disp);
//            uint32_t mask_buf_size = lv_area_get_size(&draw_area) > (uint32_t) hor_res ? hor_res : lv_area_get_size(&draw_area);
//            lv_color_t * map2 = lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));
//            lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);
//
//            int32_t x;
//            int32_t y;
//            for(y = 0; y < draw_area_h; y++) {
//                map_px = map_buf_tmp;
//                for(x = 0; x < draw_area_w; x++, map_px += px_size_byte, px_i++) {
//                    lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
//                    mask_buf[px_i] = px_opa;
//                    if(px_opa) {
//#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
//                        map2[px_i].full =  map_px[0];
//#elif LV_COLOR_DEPTH == 16
//                        map2[px_i].full =  map_px[0] + (map_px[1] << 8);
//#elif LV_COLOR_DEPTH == 32
//                        map2[px_i].full =  *((uint32_t *)map_px);
//#endif
//                    }
//#if LV_COLOR_DEPTH == 32
//                    map2[px_i].ch.alpha = 0xFF;
//#endif
//                }
//
//                map_buf_tmp += map_w * px_size_byte;
//                if(px_i + draw_area_w < mask_buf_size) {
//                    blend_area.y2 ++;
//                }
//                else {
//                    lv_draw_blend_map(clip_area, &blend_area, map2, mask_buf, LV_DRAW_MASK_RES_CHANGED, draw_dsc->opa,
//                                      draw_dsc->blend_mode);
//
//                    blend_area.y1 = blend_area.y2 + 1;
//                    blend_area.y2 = blend_area.y1;
//
//                    px_i = 0;
//                }
//            }
//            /*Flush the last part*/
//            if(blend_area.y1 != blend_area.y2) {
//                blend_area.y2--;
//                lv_draw_blend_map(clip_area, &blend_area, map2, mask_buf, LV_DRAW_MASK_RES_CHANGED, draw_dsc->opa,
//                                  draw_dsc->blend_mode);
//            }
//
//            lv_mem_buf_release(mask_buf);
//            lv_mem_buf_release(map2);
//        }
//        /*Most complicated case: transform or other mask or chroma keyed*/
//        else {
//            /*Build the image and a mask line-by-line*/
//            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(disp);
//            uint32_t mask_buf_size = lv_area_get_size(&draw_area) > hor_res ? hor_res : lv_area_get_size(&draw_area);
//            lv_color_t * map2 = lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));
//            lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);
//
//#if LV_DRAW_COMPLEX
//            lv_img_transform_dsc_t trans_dsc;
//            lv_memset_00(&trans_dsc, sizeof(lv_img_transform_dsc_t));
//            if(transform) {
//                lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;
//                if(alpha_byte) cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
//                else if(chroma_key) cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;
//
//                trans_dsc.cfg.angle = draw_dsc->angle;
//                trans_dsc.cfg.zoom = draw_dsc->zoom;
//                trans_dsc.cfg.src = map_p;
//                trans_dsc.cfg.src_w = map_w;
//                trans_dsc.cfg.src_h = lv_area_get_height(map_area);;
//                trans_dsc.cfg.cf = cf;
//                trans_dsc.cfg.pivot_x = draw_dsc->pivot.x;
//                trans_dsc.cfg.pivot_y = draw_dsc->pivot.y;
//                trans_dsc.cfg.color = draw_dsc->recolor;
//                trans_dsc.cfg.antialias = draw_dsc->antialias;
//
//                _lv_img_buf_transform_init(&trans_dsc);
//            }
//#endif
//            uint16_t recolor_premult[3] = {0};
//            lv_opa_t recolor_opa_inv = 255 - draw_dsc->recolor_opa;
//            if(draw_dsc->recolor_opa != 0) {
//                lv_color_premult(draw_dsc->recolor, draw_dsc->recolor_opa, recolor_premult);
//            }
//
//            lv_draw_mask_res_t mask_res;
//            mask_res = (alpha_byte || chroma_key || draw_dsc->angle ||
//                        draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
//
//            /*Prepare the `mask_buf`if there are other masks*/
//            if(mask_any) {
//                lv_memset_ff(mask_buf, mask_buf_size);
//            }
//
//            int32_t x;
//            int32_t y;
//#if LV_DRAW_COMPLEX
//            int32_t rot_y = disp_area->y1 + draw_area.y1 - map_area->y1;
//#endif
//            for(y = 0; y < draw_area_h; y++) {
//                map_px = map_buf_tmp;
//#if LV_DRAW_COMPLEX
//                uint32_t px_i_start = px_i;
//                int32_t rot_x = disp_area->x1 + draw_area.x1 - map_area->x1;
//#endif
//
//                for(x = 0; x < draw_area_w; x++, map_px += px_size_byte, px_i++) {
//
//#if LV_DRAW_COMPLEX
//                    if(transform) {
//
//                        /*Transform*/
//                        bool ret;
//                        ret = _lv_img_buf_transform(&trans_dsc, rot_x + x, rot_y + y);
//                        if(ret == false) {
//                            mask_buf[px_i] = LV_OPA_TRANSP;
//                            continue;
//                        }
//                        else {
//                            mask_buf[px_i] = trans_dsc.res.opa;
//                            c.full = trans_dsc.res.color.full;
//                        }
//                    }
//                    /*No transform*/
//                    else
//#endif
//                    {
//                        if(alpha_byte) {
//                            lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
//                            mask_buf[px_i] = px_opa;
//                            if(px_opa == 0) {
//#if  LV_COLOR_DEPTH == 32
//                                map2[px_i].full = 0;
//#endif
//                                continue;
//                            }
//                        }
//                        else {
//                            mask_buf[px_i] = 0xFF;
//                        }
//
//#if LV_COLOR_DEPTH == 1
//                        c.full = map_px[0];
//#elif LV_COLOR_DEPTH == 8
//                        c.full =  map_px[0];
//#elif LV_COLOR_DEPTH == 16
//                        c.full =  map_px[0] + (map_px[1] << 8);
//#elif LV_COLOR_DEPTH == 32
//                        c.full =  *((uint32_t *)map_px);
//                        c.ch.alpha = 0xFF;
//#endif
//                        if(chroma_key) {
//                            if(c.full == chroma_keyed_color.full) {
//                                mask_buf[px_i] = LV_OPA_TRANSP;
//#if  LV_COLOR_DEPTH == 32
//                                map2[px_i].full = 0;
//#endif
//                                continue;
//                            }
//                        }
//
//                    }
//                    if(draw_dsc->recolor_opa != 0) {
//                        c = lv_color_mix_premult(recolor_premult, c, recolor_opa_inv);
//                    }
//
//                    map2[px_i].full = c.full;
//                }
//#if LV_DRAW_COMPLEX
//                /*Apply the masks if any*/
//                if(mask_any) {
//                    lv_draw_mask_res_t mask_res_sub;
//                    mask_res_sub = lv_draw_mask_apply(mask_buf + px_i_start, draw_area.x1 + draw_buf->area.x1,
//                                                      y + draw_area.y1 + draw_buf->area.y1,
//                                                      draw_area_w);
//                    if(mask_res_sub == LV_DRAW_MASK_RES_TRANSP) {
//                        lv_memset_00(mask_buf + px_i_start, draw_area_w);
//                        mask_res = LV_DRAW_MASK_RES_CHANGED;
//                    }
//                    else if(mask_res_sub == LV_DRAW_MASK_RES_CHANGED) {
//                        mask_res = LV_DRAW_MASK_RES_CHANGED;
//                    }
//                }
//#endif
//
//                map_buf_tmp += map_w * px_size_byte;
//                if(px_i + draw_area_w < mask_buf_size) {
//                    blend_area.y2 ++;
//                }
//                else {
//
//                    lv_draw_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, draw_dsc->opa, draw_dsc->blend_mode);
//
//                    blend_area.y1 = blend_area.y2 + 1;
//                    blend_area.y2 = blend_area.y1;
//
//                    px_i = 0;
//                    mask_res = (alpha_byte || chroma_key || draw_dsc->angle ||
//                                draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
//
//                    /*Prepare the `mask_buf`if there are other masks*/
//                    if(mask_any) {
//                        lv_memset_ff(mask_buf, mask_buf_size);
//                    }
//                }
//            }
//
//            /*Flush the last part*/
//            if(blend_area.y1 != blend_area.y2) {
//                blend_area.y2--;
//                lv_draw_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, draw_dsc->opa, draw_dsc->blend_mode);
//            }
//
//            lv_mem_buf_release(mask_buf);
//            lv_mem_buf_release(map2);
//        }
//    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
