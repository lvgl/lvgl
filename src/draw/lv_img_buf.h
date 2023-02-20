/**
 * @file lv_img_buf.h
 *
 */

#ifndef LV_IMG_BUF_H
#define LV_IMG_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

#define LV_IMG_BUF_SIZE_TRUE_COLOR(w, h) ((LV_COLOR_DEPTH / 8) * (w) * (h))
#define LV_IMG_BUF_SIZE_TRUE_COLOR_CHROMA_KEYED(w, h) ((LV_COLOR_DEPTH / 8) * (w) * (h))
#define LV_IMG_BUF_SIZE_TRUE_COLOR_ALPHA(w, h) (LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE * (w) * (h))

/*+ 1: to be sure no fractional row*/
#define LV_IMG_BUF_SIZE_ALPHA_1BIT(w, h) (((((w) + 7) / 8) * (h)))
#define LV_IMG_BUF_SIZE_ALPHA_2BIT(w, h) (((((w) + 3) / 4) * (h)))
#define LV_IMG_BUF_SIZE_ALPHA_4BIT(w, h) (((((w) + 1 ) / 2) * (h)))
#define LV_IMG_BUF_SIZE_ALPHA_8BIT(w, h) (((w) * (h)))

/*4 * X: for palette*/
#define LV_IMG_BUF_SIZE_INDEXED_1BIT(w, h) (LV_IMG_BUF_SIZE_ALPHA_1BIT((w), (h)) + 4 * 2)
#define LV_IMG_BUF_SIZE_INDEXED_2BIT(w, h) (LV_IMG_BUF_SIZE_ALPHA_2BIT((w), (h)) + 4 * 4)
#define LV_IMG_BUF_SIZE_INDEXED_4BIT(w, h) (LV_IMG_BUF_SIZE_ALPHA_4BIT((w), (h)) + 4 * 16)
#define LV_IMG_BUF_SIZE_INDEXED_8BIT(w, h) (LV_IMG_BUF_SIZE_ALPHA_8BIT((w), (h)) + 4 * 256)

#define _LV_ZOOM_INV_UPSCALE 5

/**********************
 *      TYPEDEFS
 **********************/
/**
 * The first 8 bit is very important to distinguish the different source types.
 * For more info see `lv_img_get_src_type()` in lv_img.c
 * On big endian systems the order is reversed so cf and always_zero must be at
 * the end of the struct.
 */
#if LV_BIG_ENDIAN_SYSTEM
typedef struct {

    uint32_t h : 11; /*Height of the image map*/
    uint32_t w : 11; /*Width of the image map*/
    uint32_t reserved : 2; /*Reserved to be used later*/
    uint32_t always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/
    uint32_t cf : 5;          /*Color format: See `lv_img_color_format_t`*/

} lv_img_header_t;
#else
typedef struct {

    uint32_t cf : 5;          /*Color format: See `lv_img_color_format_t`*/
    uint32_t always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/

    uint32_t reserved : 1; /*Reserved to be used later*/
    uint32_t chroma_keyed : 2; /*Reserved to be used later*/

    uint32_t w : 11; /*Width of the image map*/
    uint32_t h : 11; /*Height of the image map*/
} lv_img_header_t;
#endif

/** Image header it is compatible with
 * the result from image converter utility*/
typedef struct {
    lv_img_header_t header; /**< A header describing the basics of the image*/
    uint32_t data_size;     /**< Size of the image in bytes*/
    const uint8_t * data;   /**< Pointer to the data of the image*/
} lv_img_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set the palette color of an indexed image. Valid only for `LV_IMG_CF_INDEXED1/2/4/8`
 * @param dsc pointer to an image descriptor
 * @param id the palette color to set:
 *   - for `LV_IMG_CF_INDEXED1`: 0..1
 *   - for `LV_IMG_CF_INDEXED2`: 0..3
 *   - for `LV_IMG_CF_INDEXED4`: 0..15
 *   - for `LV_IMG_CF_INDEXED8`: 0..255
 * @param c the color to set in lv_color32_t format
 */
void lv_img_buf_set_palette(lv_img_dsc_t * dsc, uint8_t id, lv_color32_t c);

/**
 * Free an allocated image buffer
 * @param dsc image buffer to free
 */
void lv_img_buf_free(lv_img_dsc_t * dsc);

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
                                      const lv_point_t * pivot);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_BUF_H*/
