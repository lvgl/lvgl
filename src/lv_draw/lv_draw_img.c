/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_img.h"
#include "lv_img_cache.h"
#include "../lv_hal/lv_hal_disp.h"
#include "../lv_misc/lv_log.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_math.h"
#if LV_USE_GPU_STM32_DMA2D
    #include "../lv_gpu/lv_gpu_stm32_dma2d.h"
#elif LV_USE_GPU_NXP_PXP
    #include "../lv_gpu/lv_gpu_nxp_pxp.h"
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
LV_ATTRIBUTE_FAST_MEM static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * clip_area,
                                                       const void * src,
                                                       const lv_draw_img_dsc_t * draw_dsc);

LV_ATTRIBUTE_FAST_MEM static void lv_draw_map(const lv_area_t * map_area, const lv_area_t * clip_area,
                                              const uint8_t * map_p,
                                              const lv_draw_img_dsc_t * draw_dsc,
                                              bool chroma_key, bool alpha_byte);

static void show_error(const lv_area_t * coords, const lv_area_t * clip_area, const char * msg);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_img_dsc_init(lv_draw_img_dsc_t * dsc)
{
    _lv_memset_00(dsc, sizeof(lv_draw_img_dsc_t));
    dsc->recolor = LV_COLOR_BLACK;
    dsc->opa = LV_OPA_COVER;
    dsc->zoom = LV_IMG_ZOOM_NONE;
    dsc->antialias = LV_ANTIALIAS;

}

/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask, const void * src, const lv_draw_img_dsc_t * dsc)
{
    if(src == NULL) {
        LV_LOG_WARN("Image draw: src is NULL");
        show_error(coords, mask, "No\ndata");
        return;
    }

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_res_t res;
    res = lv_img_draw_core(coords, mask, src, dsc);

    if(res == LV_RES_INV) {
        LV_LOG_WARN("Image draw error");
        show_error(coords, mask, "No\ndata");
        return;
    }
}

/**
 * Get the pixel size of a color format in bits
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return the pixel size in bits
 */
uint8_t lv_img_cf_get_px_size(lv_img_cf_t cf)
{
    uint8_t px_size = 0;

    switch(cf) {
        case LV_IMG_CF_UNKNOWN:
        case LV_IMG_CF_RAW:
            px_size = 0;
            break;
        case LV_IMG_CF_TRUE_COLOR:
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
            px_size = LV_COLOR_SIZE;
            break;
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
            px_size = LV_IMG_PX_SIZE_ALPHA_BYTE << 3;
            break;
        case LV_IMG_CF_INDEXED_1BIT:
        case LV_IMG_CF_ALPHA_1BIT:
            px_size = 1;
            break;
        case LV_IMG_CF_INDEXED_2BIT:
        case LV_IMG_CF_ALPHA_2BIT:
            px_size = 2;
            break;
        case LV_IMG_CF_INDEXED_4BIT:
        case LV_IMG_CF_ALPHA_4BIT:
            px_size = 4;
            break;
        case LV_IMG_CF_INDEXED_8BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            px_size = 8;
            break;
        default:
            px_size = 0;
            break;
    }

    return px_size;
}

/**
 * Check if a color format is chroma keyed or not
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return true: chroma keyed; false: not chroma keyed
 */
bool lv_img_cf_is_chroma_keyed(lv_img_cf_t cf)
{
    bool is_chroma_keyed = false;

    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
        case LV_IMG_CF_RAW_CHROMA_KEYED:
        case LV_IMG_CF_INDEXED_1BIT:
        case LV_IMG_CF_INDEXED_2BIT:
        case LV_IMG_CF_INDEXED_4BIT:
        case LV_IMG_CF_INDEXED_8BIT:
            is_chroma_keyed = true;
            break;

        default:
            is_chroma_keyed = false;
            break;
    }

    return is_chroma_keyed;
}

/**
 * Check if a color format has alpha channel or not
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return true: has alpha channel; false: doesn't have alpha channel
 */
bool lv_img_cf_has_alpha(lv_img_cf_t cf)
{
    bool has_alpha = false;

    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_RAW_ALPHA:
        case LV_IMG_CF_INDEXED_1BIT:
        case LV_IMG_CF_INDEXED_2BIT:
        case LV_IMG_CF_INDEXED_4BIT:
        case LV_IMG_CF_INDEXED_8BIT:
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            has_alpha = true;
            break;
        default:
            has_alpha = false;
            break;
    }

    return has_alpha;
}

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. LV_SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 */
lv_img_src_t lv_img_src_get_type(const void * src)
{
    lv_img_src_t img_src_type = LV_IMG_SRC_UNKNOWN;

    if(src == NULL) return img_src_type;
    const uint8_t * u8_p = src;

    /*The first byte shows the type of the image source*/
    if(u8_p[0] >= 0x20 && u8_p[0] <= 0x7F) {
        img_src_type = LV_IMG_SRC_FILE; /*If it's an ASCII character then it's file name*/
    }
    else if(u8_p[0] >= 0x80) {
        img_src_type = LV_IMG_SRC_SYMBOL; /*Symbols begins after 0x7F*/
    }
    else {
        img_src_type = LV_IMG_SRC_VARIABLE; /*`lv_img_dsc_t` is design to the first byte < 0x20*/
    }

    if(LV_IMG_SRC_UNKNOWN == img_src_type) {
        LV_LOG_WARN("lv_img_src_get_type: unknown image type");
    }

    return img_src_type;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * clip_area,
                                                       const void * src,
                                                       const lv_draw_img_dsc_t * draw_dsc)
{
    if(draw_dsc->opa <= LV_OPA_MIN) return LV_RES_OK;

    lv_img_cache_entry_t * cdsc = _lv_img_cache_open(src, draw_dsc->recolor);

    if(cdsc == NULL) return LV_RES_INV;

    bool chroma_keyed = lv_img_cf_is_chroma_keyed(cdsc->dec_dsc.header.cf);
    bool alpha_byte   = lv_img_cf_has_alpha(cdsc->dec_dsc.header.cf);

    if(cdsc->dec_dsc.error_msg != NULL) {
        LV_LOG_WARN("Image draw error");

        show_error(coords, clip_area, cdsc->dec_dsc.error_msg);
    }
    /* The decoder could open the image and gave the entire uncompressed image.
     * Just draw it!*/
    else if(cdsc->dec_dsc.img_data) {
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

        lv_area_t mask_com; /*Common area of mask and coords*/
        bool union_ok;
        union_ok = _lv_area_intersect(&mask_com, clip_area, &map_area_rot);
        if(union_ok == false) {
            return LV_RES_OK; /*Out of mask. There is nothing to draw so the image is drawn
                                 successfully.*/
        }

        lv_draw_map(coords, &mask_com, cdsc->dec_dsc.img_data, draw_dsc, chroma_keyed, alpha_byte);
    }
    /* The whole uncompressed image is not available. Try to read it line-by-line*/
    else {
        lv_area_t mask_com; /*Common area of mask and coords*/
        bool union_ok;
        union_ok = _lv_area_intersect(&mask_com, clip_area, coords);
        if(union_ok == false) {
            return LV_RES_OK; /*Out of mask. There is nothing to draw so the image is drawn
                                 successfully.*/
        }

        int32_t width = lv_area_get_width(&mask_com);

        uint8_t  * buf = _lv_mem_buf_get(lv_area_get_width(&mask_com) *
                                         LV_IMG_PX_SIZE_ALPHA_BYTE);  /*+1 because of the possible alpha byte*/

        lv_area_t line;
        lv_area_copy(&line, &mask_com);
        lv_area_set_height(&line, 1);
        int32_t x = mask_com.x1 - coords->x1;
        int32_t y = mask_com.y1 - coords->y1;
        int32_t row;
        lv_res_t read_res;
        for(row = mask_com.y1; row <= mask_com.y2; row++) {
            lv_area_t mask_line;
            union_ok = _lv_area_intersect(&mask_line, clip_area, &line);
            if(union_ok == false) continue;

            read_res = lv_img_decoder_read_line(&cdsc->dec_dsc, x, y, width, buf);
            if(read_res != LV_RES_OK) {
                lv_img_decoder_close(&cdsc->dec_dsc);
                LV_LOG_WARN("Image draw can't read the line");
                _lv_mem_buf_release(buf);
                return LV_RES_INV;
            }


            lv_draw_map(&line, &mask_line, buf, draw_dsc, chroma_keyed, alpha_byte);
            line.y1++;
            line.y2++;
            y++;
        }
        _lv_mem_buf_release(buf);
    }

    return LV_RES_OK;
}

/**
 * Draw a color map to the display (image)
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area  (truncated to VDB area)
 * @param map_p pointer to a lv_color_t array
 * @param draw_dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 * @param chroma_keyed true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param alpha_byte true: extra alpha byte is inserted for every pixel
 */
LV_ATTRIBUTE_FAST_MEM static void lv_draw_map(const lv_area_t * map_area, const lv_area_t * clip_area,
                                              const uint8_t * map_p,
                                              const lv_draw_img_dsc_t * draw_dsc,
                                              bool chroma_key, bool alpha_byte)
{
    /* Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, clip_area);

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);
    const lv_area_t * disp_area = &vdb->area;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    uint8_t other_mask_cnt = lv_draw_mask_get_cnt();

    /*The simplest case just copy the pixels into the VDB*/
    if(other_mask_cnt == 0 && draw_dsc->angle == 0 && draw_dsc->zoom == LV_IMG_ZOOM_NONE &&
       chroma_key == false && alpha_byte == false && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        _lv_blend_map(clip_area, map_area, (lv_color_t *)map_p, NULL, LV_DRAW_MASK_RES_FULL_COVER, draw_dsc->opa,
                      draw_dsc->blend_mode);
    }
#if LV_USE_GPU_NXP_PXP
    /* Simple case without masking and transformations */
    else if(other_mask_cnt == 0 && draw_dsc->angle == 0 && draw_dsc->zoom == LV_IMG_ZOOM_NONE && alpha_byte == false &&
            chroma_key == true && draw_dsc->recolor_opa == LV_OPA_TRANSP) { /* copy with color keying (+ alpha) */
        lv_gpu_nxp_pxp_enable_color_key();
        _lv_blend_map(clip_area, map_area, (lv_color_t *)map_p, NULL, LV_DRAW_MASK_RES_FULL_COVER, draw_dsc->opa,
                      draw_dsc->blend_mode);
        lv_gpu_nxp_pxp_disable_color_key();
    }
    else if(other_mask_cnt == 0 && draw_dsc->angle == 0 && draw_dsc->zoom == LV_IMG_ZOOM_NONE && alpha_byte == false &&
            chroma_key == false && draw_dsc->recolor_opa != LV_OPA_TRANSP) { /* copy with recolor (+ alpha) */
        lv_gpu_nxp_pxp_enable_recolor(draw_dsc->recolor, draw_dsc->recolor_opa);
        _lv_blend_map(clip_area, map_area, (lv_color_t *)map_p, NULL, LV_DRAW_MASK_RES_FULL_COVER, draw_dsc->opa,
                      draw_dsc->blend_mode);
        lv_gpu_nxp_pxp_disable_recolor();
    }
#endif
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        /*The pixel size in byte is different if an alpha byte is added too*/
        uint8_t px_size_byte = alpha_byte ? LV_IMG_PX_SIZE_ALPHA_BYTE : sizeof(lv_color_t);

        /*Go to the first displayed pixel of the map*/
        int32_t map_w = lv_area_get_width(map_area);
        const uint8_t * map_buf_tmp = map_p;
        map_buf_tmp += map_w * (draw_area.y1 - (map_area->y1 - disp_area->y1)) * px_size_byte;
        map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1)) * px_size_byte;

        lv_color_t c;
        lv_color_t chroma_keyed_color = LV_COLOR_TRANSP;
        uint32_t px_i = 0;

        const uint8_t * map_px;

        lv_area_t blend_area;
        blend_area.x1 = draw_area.x1 + disp_area->x1;
        blend_area.x2 = blend_area.x1 + lv_area_get_width(&draw_area) - 1;
        blend_area.y1 = disp_area->y1 + draw_area.y1;
        blend_area.y2 = blend_area.y1;

        lv_coord_t draw_area_h = lv_area_get_height(&draw_area);
        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

#if LV_USE_IMG_TRANSFORM
        bool transform = draw_dsc->angle != 0 || draw_dsc->zoom != LV_IMG_ZOOM_NONE ? true : false;
#else
        bool transform = false;
#endif
        /*Simple ARGB image. Handle it as special case because it's very common*/
        if(other_mask_cnt == 0 && !transform && !chroma_key && draw_dsc->recolor_opa == LV_OPA_TRANSP && alpha_byte) {
#if LV_USE_GPU_STM32_DMA2D && LV_COLOR_DEPTH == 32
            /*Blend ARGB images directly*/
            if(lv_area_get_size(&draw_area) > 240) {
                int32_t disp_w = lv_area_get_width(disp_area);
                lv_color_t * disp_buf = vdb->buf_act;
                lv_color_t * disp_buf_first = disp_buf + disp_w * draw_area.y1 + draw_area.x1;
                lv_gpu_stm32_dma2d_blend(disp_buf_first, disp_w, (const lv_color_t *)map_buf_tmp, draw_dsc->opa, map_w, draw_area_w,
                                         draw_area_h);
                return;
            }
#endif
            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(disp);
            uint32_t mask_buf_size = lv_area_get_size(&draw_area) > (uint32_t) hor_res ? hor_res : lv_area_get_size(&draw_area);
            lv_color_t * map2 = _lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));
            lv_opa_t * mask_buf = _lv_mem_buf_get(mask_buf_size);

            int32_t x;
            int32_t y;
            for(y = 0; y < draw_area_h; y++) {
                map_px = map_buf_tmp;
                for(x = 0; x < draw_area_w; x++, map_px += px_size_byte, px_i++) {
                    lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
                    mask_buf[px_i] = px_opa;
                    if(px_opa) {
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
                        map2[px_i].full =  map_px[0];
#elif LV_COLOR_DEPTH == 16
                        map2[px_i].full =  map_px[0] + (map_px[1] << 8);
#elif LV_COLOR_DEPTH == 32
                        map2[px_i].full =  *((uint32_t *)map_px);
#endif
                    }
#if LV_COLOR_DEPTH == 32
                    map2[px_i].ch.alpha = 0xFF;
#endif
                }

                map_buf_tmp += map_w * px_size_byte;
                if(px_i + lv_area_get_width(&draw_area) < mask_buf_size) {
                    blend_area.y2 ++;
                }
                else {
                    _lv_blend_map(clip_area, &blend_area, map2, mask_buf, LV_DRAW_MASK_RES_CHANGED, draw_dsc->opa, draw_dsc->blend_mode);

                    blend_area.y1 = blend_area.y2 + 1;
                    blend_area.y2 = blend_area.y1;

                    px_i = 0;
                }
            }
            /*Flush the last part*/
            if(blend_area.y1 != blend_area.y2) {
                blend_area.y2--;
                _lv_blend_map(clip_area, &blend_area, map2, mask_buf, LV_DRAW_MASK_RES_CHANGED, draw_dsc->opa, draw_dsc->blend_mode);
            }

            _lv_mem_buf_release(mask_buf);
            _lv_mem_buf_release(map2);
        }
        /*Most complicated case: transform or other mask or chroma keyed*/
        else {
            /*Build the image and a mask line-by-line*/
            uint32_t hor_res = (uint32_t) lv_disp_get_hor_res(disp);
            uint32_t mask_buf_size = lv_area_get_size(&draw_area) > hor_res ? hor_res : lv_area_get_size(&draw_area);
            lv_color_t * map2 = _lv_mem_buf_get(mask_buf_size * sizeof(lv_color_t));
            lv_opa_t * mask_buf = _lv_mem_buf_get(mask_buf_size);

#if LV_USE_IMG_TRANSFORM
            lv_img_transform_dsc_t trans_dsc;
            _lv_memset_00(&trans_dsc, sizeof(lv_img_transform_dsc_t));
            if(transform) {
                lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;
                if(alpha_byte) cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
                else if(chroma_key) cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;

                trans_dsc.cfg.angle = draw_dsc->angle;
                trans_dsc.cfg.zoom = draw_dsc->zoom;
                trans_dsc.cfg.src = map_p;
                trans_dsc.cfg.src_w = map_w;
                trans_dsc.cfg.src_h = lv_area_get_height(map_area);;
                trans_dsc.cfg.cf = cf;
                trans_dsc.cfg.pivot_x = draw_dsc->pivot.x;
                trans_dsc.cfg.pivot_y = draw_dsc->pivot.y;
                trans_dsc.cfg.color = draw_dsc->recolor;
                trans_dsc.cfg.antialias = draw_dsc->antialias;

                _lv_img_buf_transform_init(&trans_dsc);
            }
#endif
            uint16_t recolor_premult[3] = {0};
            lv_opa_t recolor_opa_inv = 255 - draw_dsc->recolor_opa;
            if(draw_dsc->recolor_opa != 0) {
                lv_color_premult(draw_dsc->recolor, draw_dsc->recolor_opa, recolor_premult);
            }

            lv_draw_mask_res_t mask_res;
            mask_res = (alpha_byte || chroma_key || draw_dsc->angle ||
                        draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;


            /*Prepare the `mask_buf`if there are other masks*/
            if(other_mask_cnt) {
                _lv_memset_ff(mask_buf, mask_buf_size);
            }

            int32_t x;
            int32_t y;
#if LV_USE_IMG_TRANSFORM
            int32_t rot_y = disp_area->y1 + draw_area.y1 - map_area->y1;
#endif
            for(y = 0; y < draw_area_h; y++) {
                map_px = map_buf_tmp;
                uint32_t px_i_start = px_i;

#if LV_USE_IMG_TRANSFORM
                int32_t rot_x = disp_area->x1 + draw_area.x1 - map_area->x1;
#endif
                for(x = 0; x < draw_area_w; x++, map_px += px_size_byte, px_i++) {


#if LV_USE_IMG_TRANSFORM
                    if(transform) {

                        /*Transform*/
                        bool ret;
                        ret = _lv_img_buf_transform(&trans_dsc, rot_x + x, rot_y + y);
                        if(ret == false) {
                            mask_buf[px_i] = LV_OPA_TRANSP;
                            continue;
                        }
                        else {
                            mask_buf[px_i] = trans_dsc.res.opa;
                            c.full = trans_dsc.res.color.full;
                        }
                    }
                    /*No transform*/
                    else
#endif
                    {
                        if(alpha_byte) {
                            lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
                            mask_buf[px_i] = px_opa;
                            if(px_opa == 0) {
#if  LV_COLOR_DEPTH == 32
                                map2[px_i].full = 0;
#endif
                                continue;
                            }
                        }
                        else {
                            mask_buf[px_i] = 0xFF;
                        }

#if LV_COLOR_DEPTH == 1
                        c.full = map_px[0];
#elif LV_COLOR_DEPTH == 8
                        c.full =  map_px[0];
#elif LV_COLOR_DEPTH == 16
                        c.full =  map_px[0] + (map_px[1] << 8);
#elif LV_COLOR_DEPTH == 32
                        c.full =  *((uint32_t *)map_px);
                        c.ch.alpha = 0xFF;
#endif
                        if(chroma_key) {
                            if(c.full == chroma_keyed_color.full) {
                                mask_buf[px_i] = LV_OPA_TRANSP;
#if  LV_COLOR_DEPTH == 32
                                map2[px_i].full = 0;
#endif
                                continue;
                            }
                        }
                    }

                    if(draw_dsc->recolor_opa != 0) {
                        c = lv_color_mix_premult(recolor_premult, c, recolor_opa_inv);
                    }

                    map2[px_i].full = c.full;
                }

                /*Apply the masks if any*/
                if(other_mask_cnt) {
                    lv_draw_mask_res_t mask_res_sub;
                    mask_res_sub = lv_draw_mask_apply(mask_buf + px_i_start, draw_area.x1 + vdb->area.x1, y + draw_area.y1 + vdb->area.y1,
                                                      lv_area_get_width(&draw_area));
                    if(mask_res_sub == LV_DRAW_MASK_RES_TRANSP) {
                        _lv_memset_00(mask_buf + px_i_start, lv_area_get_width(&draw_area));
                        mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                    else if(mask_res_sub == LV_DRAW_MASK_RES_CHANGED) {
                        mask_res = LV_DRAW_MASK_RES_CHANGED;
                    }
                }

                map_buf_tmp += map_w * px_size_byte;
                if(px_i + lv_area_get_width(&draw_area) < mask_buf_size) {
                    blend_area.y2 ++;
                }
                else {

                    _lv_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, draw_dsc->opa, draw_dsc->blend_mode);

                    blend_area.y1 = blend_area.y2 + 1;
                    blend_area.y2 = blend_area.y1;

                    px_i = 0;
                    mask_res = (alpha_byte || chroma_key || draw_dsc->angle ||
                                draw_dsc->zoom != LV_IMG_ZOOM_NONE) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;

                    /*Prepare the `mask_buf`if there are other masks*/
                    if(other_mask_cnt) {
                        _lv_memset_ff(mask_buf, mask_buf_size);
                    }
                }
            }

            /*Flush the last part*/
            if(blend_area.y1 != blend_area.y2) {
                blend_area.y2--;
                _lv_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, draw_dsc->opa, draw_dsc->blend_mode);
            }

            _lv_mem_buf_release(mask_buf);
            _lv_mem_buf_release(map2);
        }
    }
}

static void show_error(const lv_area_t * coords, const lv_area_t * clip_area, const char * msg)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = LV_COLOR_WHITE;
    lv_draw_rect(coords, clip_area, &rect_dsc);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_draw_label(coords, clip_area, &label_dsc, msg, NULL);
}

