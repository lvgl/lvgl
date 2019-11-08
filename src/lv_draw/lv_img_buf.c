/**
 * @file lv_img_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <string.h>
#include "lv_img_buf.h"
#include "lv_draw_img.h"
#include "../lv_misc/lv_math.h"
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
static inline bool transform_anti_alias(lv_img_rotate_dsc_t * dsc);

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
 * Get the color of an image's pixel
 * @param dsc an image descriptor
 * @param x x coordinate of the point to get
 * @param y x coordinate of the point to get
 * @param color the color of the image. In case of `LV_IMG_CF_ALPHA_1/2/4/8` this color is used.
 * Not used in other cases.
 * @param safe true: check out of bounds
 * @return color of the point
 */
lv_color_t lv_img_buf_get_px_color(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_color_t color)
{
    lv_color_t p_color = LV_COLOR_BLACK;
    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR || dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED ||
            dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size = lv_img_cf_get_px_size(dsc->header.cf) >> 3;
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
 * @param safe true: check out of bounds
 * @return alpha value of the point
 */
lv_opa_t lv_img_buf_get_px_alpha(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y)
{
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
 * @param safe true: check out of bounds
 */
void lv_img_buf_set_px_color(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_color_t c)
{
    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR || dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        uint8_t px_size = lv_img_cf_get_px_size(dsc->header.cf) >> 3;
        uint32_t px     = dsc->header.w * y * px_size + x * px_size;
        memcpy(&buf_u8[px], &c, px_size);
    } else if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size = lv_img_cf_get_px_size(dsc->header.cf) >> 3;
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
 * @param safe true: check out of bounds
 */
void lv_img_buf_set_px_alpha(lv_img_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_opa_t opa)
{
    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size          = lv_img_cf_get_px_size(dsc->header.cf) >> 3;
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
 * Allocate an image buffer in RAM
 * @param w width of image
 * @param h height of image
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return an allocated image, or NULL on failure
 */
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

/**
 * Free an allocated image buffer
 * @param dsc image buffer to free
 */
void lv_img_buf_free(lv_img_dsc_t *dsc)
{
    if(dsc != NULL) {
        if(dsc->data != NULL)
            lv_mem_free(dsc->data);

        lv_mem_free(dsc);
    }
}

/**
 * Get the memory consumption of a raw bitmap, given color format and dimensions.
 * @param w width
 * @param h height
 * @param cf color format
 * @return size in bytes
 */
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

/**
 * Initialize a descriptor to rotate an image
 * @param dsc pointer to an `lv_img_rotate_dsc_t` variable
 * @param angle angle to rotate
 * @param src image source (array of pixels)
 * @param src_w width of the image to rotate
 * @param src_h height of the image to rotate
 * @param cf color format of the image to rotate
 * @param pivot_x pivot x
 * @param pivot_y pivot y
 * @param color a color used for `LV_IMG_CF_INDEXED_1/2/4/8BIT` color formats
 */
void lv_img_buf_rotate_init(lv_img_rotate_dsc_t * dsc, int16_t angle, const void * src, lv_coord_t src_w, lv_coord_t src_h,
        lv_img_cf_t cf, lv_coord_t pivot_x, lv_coord_t pivot_y, lv_color_t color)
{
    memset(dsc, 0x00, sizeof(lv_img_rotate_dsc_t));

    dsc->angle = angle;
    dsc->src = src;
    dsc->src_w = src_w;
    dsc->src_h = src_h;
    dsc->cf = cf;
    dsc->color = color;
    dsc->pivot_x = pivot_x;
    dsc->pivot_y = pivot_y;
    dsc->pivot_x_256 = pivot_x * 256;
    dsc->pivot_y_256 = pivot_y * 256;
    dsc->sinma = lv_trigo_sin(-angle);
    dsc->cosma = lv_trigo_sin(-angle + 90);

    dsc->chroma_keyed = lv_img_cf_is_chroma_keyed(cf) ? 1 : 0;
    dsc->has_alpha = lv_img_cf_has_alpha(cf) ? 1 : 0;
    if(cf == LV_IMG_CF_TRUE_COLOR || cf == LV_IMG_CF_TRUE_COLOR_ALPHA || cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        dsc->native_color = 0;
    }

    dsc->img_dsc.data = src;
    dsc->img_dsc.header.always_zero = 0;
    dsc->img_dsc.header.cf = cf;
    dsc->img_dsc.header.w = src_w;
    dsc->img_dsc.header.h = src_h;

    dsc->res_opa = LV_OPA_COVER;
}

/**
 * Get which color and opa would come to a pixel if it were rotated
 * @param dsc a descriptor initialized by `lv_img_buf_rotate_init`
 * @param x the coordinate which color and opa should be get
 * @param y the coordinate which color and opa should be get
 * @return true: there is valid pixel on these x/y coordinates; false: the rotated pixel was out of the image
 * @note the result is written back to `dsc->res_color` and `dsc->res_opa`
 */
bool lv_img_buf_get_px_rotated(lv_img_rotate_dsc_t * dsc, lv_coord_t x, lv_coord_t y)
{
    const uint8_t * src_u8 = dsc->src;

    /*Get the target point relative coordinates to the pivot*/
    int32_t xt = x - dsc->pivot_x;
    int32_t yt = y - dsc->pivot_y;

    bool fast = false;
    bool zoomed = false;
    uint16_t zoom = 128;
    uint16_t zoom_inv = (256 / zoom) * 256;;
    int32_t xs;
    int32_t ys;
    if(!zoomed) {
        /*Get the source pixel from the upscaled image*/
        xs = ((dsc->cosma * xt - dsc->sinma * yt) >> (LV_TRIGO_SHIFT - 8)) + dsc->pivot_x_256;
        ys = ((dsc->sinma * xt + dsc->cosma * yt) >> (LV_TRIGO_SHIFT - 8)) + dsc->pivot_y_256;
    } else {
        xt *= zoom_inv;
        yt *= zoom_inv;
        xs = ((dsc->cosma * xt - dsc->sinma * yt) >> (LV_TRIGO_SHIFT)) + dsc->pivot_x_256;
        ys = ((dsc->sinma * xt + dsc->cosma * yt) >> (LV_TRIGO_SHIFT)) + dsc->pivot_y_256;

    }

    //    xt = xt / 2;
    //    yt = yt / 2;

    /*Get the integer part of the source pixel*/
    int xs_int = xs >> 8;
    int ys_int = ys >> 8;

    if(xs_int >= dsc->src_w) return false;
    else if(xs_int < 0) return false;

    if(ys_int >= dsc->src_h) return false;
    else if(ys_int < 0) return false;

    /* If the fractional < 0x70 mix the source pixel with the left/top pixel
     * If the fractional > 0x90 mix the source pixel with the right/bottom pixel
     * In the 0x70..0x90 range use the unchanged source pixel */

    uint8_t px_size;
    uint32_t pxi;
    if(dsc->native_color) {
        if(dsc->has_alpha == 0) {
            px_size = LV_COLOR_SIZE >> 3;

            pxi     = dsc->src_w * ys_int * px_size + xs_int * px_size;
            memcpy(&dsc->res_color, &src_u8[pxi], px_size);
        } else {
            px_size = LV_IMG_PX_SIZE_ALPHA_BYTE;
            pxi     = dsc->src_w * ys_int * px_size + xs_int * px_size;
            memcpy(&dsc->res_color, &src_u8[pxi], px_size - 1);
            dsc->res_opa = src_u8[pxi + px_size - 1];
        }
    } else {
        pxi = 0; /*unused*/
        px_size = 0;    /*unused*/
        dsc->res_color = lv_img_buf_get_px_color(&dsc->img_dsc, xs_int, ys_int, dsc->color);
        dsc->res_opa = lv_img_buf_get_px_alpha(&dsc->img_dsc, xs_int, ys_int);
    }

    if(dsc->chroma_keyed) {
        lv_color_t ct = LV_COLOR_TRANSP;
        if(dsc->res_color.full == ct.full) return false;
    }


    if(fast) return true;

    dsc->xs = xs;
    dsc->ys = ys;
    dsc->xs_int = xs_int;
    dsc->ys_int = ys_int;
    dsc->pxi = pxi;
    dsc->px_size = px_size;

    bool ret;

    ret = transform_anti_alias(dsc);

    return ret;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static inline bool transform_anti_alias(lv_img_rotate_dsc_t * dsc)
{
    const uint8_t * src_u8 = dsc->src;

    /*Get the fractional part of the source pixel*/
    int xs_fract = dsc->xs & 0xff;
    int ys_fract = dsc->ys & 0xff;
    int32_t xn;      /*x neightboor*/
    lv_opa_t xr; /*x mix ratio*/

    if(xs_fract < 0x70) {
          xn = - 1;
          if(dsc->xs_int + xn < 0) return false;
          xr = xs_fract + 0x80;
    } else if(xs_fract > 0x90) {
        xn =  1;
        if(dsc->xs_int + xn >= dsc->src_w) return false;
        xr = (0xFF - xs_fract) + 0x80;
    } else {
        xn = 0;
        xr = 0xFF;
    }

    int32_t yn;      /*x neightboor*/
    lv_opa_t yr; /*x mix ratio*/

    if(ys_fract < 0x70) {
          yn = - 1;
          if(dsc->ys_int + yn < 0) return false;

          yr = ys_fract + 0x80;
    } else if(ys_fract > 0x90) {
        yn =  1;
        if(dsc->ys_int + yn >= dsc->src_h) return false;

        yr = (0xFF - ys_fract) + 0x80;
    } else {
        yn = 0;
        yr = 0xFF;
    }

    lv_color_t c00 = dsc->res_color;
    lv_color_t c01;
    lv_color_t c10;
    lv_color_t c11;

    lv_opa_t a00 = dsc->res_opa;
    lv_opa_t a10;
    lv_opa_t a01;
    lv_opa_t a11;

    if(dsc->native_color) {
        memcpy(&c01, &src_u8[dsc->pxi + dsc->px_size * xn], sizeof(lv_color_t));
        memcpy(&c10, &src_u8[dsc->pxi + dsc->src_w * dsc->px_size * yn], sizeof(lv_color_t));
        memcpy(&c11, &src_u8[dsc->pxi + dsc->src_w * dsc->px_size * yn + dsc->px_size * xn], sizeof(lv_color_t));
        if(dsc->has_alpha) {
            a10 = src_u8[dsc->pxi + dsc->px_size * xn + dsc->px_size - 1];
            a01 = src_u8[dsc->pxi + dsc->src_w * dsc->px_size * yn + dsc->px_size - 1];
            a11 = src_u8[dsc->pxi + dsc->src_w * dsc->px_size * yn + dsc->px_size * xn + dsc->px_size - 1];
        }
    } else {
        c01 = lv_img_buf_get_px_color(&dsc->img_dsc, dsc->xs_int + xn, dsc->ys_int, dsc->color);
        c10 = lv_img_buf_get_px_color(&dsc->img_dsc, dsc->xs_int, dsc->ys_int + yn, dsc->color);
        c11 = lv_img_buf_get_px_color(&dsc->img_dsc, dsc->xs_int + xn, dsc->ys_int + yn, dsc->color);

        if(dsc->has_alpha) {
            a10 = lv_img_buf_get_px_alpha(&dsc->img_dsc, dsc->xs_int + xn, dsc->ys_int);
            a01 = lv_img_buf_get_px_alpha(&dsc->img_dsc, dsc->xs_int, dsc->ys_int + yn);
            a11 = lv_img_buf_get_px_alpha(&dsc->img_dsc, dsc->xs_int + xn, dsc->ys_int + yn);
        }

    }

    lv_opa_t a0;
    lv_opa_t a1;
    lv_opa_t xr0 = xr;
    lv_opa_t xr1 = xr;
    if(dsc->has_alpha) {
        a0 = (a00 * xr + (a10 * (255 - xr))) >> 8;
        a1 = (a01 * xr + (a11 * (255 - xr))) >> 8;
        dsc->res_opa = (a0 * yr + (a1 * (255 - yr))) >> 8;
    } else {
        xr0 = xr;
        xr1 = xr;
        dsc->res_opa = LV_OPA_COVER;
    }

    lv_color_t c0 = lv_color_mix(c00, c01, xr0);
    lv_color_t c1 = lv_color_mix(c10, c11, xr1);

    dsc->res_color = lv_color_mix(c0, c1, yr);

    return true;
}
