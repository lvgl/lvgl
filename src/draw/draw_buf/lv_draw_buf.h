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
#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    void * buf;
    lv_coord_t width;
    lv_coord_t height;
    lv_color_format_t color_format;
    uint32_t usage_cnt;
    uint32_t flags;
} lv_draw_buf_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a draw buffer object. The buffer won't be allocated
 * @param draw_buf          pointer to a draw buffer
 * @param w                 the width of the buffer in pixel
 * @param h                 the height of the buffer in pixel
 * @param color_format      the color format of the buffer
 */
void lv_draw_buf_init(lv_draw_buf_t * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format);

/**
 * Allocate a buffer in the draw_buf, considering the set width, height and color format
 * @param draw_buf          pointer to a draw buffer
 */
void lv_draw_buf_malloc(lv_draw_buf_t * draw_buf);

/**
 * Realloacte the buffer with a new width, height and color format
 * @param draw_buf          pointer to a draw buffer
 * @param w                 the new width of the buffer in pixel
 * @param h                 the new height of the buffer in pixel
 * @param color_format      the new color format of the buffer
 */
void lv_draw_buf_realloc(lv_draw_buf_t  * draw_buf, lv_coord_t w, lv_coord_t h, lv_color_format_t color_format);

/**
 * Free the allocated buffer
 * @param draw_buf          pointer to draw buffer
 */
void lv_draw_buf_free(lv_draw_buf_t  * draw_buf);

/**
 * Get the buffer of the draw_buf.
 * Never get the buffer as draw_buf->buf adn this function might align the buffer
 * @param draw_buf          pointer to a draw buffer
 * @return                  pointer to the buffer
 */
void * lv_draw_buf_get_buf(lv_draw_buf_t * draw_buf);

/**
 * Invalidate the cache of the buffer
 * @param draw_buf          pointer to a draw buffer
 */
void lv_draw_buf_invalidate_cache(lv_draw_buf_t  * draw_buf);


/**
 * Calculate the stride in bytes based on a width and color format
 * @param w                 the width in pixels
 * @param color_format      the color format
 * @return                  the stride in bytes
 */
uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format);

/**
 * Get the stride of a draw buffer using the set width and color format
 * @param draw_buf          pointer to draw buffer
 * @return                  the stride in bytes
 */
uint32_t lv_draw_buf_get_stride(const lv_draw_buf_t * draw_buf);

/**
 * Go to the X, Y coordinate on the buffer.
 * @param draw_buf          pointer to draw buffer
 * @param x                 the X coordinate to seek
 * @param y                 the Y coordinate to seek
 */
void * lv_draw_buf_go_to_xy(lv_draw_buf_t * draw_buf, lv_coord_t x, lv_coord_t y);

/**
 * Clear an area on the buffer
 * @param draw_buf          pointer to draw buffer
 * @param a                 the area to clear, or NULL to clear the whole buffer
 */
void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a);

/**
 * Copy an area from a buffer to an other
 * @param dest_buf          pointer to the destination buffer (not draw_buf)
 * @param dest_stride       the stride of the destination buffer in bytes
 * @param dest_area         pointer to the destination area
 * @param src_buf           pointer to the source buffer (not draw_buf)
 * @param src_stride        the stride of the source buffer in bytes
 * @param src_area          pointer to the source area
 * @param color_format      the color format (should be the same for the source and destination)
 */
void lv_draw_buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                      void * src_buf, uint32_t src_stride, const lv_area_t * src_area, lv_color_format_t color_format);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_H*/
