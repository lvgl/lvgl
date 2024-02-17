/**
 * @file lv_image_buf.h
 *
 */

#ifndef LV_IMAGE_BUF_H
#define LV_IMAGE_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

#define _LV_ZOOM_INV_UPSCALE 5

/** Magic number for lvgl image, 9 means lvgl version 9
 *  It must not be a valid ASCII character nor larger than 0x80. See `lv_image_src_get_type`.
 */
#define LV_IMAGE_HEADER_MAGIC (0x19)

/**********************
 *      TYPEDEFS
 **********************/

typedef enum _lv_image_flags_t {
    /**
     * For RGB map of the image data, mark if it's pre-multiplied with alpha.
     * For indexed image, this bit indicated palette data is pre-multiplied with alpha.
     */
    LV_IMAGE_FLAGS_PREMULTIPLIED    = (1 << 0),

    /**
     * If the image data is malloced and can be processed in place.
     * In image decoder post processing, this flag means we modify it in-place.
     */
    LV_IMAGE_FLAGS_MODIFIABLE       = (1 << 1),

    /**
     * Indicating it's a vector image instead of default raster image.
     * Some of the flags are not usable for vector image, like PREMULTIPLIED.
     */
    LV_IMAGE_FLAGS_VECTORS          = (1 << 2),

    /**
     * The image data is compressed, so decoder needs to decode image firstly.
     * If this flag is set, the whole image will be decompressed upon decode, and
     * `get_area_cb` won't be necessary.
     */
    LV_IMAGE_FLAGS_COMPRESSED       = (1 << 3),

    /**
     * The image is allocated from heap, thus should be freed after use.
     */
    LV_IMAGE_FLAGS_ALLOCATED        = (1 << 4),

    /**
     * Flags reserved for user, lvgl won't use these bits.
     */
    LV_IMAGE_FLAGS_USER1            = 0x1000,
    LV_IMAGE_FLAGS_USER2            = 0x2000,
    LV_IMAGE_FLAGS_USER3            = 0x4000,
    LV_IMAGE_FLAGS_USER4            = 0x8000,
    LV_IMAGE_FLAGS_USER5            = 0x0100,
    LV_IMAGE_FLAGS_USER6            = 0x0200,
    LV_IMAGE_FLAGS_USER7            = 0x0400,
    LV_IMAGE_FLAGS_USER8            = 0x0800,
} lv_image_flags_t;

typedef enum {
    LV_IMAGE_COMPRESS_NONE = 0,
    LV_IMAGE_COMPRESS_RLE,  /*LVGL custom RLE compression*/
    LV_IMAGE_COMPRESS_LZ4,
} lv_image_compress_t;

#if LV_BIG_ENDIAN_SYSTEM
typedef struct {
    uint32_t reserved_2: 16;    /*Reserved to be used later*/
    uint32_t stride: 16;        /*Number of bytes in a row*/
    uint32_t h: 16;
    uint32_t w: 16;
    uint32_t flags: 16;         /*Image flags, see `lv_image_flags_t`*/
    uint32_t cf : 8;            /*Color format: See `lv_color_format_t`*/
    uint32_t magic: 8;          /*Magic number. Must be LV_IMAGE_HEADER_MAGIC*/
} lv_image_header_t;
#else
typedef struct {
    uint32_t magic: 8;          /*Magic number. Must be LV_IMAGE_HEADER_MAGIC*/
    uint32_t cf : 8;            /*Color format: See `lv_color_format_t`*/
    uint32_t flags: 16;         /*Image flags, see `lv_image_flags_t`*/

    uint32_t w: 16;
    uint32_t h: 16;
    uint32_t stride: 16;        /*Number of bytes in a row*/
    uint32_t reserved_2: 16;    /*Reserved to be used later*/
} lv_image_header_t;
#endif

typedef struct {
    void * buf;
    uint32_t stride;            /*Number of bytes in a row*/
} lv_yuv_plane_t;

typedef union {
    lv_yuv_plane_t yuv;         /*packed format*/
    struct {
        lv_yuv_plane_t y;
        lv_yuv_plane_t u;
        lv_yuv_plane_t v;
    } planar;                   /*planar format with 3 plane*/
    struct {
        lv_yuv_plane_t y;
        lv_yuv_plane_t uv;
    } semi_planar;              /*planar format with 2 plane*/
} lv_yuv_buf_t;

/**
 * Struct to describe an image. Both decoded and raw image can share
 * the same struct.
 *
 * Image is also identical to lv_draw_buf_t for now.
 * Ideally, decoded image should be lv_draw_buf_t.
 */
typedef struct {
    lv_image_header_t header; /**< A header describing the basics of the image*/
    uint32_t data_size;     /**< Size of the image in bytes*/
    const uint8_t * data;   /**< Pointer to the data of the image*/
} lv_image_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set the palette color of an indexed image. Valid only for `LV_COLOR_FORMAT_I1/2/4/8`
 * @param dsc pointer to an image descriptor
 * @param id the palette color to set:
 *   - for `LV_COLOR_FORMAT_I1`: 0..1
 *   - for `LV_COLOR_FORMAT_I2`: 0..3
 *   - for `LV_COLOR_FORMAT_I4`: 0..15
 *   - for `LV_COLOR_FORMAT_I8`: 0..255
 * @param c the color to set in lv_color32_t format
 */
void lv_image_buf_set_palette(lv_image_dsc_t * dsc, uint8_t id, lv_color32_t c);

/**
 * Free an allocated image buffer
 * @param dsc image buffer to free
 */
void lv_image_buf_free(lv_image_dsc_t * dsc);

/**
 * Get the area of a rectangle if its rotated and scaled
 * @param res store the coordinates here
 * @param w width of the rectangle to transform
 * @param h height of the rectangle to transform
 * @param angle angle of rotation
 * @param scale_x zoom in x direction, (256 no zoom)
 * @param scale_y zoom in y direction, (256 no zoom)
 * @param pivot x,y pivot coordinates of rotation
 */
void _lv_image_buf_get_transformed_area(lv_area_t * res, int32_t w, int32_t h, int32_t angle, uint16_t scale_x,
                                        uint16_t scale_y,
                                        const lv_point_t * pivot);

static inline void lv_image_header_init(lv_image_header_t * header, uint32_t w, uint32_t h, lv_color_format_t cf,
                                        uint32_t stride, lv_image_flags_t flags)
{
    LV_ASSERT(header);
    lv_memzero(header, sizeof(*header));
    header->w = w;
    header->h = h;
    header->cf = cf;
    header->stride = stride;
    header->flags = flags;
    header->magic = LV_IMAGE_HEADER_MAGIC;
}
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_BUF_H*/
