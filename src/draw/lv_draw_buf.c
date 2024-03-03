/**
 * @file lv_draw_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "lv_draw_buf.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define handlers LV_GLOBAL_DEFAULT()->draw_buf_handlers

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * buf_malloc(size_t size, lv_color_format_t color_format);
static void buf_free(void * buf);
static void * buf_align(void * buf, lv_color_format_t color_format);
static void * draw_buf_malloc(size_t size_bytes, lv_color_format_t color_format);
static void draw_buf_free(void * buf);
static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format);
static uint32_t _calculate_draw_buf_size(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_draw_buf_init_handlers(void)
{
    lv_memzero(&handlers, sizeof(lv_draw_buf_handlers_t));
    handlers.buf_malloc_cb = buf_malloc;
    handlers.buf_free_cb = buf_free;
    handlers.align_pointer_cb = buf_align;
    handlers.invalidate_cache_cb = NULL;
    handlers.width_to_stride_cb = width_to_stride;
}

lv_draw_buf_handlers_t * lv_draw_buf_get_handlers(void)
{
    return &handlers;
}

uint32_t lv_draw_buf_width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    if(handlers.width_to_stride_cb) return handlers.width_to_stride_cb(w, color_format);
    else return 0;
}

void * lv_draw_buf_align(void * data, lv_color_format_t color_format)
{
    if(handlers.align_pointer_cb) return handlers.align_pointer_cb(data, color_format);
    else return NULL;
}

void lv_draw_buf_invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    if(handlers.invalidate_cache_cb) {
        LV_ASSERT_NULL(draw_buf);
        const lv_image_header_t * header = &draw_buf->header;
        lv_area_t full;
        if(area == NULL) {
            full = (lv_area_t) {
                0, 0, header->w - 1, header->h - 1
            };
            area = &full;
        }
        handlers.invalidate_cache_cb(draw_buf, area);
    }
}

void lv_draw_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * a)
{
    LV_ASSERT_NULL(draw_buf);
    if(a && lv_area_get_width(a) < 0) return;
    if(a && lv_area_get_height(a) < 0) return;

    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;

    if(a == NULL) {
        lv_memzero(draw_buf->data, header->h * stride);
    }
    else {
        uint8_t * bufc;
        uint32_t line_length;
        int32_t start_y, end_y;
        uint8_t px_size = lv_color_format_get_size(header->cf);
        bufc = lv_draw_buf_goto_xy(draw_buf, a->x1, a->y1);
        line_length = lv_area_get_width(a) * px_size;
        start_y = a->y1;
        end_y = a->y2;
        for(; start_y <= end_y; start_y++) {
            lv_memzero(bufc, line_length);
            bufc += stride;
        }
    }
}

void lv_draw_buf_copy(lv_draw_buf_t * dest, const lv_area_t * dest_area,
                      const lv_draw_buf_t * src, const lv_area_t * src_area)
{
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

    draw_buf->data = lv_draw_buf_align(data, cf);
    draw_buf->unaligned_data = data;
    draw_buf->data_size = data_size;
    if(draw_buf->data != draw_buf->unaligned_data) {
        LV_LOG_WARN("Data is not aligned, ignored");
    }
    return LV_RESULT_OK;
}

lv_draw_buf_t * lv_draw_buf_create(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride)
{
    lv_draw_buf_t * draw_buf = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
    LV_ASSERT_MALLOC(draw_buf);
    if(draw_buf == NULL) return NULL;
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);

    uint32_t size = _calculate_draw_buf_size(w, h, cf, stride);

    void * buf = draw_buf_malloc(size, cf);
    /*Do not assert here as LVGL or the app might just want to try creating a draw_buf*/
    if(buf == NULL) {
        LV_LOG_WARN("No memory: %"LV_PRIu32"x%"LV_PRIu32", cf: %d, stride: %"LV_PRIu32", %"LV_PRIu32"Byte, ",
                    w, h, cf, stride, size);
        lv_free(draw_buf);
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
    return draw_buf;
}

lv_draw_buf_t * lv_draw_buf_dup(const lv_draw_buf_t * draw_buf)
{
    const lv_image_header_t * header = &draw_buf->header;
    lv_draw_buf_t * new_buf = lv_draw_buf_create(header->w, header->h, header->cf, header->stride);
    if(new_buf == NULL) return NULL;

    new_buf->header.flags = draw_buf->header.flags;
    new_buf->header.flags |= LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;

    /*Choose the smaller size to copy*/
    uint32_t size = LV_MIN(draw_buf->data_size, new_buf->data_size);

    /*Copy image data*/
    lv_memcpy(new_buf->data, draw_buf->data, size);
    return new_buf;
}

lv_draw_buf_t * lv_draw_buf_reshape(lv_draw_buf_t * draw_buf, lv_color_format_t cf, uint32_t w, uint32_t h,
                                    uint32_t stride)
{
    if(draw_buf == NULL) return NULL;

    /*If color format is unknown, keep using the original color format.*/
    if(cf == LV_COLOR_FORMAT_UNKNOWN) cf = draw_buf->header.cf;
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, cf);

    uint32_t size = _calculate_draw_buf_size(w, h, cf, stride);

    if(size > draw_buf->data_size) {
        LV_LOG_TRACE("Draw buf too small for new shape");
        return NULL;
    }

    draw_buf->header.cf = cf;
    draw_buf->header.w = w;
    draw_buf->header.h = h;
    draw_buf->header.stride = stride;

    return draw_buf;
}

void lv_draw_buf_destroy(lv_draw_buf_t * buf)
{
    LV_ASSERT_NULL(buf);
    if(buf == NULL) return;

    if(buf->header.flags & LV_IMAGE_FLAGS_ALLOCATED) {
        draw_buf_free(buf->unaligned_data);
        lv_free(buf);
    }
    else {
        LV_LOG_ERROR("draw buffer is not allocated, ignored");
    }
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

    return data + x * lv_color_format_get_size(buf->header.cf);
}

lv_result_t lv_draw_buf_adjust_stride(lv_draw_buf_t * src, uint32_t stride)
{
    LV_ASSERT_NULL(src);
    LV_ASSERT_NULL(src->data);
    if(src == NULL) return LV_RESULT_INVALID;
    if(src->data == NULL) return LV_RESULT_INVALID;

    const lv_image_header_t * header = &src->header;
    uint32_t w = header->w;
    uint32_t h = header->h;

    /*Use global stride*/
    if(stride == 0) stride = lv_draw_buf_width_to_stride(w, header->cf);

    /*Check if stride already match*/
    if(header->stride == stride) return LV_RESULT_OK;

    /*Calculate the minimal stride allowed from bpp*/
    uint32_t bpp = lv_color_format_get_bpp(header->cf);
    uint32_t min_stride = (w * bpp + 7) >> 3;
    if(stride < min_stride) {
        LV_LOG_WARN("New stride is too small. min: %" LV_PRId32, min_stride);
        return LV_RESULT_INVALID;
    }

    /*Check if buffer has enough space. */
    uint32_t new_size = _calculate_draw_buf_size(w, h, header->cf, stride);
    if(new_size > src->data_size) {
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

    /*Premultiply color with alpha, do case by case by judging color format*/
    lv_color_format_t cf = draw_buf->header.cf;
    if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        int size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
        lv_color32_t * palette = (lv_color32_t *)draw_buf->data;
        for(int i = 0; i < size; i++) {
            lv_color_premultiply(&palette[i]);
        }
    }
    else if(cf == LV_COLOR_FORMAT_ARGB8888) {
        uint32_t h = draw_buf->header.h;
        uint32_t w = draw_buf->header.w;
        uint32_t stride = draw_buf->header.stride;
        uint8_t * line = (uint8_t *)draw_buf->data;
        for(uint32_t y = 0; y < h; y++) {
            lv_color32_t * pixel = (lv_color32_t *)line;
            for(uint32_t x = 0; x < w; x++) {
                lv_color_premultiply(pixel);
                pixel++;
            }
            line += stride;
        }
    }
    else if(cf == LV_COLOR_FORMAT_RGB565A8) {
        uint32_t h = draw_buf->header.h;
        uint32_t w = draw_buf->header.w;
        uint32_t stride = draw_buf->header.stride;
        uint32_t alpha_stride = stride / 2;
        uint8_t * line = (uint8_t *)draw_buf->data;
        lv_opa_t * alpha = (lv_opa_t *)(line + stride * h);
        for(uint32_t y = 0; y < h; y++) {
            lv_color16_t * pixel = (lv_color16_t *)line;
            for(uint32_t x = 0; x < w; x++) {
                lv_color16_premultiply(pixel, alpha[x]);
                pixel++;
            }
            line += stride;
            alpha += alpha_stride;
        }
    }
    else if(cf == LV_COLOR_FORMAT_ARGB8565) {
        uint32_t h = draw_buf->header.h;
        uint32_t w = draw_buf->header.w;
        uint32_t stride = draw_buf->header.stride;
        uint8_t * line = (uint8_t *)draw_buf->data;
        for(uint32_t y = 0; y < h; y++) {
            uint8_t * pixel = line;
            for(uint32_t x = 0; x < w; x++) {
                uint8_t alpha = pixel[2];
                lv_color16_premultiply((lv_color16_t *)pixel, alpha);
                pixel += 3;
            }
            line += stride;
        }
    }
    else if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf)) {
        /*Pass*/
    }
    else {
        LV_LOG_WARN("draw buf has no alpha, cf: %d", cf);
    }

    draw_buf->header.flags |= LV_IMAGE_FLAGS_PREMULTIPLIED;

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

    uint8_t * buf = (uint8_t *)draw_buf->data;
    lv_memcpy(&buf[index * sizeof(color)], &color, sizeof(color));
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

static void * buf_align(void * buf, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    uint8_t * buf_u8 = buf;
    if(buf_u8) {
        buf_u8 += LV_DRAW_BUF_ALIGN - 1;
        buf_u8 = (uint8_t *)((lv_uintptr_t) buf_u8 & ~(LV_DRAW_BUF_ALIGN - 1));
    }
    return buf_u8;
}

static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    uint32_t width_byte;
    width_byte = w * lv_color_format_get_bpp(color_format);
    width_byte = (width_byte + 7) >> 3; /*Round up*/
    return (width_byte + LV_DRAW_BUF_STRIDE_ALIGN - 1) & ~(LV_DRAW_BUF_STRIDE_ALIGN - 1);
}

static void * draw_buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    if(handlers.buf_malloc_cb) return handlers.buf_malloc_cb(size_bytes, color_format);
    else return NULL;
}

static void draw_buf_free(void * buf)
{
    if(handlers.buf_free_cb) handlers.buf_free_cb(buf);
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
