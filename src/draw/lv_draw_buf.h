/**
 * @file lv_draw_buf.h
 *
 */

#ifndef LV_DRAW_BUF_H
#define LV_DRAW_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_area.h"
#include "../misc/lv_color.h"
#include "../stdlib/lv_string.h"
#include "lv_image_dsc.h"

/*********************
 *      DEFINES
 *********************/

/*Use this value to let LVGL calculate stride automatically*/
#define LV_STRIDE_AUTO 0
LV_EXPORT_CONST_INT(LV_STRIDE_AUTO);

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_image_header_t header;
    uint32_t data_size;     /*Total buf size in bytes*/
    uint8_t * data;
    void * unaligned_data;  /*Unaligned address of `data`, used internally by lvgl*/
} lv_draw_buf_t;

/**
 * Stride alignment for draw buffers.
 * It may vary between different color formats and hardware.
 * Refine it to suit your needs.
 */

#define _LV_DRAW_BUF_STRIDE(w, cf) \
    ((((w) * LV_COLOR_FORMAT_GET_BPP(cf) + 7) / 8 + (LV_DRAW_BUF_STRIDE_ALIGN) - 1) & ~((LV_DRAW_BUF_STRIDE_ALIGN) - 1))

#define _LV_DRAW_BUF_SIZE(w, h, cf) \
    (_LV_DRAW_BUF_STRIDE(w, cf) * (h))

/**
 * Define a static draw buffer with the given width, height, and color format.
 * Stride alignment is set to LV_DRAW_BUF_STRIDE_ALIGN.
 */
#define LV_DRAW_BUF_DEFINE(name, _w, _h, _cf) \
    static uint8_t buf_##name[_LV_DRAW_BUF_SIZE(_w, _h, _cf)]; \
    static lv_draw_buf_t name = { \
                                  .header = { \
                                              .magic = LV_IMAGE_HEADER_MAGIC, \
                                              .cf = (_cf), \
                                              .flags = LV_IMAGE_FLAGS_MODIFIABLE, \
                                              .w = (_w), \
                                              .h = (_h), \
                                              .stride = _LV_DRAW_BUF_STRIDE(_w, _cf), \
                                              .reserved_2 = 0, \
                                            }, \
                                  .data_size = sizeof(buf_##name), \
                                  .data = buf_##name, \
                                  .unaligned_data = buf_##name, \
                                }

typedef void * (*lv_draw_buf_malloc_cb)(size_t size, lv_color_format_t color_format);

typedef void (*lv_draw_buf_free_cb)(void * draw_buf);

typedef void * (*lv_draw_buf_align_cb)(void * buf, lv_color_format_t color_format);

typedef void (*lv_draw_buf_invalidate_cache_cb)(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

typedef uint32_t (*lv_draw_buf_width_to_stride_cb)(uint32_t w, lv_color_format_t color_format);

typedef struct {
    lv_draw_buf_malloc_cb buf_malloc_cb;
    lv_draw_buf_free_cb buf_free_cb;
    lv_draw_buf_align_cb align_pointer_cb;
    lv_draw_buf_invalidate_cache_cb invalidate_cache_cb;
    lv_draw_buf_width_to_stride_cb width_to_stride_cb;
} lv_draw_buf_handlers_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Called internally to initialize the draw_buf_handlers in lv_global
 */
void _lv_draw_buf_init_handlers(void);

/**
 * Get the struct which holds the callbacks for draw buf management.
 * Custom callback can be set on the returned value
 * @return                  pointer to the struct of handlers
 */
lv_draw_buf_handlers_t * lv_draw_buf_get_handlers(void);

/**
 * Align the address of a buffer. The buffer needs to be large enough for the real data after alignment
 * @param buf           the data to align
 * @param color_format  the color format of the buffer
 * @return              the aligned buffer
 */
void * lv_draw_buf_align(void * buf, lv_color_format_t color_format);

/**
 * Invalidate the cache of the buffer
 * @param draw_buf     the draw buffer needs to be invalidated
 * @param area         the area to invalidate in the buffer,
 *                     use NULL to invalidate the whole draw buffer address range
 */
void lv_draw_buf_invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**
 * Calculate the stride in bytes based on a width and color format
 * @param w                 the width in pixels
 * @param color_format      the color format
 * @return                  the stride in bytes
 */
uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format);

/**
 * Clear an area on the buffer
 * @param draw_buf          pointer to draw buffer
 * @param a                 the area to clear, or NULL to clear the whole buffer
 */
void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a);

/**
 * Copy an area from a buffer to an other
 * @param dest      pointer to the destination draw buffer
 * @param dest_area the area to copy from the destination buffer, if NULL, use the whole buffer
 * @param src       pointer to the source draw buffer
 * @param src_area  the area to copy from the destination buffer, if NULL, use the whole buffer
 * @note `dest_area` and `src_area` should have the same width and height
 * @note  `dest` and `src` should have same color format. Color converting is not supported fow now.
 */
void lv_draw_buf_copy(lv_draw_buf_t * dest, const lv_area_t * dest_area,
                      const lv_draw_buf_t * src, const lv_area_t * src_area);

/**
 * Note: Eventually, lv_draw_buf_malloc/free will be kept as private.
 *       For now, we use `create` to distinguish with malloc.
 *
 * Create an draw buf by allocating struct for `lv_draw_buf_t` and allocating a buffer for it
 * that meets specified requirements.
 *
 * @param w         the buffer width in pixels
 * @param h         the buffer height in pixels
 * @param cf        the color format for image
 * @param stride    the stride in bytes for image. Use 0 for automatic calculation based on
 *                  w, cf, and global stride alignment configuration.
 */
lv_draw_buf_t * lv_draw_buf_create(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride);

/**
 * Initialize a draw buf with the given buffer and parameters.
 * @param draw_buf  the draw buf to initialize
 * @param w         the buffer width in pixels
 * @param h         the buffer height in pixels
 * @param cf        the color format
 * @param stride    the stride in bytes. Use 0 for automatic calculation
 * @param data      the buffer used for drawing. Unaligned `data` will be aligned internally
 * @param data_size the size of the buffer in bytes
 * @return          return LV_RESULT_OK on success, LV_RESULT_INVALID otherwise
 */
lv_result_t lv_draw_buf_init(lv_draw_buf_t * draw_buf, uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride,
                             void * data, uint32_t data_size);

/**
 * Duplicate a draw buf with same image size, stride and color format. Copy the image data too.
 * @param draw_buf  the draw buf to duplicate
 * @return          the duplicated draw buf on success, NULL if failed
 */
lv_draw_buf_t * lv_draw_buf_dup(const lv_draw_buf_t * draw_buf);

/**
 * Keep using the existing memory, reshape the draw buffer to the given width and height.
 * Return NULL if data_size is smaller than the required size.
 * @param draw_buf  pointer to a draw buffer
 * @param cf        the new color format, use 0 or LV_COLOR_FORMAT_UNKNOWN to keep using the original color format.
 * @param w         the new width in pixels
 * @param h         the new height in pixels
 * @param stride    the stride in bytes for image. Use 0 for automatic calculation.
 */
lv_draw_buf_t * lv_draw_buf_reshape(lv_draw_buf_t * draw_buf, lv_color_format_t cf, uint32_t w, uint32_t h,
                                    uint32_t stride);

/**
 * Destroy a draw buf by free the actual buffer if it's marked as LV_IMAGE_FLAGS_ALLOCATED in header.
 * Then free the lv_draw_buf_t struct.
 */
void lv_draw_buf_destroy(lv_draw_buf_t * buf);

/**
 * Return pointer to the buffer at the given coordinates
 */
void * lv_draw_buf_goto_xy(const lv_draw_buf_t * buf, uint32_t x, uint32_t y);

/**
 * Adjust the stride of a draw buf in place.
 * @param src       pointer to a draw buffer
 * @param stride    the new stride in bytes for image. Use LV_STRIDE_AUTO for automatic calculation.
 * @return          LV_RESULT_OK: success or LV_RESULT_INVALID: failed
 */
lv_result_t lv_draw_buf_adjust_stride(lv_draw_buf_t * src, uint32_t stride);

/**
 * Premultiply draw buffer color with alpha channel.
 * If it's already premultiplied, return directly.
 * Only color formats with alpha channel will be processed.
 *
 * @return LV_RESULT_OK: premultiply success
 */
lv_result_t lv_draw_buf_premultiply(lv_draw_buf_t * draw_buf);

static inline bool lv_draw_buf_has_flag(lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    return draw_buf->header.flags & flag;
}

static inline void lv_draw_buf_set_flag(lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    draw_buf->header.flags |= flag;
}

static inline void lv_draw_buf_clear_flag(lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    draw_buf->header.flags &= ~flag;
}

/**
 * As of now, draw buf share same definition as `lv_image_dsc_t`.
 * And is interchangeable with `lv_image_dsc_t`.
 */

static inline void lv_draw_buf_from_image(lv_draw_buf_t * buf, const lv_image_dsc_t * img)
{
    lv_memcpy(buf, img, sizeof(lv_image_dsc_t));
    buf->unaligned_data = buf->data;
}

static inline void lv_draw_buf_to_image(const lv_draw_buf_t * buf, lv_image_dsc_t * img)
{
    lv_memcpy((void *)img, buf, sizeof(lv_image_dsc_t));
}

/**
 * Set the palette color of an indexed image. Valid only for `LV_COLOR_FORMAT_I1/2/4/8`
 * @param draw_buf pointer to an image descriptor
 * @param index the palette color to set:
 *   - for `LV_COLOR_FORMAT_I1`: 0..1
 *   - for `LV_COLOR_FORMAT_I2`: 0..3
 *   - for `LV_COLOR_FORMAT_I4`: 0..15
 *   - for `LV_COLOR_FORMAT_I8`: 0..255
 * @param color the color to set in lv_color32_t format
 */
void lv_draw_buf_set_palette(lv_draw_buf_t * draw_buf, uint8_t index, lv_color32_t color);

/**
 * @deprecated Use lv_draw_buf_set_palette instead.
 */
static inline void lv_image_buf_set_palette(lv_image_dsc_t * dsc, uint8_t id, lv_color32_t c)
{
    LV_LOG_WARN("Deprecated API, use lv_draw_buf_set_palette instead.");
    lv_draw_buf_set_palette((lv_draw_buf_t *)dsc, id, c);
}

/**
 * @deprecated Use lv_draw_buffer_create/destroy instead.
 * Free the data pointer and dsc struct of an image.
 */
static inline void lv_image_buf_free(lv_image_dsc_t * dsc)
{
    LV_LOG_WARN("Deprecated API, use lv_draw_buf_destroy instead.");
    if(dsc != NULL) {
        if(dsc->data != NULL)
            lv_free((void *)dsc->data);

        lv_free((void *)dsc);
    }
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_H*/
