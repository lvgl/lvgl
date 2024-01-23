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

#define LV_VG_LITE_IMAGE_NO_CACHE LV_IMAGE_FLAGS_USER1

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
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_cache_free(lv_image_cache_data_t * cached_data, void * user_data);
static void image_try_self_pre_mul(lv_image_decoder_dsc_t * dsc);
static void image_color32_pre_mul(lv_color32_t * img_data, uint32_t px_size);
static void image_color16_pre_mul(lv_color16_alpha_t * img_data, uint32_t px_size);
static void image_copy(uint8_t * dest, const uint8_t * src,
                       size_t dest_stride, size_t src_stride,
                       uint32_t height);
static void image_invalidate_cache(void * buf, uint32_t stride,
                                   uint32_t width, uint32_t height,
                                   lv_color_format_t cf);

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
    lv_image_decoder_set_close_cb(decoder, decoder_close);
    lv_image_decoder_set_cache_free_cb(decoder, (lv_cache_free_cb_t)decoder_cache_free);
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

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

    if(dsc->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED) {
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
    void * image_data = dsc->decoded->data;

    if(cf == LV_COLOR_FORMAT_ARGB8888) {
        image_color32_pre_mul(image_data, px_size);
    }
    else if(cf == LV_COLOR_FORMAT_RGB565A8) {
        image_color16_pre_mul(image_data, px_size);
    }
    else if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        lv_color32_t * palette = (lv_color32_t *)image_data;
        uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
        image_color32_pre_mul(palette, palette_size);
    }
    else if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf)) {
        /* do nothing */
    }
    else {
        LV_LOG_WARN("unsupported cf: %d", cf);
    }

    image_invalidate_cache(image_data, stride, image_w, image_h, cf);

    dsc->header.flags |= LV_IMAGE_FLAGS_PREMULTIPLIED;
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

    /*In case of uncompressed formats the image stored in the ROM/RAM.
     *So simply give its pointer*/
    const uint8_t * image_data = ((lv_image_dsc_t *)dsc->src)->data;
    uint32_t image_data_size = ((lv_image_dsc_t *)dsc->src)->data_size;

    bool has_alpha = lv_color_format_has_alpha(cf);
    bool is_indexed = LV_COLOR_FORMAT_IS_INDEXED(cf);
    bool is_yuv = LV_COLOR_FORMAT_IS_YUV(cf);
    bool is_addr_aligned = (image_data == lv_draw_buf_align((void *)image_data, cf)) ? true : false;

    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    bool is_stride_aligned = (stride == width_byte) ? true : false;

    /* When the following conditions are met,
     * there is no need to copy image resource preprocessing.
     */
    if((is_addr_aligned
        && is_stride_aligned
        && !is_indexed
        && (!has_alpha || (has_alpha && support_blend_normal))) || is_yuv) {

        lv_draw_buf_t * draw_buf = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
        LV_ASSERT_MALLOC(draw_buf);
        lv_draw_buf_init(draw_buf, width, height, cf, stride, (void *)image_data, image_data_size);
        dsc->decoded = draw_buf;
        return LV_RESULT_OK;
    }

    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
    uint32_t palette_size_bytes = palette_size * sizeof(lv_color32_t);

    /* Since the palette and index image are next to each other,
     * the palette size needs to be aligned to ensure that the image is aligned.
     */
    uint32_t palette_size_bytes_aligned = LV_VG_LITE_ALIGN(palette_size_bytes, LV_DRAW_BUF_ALIGN);

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(width, height, cf, stride);
    if(draw_buf == NULL) {
        LV_LOG_ERROR("create draw buf failed, cf = %d", cf);
        return LV_RESULT_INVALID;
    }

    dsc->decoded = draw_buf;

    const uint8_t * src = image_data;
    uint8_t * dest = draw_buf->data;

    /* copy palette */
    if(palette_size_bytes) {
        lv_memcpy(dest, src, palette_size_bytes);
        src += palette_size_bytes;

        /* move to align size */
        dest += palette_size_bytes_aligned;
    }

    image_copy(dest, src, stride, width_byte, height);

    /* premul alpha */
    image_try_self_pre_mul(dsc);

    /* invalidate D-Cache */
    image_invalidate_cache(draw_buf->data, stride, width, height, cf);

    LV_LOG_INFO("image %p (W%" LV_PRId32 " x H%" LV_PRId32 ", buffer: %p, cf: %d) decode finish",
                image_data, width, height, draw_buf->data, cf);
    return LV_RESULT_OK;
}

static lv_result_t decoder_open_file(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_color_format_t cf = dsc->header.cf;
    uint32_t width = dsc->header.w;
    uint32_t height = dsc->header.h;
    const char * path = dsc->src;

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
    uint32_t palette_size_bytes_aligned = LV_VG_LITE_ALIGN(palette_size_bytes, LV_DRAW_BUF_ALIGN);

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(width, height, cf, stride);
    if(draw_buf == NULL) {
        LV_LOG_ERROR("create draw buf failed, cf = %d", cf);
        return LV_RESULT_INVALID;
    }

    dsc->decoded = draw_buf;
    uint8_t * dest = draw_buf->data;

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
            image_color32_pre_mul((lv_color32_t *)dest, palette_size);
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

    /* premul alpha */
    image_try_self_pre_mul(dsc);

    /* invalidate D-Cache */
    image_invalidate_cache(draw_buf->data, stride, width, height, cf);

    LV_LOG_INFO("image %s (W%" LV_PRId32 " x H%" LV_PRId32 ", buffer: %p cf: %d) decode finish",
                path, width, height, draw_buf->data, cf);
    return LV_RESULT_OK;

failed:
    lv_fs_close(&file);
    lv_draw_buf_destroy(draw_buf);
    dsc->decoded = NULL;

    return LV_RESULT_INVALID;
}

/**
 * Decode an image using the vg_lite gpu.
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{

    lv_result_t res = LV_RESULT_INVALID;

    switch(dsc->src_type) {
        case LV_IMAGE_SRC_VARIABLE:
            res = decoder_open_variable(decoder, dsc);
            break;
        case LV_IMAGE_SRC_FILE:
            res = decoder_open_file(decoder, dsc);
            break;
        default:
            break;
    }

    if(dsc->args.no_cache) return LV_RES_OK;

#if LV_CACHE_DEF_SIZE > 0
    if(res == LV_RESULT_OK) {
        lv_image_cache_data_t search_key;
        search_key.src_type = dsc->src_type;
        search_key.src = dsc->src;
        search_key.slot.size = dsc->decoded->data_size;

        lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, dsc->decoded, NULL);

        if(entry == NULL) {
            lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
            dsc->decoded = NULL;
            return LV_RESULT_INVALID;
        }
        dsc->cache_entry = entry;
    }
#endif

    return res;
}

static void decoder_draw_buf_free(lv_draw_buf_t * draw_buf)
{
    if(draw_buf->header.flags & LV_VG_LITE_IMAGE_NO_CACHE) {
        lv_free(draw_buf);
    }
    else {
        lv_draw_buf_destroy(draw_buf);
    }
}

static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->args.no_cache || LV_CACHE_DEF_SIZE == 0)
        lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
    else
        lv_cache_release(dsc->cache, dsc->cache_entry, NULL);
}

static void decoder_cache_free(lv_image_cache_data_t * cached_data, void * user_data)
{
    LV_UNUSED(user_data);

    if(cached_data->src_type == LV_IMAGE_SRC_FILE) lv_free((void *)cached_data->src);

    decoder_draw_buf_free((lv_draw_buf_t *)cached_data->decoded);
}

#endif /*LV_USE_DRAW_VG_LITE*/
