/**
 * @file lv_draw_buf_vg_lite.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

/*Fix warning for aligned_alloc. See https://stackoverflow.com/questions/29247065/compiler-cant-find-aligned-alloc-function*/
#define _ISOC11_SOURCE

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * buf_malloc(size_t size, lv_color_format_t color_format);
static void buf_free(void * buf);
static void * buf_align(void * buf, lv_color_format_t color_format);
static void invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area);
static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_vg_lite_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();

    handlers->buf_malloc_cb = buf_malloc;
    handlers->buf_free_cb = buf_free;
    handlers->align_pointer_cb = buf_align;
    handlers->invalidate_cache_cb = invalidate_cache;
    handlers->width_to_stride_cb = width_to_stride;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);
    size_bytes = LV_VG_LITE_ALIGN(size_bytes, LV_VG_LITE_BUF_ALIGN);
    return aligned_alloc(LV_VG_LITE_BUF_ALIGN, size_bytes);
}

static void buf_free(void * buf)
{
    free(buf);
}

static void * buf_align(void * buf, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);
    return (void *)LV_VG_LITE_ALIGN((lv_uintptr_t)buf, LV_VG_LITE_BUF_ALIGN);
}

static void invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area)
{
    LV_UNUSED(buf);
    LV_UNUSED(stride);
    LV_UNUSED(color_format);
    LV_UNUSED(area);
}

static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    return lv_vg_lite_width_to_stride(w, lv_vg_lite_vg_fmt(color_format));
}

#endif /*LV_USE_DRAW_VG_LITE*/
