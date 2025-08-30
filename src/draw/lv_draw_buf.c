/**
 * @file lv_draw_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_buf_private.h"
#include "../misc/lv_types.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"
#include "../misc/lv_math.h"
#include "../misc/lv_area_private.h"
#include "convert/lv_draw_buf_convert.h"

/*********************
 *      DEFINES
 *********************/
#define default_handlers LV_GLOBAL_DEFAULT()->draw_buf_handlers
#define font_draw_buf_handlers LV_GLOBAL_DEFAULT()->font_draw_buf_handlers
#define image_cache_draw_buf_handlers LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * buf_malloc(size_t size, lv_color_format_t color_format);
static void buf_free(void * buf);
static void buf_copy(lv_draw_buf_t * dest, const lv_area_t * dest_area,
                     const lv_draw_buf_t * src, const lv_area_t * src_area);
static void * buf_align(void * buf, lv_color_format_t color_format);
static void * draw_buf_malloc(const lv_draw_buf_handlers_t * handler, size_t size_bytes,
                              lv_color_format_t color_format);
static void draw_buf_free(const lv_draw_buf_handlers_t * handler, void * buf);
static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format);
static uint32_t _calculate_draw_buf_size(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride);
static void draw_buf_get_full_area(const lv_draw_buf_t * draw_buf, lv_area_t * full_area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_init_handlers(void)
{
    lv_draw_buf_init_with_default_handlers(&default_handlers);
    lv_draw_buf_init_with_default_handlers(&font_draw_buf_handlers);
    lv_draw_buf_init_with_default_handlers(&image_cache_draw_buf_handlers);
}

void lv_draw_buf_init_with_default_handlers(lv_draw_buf_handlers_t * handlers)
{
    lv_draw_buf_handlers_init(handlers, buf_malloc, buf_free, buf_copy, buf_align, NULL, NULL, width_to_stride);
}

void lv_draw_buf_handlers_init(lv_draw_buf_handlers_t * handlers,
                               lv_draw_buf_malloc_cb_t buf_malloc_cb,
                               lv_draw_buf_free_cb_t buf_free_cb,
                               lv_draw_buf_copy_cb_t buf_copy_cb,
                               lv_draw_buf_align_cb_t align_pointer_cb,
                               lv_draw_buf_cache_operation_cb_t invalidate_cache_cb,
                               lv_draw_buf_cache_operation_cb_t flush_cache_cb,
                               lv_draw_buf_width_to_stride_cb_t width_to_stride_cb)
{
    lv_memzero(handlers, sizeof(lv_draw_buf_handlers_t));
    handlers->buf_malloc_cb = buf_malloc_cb;
    handlers->buf_free_cb = buf_free_cb;
    handlers->buf_copy_cb = buf_copy_cb;
    handlers->align_pointer_cb = align_pointer_cb;
    handlers->invalidate_cache_cb = invalidate_cache_cb;
    handlers->flush_cache_cb = flush_cache_cb;
    handlers->width_to_stride_cb = width_to_stride_cb;
}

lv_draw_buf_handlers_t * lv_draw_buf_get_handlers(void)
{
    return &default_handlers;
}

lv_draw_buf_handlers_t * lv_draw_buf_get_font_handlers(void)
{
    return &font_draw_buf_handlers;
}

lv_draw_buf_handlers_t * lv_draw_buf_get_image_handlers(void)
{
    return &image_cache_draw_buf_handlers;
}

uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    return lv_draw_buf_width_to_stride_ex(&default_handlers, w, color_format);
}

uint32_t lv_draw_buf_width_to_stride_ex(const lv_draw_buf_handlers_t * handlers, uint32_t w,
                                        lv_color_format_t color_format)
{
    if(handlers->width_to_stride_cb) return handlers->width_to_stride_cb(w, color_format);
    else return 0;
}

void * lv_draw_buf_align(void * data, lv_color_format_t color_format)
{
    return lv_draw_buf_align_ex(&default_handlers, data, color_format);
}

void * lv_draw_buf_align_ex(const lv_draw_buf_handlers_t * handlers, void * data, lv_color_format_t color_format)
{
    if(handlers->align_pointer_cb) return handlers->align_pointer_cb(data, color_format);
    else return NULL;
}

void lv_draw_buf_invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    LV_ASSERT_NULL(draw_buf);
    LV_ASSERT_NULL(draw_buf->handlers);

    const lv_draw_buf_handlers_t * handlers = draw_buf->handlers;
    if(!handlers->invalidate_cache_cb) {
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    lv_area_t full;
    if(area == NULL) {
        draw_buf_get_full_area(draw_buf, &full);
        area = &full;
    }

    handlers->invalidate_cache_cb(draw_buf, area);
    LV_PROFILER_DRAW_END;
}

void lv_draw_buf_flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    LV_ASSERT_NULL(draw_buf);
    LV_ASSERT_NULL(draw_buf->handlers);

    const lv_draw_buf_handlers_t * handlers = draw_buf->handlers;
    if(!handlers->flush_cache_cb) {
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    lv_area_t full;
    if(area == NULL) {
        draw_buf_get_full_area(draw_buf, &full);
        area = &full;
    }

    handlers->flush_cache_cb(draw_buf, area);
    LV_PROFILER_DRAW_END;
}

void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a)
{
    LV_ASSERT_NULL(draw_buf);
    LV_PROFILER_DRAW_BEGIN;

    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;

    if(a == NULL) {
        uint8_t * buf = lv_draw_buf_goto_xy(draw_buf, 0, 0);
        lv_memzero(buf, header->h * stride);
        lv_draw_buf_flush_cache(draw_buf, a);
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_area_t a_draw_buf;
    a_draw_buf.x1 = 0;
    a_draw_buf.y1 = 0;
    a_draw_buf.x2 = draw_buf->header.w - 1;
    a_draw_buf.y2 = draw_buf->header.h - 1;

    lv_area_t a_clipped;
    if(!lv_area_intersect(&a_clipped, a, &a_draw_buf)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    if(lv_area_get_width(&a_clipped) <= 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    if(lv_area_get_height(&a_clipped) <= 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    uint8_t * buf = lv_draw_buf_goto_xy(draw_buf, a_clipped.x1, a_clipped.y1);
    uint8_t bpp = lv_color_format_get_bpp(header->cf);
    uint32_t line_length = (lv_area_get_width(&a_clipped) * bpp + 7) >> 3;
    int32_t y;
    for(y = a_clipped.y1; y <= a_clipped.y2; y++) {
        lv_memzero(buf, line_length);
        buf += stride;
    }
    lv_draw_buf_flush_cache(draw_buf, a);
    LV_PROFILER_DRAW_END;
}

lv_result_t lv_draw_buf_init(lv_draw_buf_t * draw_buf, uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride,
                             void * data, uint32_t data_size)
{
    LV_ASSERT_NULL(draw_buf);
    if(draw_buf == NULL) return LV_RESULT_INVALID;

    lv_memzero(draw_buf, sizeof(lv_draw_buf_t));
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);
    if(stride * h > data_size) {
        LV_LOG_WARN("Data size too small, required: %" LV_PRId32 ", provided: %" LV_PRId32, stride * h,
                    data_size);
        return LV_RESULT_INVALID;
    }

    lv_image_header_t * header = &draw_buf->header;
    header->w = w;
    header->h = h;
    header->cf = cf;
    header->stride = stride;
    header->flags = 0;
    header->magic = LV_IMAGE_HEADER_MAGIC;

    draw_buf->data = data;
    draw_buf->unaligned_data = data;
    draw_buf->handlers = &default_handlers;
    draw_buf->data_size = data_size;
    if(lv_draw_buf_align(data, cf) != draw_buf->unaligned_data) {
        LV_LOG_WARN("Data is not aligned, ignored");
    }
    return LV_RESULT_OK;
}

lv_draw_buf_t * lv_draw_buf_create(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride)
{
    return lv_draw_buf_create_ex(&default_handlers, w, h, cf, stride);
}

lv_draw_buf_t * lv_draw_buf_create_ex(const lv_draw_buf_handlers_t * handlers, uint32_t w, uint32_t h,
                                      lv_color_format_t cf, uint32_t stride)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_buf_t * draw_buf = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
    LV_ASSERT_MALLOC(draw_buf);
    if(draw_buf == NULL) {
        LV_PROFILER_DRAW_END;
        return NULL;
    }
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);

    uint32_t size = _calculate_draw_buf_size(w, h, cf, stride);

    void * buf = draw_buf_malloc(handlers, size, cf);
    /*Do not assert here as LVGL or the app might just want to try creating a draw_buf*/
    if(buf == NULL) {
        LV_LOG_WARN("No memory: %"LV_PRIu32"x%"LV_PRIu32", cf: %d, stride: %"LV_PRIu32", %"LV_PRIu32"Byte, ",
                    w, h, cf, stride, size);
        lv_free(draw_buf);
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    draw_buf->header.w = w;
    draw_buf->header.h = h;
    draw_buf->header.cf = cf;
    draw_buf->header.flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;
    draw_buf->header.stride = stride;
    draw_buf->header.magic = LV_IMAGE_HEADER_MAGIC;
    draw_buf->data = lv_draw_buf_align(buf, cf);
    draw_buf->unaligned_data = buf;
    draw_buf->data_size = size;
    draw_buf->handlers = handlers;
    LV_PROFILER_DRAW_END;
    return draw_buf;
}

lv_draw_buf_t * lv_draw_buf_dup(const lv_draw_buf_t * draw_buf)
{
    return lv_draw_buf_dup_ex(&default_handlers, draw_buf);
}

lv_draw_buf_t * lv_draw_buf_dup_ex(const lv_draw_buf_handlers_t * handlers, const lv_draw_buf_t * draw_buf)
{
    LV_PROFILER_DRAW_BEGIN;
    const lv_image_header_t * header = &draw_buf->header;
    lv_draw_buf_t * new_buf = lv_draw_buf_create_ex(handlers, header->w, header->h, header->cf, header->stride);
    if(new_buf == NULL) {
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    new_buf->header.flags = draw_buf->header.flags;
    new_buf->header.flags |= LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;

    /*Choose the smaller size to copy*/
    uint32_t size = LV_MIN(draw_buf->data_size, new_buf->data_size);

    /*Copy image data*/
    lv_memcpy(new_buf->data, draw_buf->data, size);
    LV_PROFILER_DRAW_END;
    return new_buf;
}

lv_draw_buf_t * lv_draw_buf_reshape(lv_draw_buf_t * draw_buf, lv_color_format_t cf, uint32_t w, uint32_t h,
                                    uint32_t stride)
{
    if(draw_buf == NULL) return NULL;
    LV_PROFILER_DRAW_BEGIN;

    /*If color format is unknown, keep using the original color format.*/
    if(cf == LV_COLOR_FORMAT_UNKNOWN) cf = draw_buf->header.cf;
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);

    uint32_t size = _calculate_draw_buf_size(w, h, cf, stride);

    if(size > draw_buf->data_size) {
        LV_LOG_TRACE("Draw buf too small for new shape");
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    draw_buf->header.cf = cf;
    draw_buf->header.w = w;
    draw_buf->header.h = h;
    draw_buf->header.stride = stride;

    LV_PROFILER_DRAW_END;
    return draw_buf;
}

void lv_draw_buf_destroy(lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_NULL(draw_buf);
    if(draw_buf == NULL) return;
    LV_PROFILER_DRAW_BEGIN;

    if(draw_buf->header.flags & LV_IMAGE_FLAGS_ALLOCATED) {
        LV_ASSERT_NULL(draw_buf->handlers);

        const lv_draw_buf_handlers_t * handlers = draw_buf->handlers;
        draw_buf_free(handlers, draw_buf->unaligned_data);
        lv_free(draw_buf);
    }
    else {
        LV_LOG_ERROR("draw buffer is not allocated, ignored");
    }
    LV_PROFILER_DRAW_END;
}

void lv_draw_buf_copy(lv_draw_buf_t * dest, const lv_area_t * dest_area,
                      const lv_draw_buf_t * src, const lv_area_t * src_area)
{
    LV_ASSERT_NULL(dest);
    LV_ASSERT_NULL(dest->handlers);
    LV_ASSERT_NULL(dest->handlers->buf_copy_cb);
    LV_ASSERT_NULL(src);

    dest->handlers->buf_copy_cb(dest, dest_area, src, src_area);
}

void * lv_draw_buf_goto_xy(const lv_draw_buf_t * buf, uint32_t x, uint32_t y)
{
    LV_ASSERT_NULL(buf);
    if(buf == NULL) return NULL;

    uint8_t * data = buf->data;

    /*Skip palette*/
    data += LV_COLOR_INDEXED_PALETTE_SIZE(buf->header.cf) * sizeof(lv_color32_t);
    data += buf->header.stride * y;

    if(x == 0) return data;

    return data + x * lv_color_format_get_bpp(buf->header.cf) / 8;
}

lv_result_t lv_draw_buf_adjust_stride(lv_draw_buf_t * src, uint32_t stride)
{
    LV_ASSERT_NULL(src);
    LV_ASSERT_NULL(src->data);
    if(src == NULL) return LV_RESULT_INVALID;
    if(src->data == NULL) return LV_RESULT_INVALID;
    LV_PROFILER_DRAW_BEGIN;

    const lv_image_header_t * header = &src->header;
    uint32_t w = header->w;
    uint32_t h = header->h;

    if(!lv_draw_buf_has_flag(src, LV_IMAGE_FLAGS_MODIFIABLE)) {
        LV_PROFILER_DRAW_END;
        return LV_RESULT_INVALID;
    }

    /*Use global stride*/
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, header->cf);

    /*Check if stride already match*/
    if(header->stride == stride) {
        LV_PROFILER_DRAW_END;
        return LV_RESULT_OK;
    }

    /*Calculate the minimal stride allowed from bpp*/
    uint32_t bpp = lv_color_format_get_bpp(header->cf);
    uint32_t min_stride = (w * bpp + 7) >> 3;
    if(stride < min_stride) {
        LV_LOG_WARN("New stride is too small. min: %" LV_PRId32, min_stride);
        LV_PROFILER_DRAW_END;
        return LV_RESULT_INVALID;
    }

    /*Check if buffer has enough space. */
    uint32_t new_size = _calculate_draw_buf_size(w, h, header->cf, stride);
    if(new_size > src->data_size) {
        LV_PROFILER_DRAW_END;
        return LV_RESULT_INVALID;
    }

    uint32_t offset = LV_COLOR_INDEXED_PALETTE_SIZE(header->cf) * 4;

    if(stride > header->stride) {
        /*Copy from the last line to the first*/
        uint8_t * src_data = src->data + offset + header->stride * (h - 1);
        uint8_t * dst_data = src->data + offset + stride * (h - 1);
        for(uint32_t y = 0; y < h; y++) {
            lv_memmove(dst_data, src_data, min_stride);
            src_data -= header->stride;
            dst_data -= stride;
        }
    }
    else {
        /*Copy from the first line to the last*/
        uint8_t * src_data = src->data + offset;
        uint8_t * dst_data = src->data + offset;
        for(uint32_t y = 0; y < h; y++) {
            lv_memmove(dst_data, src_data, min_stride);
            src_data += header->stride;
            dst_data += stride;
        }
    }

    src->header.stride = stride;

    LV_PROFILER_DRAW_END;
    return LV_RESULT_OK;
}

lv_result_t lv_draw_buf_premultiply(lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_NULL(draw_buf);
    if(draw_buf == NULL) return LV_RESULT_INVALID;

    if(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED) return LV_RESULT_INVALID;
    if((draw_buf->header.flags & LV_IMAGE_FLAGS_MODIFIABLE) == 0) {
        LV_LOG_WARN("draw buf is not modifiable: 0x%04x", draw_buf->header.flags);
        return LV_RESULT_INVALID;
    }
    LV_PROFILER_DRAW_BEGIN;

    lv_draw_buf_convert_premultiply(draw_buf);

    draw_buf->header.flags |= LV_IMAGE_FLAGS_PREMULTIPLIED;

    LV_PROFILER_DRAW_END;
    return LV_RESULT_OK;
}

void lv_draw_buf_set_palette(lv_draw_buf_t * draw_buf, uint8_t index, lv_color32_t color)
{
    LV_ASSERT_NULL(draw_buf);
    if(draw_buf == NULL) return;

    if(!LV_COLOR_FORMAT_IS_INDEXED(draw_buf->header.cf)) {
        LV_LOG_WARN("Not indexed color format");
        return;
    }

    lv_color32_t * palette = (lv_color32_t *)draw_buf->data;
    palette[index] = color;
}

bool lv_draw_buf_has_flag(const lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    return draw_buf->header.flags & flag;
}

void lv_draw_buf_set_flag(lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    draw_buf->header.flags |= flag;
}

void lv_draw_buf_clear_flag(lv_draw_buf_t * draw_buf, lv_image_flags_t flag)
{
    draw_buf->header.flags &= ~flag;
}

lv_result_t lv_draw_buf_from_image(lv_draw_buf_t * buf, const lv_image_dsc_t * img)
{
    const lv_result_t res = lv_draw_buf_init(buf, img->header.w, img->header.h, img->header.cf, img->header.stride,
                                             (void *)img->data, img->data_size);
    if(res != LV_RESULT_OK) {
        return res;
    }

    buf->header.flags = img->header.flags;
    return res;
}

void lv_draw_buf_to_image(const lv_draw_buf_t * buf, lv_image_dsc_t * img)
{
    lv_memcpy((void *)img, buf, sizeof(lv_image_dsc_t));
}

void lv_image_buf_set_palette(lv_image_dsc_t * dsc, uint8_t id, lv_color32_t c)
{
    LV_LOG_WARN("Deprecated API, use lv_draw_buf_set_palette instead.");
    lv_draw_buf_set_palette((lv_draw_buf_t *)dsc, id, c);
}

void lv_image_buf_free(lv_image_dsc_t * dsc)
{
    LV_LOG_WARN("Deprecated API, use lv_draw_buf_destroy instead.");
    if(dsc != NULL) {
        if(dsc->data != NULL)
            lv_free((void *)dsc->data);

        lv_free((void *)dsc);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    /*Allocate larger memory to be sure it can be aligned as needed*/
    size_bytes += LV_DRAW_BUF_ALIGN - 1;
    return lv_malloc(size_bytes);
}

static void buf_free(void * buf)
{
    lv_free(buf);
}

static void buf_copy(lv_draw_buf_t * dest, const lv_area_t * dest_area,
                     const lv_draw_buf_t * src, const lv_area_t * src_area)
{
    LV_PROFILER_DRAW_BEGIN;
    uint8_t * dest_bufc;
    uint8_t * src_bufc;
    int32_t line_width;

    /*Source and dest color format must be same. Color conversion is not supported yet.*/
    LV_ASSERT_FORMAT_MSG(dest->header.cf == src->header.cf, "Color format mismatch: %d != %d",
                         dest->header.cf, src->header.cf);

    if(dest_area == NULL) line_width = dest->header.w;
    else line_width = lv_area_get_width(dest_area);

    /* For indexed image, copy the palette if we are copying full image area*/
    if(dest_area == NULL || src_area == NULL) {
        if(LV_COLOR_FORMAT_IS_INDEXED(dest->header.cf)) {
            lv_memcpy(dest->data, src->data, LV_COLOR_INDEXED_PALETTE_SIZE(dest->header.cf) * sizeof(lv_color32_t));
        }
    }

    /*Check source and dest area have same width*/
    if((src_area == NULL && line_width != src->header.w) || \
       (src_area != NULL && line_width != lv_area_get_width(src_area))) {
        LV_ASSERT_MSG(0, "Source and destination areas have different width");
        LV_PROFILER_DRAW_END;
        return;
    }

    if(src_area) src_bufc = lv_draw_buf_goto_xy(src, src_area->x1, src_area->y1);
    else src_bufc = lv_draw_buf_goto_xy(src, 0, 0);

    if(dest_area) dest_bufc = lv_draw_buf_goto_xy(dest, dest_area->x1, dest_area->y1);
    else dest_bufc = lv_draw_buf_goto_xy(dest, 0, 0);

    int32_t start_y, end_y;
    if(dest_area) {
        start_y = dest_area->y1;
        end_y = dest_area->y2;
    }
    else {
        start_y = 0;
        end_y = dest->header.h - 1;
    }

    uint32_t dest_stride = dest->header.stride;
    uint32_t src_stride = src->header.stride;
    uint32_t line_bytes = (line_width * lv_color_format_get_bpp(dest->header.cf) + 7) >> 3;

    for(; start_y <= end_y; start_y++) {
        lv_memcpy(dest_bufc, src_bufc, line_bytes);
        dest_bufc += dest_stride;
        src_bufc += src_stride;
    }
    LV_PROFILER_DRAW_END;
}

static void * buf_align(void * buf, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    uint8_t * buf_u8 = buf;
    if(buf_u8) {
        buf_u8 = (uint8_t *)LV_ROUND_UP((lv_uintptr_t)buf_u8, LV_DRAW_BUF_ALIGN);
    }
    return buf_u8;
}

static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    uint32_t width_byte;
    width_byte = w * lv_color_format_get_bpp(color_format);
    width_byte = (width_byte + 7) >> 3; /*Round up*/

    return LV_ROUND_UP(width_byte, LV_DRAW_BUF_STRIDE_ALIGN);
}

static void * draw_buf_malloc(const lv_draw_buf_handlers_t * handlers, size_t size_bytes,
                              lv_color_format_t color_format)
{
    if(handlers->buf_malloc_cb) return handlers->buf_malloc_cb(size_bytes, color_format);
    else return NULL;
}

static void draw_buf_free(const lv_draw_buf_handlers_t * handlers, void * buf)
{
    if(handlers->buf_free_cb)
        handlers->buf_free_cb(buf);
}

/**
 * For given width, height, color format, and stride, calculate the size needed for a new draw buffer.
 */
static uint32_t _calculate_draw_buf_size(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride)
{
    uint32_t size;

    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);

    size = stride * h;
    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        size += (stride / 2) * h; /*A8 mask*/
    }
    else if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        /*@todo we have to include palette right before image data*/
        size += LV_COLOR_INDEXED_PALETTE_SIZE(cf) * 4;
    }

    return size;
}

static void draw_buf_get_full_area(const lv_draw_buf_t * draw_buf, lv_area_t * full_area)
{
    const lv_image_header_t * header = &draw_buf->header;
    lv_area_set(full_area, 0, 0, header->w - 1, header->h - 1);
}
