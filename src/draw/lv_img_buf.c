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
#include "../misc/lv_math.h"
#include "../misc/lv_log.h"
#include "../misc/lv_mem.h"

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
    lv_color_t p_color = lv_color_black();
    uint8_t * buf_u8 = (uint8_t *)dsc->data;

    if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR || dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED ||
       dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size = lv_img_cf_get_px_size(dsc->header.cf) >> 3;
        uint32_t px     = dsc->header.w * y * px_size + x * px_size;
        lv_memcpy_small(&p_color, &buf_u8[px], sizeof(lv_color_t));
#if LV_COLOR_SIZE == 32
        p_color.ch.alpha = 0xFF; /*Only the color should be get so use a default alpha value*/
#endif
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_1BIT) {
        buf_u8 += 4 * 2;
        uint8_t bit = x & 0x7;
        x           = x >> 3;

        /*Get the current pixel.
         *dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 8, 16, 24 ...*/
        uint32_t px  = ((dsc->header.w + 7) >> 3) * y + x;
        p_color.full = (buf_u8[px] & (1 << (7 - bit))) >> (7 - bit);
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_2BIT) {
        buf_u8 += 4 * 4;
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /*Get the current pixel.
         *dsc->header.w + 3 means rounding up to 4 because the lines are byte aligned
         *so the possible real width are 4, 8, 12 ...*/
        uint32_t px  = ((dsc->header.w + 3) >> 2) * y + x;
        p_color.full = (buf_u8[px] & (3 << (6 - bit))) >> (6 - bit);
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_4BIT) {
        buf_u8 += 4 * 16;
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /*Get the current pixel.
         *dsc->header.w + 1 means rounding up to 2 because the lines are byte aligned
         *so the possible real width are 2, 4, 6 ...*/
        uint32_t px  = ((dsc->header.w + 1) >> 1) * y + x;
        p_color.full = (buf_u8[px] & (0xF << (4 - bit))) >> (4 - bit);
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
        buf_u8 += 4 * 256;
        uint32_t px  = dsc->header.w * y + x;
        p_color.full = buf_u8[px];
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT || dsc->header.cf == LV_IMG_CF_ALPHA_2BIT ||
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
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT) {
        uint8_t bit = x & 0x7;
        x           = x >> 3;

        /*Get the current pixel.
         *dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 8 ,16, 24 ...*/
        uint32_t px    = ((dsc->header.w + 7) >> 3) * y + x;
        uint8_t px_opa = (buf_u8[px] & (1 << (7 - bit))) >> (7 - bit);
        return px_opa ? LV_OPA_TRANSP : LV_OPA_COVER;
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_2BIT) {
        const uint8_t opa_table[4] = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/

        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /*Get the current pixel.
         *dsc->header.w + 4 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 4 ,8, 12 ...*/
        uint32_t px    = ((dsc->header.w + 3) >> 2) * y + x;
        uint8_t px_opa = (buf_u8[px] & (3 << (6 - bit))) >> (6 - bit);
        return opa_table[px_opa];
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_4BIT) {
        const uint8_t opa_table[16] = {0,  17, 34,  51, /*Opacity mapping with bpp = 4*/
                                       68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255
                                      };

        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /*Get the current pixel.
         *dsc->header.w + 1 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 2 ,4, 6 ...*/
        uint32_t px    = ((dsc->header.w + 1) >> 1) * y + x;
        uint8_t px_opa = (buf_u8[px] & (0xF << (4 - bit))) >> (4 - bit);
        return opa_table[px_opa];
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
        uint32_t px = dsc->header.w * y + x;
        return buf_u8[px];
    }

    return LV_OPA_COVER;
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
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_1BIT) {
        opa         = opa >> 7; /*opa -> [0,1]*/
        uint8_t bit = x & 0x7;
        x           = x >> 3;

        /*Get the current pixel.
         *dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 8 ,16, 24 ...*/
        uint32_t px = ((dsc->header.w + 7) >> 3) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(1 << (7 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0x1) << (7 - bit));
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_2BIT) {
        opa         = opa >> 6; /*opa -> [0,3]*/
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /*Get the current pixel.
         *dsc->header.w + 4 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 4 ,8, 12 ...*/
        uint32_t px = ((dsc->header.w + 3) >> 2) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(3 << (6 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0x3) << (6 - bit));
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_4BIT) {
        opa         = opa >> 4; /*opa -> [0,15]*/
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /*Get the current pixel.
         *dsc->header.w + 1 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 2 ,4, 6 ...*/
        uint32_t px = ((dsc->header.w + 1) >> 1) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(0xF << (4 - bit));
        buf_u8[px]  = buf_u8[px] | ((opa & 0xF) << (4 - bit));
    }
    else if(dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
        uint32_t px = dsc->header.w * y + x;
        buf_u8[px]  = opa;
    }
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
        lv_memcpy_small(&buf_u8[px], &c, px_size);
    }
    else if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA) {
        uint8_t px_size = lv_img_cf_get_px_size(dsc->header.cf) >> 3;
        uint32_t px     = dsc->header.w * y * px_size + x * px_size;
        lv_memcpy_small(&buf_u8[px], &c, px_size - 1); /*-1 to not overwrite the alpha value*/
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_1BIT) {
        buf_u8 += sizeof(lv_color32_t) * 2; /*Skip the palette*/

        uint8_t bit = x & 0x7;
        x           = x >> 3;

        /*Get the current pixel.
         *dsc->header.w + 7 means rounding up to 8 because the lines are byte aligned
         *so the possible real width are 8 ,16, 24 ...*/
        uint32_t px = ((dsc->header.w + 7) >> 3) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(1 << (7 - bit));
        buf_u8[px]  = buf_u8[px] | ((c.full & 0x1) << (7 - bit));
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_2BIT) {
        buf_u8 += sizeof(lv_color32_t) * 4; /*Skip the palette*/
        uint8_t bit = (x & 0x3) * 2;
        x           = x >> 2;

        /*Get the current pixel.
         *dsc->header.w + 3 means rounding up to 4 because the lines are byte aligned
         *so the possible real width are 4, 8 ,12 ...*/
        uint32_t px = ((dsc->header.w + 3) >> 2) * y + x;

        buf_u8[px] = buf_u8[px] & ~(3 << (6 - bit));
        buf_u8[px] = buf_u8[px] | ((c.full & 0x3) << (6 - bit));
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_4BIT) {
        buf_u8 += sizeof(lv_color32_t) * 16; /*Skip the palette*/
        uint8_t bit = (x & 0x1) * 4;
        x           = x >> 1;

        /*Get the current pixel.
         *dsc->header.w + 1 means rounding up to 2 because the lines are byte aligned
         *so the possible real width are 2 ,4, 6 ...*/
        uint32_t px = ((dsc->header.w + 1) >> 1) * y + x;
        buf_u8[px]  = buf_u8[px] & ~(0xF << (4 - bit));
        buf_u8[px]  = buf_u8[px] | ((c.full & 0xF) << (4 - bit));
    }
    else if(dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
        buf_u8 += sizeof(lv_color32_t) * 256; /*Skip the palette*/
        uint32_t px = dsc->header.w * y + x;
        buf_u8[px]  = c.full;
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
    lv_memcpy_small(&buf[id * sizeof(c32)], &c32, sizeof(c32));
}

/**
 * Allocate an image buffer in RAM
 * @param w width of image
 * @param h height of image
 * @param cf a color format (`LV_IMG_CF_...`)
 * @return an allocated image, or NULL on failure
 */
lv_img_dsc_t * lv_img_buf_alloc(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf)
{
    /*Allocate image descriptor*/
    lv_img_dsc_t * dsc = lv_mem_alloc(sizeof(lv_img_dsc_t));
    if(dsc == NULL)
        return NULL;

    lv_memset_00(dsc, sizeof(lv_img_dsc_t));

    /*Get image data size*/
    dsc->data_size = lv_img_buf_get_img_size(w, h, cf);
    if(dsc->data_size == 0) {
        lv_mem_free(dsc);
        return NULL;
    }

    /*Allocate raw buffer*/
    dsc->data = lv_mem_alloc(dsc->data_size);
    if(dsc->data == NULL) {
        lv_mem_free(dsc);
        return NULL;
    }
    lv_memset_00((uint8_t *)dsc->data, dsc->data_size);

    /*Fill in header*/
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
void lv_img_buf_free(lv_img_dsc_t * dsc)
{
    if(dsc != NULL) {
        if(dsc->data != NULL)
            lv_mem_free((void *)dsc->data);

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
        case LV_IMG_CF_TRUE_COLOR:
            return LV_IMG_BUF_SIZE_TRUE_COLOR(w, h);
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
            return LV_IMG_BUF_SIZE_TRUE_COLOR_ALPHA(w, h);
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
            return LV_IMG_BUF_SIZE_TRUE_COLOR_CHROMA_KEYED(w, h);
        case LV_IMG_CF_ALPHA_1BIT:
            return LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h);
        case LV_IMG_CF_ALPHA_2BIT:
            return LV_IMG_BUF_SIZE_ALPHA_2BIT(w, h);
        case LV_IMG_CF_ALPHA_4BIT:
            return LV_IMG_BUF_SIZE_ALPHA_4BIT(w, h);
        case LV_IMG_CF_ALPHA_8BIT:
            return LV_IMG_BUF_SIZE_ALPHA_8BIT(w, h);
        case LV_IMG_CF_INDEXED_1BIT:
            return LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h);
        case LV_IMG_CF_INDEXED_2BIT:
            return LV_IMG_BUF_SIZE_INDEXED_2BIT(w, h);
        case LV_IMG_CF_INDEXED_4BIT:
            return LV_IMG_BUF_SIZE_INDEXED_4BIT(w, h);
        case LV_IMG_CF_INDEXED_8BIT:
            return LV_IMG_BUF_SIZE_INDEXED_8BIT(w, h);
        default:
            return 0;
    }
}

#if LV_DRAW_COMPLEX
/**
 * Initialize a descriptor to transform an image
 * @param dsc pointer to an `lv_img_transform_dsc_t` variable whose `cfg` field is initialized
 */
void _lv_img_buf_transform_init(lv_img_transform_dsc_t * dsc)
{
    dsc->tmp.pivot_x_256 = dsc->cfg.pivot_x * 256;
    dsc->tmp.pivot_y_256 = dsc->cfg.pivot_y * 256;

    int32_t angle_low = dsc->cfg.angle / 10;
    int32_t angle_high = angle_low + 1;
    int32_t angle_rem = dsc->cfg.angle  - (angle_low * 10);

    int32_t s1 = lv_trigo_sin(-angle_low);
    int32_t s2 = lv_trigo_sin(-angle_high);

    int32_t c1 = lv_trigo_sin(-angle_low + 90);
    int32_t c2 = lv_trigo_sin(-angle_high + 90);

    dsc->tmp.sinma = (s1 * (10 - angle_rem) + s2 * angle_rem) / 10;
    dsc->tmp.cosma = (c1 * (10 - angle_rem) + c2 * angle_rem) / 10;

    /*Use smaller value to avoid overflow*/
    dsc->tmp.sinma = dsc->tmp.sinma >> (LV_TRIGO_SHIFT - _LV_TRANSFORM_TRIGO_SHIFT);
    dsc->tmp.cosma = dsc->tmp.cosma >> (LV_TRIGO_SHIFT - _LV_TRANSFORM_TRIGO_SHIFT);

    dsc->tmp.chroma_keyed = lv_img_cf_is_chroma_keyed(dsc->cfg.cf) ? 1 : 0;
    dsc->tmp.has_alpha = lv_img_cf_has_alpha(dsc->cfg.cf) ? 1 : 0;
    if(dsc->cfg.cf == LV_IMG_CF_TRUE_COLOR || dsc->cfg.cf == LV_IMG_CF_TRUE_COLOR_ALPHA ||
       dsc->cfg.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        dsc->tmp.native_color = 1;
    }
    else {
        dsc->tmp.native_color = 0;
    }

    dsc->tmp.img_dsc.data = dsc->cfg.src;
    dsc->tmp.img_dsc.header.always_zero = 0;
    dsc->tmp.img_dsc.header.cf = dsc->cfg.cf;
    dsc->tmp.img_dsc.header.w = dsc->cfg.src_w;
    dsc->tmp.img_dsc.header.h = dsc->cfg.src_h;

    /*The inverse of the zoom will be sued during the transformation
     * + dsc->cfg.zoom / 2 for rounding*/
    dsc->tmp.zoom_inv = (((256 * 256) << _LV_ZOOM_INV_UPSCALE) + dsc->cfg.zoom / 2) / dsc->cfg.zoom;

    dsc->res.opa = LV_OPA_COVER;
    dsc->res.color = dsc->cfg.color;
}
#endif

/**
 * Get the area of a rectangle if its rotated and scaled
 * @param res store the coordinates here
 * @param w width of the rectangle to transform
 * @param h height of the rectangle to transform
 * @param angle angle of rotation
 * @param zoom zoom, (256 no zoom)
 * @param pivot x,y pivot coordinates of rotation
 */
void _lv_img_buf_get_transformed_area(lv_area_t * res, lv_coord_t w, lv_coord_t h, int16_t angle, uint16_t zoom,
                                      const lv_point_t * pivot)
{
#if LV_DRAW_COMPLEX
    if(angle == 0 && zoom == LV_IMG_ZOOM_NONE) {
        res->x1 = 0;
        res->y1 = 0;
        res->x2 = w - 1;
        res->y2 = h - 1;
        return;
    }

    res->x1 = (((int32_t)(-pivot->x) * zoom) >> 8) - 1;
    res->y1 = (((int32_t)(-pivot->y) * zoom) >> 8) - 1;
    res->x2 = (((int32_t)(w - pivot->x) * zoom) >> 8) + 2;
    res->y2 = (((int32_t)(h - pivot->y) * zoom) >> 8) + 2;

    if(angle == 0) {
        res->x1 += pivot->x;
        res->y1 += pivot->y;
        res->x2 += pivot->x;
        res->y2 += pivot->y;
        return;
    }

    int32_t angle_low = angle / 10;
    int32_t angle_high = angle_low + 1;
    int32_t angle_rem = angle  - (angle_low * 10);

    int32_t s1 = lv_trigo_sin(angle_low);
    int32_t s2 = lv_trigo_sin(angle_high);

    int32_t c1 = lv_trigo_sin(angle_low + 90);
    int32_t c2 = lv_trigo_sin(angle_high + 90);

    int32_t sinma = (s1 * (10 - angle_rem) + s2 * angle_rem) / 10;
    int32_t cosma = (c1 * (10 - angle_rem) + c2 * angle_rem) / 10;

    /*Use smaller value to avoid overflow*/
    sinma = sinma >> (LV_TRIGO_SHIFT - _LV_TRANSFORM_TRIGO_SHIFT);
    cosma = cosma >> (LV_TRIGO_SHIFT - _LV_TRANSFORM_TRIGO_SHIFT);

    lv_point_t lt;
    lv_point_t rt;
    lv_point_t lb;
    lv_point_t rb;

    lv_coord_t xt;
    lv_coord_t yt;

    xt = res->x1;
    yt = res->y1;
    lt.x = ((cosma * xt - sinma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->x;
    lt.y = ((sinma * xt + cosma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->y;

    xt = res->x2;
    yt = res->y1;
    rt.x = ((cosma * xt - sinma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->x;
    rt.y = ((sinma * xt + cosma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->y;

    xt = res->x1;
    yt = res->y2;
    lb.x = ((cosma * xt - sinma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->x;
    lb.y = ((sinma * xt + cosma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->y;

    xt = res->x2;
    yt = res->y2;
    rb.x = ((cosma * xt - sinma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->x;
    rb.y = ((sinma * xt + cosma * yt) >> _LV_TRANSFORM_TRIGO_SHIFT) + pivot->y;

    res->x1 = LV_MIN4(lb.x, lt.x, rb.x, rt.x);
    res->x2 = LV_MAX4(lb.x, lt.x, rb.x, rt.x);
    res->y1 = LV_MIN4(lb.y, lt.y, rb.y, rt.y);
    res->y2 = LV_MAX4(lb.y, lt.y, rb.y, rt.y);
#else
    LV_UNUSED(angle);
    LV_UNUSED(zoom);
    LV_UNUSED(pivot);
    res->x1 = 0;
    res->y1 = 0;
    res->x2 = w - 1;
    res->y2 = h - 1;
#endif
}


#if LV_DRAW_COMPLEX
/**
 * Get which color and opa would come to a pixel if it were rotated
 * @param dsc a descriptor initialized by `lv_img_buf_rotate_init`
 * @param x the coordinate which color and opa should be get
 * @param y the coordinate which color and opa should be get
 * @return true: there is valid pixel on these x/y coordinates; false: the rotated pixel was out of the image
 * @note the result is written back to `dsc->res_color` and `dsc->res_opa`
 */
bool _lv_img_buf_transform(lv_img_transform_dsc_t * dsc, lv_coord_t x, lv_coord_t y)
{
    const uint8_t * src_u8 = (const uint8_t *)dsc->cfg.src;

    /*Get the target point relative coordinates to the pivot*/
    int32_t xt = x - dsc->cfg.pivot_x;
    int32_t yt = y - dsc->cfg.pivot_y;

    int32_t xs;
    int32_t ys;
    if(dsc->cfg.zoom == LV_IMG_ZOOM_NONE) {
        /*Get the source pixel from the upscaled image*/
        xs = ((dsc->tmp.cosma * xt - dsc->tmp.sinma * yt) >> (_LV_TRANSFORM_TRIGO_SHIFT - 8)) + dsc->tmp.pivot_x_256;
        ys = ((dsc->tmp.sinma * xt + dsc->tmp.cosma * yt) >> (_LV_TRANSFORM_TRIGO_SHIFT - 8)) + dsc->tmp.pivot_y_256;
    }
    else if(dsc->cfg.angle == 0) {
        xt = (int32_t)((int32_t)xt * dsc->tmp.zoom_inv) >> _LV_ZOOM_INV_UPSCALE;
        yt = (int32_t)((int32_t)yt * dsc->tmp.zoom_inv) >> _LV_ZOOM_INV_UPSCALE;
        xs = xt + dsc->tmp.pivot_x_256;
        ys = yt + dsc->tmp.pivot_y_256;
    }
    else {
        xt = (int32_t)((int32_t)xt * dsc->tmp.zoom_inv) >> _LV_ZOOM_INV_UPSCALE;
        yt = (int32_t)((int32_t)yt * dsc->tmp.zoom_inv) >> _LV_ZOOM_INV_UPSCALE;
        xs = ((dsc->tmp.cosma * xt - dsc->tmp.sinma * yt) >> (_LV_TRANSFORM_TRIGO_SHIFT)) + dsc->tmp.pivot_x_256;
        ys = ((dsc->tmp.sinma * xt + dsc->tmp.cosma * yt) >> (_LV_TRANSFORM_TRIGO_SHIFT)) + dsc->tmp.pivot_y_256;
    }

    /*Get the integer part of the source pixel*/
    int32_t xs_int = xs >> 8;
    int32_t ys_int = ys >> 8;

    if(xs_int >= dsc->cfg.src_w) return false;
    else if(xs_int < 0) return false;

    if(ys_int >= dsc->cfg.src_h) return false;
    else if(ys_int < 0) return false;

    uint8_t px_size;
    uint32_t pxi;
    if(dsc->tmp.native_color) {
        if(dsc->tmp.has_alpha == 0) {
            px_size = LV_COLOR_SIZE >> 3;

            pxi     = dsc->cfg.src_w * ys_int * px_size + xs_int * px_size;
            lv_memcpy_small(&dsc->res.color, &src_u8[pxi], px_size);
        }
        else {
            px_size = LV_IMG_PX_SIZE_ALPHA_BYTE;
            pxi     = dsc->cfg.src_w * ys_int * px_size + xs_int * px_size;
            lv_memcpy_small(&dsc->res.color, &src_u8[pxi], px_size - 1);
            dsc->res.opa = src_u8[pxi + px_size - 1];
        }
    }
    else {
        pxi = 0; /*unused*/
        px_size = 0;    /*unused*/
        dsc->res.color = lv_img_buf_get_px_color(&dsc->tmp.img_dsc, xs_int, ys_int, dsc->cfg.color);
        dsc->res.opa = lv_img_buf_get_px_alpha(&dsc->tmp.img_dsc, xs_int, ys_int);
    }

    if(dsc->tmp.chroma_keyed) {
        lv_color_t ct = LV_COLOR_CHROMA_KEY;
        if(dsc->res.color.full == ct.full) return false;
    }

    if(dsc->cfg.antialias == false) return true;

    dsc->tmp.xs = xs;
    dsc->tmp.ys = ys;
    dsc->tmp.xs_int = xs_int;
    dsc->tmp.ys_int = ys_int;
    dsc->tmp.pxi = pxi;
    dsc->tmp.px_size = px_size;

    bool ret;
    ret = _lv_img_buf_transform_anti_alias(dsc);

    return ret;
}

/**
 * Continue transformation by taking the neighbors into account
 * @param dsc pointer to the transformation descriptor
 */
bool _lv_img_buf_transform_anti_alias(lv_img_transform_dsc_t * dsc)
{
    const uint8_t * src_u8 = dsc->cfg.src;

    /*Get the fractional part of the source pixel*/
    int xs_fract = dsc->tmp.xs & 0xff;
    int ys_fract = dsc->tmp.ys & 0xff;
    int32_t xn;      /*x neighbor*/
    lv_opa_t xr; /*x mix ratio*/

    if(xs_fract < 0x70) {
        xn = - 1;
        if(dsc->tmp.xs_int + xn < 0) xn = 0;
        xr = xs_fract + 0x80;
    }
    else if(xs_fract > 0x90) {
        xn = 1;
        if(dsc->tmp.xs_int + xn >= dsc->cfg.src_w) xn = 0;
        xr = (0xFF - xs_fract) + 0x80;
    }
    else {
        xn = 0;
        xr = 0xFF;
    }

    int32_t yn;      /*x neighbor*/
    lv_opa_t yr; /*x mix ratio*/

    if(ys_fract < 0x70) {
        yn = - 1;
        if(dsc->tmp.ys_int + yn < 0) yn = 0;

        yr = ys_fract + 0x80;
    }
    else if(ys_fract > 0x90) {
        yn = 1;
        if(dsc->tmp.ys_int + yn >= dsc->cfg.src_h) yn = 0;

        yr = (0xFF - ys_fract) + 0x80;
    }
    else {
        yn = 0;
        yr = 0xFF;
    }

    lv_color_t c00 = dsc->res.color;
    lv_color_t c01;
    lv_color_t c10;
    lv_color_t c11;

    lv_opa_t a00 = dsc->res.opa;
    lv_opa_t a10 = 0;
    lv_opa_t a01 = 0;
    lv_opa_t a11 = 0;

    if(dsc->tmp.native_color) {
        lv_memcpy_small(&c01, &src_u8[dsc->tmp.pxi + dsc->tmp.px_size * xn], sizeof(lv_color_t));
        lv_memcpy_small(&c10, &src_u8[dsc->tmp.pxi + dsc->cfg.src_w * dsc->tmp.px_size * yn], sizeof(lv_color_t));
        lv_memcpy_small(&c11, &src_u8[dsc->tmp.pxi + dsc->cfg.src_w * dsc->tmp.px_size * yn + dsc->tmp.px_size * xn],
                        sizeof(lv_color_t));
        if(dsc->tmp.has_alpha) {
            a10 = src_u8[dsc->tmp.pxi + dsc->tmp.px_size * xn + dsc->tmp.px_size - 1];
            a01 = src_u8[dsc->tmp.pxi + dsc->cfg.src_w * dsc->tmp.px_size * yn + dsc->tmp.px_size - 1];
            a11 = src_u8[dsc->tmp.pxi + dsc->cfg.src_w * dsc->tmp.px_size * yn + dsc->tmp.px_size * xn + dsc->tmp.px_size - 1];
        }
    }
    else {
        c01 = lv_img_buf_get_px_color(&dsc->tmp.img_dsc, dsc->tmp.xs_int + xn, dsc->tmp.ys_int, dsc->cfg.color);
        c10 = lv_img_buf_get_px_color(&dsc->tmp.img_dsc, dsc->tmp.xs_int, dsc->tmp.ys_int + yn, dsc->cfg.color);
        c11 = lv_img_buf_get_px_color(&dsc->tmp.img_dsc, dsc->tmp.xs_int + xn, dsc->tmp.ys_int + yn, dsc->cfg.color);

        if(dsc->tmp.has_alpha) {
            a10 = lv_img_buf_get_px_alpha(&dsc->tmp.img_dsc, dsc->tmp.xs_int + xn, dsc->tmp.ys_int);
            a01 = lv_img_buf_get_px_alpha(&dsc->tmp.img_dsc, dsc->tmp.xs_int, dsc->tmp.ys_int + yn);
            a11 = lv_img_buf_get_px_alpha(&dsc->tmp.img_dsc, dsc->tmp.xs_int + xn, dsc->tmp.ys_int + yn);
        }
    }

    lv_opa_t xr0 = xr;
    lv_opa_t xr1 = xr;
    if(dsc->tmp.has_alpha) {
        lv_opa_t a0 = (a00 * xr + (a10 * (255 - xr))) >> 8;
        lv_opa_t a1 = (a01 * xr + (a11 * (255 - xr))) >> 8;
        dsc->res.opa = (a0 * yr + (a1 * (255 - yr))) >> 8;

        if(a0 <= LV_OPA_MIN && a1 <= LV_OPA_MIN) return false;
        if(a0 <= LV_OPA_MIN) yr = LV_OPA_TRANSP;
        if(a1 <= LV_OPA_MIN) yr = LV_OPA_COVER;
        if(a00 <= LV_OPA_MIN) xr0 = LV_OPA_TRANSP;
        if(a10 <= LV_OPA_MIN) xr0 = LV_OPA_COVER;
        if(a01 <= LV_OPA_MIN) xr1 = LV_OPA_TRANSP;
        if(a11 <= LV_OPA_MIN) xr1 = LV_OPA_COVER;
    }
    else {
        xr0 = xr;
        xr1 = xr;
        dsc->res.opa = LV_OPA_COVER;
    }

    lv_color_t c0;
    if(xr0 == LV_OPA_TRANSP) c0 = c01;
    else if(xr0 == LV_OPA_COVER) c0 = c00;
    else c0 = lv_color_mix(c00, c01, xr0);

    lv_color_t c1;
    if(xr1 == LV_OPA_TRANSP) c1 = c11;
    else if(xr1 == LV_OPA_COVER) c1 = c10;
    else c1 = lv_color_mix(c10, c11, xr1);

    if(yr == LV_OPA_TRANSP) dsc->res.color = c1;
    else if(yr == LV_OPA_COVER) dsc->res.color = c0;
    else dsc->res.color = lv_color_mix(c0, c1, yr);

    return true;
}
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/
