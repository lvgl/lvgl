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

#include "lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Parameter of the @ref lv_draw_buf_handlers_t::import_cb callback
 */
struct _lv_draw_buf_import_dsc_t {
    uint32_t w;               /**< Should be copied into the `header.w` member of the imported draw buffer. */
    uint32_t h;               /**< Should be copied into the `header.h` member of the imported draw buffer. */
    lv_color_format_t cf;     /**< Should be copied into the `header.cf` member of the imported draw buffer. */
    uint32_t stride;          /**< Should be copied into the `header.stride` member of the imported draw buffer. */
    int fd;         /**< When importing Linux DRM GEM objects, the file descriptor to be passed to the DRM_IOCTL_PRIME_FD_TO_HANDLE ioctl. */
    void * data;    /**< Should be copied into the `unaligned_data` member of the imported draw buffer. */
};

struct _lv_draw_buf_handlers_t {
    lv_draw_buf_malloc_cb_t buf_malloc_cb;
    lv_draw_buf_free_cb_t buf_free_cb;
    lv_draw_buf_copy_cb_t buf_copy_cb;
    lv_draw_buf_align_cb_t align_pointer_cb;
    lv_draw_buf_cache_operation_cb_t invalidate_cache_cb;
    lv_draw_buf_cache_operation_cb_t flush_cache_cb;
    lv_draw_buf_width_to_stride_cb_t width_to_stride_cb;
    lv_draw_buf_create_cb_t create_cb;
    lv_draw_buf_import_cb_t import_cb;
    lv_draw_buf_destroy_cb_t destroy_cb;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Called internally to initialize the draw_buf_handlers in lv_global
 */
void lv_draw_buf_init_handlers(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BUF_PRIVATE_H*/
