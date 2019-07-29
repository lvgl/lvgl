/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_img.h"
#include "lv_img_cache.h"
#include "../lv_misc/lv_log.h"

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
                                 const lv_style_t * style, lv_opa_t opa_scale);

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
                 lv_opa_t opa_scale)
{
    if(src == NULL) {
        LV_LOG_WARN("Image draw: src is NULL");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL, -1, -1, NULL);
        return;
    }

    lv_res_t res;
    res = lv_img_draw_core(coords, mask, src, style, opa_scale);

    if(res == LV_RES_INV) {
        LV_LOG_WARN("Image draw error");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL, -1, -1, NULL);
        return;
    }
}

/**
 * Get the color of an image's pixel
 * @param dsc an image descriptor
 * @param x x coordinate of the point to get
 * @param y x coordinate of the point to get
 * @param style style of the image. In case of `LV_IMG_CF_ALPHA_1/2/4/8` `style->image.color` shows
 * the color. Can be `NULL` but for `ALPHA` images black will be returned. In other cases it is not
 * used.
 * @return color of the point
 */
lv_color_t lv_img_buf_get_px_color(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y, const lv_style_t * style)
{
    lv_color_t p_color = LV_COLOR_BLACK;
    if(x >= dsc->header.w) {
        x = dsc->header.w - 1;
        LV_LOG_WARN("lv_canvas_get_px: x is too large (out of canvas)");
    } else if(x < 0) {
        x = 0;
        LV_LOG_WARN("lv_canvas_get_px: x is < 0 (out of canvas)");
    }

    if(y >= dsc->header.h) {
        y = dsc->header.h - 1;
        LV_LOG_WARN("lv_canvas_get_px: y is too large (out of canvas)");
    } else if(y < 0) {
        y = 0;
        LV_LOG_WARN("lv_canvas_get_px: y is < 0 (out of canvas)");
    }

    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR || dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED ||
       dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size = lv_img_color_format_get_px_size(dsc->header.cf) >> 3;
        uint32_t px     = dsc->header.w * y * px_size + x * px_size;
        memcpy(&p_color, &buf_u8[px], sizeof(lv_color_t));
#if LV_COLOR_SIZE == 32
        p_color.ch.alpha = 0xFF; /*Only the color should be get so use a deafult alpha value*/
#endif
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_1BIT) {
        buf_u8 += 4 * 2;
        uint8_t bit = x & 0x7;
        x           = x >> 3;

        uint32_t px  = (dsc->header.w >> 3) * y + x;
        p_color.full = (buf_u8[px] & (1 << (7 - bit))) >> (7 - bit);
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_2BIT) {
        buf_u8 += 4 * 4;
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        uint32_t px  = (dsc->header.w >> 2) * y + x;
        p_color.full = (buf_u8[px] & (3 << (6 - bit))) >> (6 - bit);
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_4BIT) {
        buf_u8 += 4 * 16;
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        uint32_t px  = (dsc->header.w >> 1) * y + x;
        p_color.full = (buf_u8[px] & (0xF << (4 - bit))) >> (4 - bit);
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
        buf_u8 += 4 * 256;
        uint32_t px  = dsc->header.w * y + x;
        p_color.full = buf_u8[px];
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT || dsc->header.cf == LV_IMG_CF_ALPHA_2BIT ||
              dsc->header.cf == LV_IMG_CF_ALPHA_4BIT || dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
        if(style)
            p_color = style->image.color;
        else
            p_color = LV_COLOR_BLACK;
    }
    return p_color;
}

/**
 * Get the alpha value of an image's pixel
 * @param dsc pointer to an image descriptor
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @return alpha value of the point
 */
lv_opa_t lv_img_buf_get_px_alpha(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y)
{
    if(x >= dsc->header.w) {
        x = dsc->header.w - 1;
        LV_LOG_WARN("lv_canvas_get_px: x is too large (out of canvas)");
    } else if(x < 0) {
        x = 0;
        LV_LOG_WARN("lv_canvas_get_px: x is < 0 (out of canvas)");
    }

    if(y >= dsc->header.h) {
        y = dsc->header.h - 1;
        LV_LOG_WARN("lv_canvas_get_px: y is too large (out of canvas)");
    } else if(y < 0) {
        y = 0;
        LV_LOG_WARN("lv_canvas_get_px: y is < 0 (out of canvas)");
    }

    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint32_t px = dsc->header.w * y * LV_IMG_PX_SIZE_ALPHA_BYTE + x * LV_IMG_PX_SIZE_ALPHA_BYTE;
        return buf_u8[px + LV_IMG_PX_SIZE_ALPHA_BYTE - 1];
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT) {
        uint8_t bit = x & 0x7;
        x           = x >> 3;

        uint32_t px    = (dsc->header.w >> 3) * y + x;
        uint8_t px_opa = (buf_u8[px] & (1 << (7 - bit))) >> (7 - bit);
        return px_opa ? LV_OPA_TRANSP : LV_OPA_COVER;
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_2BIT) {
        const uint8_t opa_table[4] = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/

        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        uint32_t px    = (dsc->header.w >> 2) * y + x;
        uint8_t px_opa = (buf_u8[px] & (3 << (6 - bit))) >> (6 - bit);
        return opa_table[px_opa];
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_4BIT) {
        const uint8_t opa_table[16] = {0,  17, 34,  51, /*Opacity mapping with bpp = 4*/
                                       68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};

        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        uint32_t px    = (dsc->header.w >> 1) * y + x;
        uint8_t px_opa = (buf_u8[px] & (0xF << (4 - bit))) >> (4 - bit);
        return opa_table[px_opa];
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
        uint32_t px = dsc->header.w * y + x;
        return buf_u8[px];
    }

    return LV_OPA_COVER;
}

/**
 * Set the color of a pixel of an image. The alpha channel won't be affected.
 * @param dsc pointer to an image descriptor
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param c color of the point
 */
void lv_img_buf_set_px_color(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_color_t c)
{
    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR || dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        uint8_t px_size = lv_img_color_format_get_px_size(dsc->header.cf) >> 3;
        uint32_t px     = dsc->header.w * y * px_size + x * px_size;
        memcpy(&buf_u8[px], &c, px_size);
    } else if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size = lv_img_color_format_get_px_size(dsc->header.cf) >> 3;
        uint32_t px     = dsc->header.w * y * px_size + x * px_size;
        memcpy(&buf_u8[px], &c, px_size - 1); /*-1 to not overwrite the alpha value*/
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_1BIT) {
        buf_u8 += sizeof(lv_color32_t) * 2; /*Skip the palette*/

        uint8_t bit = x & 0x7;
        x           = x >> 3;
        uint32_t px = (dsc->header.w >> 3) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(1 << (7 - bit));
        buf_u8[px]  = buf_u8[px] | ((c.full & 0x1) << (7 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_2BIT) {
        buf_u8 += sizeof(lv_color32_t) * 4; /*Skip the palette*/
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        uint32_t px = (dsc->header.w >> 2) * y + x;

        buf_u8[px] = buf_u8[px] & ~(3 << (6 - bit));
        buf_u8[px] = buf_u8[px] | ((c.full & 0x3) << (6 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_4BIT) {
        buf_u8 += sizeof(lv_color32_t) * 16; /*Skip the palette*/
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        uint32_t px = (dsc->header.w >> 1) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(0xF << (4 - bit));
        buf_u8[px]  = buf_u8[px] | ((c.full & 0xF) << (4 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
        buf_u8 += sizeof(lv_color32_t) * 256; /*Skip the palette*/
        uint32_t px = dsc->header.w * y + x;
        buf_u8[px]  = c.full;
    }
}

/**
 * Set the alpha value of a pixel of an image. The color won't be affected
 * @param dsc pointer to an image descriptor
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param opa the desired opacity
 */
void lv_img_buf_set_px_alpha(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_opa_t opa)
{
    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size          = lv_img_color_format_get_px_size(dsc->header.cf) >> 3;
        uint32_t px              = dsc->header.w * y * px_size + x * px_size;
        buf_u8[px + px_size - 1] = opa;
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT) {
        opa         = opa >> 7; /*opa -> [0,1]*/
        uint8_t bit = x & 0x7;
        x           = x >> 3;
        uint32_t px = (dsc->header.w >> 3) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(1 << (7 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0x1) << (7 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_2BIT) {
        opa         = opa >> 6; /*opa -> [0,3]*/
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;
        uint32_t px = (dsc->header.w >> 2) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(3 << (6 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0x3) << (6 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_4BIT) {
        opa         = opa >> 4; /*opa -> [0,15]*/
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        uint32_t px = (dsc->header.w >> 1) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(0xF << (4 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0xF) << (4 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
        uint32_t px = dsc->header.w * y + x;
        buf_u8[px]  = opa;
    }
}

/**
 * Set the palette color of an indexed image. Valid only for `LV_IMG_CF_INDEXED1/2/4/8`
 * @param dsc pointer to an image descriptor
 * @param id the palette color to set:
 *   - for `LV_IMG_CF_INDEXED1`: 0..1
 *   - for `LV_IMG_CF_INDEXED2`: 0..3
 *   - for `LV_IMG_CF_INDEXED4`: 0..15
 *   - for `LV_IMG_CF_INDEXED8`: 0..255
 * @param c the color to set
 */
void lv_img_buf_set_palette(lv_img_dsc_t * dsc, uint8_t id, lv_color_t c)
{
    if((dsc->header.cf == LV_IMG_CF_ALPHA_1BIT && id > 1) || (dsc->header.cf == LV_IMG_CF_ALPHA_2BIT && id > 3) ||
       (dsc->header.cf == LV_IMG_CF_ALPHA_4BIT && id > 15) || (dsc->header.cf == LV_IMG_CF_ALPHA_8BIT)) {
        LV_LOG_WARN("lv_img_buf_set_px_alpha: invalid 'id'");
        return;
    }

    lv_color32_t c32;
    c32.full      = lv_color_to32(c);
    uint8_t * buf = (uint8_t *)dsc->data;
    memcpy(&buf[id * sizeof(c32)], &c32, sizeof(c32));
}

/**
 * Get the pixel size of a color format in bits
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return the pixel size in bits
 */
uint8_t lv_img_color_format_get_px_size(lv_img_cf_t cf)
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
bool lv_img_color_format_is_chroma_keyed(lv_img_cf_t cf)
{
    bool is_chroma_keyed = false;

    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
        case LV_IMG_CF_RAW_CHROMA_KEYED:
        case LV_IMG_CF_INDEXED_1BIT:
        case LV_IMG_CF_INDEXED_2BIT:
        case LV_IMG_CF_INDEXED_4BIT:
        case LV_IMG_CF_INDEXED_8BIT: is_chroma_keyed = true; break;
        default: is_chroma_keyed = false; break;
    }

    return is_chroma_keyed;
}

/**
 * Check if a color format has alpha channel or not
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return true: has alpha channel; false: doesn't have alpha channel
 */
bool lv_img_color_format_has_alpha(lv_img_cf_t cf)
{
    bool has_alpha = false;

    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_RAW_ALPHA:
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
                                 const lv_style_t * style, lv_opa_t opa_scale)
{

    lv_area_t mask_com; /*Common area of mask and coords*/
    bool union_ok;
    union_ok = lv_area_intersect(&mask_com, mask, coords);
    if(union_ok == false) {
        return LV_RES_OK; /*Out of mask. There is nothing to draw so the image is drawn
                             successfully.*/
    }

    lv_opa_t opa =
        opa_scale == LV_OPA_COVER ? style->image.opa : (uint16_t)((uint16_t)style->image.opa * opa_scale) >> 8;

    lv_img_cache_entry_t * cdsc = lv_img_cache_open(src, style);

    if(cdsc == NULL) return LV_RES_INV;

    bool chroma_keyed = lv_img_color_format_is_chroma_keyed(cdsc->dec_dsc.header.cf);
    bool alpha_byte   = lv_img_color_format_has_alpha(cdsc->dec_dsc.header.cf);

    if(cdsc->dec_dsc.error_msg != NULL) {
        LV_LOG_WARN("Image draw error");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, cdsc->dec_dsc.error_msg, LV_TXT_FLAG_NONE, NULL, -1,
                      -1, NULL);
    }
    /* The decoder open could open the image and gave the entire uncompressed image.
     * Just draw it!*/
    else if(cdsc->dec_dsc.img_data) {
        lv_draw_map(coords, mask, cdsc->dec_dsc.img_data, opa, chroma_keyed, alpha_byte, style->image.color,
                    style->image.intense);
    }
    /* The whole uncompressed image is not available. Try to read it line-by-line*/
    else {
        lv_coord_t width = lv_area_get_width(&mask_com);

        uint8_t  * buf = lv_draw_get_buf(lv_area_get_width(&mask_com) * ((LV_COLOR_DEPTH >> 3) + 1));  /*+1 because of the possible alpha byte*/

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
                return LV_RES_INV;
            }
            lv_draw_map(&line, mask, buf, opa, chroma_keyed, alpha_byte, style->image.color, style->image.intense);
            line.y1++;
            line.y2++;
            y++;
        }
    }

    return LV_RES_OK;
}
