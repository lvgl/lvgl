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

/* VG_LITE_INDEX1, 2, and 4 require endian flipping + bit flipping,
 * so for simplicity, they are uniformly converted to I8 for display.
 */
#define DEST_IMG_FORMAT LV_COLOR_FORMAT_I8

/* Since the palette and index image are next to each other,
 * the palette size needs to be aligned to ensure that the image is aligned.
 */
#define DEST_IMG_OFFSET \
    LV_VG_LITE_ALIGN(LV_COLOR_INDEXED_PALETTE_SIZE(DEST_IMG_FORMAT) * sizeof(lv_color32_t), LV_DRAW_BUF_ALIGN)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_buf_t yuv;  /*A draw buffer struct for yuv variable image*/
} decoder_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void image_color32_pre_mul(lv_color32_t * img_data, uint32_t px_size);

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
    lv_image_decoder_set_cache_free_cb(decoder, NULL); /*Use general cache free method*/
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
        lv_color_premultiply(img_data);
        img_data++;
    }
}

static uint32_t image_stride(const lv_image_header_t * header)
{
    /* use stride in header */
    if(header->stride) {
        return header->stride;
    }

    /* guess stride */
    uint32_t ori_stride = header->w * lv_color_format_get_bpp(header->cf);
    ori_stride = (ori_stride + 7) >> 3; /*Round up*/
    return ori_stride;
}

static void image_decode_to_index8_line(uint8_t * dest, const uint8_t * src, int32_t w_px,
                                        lv_color_format_t color_format)
{
    uint8_t px_size;
    uint16_t mask;

    int8_t shift = 0;
    switch(color_format) {
        case LV_COLOR_FORMAT_I1:
            px_size = 1;
            shift = 7;
            break;
        case LV_COLOR_FORMAT_I2:
            px_size = 2;
            shift = 6;
            break;
        case LV_COLOR_FORMAT_I4:
            px_size = 4;
            shift = 4;
            break;
        case LV_COLOR_FORMAT_I8:
            lv_memcpy(dest, src, w_px);
            return;
        default:
            LV_ASSERT_FORMAT_MSG(false, "Unsupported color format: %d", color_format);
            return;
    }

    mask = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    for(int32_t i = 0; i < w_px; i++) {
        uint8_t val_act = (*src >> shift) & mask;
        dest[i] = val_act;

        shift -= px_size;
        if(shift < 0) {
            shift = 8 - px_size;
            src++;
        }
    }
}

static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    lv_result_t res = lv_bin_decoder_info(decoder, src, header);
    if(res != LV_RESULT_OK) {
        return res;
    }

    if(LV_COLOR_FORMAT_IS_YUV(header->cf)) {
        return LV_RESULT_OK;
    }

    if(LV_COLOR_FORMAT_IS_INDEXED(header->cf)) {
        header->cf = DEST_IMG_FORMAT;
        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;
}

static lv_result_t decoder_open_variable(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_draw_buf_t src_img_buf;
    lv_draw_buf_from_image(&src_img_buf, dsc->src);

    /* Since dsc->header.cf is uniformly set to I8,
     * the original format is obtained from src for conversion.
     */
    lv_color_format_t src_cf = src_img_buf.header.cf;

    int32_t width = dsc->header.w;
    int32_t height = dsc->header.h;

    /*In case of uncompressed formats the image stored in the ROM/RAM.
     *So simply give its pointer*/
    const uint8_t * image_data = ((lv_image_dsc_t *)dsc->src)->data;
    uint32_t image_data_size = ((lv_image_dsc_t *)dsc->src)->data_size;

    /* if is YUV format, no need to copy */
    if(LV_COLOR_FORMAT_IS_YUV(src_cf)) {
        decoder_data_t * decoder_data = dsc->user_data;
        if(decoder_data == NULL) {
            decoder_data = lv_malloc_zeroed(sizeof(decoder_data_t));
            LV_ASSERT_MALLOC(decoder_data);
        }
        lv_draw_buf_t * draw_buf = &decoder_data->yuv;
        uint32_t stride = lv_draw_buf_width_to_stride(width, src_cf);
        lv_draw_buf_init(draw_buf, width, height, src_cf, stride, (void *)image_data, image_data_size);

        /* Use alloced bit to indicate we should not free the memory */
        draw_buf->header.flags &= ~LV_IMAGE_FLAGS_ALLOCATED;

        /* Do not add this kind of image to cache, since its life is managed by user. */
        dsc->args.no_cache = true;

        dsc->decoded = draw_buf;
        return LV_RESULT_OK;
    }

    /* create draw buf */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(width, height, DEST_IMG_FORMAT, LV_STRIDE_AUTO);
    if(draw_buf == NULL) {
        return LV_RESULT_INVALID;
    }
    dsc->decoded = draw_buf;

    uint32_t src_stride = image_stride(&src_img_buf.header);
    uint32_t dest_stride = draw_buf->header.stride;

    const uint8_t * src = image_data;
    uint8_t * dest = draw_buf->data;

    /* index format only */
    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(src_cf);
    LV_ASSERT(palette_size > 0);

    uint32_t palette_size_bytes = palette_size * sizeof(lv_color32_t);

    /* copy palette */
    lv_memcpy(dest, src, palette_size_bytes);

    if(!dsc->args.premultiply) {
        /* pre-multiply palette */
        image_color32_pre_mul((lv_color32_t *)dest, palette_size);
        draw_buf->header.flags |= LV_IMAGE_FLAGS_PREMULTIPLIED;
    }

    /* move to index image map */
    src += palette_size_bytes;
    dest += DEST_IMG_OFFSET;

    /* copy index image */
    for(int32_t y = 0; y < height; y++) {
        image_decode_to_index8_line(dest, src, width, src_cf);
        src += src_stride;
        dest += dest_stride;
    }

    /* invalidate D-Cache */
    lv_draw_buf_invalidate_cache(draw_buf, NULL);
    LV_LOG_INFO("image %p (W%" LV_PRId32 " x H%" LV_PRId32 ", buffer: %p, cf: %d) decode finish",
                image_data, width, height, draw_buf->data, src_cf);

    return LV_RESULT_OK;
}

static lv_result_t decoder_open_file(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    uint32_t width = dsc->header.w;
    uint32_t height = dsc->header.h;
    const char * path = dsc->src;
    uint8_t * src_temp = NULL;

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("open %s failed", path);
        return LV_RESULT_INVALID;
    }

    /* get real src header */
    lv_image_header_t src_header;
    uint32_t header_br = 0;
    res = lv_fs_read(&file, &src_header, sizeof(src_header), &header_br);
    if(res != LV_FS_RES_OK || header_br != sizeof(src_header)) {
        LV_LOG_ERROR("read %s lv_image_header_t failed", path);
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(width, height, DEST_IMG_FORMAT, LV_STRIDE_AUTO);
    if(draw_buf == NULL) {
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    /* get stride */
    uint32_t src_stride = image_stride(&src_header);
    uint32_t dest_stride = draw_buf->header.stride;

    dsc->decoded = draw_buf;
    uint8_t * dest = draw_buf->data;

    /* index format only */
    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(src_header.cf);
    if(palette_size == 0) {
        LV_LOG_ERROR("file %s invalid palette size: %" LV_PRIu32, path, palette_size);
        goto failed;
    }

    uint32_t palette_size_bytes = palette_size * sizeof(lv_color32_t);

    /* read palette */
    uint32_t palette_br = 0;
    res = lv_fs_read(&file, dest, palette_size_bytes, &palette_br);
    if(res != LV_FS_RES_OK || palette_br != palette_size_bytes) {
        LV_LOG_ERROR("read %s (palette: %" LV_PRIu32 ", br: %" LV_PRIu32 ") failed",
                     path, palette_size_bytes, palette_br);
        goto failed;
    }

    if(dsc->args.premultiply) {
        /* pre-multiply palette */
        image_color32_pre_mul((lv_color32_t *)dest, palette_size);
        draw_buf->header.flags |= LV_IMAGE_FLAGS_PREMULTIPLIED;
    }

    src_temp = lv_malloc(src_stride);
    if(src_temp == NULL) {
        LV_LOG_ERROR("malloc src_temp failed");
        goto failed;
    }

    /* move to index image map */
    dest += DEST_IMG_OFFSET;

    for(uint32_t y = 0; y < height; y++) {
        uint32_t br = 0;
        res = lv_fs_read(&file, src_temp, src_stride, &br);
        if(res != LV_FS_RES_OK || br != src_stride) {
            LV_LOG_ERROR("read %s (y: %" LV_PRIu32 ", src_stride: %" LV_PRIu32 ", br: %" LV_PRIu32 ") failed",
                         path, y, src_stride, br);
            goto failed;
        }

        /* convert to index8 */
        image_decode_to_index8_line(dest, src_temp, width, src_header.cf);
        dest += dest_stride;
    }

    lv_free(src_temp);

    lv_fs_close(&file);

    /* invalidate D-Cache */
    lv_draw_buf_invalidate_cache(draw_buf, NULL);

    LV_LOG_INFO("image %s (W%" LV_PRId32 " x H%" LV_PRId32 ", buffer: %p cf: %d) decode finish",
                path, width, height, draw_buf->data, src_header.cf);
    return LV_RESULT_OK;

failed:
    if(src_temp) {
        lv_free(src_temp);
    }
    lv_fs_close(&file);
    lv_draw_buf_destroy(draw_buf);
    dsc->decoded = NULL;

    return LV_RESULT_INVALID;
}

static void decoder_draw_buf_free(lv_draw_buf_t * draw_buf)
{
    if((draw_buf->header.flags & LV_IMAGE_FLAGS_ALLOCATED) == 0) {
        /* This must be the yuv variable image. */
        return;
    }

    lv_draw_buf_destroy(draw_buf);
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

    if(dsc->args.no_cache) return res;

#if LV_CACHE_DEF_SIZE > 0
    if(res == LV_RESULT_OK) {
        lv_image_cache_data_t search_key;
        search_key.src_type = dsc->src_type;
        search_key.src = dsc->src;
        search_key.slot.size = dsc->decoded->data_size;

        lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, dsc->decoded, NULL);

        if(entry == NULL) {
            decoder_draw_buf_free((lv_draw_buf_t *)dsc->decoded);
            dsc->decoded = NULL;
            return LV_RESULT_INVALID;
        }
        dsc->cache_entry = entry;
    }
#endif

    return res;
}

static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->args.no_cache || LV_CACHE_DEF_SIZE == 0)
        decoder_draw_buf_free((lv_draw_buf_t *)dsc->decoded);
    else
        lv_cache_release(dsc->cache, dsc->cache_entry, NULL);

    if(decoder->user_data) free(decoder->user_data);
}

#endif /*LV_USE_DRAW_VG_LITE*/
