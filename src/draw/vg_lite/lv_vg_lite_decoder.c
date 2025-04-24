/**
 * @file lv_vg_lite_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_image_decoder_private.h"
#include "lv_vg_lite_decoder.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include <stdlib.h>
#include <string.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "VG_LITE"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/* VG_LITE_INDEX1, 2, and 4 require endian flipping + bit flipping,
 * so for simplicity, they are uniformly converted to I8 for display.
 */
#define DEST_IMG_FORMAT LV_COLOR_FORMAT_I8
#define IS_CONV_INDEX_FORMAT(cf) (cf == LV_COLOR_FORMAT_I1 || cf == LV_COLOR_FORMAT_I2 || cf == LV_COLOR_FORMAT_I4)

/* Since the palette and index image are next to each other,
 * the palette size needs to be aligned to ensure that the image is aligned.
 */
#define DEST_IMG_OFFSET \
    LV_VG_LITE_ALIGN(LV_COLOR_INDEXED_PALETTE_SIZE(DEST_IMG_FORMAT) * sizeof(lv_color32_t), LV_DRAW_BUF_ALIGN)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * src, lv_image_header_t * header);
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

    decoder->name = DECODER_NAME;
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

static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    lv_result_t res = lv_bin_decoder_info(decoder, dsc, header);
    if(res != LV_RESULT_OK) {
        return res;
    }

    if(!IS_CONV_INDEX_FORMAT(header->cf)) {
        return LV_RESULT_INVALID;
    }

    if(header->flags & LV_IMAGE_FLAGS_COMPRESSED) {
        LV_LOG_WARN("NOT Supported compressed index format: %d", header->cf);
        return LV_RESULT_INVALID;
    }

    header->cf = DEST_IMG_FORMAT;
    return LV_RESULT_OK;
}

static lv_result_t decoder_open_variable(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_draw_buf_t src_img_buf;
    lv_result_t res = lv_draw_buf_from_image(&src_img_buf, dsc->src);
    if(res != LV_RESULT_OK) {
        return res;
    }

    /* Since dsc->header.cf is uniformly set to I8,
     * the original format is obtained from src for conversion.
     */
    lv_color_format_t src_cf = src_img_buf.header.cf;

    int32_t width = dsc->header.w;
    int32_t height = dsc->header.h;

    /* create draw buf */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, width, height, DEST_IMG_FORMAT,
                                                     LV_STRIDE_AUTO);
    if(draw_buf == NULL) {
        return LV_RESULT_INVALID;
    }

    lv_draw_buf_clear(draw_buf, NULL);
    dsc->decoded = draw_buf;

    uint32_t src_stride = image_stride(&src_img_buf.header);
    uint32_t dest_stride = draw_buf->header.stride;

    /*In case of uncompressed formats the image stored in the ROM/RAM.
     *So simply give its pointer*/
    const uint8_t * src = ((lv_image_dsc_t *)dsc->src)->data;
    uint8_t * dest = draw_buf->data;

    /* index format only */
    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(src_cf);
    LV_ASSERT(palette_size > 0);

    uint32_t palette_size_bytes = palette_size * sizeof(lv_color32_t);

    /* copy palette */
    lv_memcpy(dest, src, palette_size_bytes);

    if(dsc->args.premultiply) {
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

    lv_draw_buf_t * draw_buf = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, width, height, DEST_IMG_FORMAT,
                                                     LV_STRIDE_AUTO);
    if(draw_buf == NULL) {
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    lv_draw_buf_clear(draw_buf, NULL);

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

    /*If the image cache is disabled, just return the decoded image*/
    if(!lv_image_cache_is_enabled()) return res;

    /*Add the decoded image to the cache*/
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

    return res;
}

static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->args.no_cache || !lv_image_cache_is_enabled()) lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
}

#endif /*LV_USE_DRAW_VG_LITE*/
