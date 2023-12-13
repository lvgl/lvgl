/**
 * @file lv_vg_lite_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_decoder.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#pragma pack(1)

typedef struct {
    lv_color16_t c;
    uint8_t alpha;
} lv_color16_alpha_t;

#pragma pack()

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                const lv_image_decoder_args_t * args);
static void decode_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void image_try_self_pre_mul(lv_image_decoder_dsc_t * dsc);
static void image_color32_pre_mul(lv_color32_t * img_data, uint32_t px_size);
static void image_color16_pre_mul(lv_color16_alpha_t * img_data, uint32_t px_size);
static void image_copy(uint8_t * dest, const uint8_t * src,
                       size_t dest_stride, size_t src_stride,
                       uint32_t height);
static void image_invalidate_cache(void * buf, uint32_t stride,
                                   uint32_t width, uint32_t height,
                                   lv_color_format_t cf);
static void cache_invalidate_cb(lv_cache_entry_t * entry);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_decoder_init(void)
{
    lv_image_decoder_t * decoder = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(decoder, decoder_info);
    lv_image_decoder_set_open_cb(decoder, decoder_open);
    lv_image_decoder_set_close_cb(decoder, decode_close);
    decoder->cache_data_type = lv_cache_register_data_type();
}

void lv_vg_lite_decoder_deinit(void)
{
    lv_image_decoder_t * dec = NULL;
    while((dec = lv_image_decoder_get_next(dec)) != NULL) {
        if(dec->info_cb == decoder_info) {
            lv_image_decoder_delete(dec);
            break;
        }
    }
}

lv_result_t lv_vg_lite_decoder_post_process(lv_image_decoder_dsc_t * dsc)
{
    lv_color_format_t color_format = dsc->header.cf;
    lv_result_t res = LV_RESULT_OK;

    /* Only support this color format */
    switch(color_format) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_RGB565:
            break;

        default:
            return LV_RESULT_OK;
    }

    lv_cache_entry_t * entry = dsc->cache_entry;
    if(!entry) {
        LV_LOG_WARN("No detected cache entry, src_type: %d, src: %p",
                    dsc->src_type, dsc->src);
        return LV_RESULT_INVALID;
    }

    lv_cache_lock();

    /* Check if the image is aligned */
    if(!(entry->process_state & LV_VG_LITE_IMAGE_FLAG_ALIGNED)) {
        int32_t image_w = dsc->header.w;
        int32_t image_h = dsc->header.h;
        uint32_t width_byte = image_w * lv_color_format_get_size(color_format);
        uint32_t stride = lv_draw_buf_width_to_stride(image_w, color_format);

        const uint8_t * ori_image = lv_cache_get_data(entry);

        /* Check stride alignment requirements */
        bool is_aligned = (stride == width_byte)
                          && (ori_image == lv_draw_buf_align((void *)ori_image, color_format));

        if(!is_aligned) {
            /* alloc new image */
            size_t size_bytes = stride * dsc->header.h;
            uint8_t * new_image = lv_draw_buf_malloc(size_bytes, color_format);
            if(!new_image) {
                LV_LOG_ERROR("alloc %zu failed, cf = %d", size_bytes, color_format);
                res = LV_RESULT_INVALID;
                goto alloc_failed;
            }

            /* Replace the image data pointer */
            entry->data = new_image;
            dsc->img_data = new_image;

            /* Copy image data */
            image_copy(new_image, ori_image, stride, width_byte, image_h);

            /* invalidate D-Cache */
            image_invalidate_cache(new_image, stride, image_w, image_h, color_format);

            /* free memory for old image */
            lv_draw_buf_free((void *)ori_image);
        }
        else {
            LV_LOG_USER("no need to realign stride: %" LV_PRIu32, stride);
        }

        entry->process_state |= LV_VG_LITE_IMAGE_FLAG_ALIGNED;
    }

    /* Since image_try_self_pre_mul requires width alignment,
     * premul alpha is placed after the image width alignment process.
     */
    image_try_self_pre_mul(dsc);

alloc_failed:
    lv_cache_unlock();

    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t try_cache(lv_image_decoder_dsc_t * dsc)
{
    lv_cache_lock();
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;

        lv_cache_entry_t * cache = lv_cache_find_by_src(NULL, fn, LV_CACHE_SRC_TYPE_PATH);
        if(cache) {
            dsc->img_data = lv_cache_get_data(cache);
            dsc->cache_entry = cache; /*Save the cache to release it in decoder_close*/
            lv_cache_unlock();
            return LV_RESULT_OK;
        }
    }

    if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        lv_cache_entry_t * cache = lv_cache_find_by_src(NULL, dsc->src, LV_CACHE_SRC_TYPE_POINTER);
        if(cache) {
            dsc->img_data = lv_cache_get_data(cache);
            dsc->cache_entry = cache;
            lv_cache_unlock();
            return LV_RESULT_OK;
        }
    }

    lv_cache_unlock();
    return LV_RESULT_INVALID;
}

static void image_color32_pre_mul(lv_color32_t * img_data, uint32_t px_size)
{
    while(px_size--) {
        img_data->red = LV_UDIV255(img_data->red * img_data->alpha);
        img_data->green = LV_UDIV255(img_data->green * img_data->alpha);
        img_data->blue = LV_UDIV255(img_data->blue * img_data->alpha);
        img_data++;
    }
}

static void image_color16_pre_mul(lv_color16_alpha_t * img_data, uint32_t px_size)
{
    while(px_size--) {
        img_data->c.red = LV_UDIV255(img_data->c.red * img_data->alpha);
        img_data->c.green = LV_UDIV255(img_data->c.green * img_data->alpha);
        img_data->c.blue = LV_UDIV255(img_data->c.blue * img_data->alpha);
        img_data++;
    }
}

static void image_try_self_pre_mul(lv_image_decoder_dsc_t * dsc)
{
    /* !!! WARNING !!!
     * self-premultiplied images
     * should be width-aligned and in modifiable RAM
     */
    if(lv_vg_lite_support_blend_normal()) {
        return;
    }

    if(dsc->cache_entry->process_state & LV_VG_LITE_IMAGE_FLAG_PRE_MUL) {
        return;
    }

    lv_color_format_t cf = dsc->header.cf;
    if(!lv_color_format_has_alpha(cf)) {
        return;
    }

    int32_t image_w = dsc->header.w;
    int32_t image_h = dsc->header.h;
    uint32_t stride = lv_draw_buf_width_to_stride(image_w, cf);
    uint32_t aligned_w = lv_vg_lite_width_align(image_w);
    size_t px_size = aligned_w * image_h;

    if(cf == LV_COLOR_FORMAT_ARGB8888) {
        image_color32_pre_mul((lv_color32_t *)dsc->img_data, px_size);
    }
    else if(cf == LV_COLOR_FORMAT_RGB565A8) {
        image_color16_pre_mul((lv_color16_alpha_t *)dsc->img_data, px_size);
    }
    else if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        lv_color32_t * palette = (lv_color32_t *)dsc->img_data;
        uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
        image_color32_pre_mul(palette, palette_size);
    }
    else if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf)) {
        /* do nothing */
    }
    else {
        LV_LOG_WARN("unsupported cf: %d", cf);
    }

    image_invalidate_cache((void *)dsc->img_data, stride, image_w, image_h, cf);

    dsc->cache_entry->process_state |= LV_VG_LITE_IMAGE_FLAG_PRE_MUL;
}

static void image_copy(uint8_t * dest, const uint8_t * src,
                       size_t dest_stride, size_t src_stride,
                       uint32_t height)
{
    for(uint32_t y = 0; y < height; y++) {
        lv_memcpy(dest, src, src_stride);
        src += src_stride;
        dest += dest_stride;
    }
}

static void image_invalidate_cache(void * buf, uint32_t stride,
                                   uint32_t width, uint32_t height,
                                   lv_color_format_t cf)
{
    width = lv_vg_lite_width_align(width);
    lv_area_t image_area;
    lv_area_set(&image_area, 0, 0, width - 1, height - 1);
    lv_draw_buf_invalidate_cache(buf, stride, cf, &image_area);
}

static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    return lv_bin_decoder_info(decoder, src, header);
}

static lv_result_t decoder_open_variable(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_color_format_t cf = dsc->header.cf;
    int32_t width = dsc->header.w;
    int32_t height = dsc->header.h;

    /* native stride */
    uint32_t width_byte;
    width_byte = width * lv_color_format_get_bpp(cf);
    width_byte = (width_byte + 7) >> 3; /*Round up*/

    bool support_blend_normal = lv_vg_lite_support_blend_normal();

    uint32_t start = lv_tick_get();

    /*In case of uncompressed formats the image stored in the ROM/RAM.
     *So simply give its pointer*/
    const uint8_t * image_data = ((lv_image_dsc_t *)dsc->src)->data;

    bool has_alpha = lv_color_format_has_alpha(cf);
    bool is_indexed = LV_COLOR_FORMAT_IS_INDEXED(cf);
    bool is_addr_aligned = (image_data == lv_draw_buf_align((void *)image_data, cf)) ? true : false;

    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    bool is_stride_aligned = (stride == width_byte) ? true : false;

    /* When the following conditions are met,
     * there is no need to copy image resource preprocessing.
     */
    if(is_addr_aligned
       && is_stride_aligned
       && !is_indexed
       && (!has_alpha || (has_alpha && support_blend_normal))) {

        /*add cache*/
        lv_cache_lock();
        lv_cache_entry_t * cache = lv_cache_add(image_data, 0, decoder->cache_data_type, 1);
        cache->process_state = LV_VG_LITE_IMAGE_FLAG_ALIGNED;
        cache->weight = lv_tick_elaps(start);
        cache->src_type = LV_CACHE_SRC_TYPE_POINTER;
        cache->src = dsc->src;

        dsc->cache_entry = cache;
        dsc->img_data = lv_cache_get_data(cache);
        lv_cache_unlock();
        return LV_RESULT_OK;
    }

    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
    uint32_t palette_size_bytes = palette_size * sizeof(lv_color32_t);

    /* Since the palette and index image are next to each other,
     * the palette size needs to be aligned to ensure that the image is aligned.
     */
    uint32_t palette_size_bytes_aligned = LV_VG_LITE_ALIGN(palette_size_bytes, LV_VG_LITE_BUF_ALIGN);

    size_t image_size = height * stride + palette_size_bytes_aligned;

    void * image_buf = lv_draw_buf_malloc(image_size, cf);
    if(image_buf == NULL) {
        LV_LOG_ERROR("alloc %zu failed, cf = %d", image_size, cf);
        return LV_RESULT_INVALID;
    }

    const uint8_t * src = image_data;
    uint8_t * dest = image_buf;

    /* copy palette */
    if(palette_size_bytes) {
        lv_memcpy(dest, src, palette_size_bytes);
        src += palette_size_bytes;

        /* move to align size */
        dest += palette_size_bytes_aligned;
    }

    image_copy(dest, src, stride, width_byte, height);

    lv_cache_lock();
    lv_cache_entry_t * cache = lv_cache_add(image_buf, 0, decoder->cache_data_type, image_size);

    dsc->img_data = lv_cache_get_data(cache);
    dsc->cache_entry = cache;

    /* premul alpha */
    image_try_self_pre_mul(dsc);

    /* invalidate D-Cache */
    image_invalidate_cache(image_buf, stride, width, height, cf);

    cache->process_state |= LV_VG_LITE_IMAGE_FLAG_ALLOCED | LV_VG_LITE_IMAGE_FLAG_ALIGNED;
    cache->weight = lv_tick_elaps(start);
    cache->src_type = LV_CACHE_SRC_TYPE_POINTER;
    cache->src = dsc->src;
    lv_cache_unlock();

    LV_LOG_USER("image %p (W%" LV_PRId32 " x H%" LV_PRId32 ", buffer: %p, cf: %d) decode finish %" LV_PRIu32 "ms",
                image_data, width, height, image_buf, cf, cache->weight);
    return LV_RESULT_OK;
}

static lv_result_t decoder_open_file(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_color_format_t cf = dsc->header.cf;
    int32_t width = dsc->header.w;
    int32_t height = dsc->header.h;
    const char * path = dsc->src;

    uint32_t start = lv_tick_get();

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("open %s failed", path);
        return LV_RESULT_INVALID;
    }

    /* skip image header bytes */
    res = lv_fs_seek(&file, sizeof(lv_image_header_t), LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("seek %s lv_image_header_t failed", path);
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    /* native stride */
    uint32_t width_byte;
    width_byte = width * lv_color_format_get_bpp(cf);
    width_byte = (width_byte + 7) >> 3; /*Round up*/

    bool support_blend_normal = lv_vg_lite_support_blend_normal();

    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);

    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
    uint32_t palette_size_bytes = palette_size * sizeof(lv_color32_t);

    /* Since the palette and index image are next to each other,
     * the palette size needs to be aligned to ensure that the image is aligned.
     */
    uint32_t palette_size_bytes_aligned = LV_VG_LITE_ALIGN(palette_size_bytes, LV_VG_LITE_BUF_ALIGN);

    size_t image_size = height * stride + palette_size_bytes_aligned;

    void * image_buf = lv_draw_buf_malloc(image_size, cf);
    if(image_buf == NULL) {
        LV_LOG_ERROR("alloc %zu failed, cf = %d", image_size, cf);
        goto failed;
    }

    uint8_t * dest = image_buf;

    /* copy palette */
    if(palette_size_bytes) {
        uint32_t br;
        /* read palette */
        res = lv_fs_read(&file, dest, palette_size_bytes, &br);
        if(res != LV_FS_RES_OK || br != palette_size_bytes) {
            LV_LOG_ERROR("read %s (palette: %" LV_PRIu32 ", br: %" LV_PRIu32 ") failed",
                         path, palette_size_bytes, br);
            goto failed;
        }

        if(!support_blend_normal) {
            image_color32_pre_mul((lv_color32_t *)image_buf, palette_size);
        }

        /* move to index image map */
        dest += palette_size_bytes_aligned;
    }

    for(uint32_t y = 0; y < height; y++) {
        uint32_t br;
        res = lv_fs_read(&file, dest, width_byte, &br);
        if(res != LV_FS_RES_OK || br != width_byte) {
            LV_LOG_ERROR("read %s (y: %" LV_PRIu32 ", width_byte: %" LV_PRIu32 ", br: %" LV_PRIu32 ") failed",
                         path, y, width_byte, br);
            goto failed;
        }

        dest += stride;
    }

    lv_fs_close(&file);

    lv_cache_lock();
    lv_cache_entry_t * cache = lv_cache_add(image_buf, 0, decoder->cache_data_type, image_size);

    dsc->cache_entry = cache;
    dsc->img_data = lv_cache_get_data(cache);

    /* premul alpha */
    image_try_self_pre_mul(dsc);

    /* invalidate D-Cache */
    image_invalidate_cache(image_buf, stride, width, height, cf);

    cache->process_state |= LV_VG_LITE_IMAGE_FLAG_ALLOCED | LV_VG_LITE_IMAGE_FLAG_ALIGNED;
    cache->weight = lv_tick_elaps(start);
    cache->invalidate_cb = cache_invalidate_cb;
    cache->src = lv_strdup(dsc->src);
    cache->src_type = LV_CACHE_SRC_TYPE_PATH;
    lv_cache_unlock();

    LV_LOG_USER("image %s (W%" LV_PRId32 " x H%" LV_PRId32 ", buffer: %p cf: %d) decode finish %" LV_PRIu32 "ms",
                path, width, height, image_buf, cf, cache->weight);
    return LV_RESULT_OK;

failed:
    lv_fs_close(&file);

    if(image_buf) {
        LV_LOG_INFO("free image_buf: %p", image_buf);
        lv_draw_buf_free(image_buf);
    }

    return LV_RESULT_INVALID;
}

static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                const lv_image_decoder_args_t * args)
{
    LV_UNUSED(args); /*Unused*/

    /*Check the cache first*/
    if(try_cache(dsc) == LV_RESULT_OK) {
        return LV_RESULT_OK;
    }

    switch(dsc->src_type) {
        case LV_IMAGE_SRC_VARIABLE:
            return decoder_open_variable(decoder, dsc);
        case LV_IMAGE_SRC_FILE:
            return decoder_open_file(decoder, dsc);
    }

    return LV_RESULT_INVALID;
}

static void decode_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_cache_lock();
    lv_cache_release(dsc->cache_entry);
    lv_cache_unlock();
}

static void cache_invalidate_cb(lv_cache_entry_t * entry)
{
    if(entry->src_type == LV_CACHE_SRC_TYPE_PATH) lv_free((void *)entry->src);
    lv_draw_buf_free((void *)entry->data);
}

#endif /*LV_USE_DRAW_VG_LITE*/
