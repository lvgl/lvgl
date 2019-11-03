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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * mask, const void * src,
        const lv_style_t * style, uint16_t angle, lv_opa_t opa_scale);

static void lv_draw_map(const lv_area_t * map_area, const lv_area_t * clip_area, const uint8_t * map_p, lv_opa_t opa,
        bool chroma_key, bool alpha_byte, const lv_style_t * style, uint16_t angle);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param style style of the image
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask, const void * src, const lv_style_t * style,
        uint16_t angle, lv_opa_t opa_scale)
{
    if(src == NULL) {
        LV_LOG_WARN("Image draw: src is NULL");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL, NULL, NULL);
        return;
    }

    lv_res_t res;
    res = lv_img_draw_core(coords, mask, src, style, angle, opa_scale);

    if(res == LV_RES_INV) {
        LV_LOG_WARN("Image draw error");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL,  NULL, NULL);
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
    case LV_IMG_CF_RAW: px_size = 0; break;
    case LV_IMG_CF_TRUE_COLOR:
    case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED: px_size = LV_COLOR_SIZE; break;
    case LV_IMG_CF_TRUE_COLOR_ALPHA: px_size = LV_IMG_PX_SIZE_ALPHA_BYTE << 3; break;
    case LV_IMG_CF_INDEXED_1BIT:
    case LV_IMG_CF_ALPHA_1BIT: px_size = 1; break;
    case LV_IMG_CF_INDEXED_2BIT:
    case LV_IMG_CF_ALPHA_2BIT: px_size = 2; break;
    case LV_IMG_CF_INDEXED_4BIT:
    case LV_IMG_CF_ALPHA_4BIT: px_size = 4; break;
    case LV_IMG_CF_INDEXED_8BIT:
    case LV_IMG_CF_ALPHA_8BIT: px_size = 8; break;
    default: px_size = 0; break;
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
#if LV_INDEXED_CHROMA
    case LV_IMG_CF_INDEXED_1BIT:
    case LV_IMG_CF_INDEXED_2BIT:
    case LV_IMG_CF_INDEXED_4BIT:
    case LV_IMG_CF_INDEXED_8BIT:
#endif
        is_chroma_keyed = true; break;

    default: is_chroma_keyed = false; break;
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
    case LV_IMG_CF_ALPHA_8BIT: has_alpha = true; break;
    default: has_alpha = false; break;
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
    } else if(u8_p[0] >= 0x80) {
        img_src_type = LV_IMG_SRC_SYMBOL; /*Symbols begins after 0x7F*/
    } else {
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

static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * mask, const void * src,
        const lv_style_t * style, uint16_t angle, lv_opa_t opa_scale)
{
    lv_area_t map_area_rot;
    lv_area_copy(&map_area_rot, coords);
    if(angle) {
        /*Get the exact area which is required to show the rotated image*/
        lv_coord_t pivot_x = lv_area_get_width(coords) / 2 + coords->x1;
        lv_coord_t pivot_y = lv_area_get_height(coords) / 2 + coords->y1;

        lv_area_t norm;
        norm.x1 = + coords->x1 - pivot_x;
        norm.y1 = + coords->y1 - pivot_y;
        norm.x2 = + coords->x2 - pivot_x;
        norm.y2 = + coords->y2 - pivot_y;

        int16_t sinma = lv_trigo_sin(-angle);
        int16_t cosma = lv_trigo_sin(-angle + 90);

        lv_point_t lt;
        lv_point_t rt;
        lv_point_t lb;
        lv_point_t rb;
        lt.x = ((cosma * norm.x1 - sinma * norm.y1) >> (LV_TRIGO_SHIFT)) + pivot_x;
        lt.y = ((sinma * norm.x1 + cosma * norm.y1) >> (LV_TRIGO_SHIFT)) + pivot_y;

        rt.x = ((cosma * norm.x2 - sinma * norm.y1) >> (LV_TRIGO_SHIFT)) + pivot_x;
        rt.y = ((sinma * norm.x2 + cosma * norm.y1) >> (LV_TRIGO_SHIFT)) + pivot_y;

        lb.x = ((cosma * norm.x1 - sinma * norm.y2) >> (LV_TRIGO_SHIFT)) + pivot_x;
        lb.y = ((sinma * norm.x1 + cosma * norm.y2) >> (LV_TRIGO_SHIFT)) + pivot_y;

        rb.x = ((cosma * norm.x2 - sinma * norm.y2) >> (LV_TRIGO_SHIFT)) + pivot_x;
        rb.y = ((sinma * norm.x2 + cosma * norm.y2) >> (LV_TRIGO_SHIFT)) + pivot_y;

        map_area_rot.x1 = LV_MATH_MIN(LV_MATH_MIN(LV_MATH_MIN(lb.x, lt.x), rb.x), rt.x);
        map_area_rot.x2 = LV_MATH_MAX(LV_MATH_MAX(LV_MATH_MAX(lb.x, lt.x), rb.x), rt.x);
        map_area_rot.y1 = LV_MATH_MIN(LV_MATH_MIN(LV_MATH_MIN(lb.y, lt.y), rb.y), rt.y);
        map_area_rot.y2 = LV_MATH_MAX(LV_MATH_MAX(LV_MATH_MAX(lb.y, lt.y), rb.y), rt.y);
    }

    lv_area_t mask_com; /*Common area of mask and coords*/
    bool union_ok;
    union_ok = lv_area_intersect(&mask_com, mask, &map_area_rot);
    if(union_ok == false) {
        return LV_RES_OK; /*Out of mask. There is nothing to draw so the image is drawn
                             successfully.*/
    }

    lv_opa_t opa =
            opa_scale == LV_OPA_COVER ? style->image.opa : (uint16_t)((uint16_t)style->image.opa * opa_scale) >> 8;

    lv_img_cache_entry_t * cdsc = lv_img_cache_open(src, style);

    if(cdsc == NULL) return LV_RES_INV;

    bool chroma_keyed = lv_img_cf_is_chroma_keyed(cdsc->dec_dsc.header.cf);
    bool alpha_byte   = lv_img_cf_has_alpha(cdsc->dec_dsc.header.cf);

    if(cdsc->dec_dsc.error_msg != NULL) {
        LV_LOG_WARN("Image draw error");
        lv_draw_rect(coords, &mask_com, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, &mask_com, &lv_style_plain, LV_OPA_COVER, cdsc->dec_dsc.error_msg, LV_TXT_FLAG_NONE, NULL, NULL, NULL);
    }
    /* The decoder open could open the image and gave the entire uncompressed image.
     * Just draw it!*/
    else if(cdsc->dec_dsc.img_data) {
        lv_draw_map(coords, &mask_com, cdsc->dec_dsc.img_data, opa, chroma_keyed, alpha_byte, style, angle);
    }
    /* The whole uncompressed image is not available. Try to read it line-by-line*/
    else {
        lv_coord_t width = lv_area_get_width(&mask_com);

        uint8_t  * buf = lv_draw_buf_get(lv_area_get_width(&mask_com) * LV_IMG_PX_SIZE_ALPHA_BYTE);  /*+1 because of the possible alpha byte*/

        lv_area_t line;
        lv_area_copy(&line, &mask_com);
        lv_area_set_height(&line, 1);
        lv_coord_t x = mask_com.x1 - coords->x1;
        lv_coord_t y = mask_com.y1 - coords->y1;
        lv_coord_t row;
        lv_res_t read_res;
        for(row = mask_com.y1; row <= mask_com.y2; row++) {
            read_res = lv_img_decoder_read_line(&cdsc->dec_dsc, x, y, width, buf);
            if(read_res != LV_RES_OK) {
                lv_img_decoder_close(&cdsc->dec_dsc);
                LV_LOG_WARN("Image draw can't read the line");
                lv_draw_buf_release(buf);
                return LV_RES_INV;
            }
            lv_draw_map(&line, &mask_com, buf, opa, chroma_keyed, alpha_byte, style, 0);
            line.y1++;
            line.y2++;
            y++;
        }
        lv_draw_buf_release(buf);
    }

    return LV_RES_OK;
}

/**
 * Draw a color map to the display (image)
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area  (truncated to VDB area)
 * @param map_p pointer to a lv_color_t array
 * @param opa opacity of the map
 * @param chroma_keyed true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param alpha_byte true: extra alpha byte is inserted for every pixel
 * @param style style of the image
 */
static void lv_draw_map(const lv_area_t * map_area, const lv_area_t * clip_area, const uint8_t * map_p, lv_opa_t opa,
        bool chroma_key, bool alpha_byte, const lv_style_t * style, uint16_t angle)
{

    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /* Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, clip_area);

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
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
    if(angle == 0 && other_mask_cnt == 0 && chroma_key == false && alpha_byte == false && opa == LV_OPA_COVER && style->image.intense == LV_OPA_TRANSP) {
        lv_blend_map(clip_area, map_area, (lv_color_t *)map_p, NULL, LV_DRAW_MASK_RES_FULL_COVER, LV_OPA_COVER, style->image.blend_mode);
    }
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        /*The pixel size in byte is different if an alpha byte is added too*/
        uint8_t px_size_byte = alpha_byte ? LV_IMG_PX_SIZE_ALPHA_BYTE : sizeof(lv_color_t);

        /*Build the image and a mask line-by-line*/
        uint32_t mask_buf_size = lv_area_get_size(&draw_area) > LV_HOR_RES_MAX ? LV_HOR_RES_MAX : lv_area_get_size(&draw_area);
        lv_color_t * map2 = lv_draw_buf_get(mask_buf_size * sizeof(lv_color_t));
        lv_opa_t * mask_buf = lv_draw_buf_get(mask_buf_size);

        /*Go to the first displayed pixel of the map*/
        lv_coord_t map_w = lv_area_get_width(map_area);
        lv_coord_t map_h = lv_area_get_height(map_area);
        const uint8_t * map_buf_tmp = map_p;
        map_buf_tmp += map_w * (draw_area.y1 - (map_area->y1 - disp_area->y1)) * px_size_byte;
        map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1)) * px_size_byte;

        lv_color_t c;
        lv_color_t chroma_keyed_color = LV_COLOR_TRANSP;
        uint32_t px_i = 0;
        uint32_t px_i_start;

        const uint8_t * map_px;

        lv_area_t blend_area;
        blend_area.x1 = draw_area.x1 + disp_area->x1;
        blend_area.x2 = blend_area.x1 + lv_area_get_width(&draw_area) - 1;
        blend_area.y1 = disp_area->y1 + draw_area.y1;
        blend_area.y2 = blend_area.y1;

        /*Prepare the `mask_buf`if there are other masks*/
        if(other_mask_cnt) {
            memset(mask_buf, 0xFF, mask_buf_size);
        }


        lv_img_rotate_dsc_t rotate_dsc = {};
        if(angle) {
            lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;
            if(alpha_byte) cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
            else if(chroma_key) cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED;
            lv_img_buf_rotate_init(&rotate_dsc, angle, map_p, map_w, map_h, cf, map_w/2, map_h / 2, LV_COLOR_BLACK);
        }
        lv_draw_mask_res_t mask_res;
        mask_res = (alpha_byte || chroma_key || angle) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
        lv_coord_t x;
        lv_coord_t y;
        for(y = 0; y < lv_area_get_height(&draw_area); y++) {
            map_px = map_buf_tmp;
            px_i_start = px_i;

            for(x = 0; x < lv_area_get_width(&draw_area); x++, map_px += px_size_byte, px_i++) {

                if(angle == 0) {
                    if(alpha_byte) {
                        lv_opa_t px_opa = map_px[LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
                        mask_buf[px_i] = px_opa;
                        if(px_opa < LV_OPA_MIN) continue;
                    } else {
                        mask_buf[px_i] = LV_OPA_COVER;
                    }

#if LV_COLOR_DEPTH == 8
                    c.full =  map_px[0];
#elif LV_COLOR_DEPTH == 16
                    c.full =  map_px[0] + (map_px[1] << 8);
#elif LV_COLOR_DEPTH == 32
                    c.full =  *((uint32_t*)map_px);
#endif
                    if (chroma_key) {
                        if(c.full == chroma_keyed_color.full) {
                            mask_buf[px_i] = LV_OPA_TRANSP;
                            continue;
                        }
                    }
                } else {
                    /*Rotate*/
                    bool ret;
                    lv_coord_t rot_x = x + (disp_area->x1 + draw_area.x1) - map_area->x1;
                    lv_coord_t rot_y = y + (disp_area->y1 + draw_area.y1) - map_area->y1;
                    ret = lv_img_buf_get_px_rotated(&rotate_dsc, rot_x, rot_y);
                    if(ret == false) {
                        mask_buf[px_i] = LV_OPA_TRANSP;
                        continue;
                    } else {
                        mask_buf[px_i] = rotate_dsc.res_opa;
                        c.full = rotate_dsc.res_color.full;
                    }
                }

                if(style->image.intense != 0) {
                    c = lv_color_mix(style->image.color, c, style->image.intense);
                }

                map2[px_i].full = c.full;
            }

            /*Apply the masks if any*/
            if(other_mask_cnt) {
                lv_draw_mask_res_t mask_res_sub;
                mask_res_sub = lv_draw_mask_apply(mask_buf + px_i_start, draw_area.x1 + vdb->area.x1, y + draw_area.y1 + vdb->area.y1, lv_area_get_width(&draw_area));
                if(mask_res_sub == LV_DRAW_MASK_RES_FULL_TRANSP) {
                    memset(mask_buf + px_i_start, 0x00, lv_area_get_width(&draw_area));
                    mask_res = LV_DRAW_MASK_RES_CHANGED;
                } else if(mask_res_sub == LV_DRAW_MASK_RES_CHANGED) {
                    mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
            }

            map_buf_tmp += map_w * px_size_byte;
            if(px_i + lv_area_get_width(&draw_area) < mask_buf_size) {
                blend_area.y2 ++;
            } else {
                lv_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, opa, style->image.blend_mode);

                blend_area.y1 = blend_area.y2 + 1;
                blend_area.y2 = blend_area.y1;

                px_i = 0;
                mask_res = (alpha_byte || chroma_key || angle) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;

                /*Prepare the `mask_buf`if there are other masks*/
                if(other_mask_cnt) {
                    memset(mask_buf, 0xFF, mask_buf_size);
                }
            }
        }
        /*Flush the last part*/
        if(blend_area.y1 != blend_area.y2) {
            blend_area.y2--;
            lv_blend_map(clip_area, &blend_area, map2, mask_buf, mask_res, opa, style->image.blend_mode);
        }

        lv_draw_buf_release(mask_buf);
        lv_draw_buf_release(map2);
    }
}
