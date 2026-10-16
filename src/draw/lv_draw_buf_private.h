/**
 * @file lv_draw_buf_private.h
 *
 */

#ifndef LV_DRAW_BUF_PRIVATE_H
#define LV_DRAW_BUF_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lvgl_public.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_buf_handlers_t {
    lv_draw_buf_malloc_cb_t buf_malloc_cb;
    lv_draw_buf_free_cb_t buf_free_cb;
    lv_draw_buf_copy_cb_t buf_copy_cb;
    lv_draw_buf_align_cb_t align_pointer_cb;
    lv_draw_buf_cache_operation_cb_t invalidate_cache_cb;
    lv_draw_buf_cache_operation_cb_t flush_cache_cb;
    lv_draw_buf_width_to_stride_cb_t width_to_stride_cb;
    lv_draw_buf_clear_cb_t buf_clear_cb;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Called internally to initialize the draw_buf_handlers in lv_global
 */
void lv_draw_buf_init_handlers(void);

/**
 * TODO(v10): the draw buffer should describe its own storage well enough that the
 *            layer isn't needed. Merge this back into `lv_draw_buf_clear()` then.
 *
 * @param draw_buf          pointer to draw buffer
 * @param a                 the area to clear @nullable. When NULL the whole buffer is cleared
 * @param layer             the layer `draw_buf` belongs to @nullable
 */
void lv_draw_buf_clear_ex(lv_draw_buf_t * draw_buf, const lv_area_t * a, lv_layer_t * layer);

lv_result_t lv_draw_buf_init_with_mono_flags(lv_draw_buf_t * draw_buf, uint32_t w, uint32_t h,
                                             lv_color_format_t cf, uint32_t stride, void * data,
                                             uint32_t data_size, bool vtiled,
                                             bool lsb_first);

lv_draw_buf_t * lv_draw_buf_create_ex_with_mono_flags(const lv_draw_buf_handlers_t * handlers,
                                                      uint32_t w, uint32_t h, lv_color_format_t cf,
                                                      uint32_t stride, bool vtiled,
                                                      bool lsb_first);

/**
 * For packed color formats, get the number of `stride` sized chunks
 * needed to store `h` pixel rows, taking vertical tiling into account.
 * For horizontally tiled buffers (or any other color format) this is simply `h`: each
 * pixel row needs its own `stride` sized chunk.
 * For vertically tiled 1 bit per pixel buffers each byte packs 8 vertically stacked
 * pixels of a column, so every 8 pixel rows only need one additional chunk.
 * @param h         the number of pixel rows
 * @param cf        the color format of the buffer
 * @param vtiled    the vertical tiling flag of the buffer (`lv_image_header_t.vtiled`)
 * @return          the number of `stride` sized chunks needed to store `h` pixel rows
 */
uint32_t lv_draw_buf_stride_rows(uint32_t h, lv_color_format_t cf, bool vtiled);

/**
 * Get the stride (the size in bytes of one packed-layout chunk) of a
 * buffer, taking the vertical tiling of packed color formats into
 * account. For horizontally tiled buffers (or any other color format) this returns the
 * same value as `lv_draw_buf_width_to_stride`.
 * @param w         the width in pixels
 * @param cf        the color format
 * @param vtiled    the vertical tiling flag (`lv_image_header_t.vtiled`)
 * @return          the stride in bytes
 */
uint32_t lv_draw_buf_width_to_stride_packed(uint32_t w, lv_color_format_t cf, bool vtiled);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_PRIVATE_H*/
