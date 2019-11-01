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
        const lv_style_t * style, lv_opa_t opa_scale);

static void lv_draw_map(const lv_area_t * map_area, const lv_area_t * clip_area, const uint8_t * map_p, lv_opa_t opa,
        bool chroma_key, bool alpha_byte, const lv_style_t * style);

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
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL, NULL, NULL);
        return;
    }

    lv_res_t res;
    res = lv_img_draw_core(coords, mask, src, style, opa_scale);

    if(res == LV_RES_INV) {
        LV_LOG_WARN("Image draw error");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL,  NULL, NULL);
        return;
    }
}

/**
 * Get the color of an image's pixel
 * @param dsc an image descriptor
 * @param x x coordinate of the point to get
 * @param y x coordinate of the point to get
 * @param color the color of the image. In case of `LV_IMG_CF_ALPHA_1/2/4/8` this color is used.
 * Not used in other cases.
 * @return color of the point
 */
lv_color_t lv_img_buf_get_px_color(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_color_t color)
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

        /* Get the current pixel.
         * dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 8, 16, 24 ...*/
        uint32_t px  = ((dsc->header.w + 7) >> 3) * y + x;
        p_color.full = (buf_u8[px] & (1 << (7 - bit))) >> (7 - bit);
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_2BIT) {
        buf_u8 += 4 * 4;
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /* Get the current pixel.
         * dsc->header.w + 3 means rounding up to 4 because the lines are byte aligned
         * so the possible real width are 4, 8, 12 ...*/
        uint32_t px  = ((dsc->header.w + 3) >> 2) * y + x;
        p_color.full = (buf_u8[px] & (3 << (6 - bit))) >> (6 - bit);
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_4BIT) {
        buf_u8 += 4 * 16;
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /* Get the current pixel.
         * dsc->header.w + 1 means rounding up to 2 because the lines are byte aligned
         * so the possible real width are 2, 4, 6 ...*/
        uint32_t px  = ((dsc->header.w + 1) >> 1) * y + x;
        p_color.full = (buf_u8[px] & (0xF << (4 - bit))) >> (4 - bit);
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
        buf_u8 += 4 * 256;
        uint32_t px  = dsc->header.w * y + x;
        p_color.full = buf_u8[px];
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT || dsc->header.cf == LV_IMG_CF_ALPHA_2BIT ||
            dsc->header.cf == LV_IMG_CF_ALPHA_4BIT || dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
            p_color = color;
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

        /* Get the current pixel.
         * dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 8 ,16, 24 ...*/
        uint32_t px    = ((dsc->header.w + 7) >> 3) * y + x;
        uint8_t px_opa = (buf_u8[px] & (1 << (7 - bit))) >> (7 - bit);
        return px_opa ? LV_OPA_TRANSP : LV_OPA_COVER;
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_2BIT) {
        const uint8_t opa_table[4] = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/

        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /* Get the current pixel.
         * dsc->header.w + 4 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 4 ,8, 12 ...*/
        uint32_t px    = ((dsc->header.w + 3) >> 2) * y + x;
        uint8_t px_opa = (buf_u8[px] & (3 << (6 - bit))) >> (6 - bit);
        return opa_table[px_opa];
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_4BIT) {
        const uint8_t opa_table[16] = {0,  17, 34,  51, /*Opacity mapping with bpp = 4*/
                68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};

        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /* Get the current pixel.
         * dsc->header.w + 1 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 2 ,4, 6 ...*/
        uint32_t px    = ((dsc->header.w + 1) >> 1) * y + x;
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

        /* Get the current pixel.
         * dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 8 ,16, 24 ...*/
        uint32_t px = ((dsc->header.w + 7) >> 3) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(1 << (7 - bit));
        buf_u8[px]  = buf_u8[px] | ((c.full & 0x1) << (7 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_2BIT) {
        buf_u8 += sizeof(lv_color32_t) * 4; /*Skip the palette*/
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /* Get the current pixel.
         * dsc->header.w + 3 means rounding up to 4 because the lines are byte aligned
         * so the possible real width are 4, 8 ,12 ...*/
        uint32_t px = ((dsc->header.w + 3) >> 2) * y + x;

        buf_u8[px] = buf_u8[px] & ~(3 << (6 - bit));
        buf_u8[px] = buf_u8[px] | ((c.full & 0x3) << (6 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_INDEXED_4BIT) {
        buf_u8 += sizeof(lv_color32_t) * 16; /*Skip the palette*/
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /* Get the current pixel.
         * dsc->header.w + 1 means rounding up to 2 because the lines are byte aligned
         * so the possible real width are 2 ,4, 6 ...*/
        uint32_t px = ((dsc->header.w + 1) >> 1) * y + x;
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

        /* Get the current pixel.
         * dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 8 ,16, 24 ...*/
        uint32_t px = ((dsc->header.w + 7) >> 3) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(1 << (7 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0x1) << (7 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_2BIT) {
        opa         = opa >> 6; /*opa -> [0,3]*/
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /* Get the current pixel.
         * dsc->header.w + 4 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 4 ,8, 12 ...*/
        uint32_t px = ((dsc->header.w + 3) >> 2) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(3 << (6 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0x3) << (6 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_4BIT) {
        opa         = opa >> 4; /*opa -> [0,15]*/
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /* Get the current pixel.
         * dsc->header.w + 1 means rounding up to 8 because the lines are byte aligned
         * so the possible real width are 2 ,4, 6 ...*/
        uint32_t px = ((dsc->header.w + 1) >> 1) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(0xF << (4 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0xF) << (4 - bit));
    } else if(dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
        uint32_t px = dsc->header.w * y + x;
        buf_u8[px]  = opa;
    }
}

bool lv_img_get_px_rotated(lv_img_dsc_t * img, uint16_t angle, lv_color_t color, lv_point_t * point, lv_point_t * pivot, lv_color_t * res_color, lv_opa_t * res_opa)
{

    int32_t sinma             = lv_trigo_sin(-angle);
    int32_t cosma             = lv_trigo_sin(-angle + 90); /* cos */

    /*Get the target point relative coordinates to the pivot*/
    int32_t xt = point->x - pivot->x;
    int32_t yt = point->y - pivot->y;

    /*Get the source pixel from the upscaled image*/
    int32_t xs = ((cosma * xt - sinma * yt) >> (LV_TRIGO_SHIFT - 8)) + pivot->x * 256;
    int32_t ys = ((sinma * xt + cosma * yt) >> (LV_TRIGO_SHIFT - 8)) + pivot->y * 256;

    /*Get the integer part of the source pixel*/
    int xs_int = xs >> 8;
    int ys_int = ys >> 8;

    if(xs_int >= img->header.w) return false;
    else if(xs_int < 0) return false;

    if(ys_int >= img->header.h) return false;
    else if(ys_int < 0) return false;

    /*Get the fractional part of the source pixel*/
    int xs_fract = xs & 0xff;
    int ys_fract = ys & 0xff;

    /* If the fractional < 0x70 mix the source pixel with the left/top pixel
     * If the fractional > 0x90 mix the source pixel with the right/bottom pixel
     * In the 0x70..0x90 range use the unchanged source pixel */

    int xn;      /*x neightboor*/
    lv_opa_t xr; /*x mix ratio*/
    if(xs_fract < 0x70) {
        xn = xs_int - 1;
        xr = xs_fract * 2;
    } else if(xs_fract > 0x90) {
        xn = xs_int + 1;
        xr = (0xFF - xs_fract) * 2;
    } else {
        xn = xs_int;
        xr = 0xFF;
    }

    /*Handle under/overflow*/
    if(xn >= img->header.w) return false;
    else if(xn < 0) return false;

    int yn;      /*y neightboor*/
    lv_opa_t yr; /*y mix ratio*/
    if(ys_fract < 0x70) {
        yn = ys_int - 1;
        yr = ys_fract * 2;
    } else if(ys_fract > 0x90) {
        yn = ys_int + 1;
        yr = (0xFF - ys_fract) * 2;
    } else {
        yn = ys_int;
        yr = 0xFF;
    }

    /*Handle under/overflow*/
    if(yn >= img->header.h) return false;
    else if(yn < 0) return false;

    /*Get the mixture of the original source and the neightboor pixels in both directions*/
    /*Get the mixture of the original source and the neightboor pixels in both directions*/
    lv_color_t c_dest_int = lv_img_buf_get_px_color(img, xs_int, ys_int, color);

    if(lv_img_color_format_is_chroma_keyed(img->header.cf)) {
        lv_color_t ct = LV_COLOR_TRANSP;
        if(c_dest_int.full == ct.full) return false;
    }

    lv_color_t c_dest_xn = lv_img_buf_get_px_color(img, xn, ys_int, color);
    lv_color_t c_dest_yn = lv_img_buf_get_px_color(img, xs_int, yn, color);

    lv_color_t x_dest    = lv_color_mix(c_dest_int, c_dest_xn, xr);
    lv_color_t y_dest    = lv_color_mix(c_dest_int, c_dest_yn, yr);
    *res_color = lv_color_mix(x_dest, y_dest, LV_OPA_50);

    /*Get result pixel opacity*/
    if(lv_img_color_format_has_alpha(img->header.cf)) {
        lv_opa_t opa_int = lv_img_buf_get_px_alpha(img, xs_int, ys_int);
        lv_opa_t opa_xn  = lv_img_buf_get_px_alpha(img, xn, ys_int);
        lv_opa_t opa_yn  = lv_img_buf_get_px_alpha(img, xs_int, yn);
        lv_opa_t opa_x   = (opa_int * xr + (opa_xn * (255 - xr))) >> 8;
        lv_opa_t opa_y   = (opa_int * yr + (opa_yn * (255 - yr))) >> 8;

        *res_opa = (opa_x + opa_y) / 2;
        if(*res_opa <= LV_OPA_MIN) return false;
    }

    return true;
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
bool lv_img_color_format_has_alpha(lv_img_cf_t cf)
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

lv_img_dsc_t *lv_img_buf_alloc(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf)
{
    /* Allocate image descriptor */
    lv_img_dsc_t *dsc = lv_mem_alloc(sizeof(lv_img_dsc_t));
    if(dsc == NULL)
        return NULL;

    memset(dsc, 0, sizeof(lv_img_dsc_t));

    /* Get image data size */
    dsc->data_size = lv_img_buf_get_img_size(w, h, cf);
    if(dsc->data_size == 0) {
        lv_mem_free(dsc);
        return NULL;
    }

    /* Allocate raw buffer */
    dsc->data = lv_mem_alloc(dsc->data_size);
    if(dsc->data == NULL) {
        lv_mem_free(dsc);
        return NULL;
    }
    memset((uint8_t *)dsc->data, 0, dsc->data_size);

    /* Fill in header */
    dsc->header.always_zero = 0;
    dsc->header.w = w;
    dsc->header.h = h;
    dsc->header.cf = cf;
    return dsc;
}

void lv_img_buf_free(lv_img_dsc_t *dsc)
{
    if(dsc != NULL) {
        if(dsc->data != NULL)
            lv_mem_free(dsc->data);

        lv_mem_free(dsc);
    }
}

uint32_t lv_img_buf_get_img_size(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf)
{
    switch(cf) {
    case LV_IMG_CF_TRUE_COLOR: return LV_IMG_BUF_SIZE_TRUE_COLOR(w, h);
    case LV_IMG_CF_TRUE_COLOR_ALPHA: return LV_IMG_BUF_SIZE_TRUE_COLOR_ALPHA(w, h);
    case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED: return LV_IMG_BUF_SIZE_TRUE_COLOR_CHROMA_KEYED(w, h);
    case LV_IMG_CF_ALPHA_1BIT: return LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h);
    case LV_IMG_CF_ALPHA_2BIT: return LV_IMG_BUF_SIZE_ALPHA_2BIT(w, h);
    case LV_IMG_CF_ALPHA_4BIT: return LV_IMG_BUF_SIZE_ALPHA_4BIT(w, h);
    case LV_IMG_CF_ALPHA_8BIT: return LV_IMG_BUF_SIZE_ALPHA_8BIT(w, h);
    case LV_IMG_CF_INDEXED_1BIT: return LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h);
    case LV_IMG_CF_INDEXED_2BIT: return LV_IMG_BUF_SIZE_INDEXED_2BIT(w, h);
    case LV_IMG_CF_INDEXED_4BIT: return LV_IMG_BUF_SIZE_INDEXED_4BIT(w, h);
    case LV_IMG_CF_INDEXED_8BIT: return LV_IMG_BUF_SIZE_INDEXED_8BIT(w, h);
    default: return 0;
    }
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
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, cdsc->dec_dsc.error_msg, LV_TXT_FLAG_NONE, NULL, NULL, NULL);
    }
    /* The decoder open could open the image and gave the entire uncompressed image.
     * Just draw it!*/
    else if(cdsc->dec_dsc.img_data) {
        lv_draw_map(coords, mask, cdsc->dec_dsc.img_data, opa, chroma_keyed, alpha_byte, style);
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
            lv_draw_map(&line, mask, buf, opa, chroma_keyed, alpha_byte, style);
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
        bool chroma_key, bool alpha_byte, const lv_style_t * style)
{
    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_area_t draw_area;
    bool union_ok;

    /* Get clipped map area which is the real draw area.
     * It is always the same or inside `map_area` */
    union_ok = lv_area_intersect(&draw_area, map_area, clip_area);

    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == false) return;

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
    if(0 && other_mask_cnt == 0 && chroma_key == false && alpha_byte == false && opa == LV_OPA_COVER && style->image.intense == LV_OPA_TRANSP) {
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

        uint16_t angle = 30;

        lv_draw_mask_res_t mask_res;
        mask_res = (alpha_byte || chroma_key || angle) ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
        lv_coord_t x;
        lv_coord_t y;
        for(y = 0; y < lv_area_get_height(&draw_area); y++) {
            map_px = map_buf_tmp;
            px_i_start = px_i;

            for(x = 0; x < lv_area_get_width(&draw_area); x++, map_px += px_size_byte, px_i++) {
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

                lv_img_dsc_t img;
                img.data = map_p;
                img.header.w = lv_area_get_width(map_area);
                img.header.h = lv_area_get_height(map_area);
                img.header.cf = LV_IMG_CF_TRUE_COLOR;
                lv_point_t p = {x + disp_area->x1 - map_area->x1 ,y + disp_area->y1 - map_area->y1};
                lv_point_t piv = {img.header.w / 2 ,img.header.h / 2};
                bool ret;
                ret = lv_img_get_px_rotated(&img, angle, LV_COLOR_BLACK, &p, &piv, &c, NULL);
                if(ret == false) {
                    mask_buf[px_i] = LV_OPA_TRANSP;
                    continue;
                }

                if (chroma_key) {
                    if(c.full == chroma_keyed_color.full) {
                        mask_buf[px_i] = LV_OPA_TRANSP;
                        continue;
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
