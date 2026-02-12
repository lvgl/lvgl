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
#include "../../core/lv_global.h"
#include "../lv_image_decoder_private.h"
#include "../../libs/bin_decoder/lv_bin_decoder.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "VG_LITE"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/* Since the palette and index image are next to each other,
 * the palette size needs to be aligned to ensure that the image is aligned.
 */
#define I8_IMG_OFFSET \
    LV_VG_LITE_ALIGN(LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I8) * sizeof(lv_color32_t), LV_DRAW_BUF_ALIGN)

#define SWAP_UINT16(x) ((((x) & 0x00FF) << 8) | (((x) & 0xFF00) >> 8))

/**********************
 *      TYPEDEFS
 **********************/

/* This structure represents the ARGB8565 format (3 bytes per pixel). */
typedef struct {
    lv_color16_t color;
    uint8_t alpha;
} color16_alpha_t;

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

static uint32_t get_image_stride(const lv_image_header_t * header)
{
    /* use stride in header */
    if(header->stride) {
        return header->stride;
    }

    /* compact format stride */
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

static lv_color_format_t get_converted_cf(lv_color_format_t cf)
{
    switch(cf) {
        /**
         * VG_LITE_INDEX1, 2, and 4 require endian flipping + bit flipping,
         * so for simplicity, convert them to I8.
         */
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
            return LV_COLOR_FORMAT_I8;

        case LV_COLOR_FORMAT_A1:
        case LV_COLOR_FORMAT_A2:
            return LV_COLOR_FORMAT_A8;

        /**
         * If the GPU does not support the 24-bit format, convert it to ARGB8888;
         * otherwise, use the normal bin_decoder processing flow.
         */
        case LV_COLOR_FORMAT_RGB888:
            return vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT) ? LV_COLOR_FORMAT_UNKNOWN : LV_COLOR_FORMAT_XRGB8888;
        case LV_COLOR_FORMAT_ARGB8565:
            return vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT) ? LV_COLOR_FORMAT_UNKNOWN : LV_COLOR_FORMAT_ARGB8888;

        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_AL88:
            return LV_COLOR_FORMAT_ARGB8888;

        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            return LV_COLOR_FORMAT_RGB565;

        default:
            break;
    }

    return LV_COLOR_FORMAT_UNKNOWN;
}

static void set_premultiplied_flag_if_needed(lv_draw_buf_t * dest_buf, bool premultiply)
{
    if(premultiply && lv_color_format_has_alpha(dest_buf->header.cf)) {
        lv_draw_buf_set_flag(dest_buf, LV_IMAGE_FLAGS_PREMULTIPLIED);
    }
}

static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    lv_result_t res = lv_bin_decoder_info(decoder, dsc, header);
    if(res != LV_RESULT_OK) {
        return res;
    }

    lv_color_format_t cf = get_converted_cf(header->cf);
    if(cf == LV_COLOR_FORMAT_UNKNOWN) {
        return LV_RESULT_INVALID;
    }

    if(header->flags & LV_IMAGE_FLAGS_COMPRESSED) {
        lv_image_src_t src_type = lv_image_src_get_type(dsc->src);
        LV_LOG_WARN("NOT Supported compressed flags: %d, format: %d, type: %d, src: %p (%s)",
                    header->flags, header->cf, src_type, dsc->src, src_type == LV_IMAGE_SRC_FILE ? (const char *)dsc->src : "variable");
        return LV_RESULT_INVALID;
    }

    header->cf = cf;
    return LV_RESULT_OK;
}

static lv_result_t decoder_open_variable_index(lv_draw_buf_t * dest_buf, const lv_draw_buf_t * src_buf,
                                               bool premultiply)
{
    LV_PROFILER_DECODER_BEGIN;
    /* Since dsc->header.cf is uniformly set to I8,
     * the original format is obtained from src for conversion.
     */

    /*In case of uncompressed formats the image stored in the ROM/RAM.
     *So simply give its pointer*/
    const uint8_t * src = src_buf->data;
    uint8_t * dest = dest_buf->data;

    /* index format only */
    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(src_buf->header.cf);
    LV_ASSERT(palette_size > 0);
    uint32_t palette_size_bytes = palette_size * sizeof(lv_color32_t);

    /* copy palette */
    lv_memcpy(dest, src, palette_size_bytes);

    if(premultiply) {
        /* pre-multiply palette */
        image_color32_pre_mul((lv_color32_t *)dest, palette_size);
    }

    /* move to index image map */
    src += palette_size_bytes;
    dest += I8_IMG_OFFSET;

    /* copy index image */
    for(int32_t y = 0; y < src_buf->header.h; y++) {
        image_decode_to_index8_line(dest, src, src_buf->header.w, src_buf->header.cf);
        src += src_buf->header.stride;
        dest += dest_buf->header.stride;
    }

    LV_PROFILER_DECODER_END;
    return LV_RESULT_OK;
}

static void convert_rgb565_swapped_line(uint16_t * dest, const uint16_t * src, uint32_t px_cnt)
{
    while(px_cnt--) {
        *dest = SWAP_UINT16(*src);
        dest++;
        src++;
    }
}

static void convert_rgb888_line(lv_color32_t * dest, const lv_color_t * src, uint32_t px_cnt)
{
    while(px_cnt--) {
        dest->red = src->red;
        dest->green = src->green;
        dest->blue = src->blue;
        dest->alpha = 0xFF;
        src++;
        dest++;
    }
}

static void convert_argb8565_line(lv_color32_t * dest, const color16_alpha_t * src, uint32_t px_cnt, bool premultiply)
{
    while(px_cnt--) {
        dest->red = src->color.red * 0xFF / 0x1F;
        dest->green = src->color.green * 0xFF / 0x3F;
        dest->blue = src->color.blue * 0xFF / 0x1F;
        dest->alpha = src->alpha;

        if(premultiply) {
            lv_color_premultiply(dest);
        }

        src++;
        dest++;
    }
}

static void convert_al88_line(lv_color32_t * dest, const lv_color16a_t * src, uint32_t px_cnt, bool premultiply)
{
    while(px_cnt--) {
        dest->red = src->lumi;
        dest->green = src->lumi;
        dest->blue = src->lumi;
        dest->alpha = src->alpha;

        if(premultiply) {
            lv_color_premultiply(dest);
        }

        src++;
        dest++;
    }
}

static void convert_alpha_to_a8_line(uint8_t * dest, const uint8_t * src, int32_t w_px,
                                     lv_color_format_t color_format)
{
    uint8_t px_size;
    uint8_t max_val;

    int8_t shift = 0;
    switch(color_format) {
        case LV_COLOR_FORMAT_A1:
            px_size = 1;
            shift = 7;
            max_val = 1;
            break;
        case LV_COLOR_FORMAT_A2:
            px_size = 2;
            shift = 6;
            max_val = 3;
            break;
        case LV_COLOR_FORMAT_A4:
            px_size = 4;
            shift = 4;
            max_val = 15;
            break;
        case LV_COLOR_FORMAT_A8:
            lv_memcpy(dest, src, w_px);
            return;
        default:
            LV_ASSERT_FORMAT_MSG(false, "Unsupported color format: %d", color_format);
            return;
    }

    uint16_t mask = (1 << px_size) - 1;

    for(int32_t i = 0; i < w_px; i++) {
        uint8_t val_act = (*src >> shift) & mask;
        dest[i] = (val_act * 0xFF) / max_val;

        shift -= px_size;
        if(shift < 0) {
            shift = 8 - px_size;
            src++;
        }
    }
}

static lv_result_t decoder_open_variable_alpha(lv_draw_buf_t * dest_buf, const lv_draw_buf_t * src_buf)
{
    LV_PROFILER_DECODER_BEGIN;

    const uint8_t * src = src_buf->data;
    uint8_t * dest = dest_buf->data;

    for(int32_t y = 0; y < src_buf->header.h; y++) {
        convert_alpha_to_a8_line(dest, src, src_buf->header.w, src_buf->header.cf);
        src += src_buf->header.stride;
        dest += dest_buf->header.stride;
    }

    LV_PROFILER_DECODER_END;
    return LV_RESULT_OK;
}

static lv_result_t decoder_open_variable_rgb(lv_draw_buf_t * dest_buf,
                                             const lv_draw_buf_t * src_buf, bool premultiply)
{
    LV_PROFILER_DECODER_BEGIN;

    switch(src_buf->header.cf) {
        case LV_COLOR_FORMAT_RGB565_SWAPPED: {
                for(uint32_t y = 0; y < src_buf->header.h; y++) {
                    const uint16_t * src = lv_draw_buf_goto_xy(src_buf, 0, y);
                    uint16_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_rgb565_swapped_line(dest, src, src_buf->header.w);
                }
            }
            break;

        case LV_COLOR_FORMAT_RGB888: {
                for(uint32_t y = 0; y < src_buf->header.h; y++) {
                    const lv_color_t * src = lv_draw_buf_goto_xy(src_buf, 0, y);
                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_rgb888_line(dest, src, src_buf->header.w);
                }
            }
            break;

        case LV_COLOR_FORMAT_ARGB8565: {
                for(uint32_t y = 0; y < src_buf->header.h; y++) {
                    const color16_alpha_t * src = lv_draw_buf_goto_xy(src_buf, 0, y);
                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_argb8565_line(dest, src, src_buf->header.w, premultiply);
                }
            }
            break;

        case LV_COLOR_FORMAT_RGB565A8: {
                const uint8_t * alpha_row = lv_draw_buf_goto_xy(src_buf, 0, src_buf->header.h - 1);
                alpha_row += src_buf->header.stride;

                for(uint32_t y = 0; y < src_buf->header.h; y++) {
                    const lv_color16_t * src = lv_draw_buf_goto_xy(src_buf, 0, y);
                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    const uint8_t * alpha_p = alpha_row;
                    for(uint32_t x = 0; x < src_buf->header.w; x++) {
                        dest->red = src->red * 0xFF / 0x1F;
                        dest->green = src->green * 0xFF / 0x3F;
                        dest->blue = src->blue * 0xFF / 0x1F;
                        dest->alpha = *alpha_p;

                        if(premultiply) {
                            lv_color_premultiply(dest);
                        }

                        src++;
                        dest++;
                        alpha_p++;
                    }

                    alpha_row += src_buf->header.stride / 2;
                }
            }
            break;

        case LV_COLOR_FORMAT_AL88: {
                for(uint32_t y = 0; y < src_buf->header.h; y++) {
                    const lv_color16a_t * src = lv_draw_buf_goto_xy(src_buf, 0, y);
                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_al88_line(dest, src, src_buf->header.w, premultiply);
                }
            }
            break;

        default:
            return LV_RESULT_INVALID;
    }

    LV_PROFILER_DECODER_END;
    return LV_RESULT_OK;
}

static bool file_read_line(lv_fs_file_t * file, void * buf, uint32_t size)
{
    uint32_t br = 0;
    lv_fs_res_t res = lv_fs_read(file, buf, size, &br);
    if(res != LV_FS_RES_OK || br != size) {
        LV_LOG_ERROR("read size: %" LV_PRIu32 " failed, br: %" LV_PRIu32 " res: %d",
                     size, br, res);
        return false;
    }

    return true;
}

static lv_result_t decoder_open_file_index(lv_draw_buf_t * dest_buf,
                                           lv_fs_file_t * file,
                                           const lv_image_header_t * src_header,
                                           bool premultiply)
{
    LV_PROFILER_DECODER_BEGIN;
    uint32_t width = src_header->w;
    uint32_t height = src_header->h;
    uint8_t * src_temp = NULL;

    uint8_t * dest = dest_buf->data;

    /* index format only */
    uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(src_header->cf);
    LV_ASSERT(palette_size > 0);

    /* read palette */
    if(!file_read_line(file, dest, palette_size * sizeof(lv_color32_t))) {
        LV_LOG_ERROR("read palette failed");
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    if(premultiply) {
        /* pre-multiply palette */
        image_color32_pre_mul((lv_color32_t *)dest, palette_size);
    }

    src_temp = lv_malloc(src_header->stride);
    if(!src_temp) {
        LV_LOG_ERROR("malloc src_stride: %" LV_PRIu32 " failed", src_header->stride);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    /* move to index image map */
    dest += I8_IMG_OFFSET;

    for(uint32_t y = 0; y < height; y++) {
        if(!file_read_line(file, src_temp, src_header->stride)) {
            lv_free(src_temp);
            LV_PROFILER_DECODER_END;
            return LV_RESULT_INVALID;
        }

        /* convert to index8 */
        image_decode_to_index8_line(dest, src_temp, width, src_header->cf);
        dest += dest_buf->header.stride;
    }

    lv_free(src_temp);
    LV_PROFILER_DECODER_END;
    return LV_RESULT_OK;
}

static lv_result_t decoder_open_file_alpha(lv_draw_buf_t * dest_buf,
                                           lv_fs_file_t * file,
                                           const lv_image_header_t * src_header)
{
    LV_PROFILER_DECODER_BEGIN;
    uint32_t width = src_header->w;
    uint32_t height = src_header->h;

    uint8_t * src_temp = lv_malloc(src_header->stride);
    if(!src_temp) {
        LV_LOG_ERROR("malloc src_stride: %" LV_PRIu32 " failed", src_header->stride);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    uint8_t * dest = dest_buf->data;

    for(uint32_t y = 0; y < height; y++) {
        if(!file_read_line(file, src_temp, src_header->stride)) {
            lv_free(src_temp);
            LV_PROFILER_DECODER_END;
            return LV_RESULT_INVALID;
        }

        convert_alpha_to_a8_line(dest, src_temp, width, src_header->cf);
        dest += dest_buf->header.stride;
    }

    lv_free(src_temp);
    LV_PROFILER_DECODER_END;
    return LV_RESULT_OK;
}

static lv_result_t decoder_open_file_rgb(lv_draw_buf_t * dest_buf,
                                         lv_fs_file_t * file,
                                         const lv_image_header_t * src_header,
                                         bool premultiply)
{
    LV_PROFILER_DECODER_BEGIN;
    uint32_t width = src_header->w;
    uint32_t height = src_header->h;

    void * src_temp = lv_malloc(src_header->stride);
    if(!src_temp) {
        LV_LOG_ERROR("malloc src_stride: %" LV_PRIu32 " failed", src_header->stride);
        LV_PROFILER_DECODER_END;
        return LV_RESULT_INVALID;
    }

    lv_result_t res = LV_RESULT_INVALID;

    switch(src_header->cf) {
        case LV_COLOR_FORMAT_RGB565_SWAPPED: {
                for(uint32_t y = 0; y < height; y++) {
                    if(!file_read_line(file, src_temp, src_header->stride)) {
                        goto failed;
                    }

                    uint16_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_rgb565_swapped_line(dest, src_temp, width);
                }

                res = LV_RESULT_OK;
            }
            break;

        case LV_COLOR_FORMAT_RGB888: {
                for(uint32_t y = 0; y < height; y++) {
                    if(!file_read_line(file, src_temp, src_header->stride)) {
                        goto failed;
                    }

                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_rgb888_line(dest, src_temp, width);
                }

                res = LV_RESULT_OK;
            }
            break;

        case LV_COLOR_FORMAT_ARGB8565: {
                for(uint32_t y = 0; y < height; y++) {
                    if(!file_read_line(file, src_temp, src_header->stride)) {
                        goto failed;
                    }

                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_argb8565_line(dest, src_temp, width, premultiply);
                }

                res = LV_RESULT_OK;
            }
            break;

        case LV_COLOR_FORMAT_RGB565A8: {
                /* First pass: read RGB565 and convert to ARGB8888, skip alpha */
                for(uint32_t y = 0; y < height; y++) {
                    if(!file_read_line(file, src_temp, src_header->stride)) {
                        goto failed;
                    }

                    const lv_color16_t * src = src_temp;
                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    for(uint32_t x = 0; x < width; x++) {
                        dest->red = src->red * 0xFF / 0x1F;
                        dest->green = src->green * 0xFF / 0x3F;
                        dest->blue = src->blue * 0xFF / 0x1F;
                        src++;
                        dest++;
                    }
                }

                /* Second pass: read A8 and update alpha, handle premultiply if needed */
                uint32_t alpha_stride = src_header->stride / 2;
                for(uint32_t y = 0; y < height; y++) {
                    if(!file_read_line(file, src_temp, alpha_stride)) {
                        goto failed;
                    }

                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    const uint8_t * src = src_temp;
                    for(uint32_t x = 0; x < width; x++) {
                        dest->alpha = *src;

                        if(premultiply) {
                            lv_color_premultiply(dest);
                        }

                        src++;
                        dest++;
                    }
                }

                res = LV_RESULT_OK;
            }
            break;

        case LV_COLOR_FORMAT_AL88: {
                for(uint32_t y = 0; y < height; y++) {
                    if(!file_read_line(file, src_temp, src_header->stride)) {
                        goto failed;
                    }

                    lv_color32_t * dest = lv_draw_buf_goto_xy(dest_buf, 0, y);
                    convert_al88_line(dest, src_temp, width, premultiply);
                }

                res = LV_RESULT_OK;
            }
            break;

        default:
            break;
    }

failed:
    lv_free(src_temp);
    LV_PROFILER_DECODER_END;
    return res;
}

static lv_draw_buf_t * create_dest_buf(uint32_t width, uint32_t height, lv_color_format_t src_cf)
{
    lv_color_format_t dest_cf = get_converted_cf(src_cf);
    if(dest_cf == LV_COLOR_FORMAT_UNKNOWN) {
        LV_LOG_WARN("NOT Supported src_cf: %d", src_cf);
        return NULL;
    }

    lv_draw_buf_t * dest_buf = lv_draw_buf_create_ex(image_cache_draw_buf_handlers,
                                                     width, height, dest_cf,
                                                     LV_STRIDE_AUTO);
    if(!dest_buf) {
        return NULL;
    }

    lv_draw_buf_clear(dest_buf, NULL);
    return dest_buf;
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
        case LV_IMAGE_SRC_VARIABLE: {
                lv_draw_buf_t src_buf;
                res = lv_draw_buf_from_image(&src_buf, dsc->src);
                if(res != LV_RESULT_OK) {
                    return res;
                }

                const bool src_premultiplied = lv_draw_buf_has_flag(&src_buf, LV_IMAGE_FLAGS_PREMULTIPLIED);
                const bool premultiply = src_premultiplied ? false : dsc->args.premultiply;

                /**
                 * Since lv_draw_buf_from_image automatically calculates the stride,
                 * we need to obtain the original stride information.
                 */
                src_buf.header.stride = get_image_stride(&((lv_image_dsc_t *)dsc->src)->header);

                lv_draw_buf_t * dest_buf = create_dest_buf(dsc->header.w, dsc->header.h, src_buf.header.cf);
                if(!dest_buf) {
                    return LV_RESULT_INVALID;
                }

                switch(src_buf.header.cf) {
                    case LV_COLOR_FORMAT_I1:
                    case LV_COLOR_FORMAT_I2:
                    case LV_COLOR_FORMAT_I4:
                    case LV_COLOR_FORMAT_I8:
                        res = decoder_open_variable_index(dest_buf, &src_buf, premultiply);
                        break;

                    case LV_COLOR_FORMAT_A1:
                    case LV_COLOR_FORMAT_A2:
                        res = decoder_open_variable_alpha(dest_buf, &src_buf);
                        break;

                    case LV_COLOR_FORMAT_RGB565_SWAPPED:
                    case LV_COLOR_FORMAT_RGB888:
                    case LV_COLOR_FORMAT_ARGB8565:
                    case LV_COLOR_FORMAT_RGB565A8:
                    case LV_COLOR_FORMAT_AL88:
                        res = decoder_open_variable_rgb(dest_buf, &src_buf, premultiply);
                        break;

                    default:
                        LV_LOG_WARN("NOT Supported color format: %d, src: %p", src_buf.header.cf, dsc->src);
                        res = LV_RESULT_INVALID;
                        break;
                }

                if(res == LV_RESULT_OK) {
                    set_premultiplied_flag_if_needed(dest_buf, src_premultiplied || dsc->args.premultiply);
                    dsc->decoded = dest_buf;
                }
                else {
                    lv_draw_buf_destroy(dest_buf);
                    dsc->decoded = NULL;
                    return res;
                }
            }

            break;
        case LV_IMAGE_SRC_FILE: {
                lv_fs_file_t file;
                lv_fs_res_t fs_res = lv_fs_open(&file, dsc->src, LV_FS_MODE_RD);
                if(fs_res != LV_FS_RES_OK) {
                    LV_LOG_ERROR("open %s failed, res: %d", (const char *)dsc->src, fs_res);
                    return LV_RESULT_INVALID;
                }

                /* get real src header */
                lv_image_header_t src_header;
                uint32_t header_br = 0;
                fs_res = lv_fs_read(&file, &src_header, sizeof(src_header), &header_br);
                if(fs_res != LV_FS_RES_OK || header_br != sizeof(src_header)) {
                    LV_LOG_ERROR("read %s lv_image_header_t failed, res: %d, br: %" LV_PRIu32, (const char *)dsc->src, fs_res, header_br);
                    lv_fs_close(&file);
                    return LV_RESULT_INVALID;
                }

                if(src_header.magic != LV_IMAGE_HEADER_MAGIC) {
                    LV_LOG_WARN("Legacy bin image detected: %s", (const char *)dsc->src);
                    src_header.cf = src_header.magic;
                    src_header.magic = LV_IMAGE_HEADER_MAGIC;
                    src_header.flags &= ~LV_IMAGE_FLAGS_PREMULTIPLIED;
                }

                const bool src_premultiplied = src_header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED;
                const bool premultiply = src_premultiplied ? false : dsc->args.premultiply;

                src_header.stride = get_image_stride(&src_header);

                lv_draw_buf_t * dest_buf = create_dest_buf(dsc->header.w, dsc->header.h, src_header.cf);
                if(!dest_buf) {
                    lv_fs_close(&file);
                    return LV_RESULT_INVALID;
                }

                switch(src_header.cf) {
                    case LV_COLOR_FORMAT_I1:
                    case LV_COLOR_FORMAT_I2:
                    case LV_COLOR_FORMAT_I4:
                    case LV_COLOR_FORMAT_I8:
                        res = decoder_open_file_index(dest_buf, &file, &src_header, premultiply);
                        break;

                    case LV_COLOR_FORMAT_A1:
                    case LV_COLOR_FORMAT_A2:
                        res = decoder_open_file_alpha(dest_buf, &file, &src_header);
                        break;

                    case LV_COLOR_FORMAT_RGB565_SWAPPED:
                    case LV_COLOR_FORMAT_RGB888:
                    case LV_COLOR_FORMAT_ARGB8565:
                    case LV_COLOR_FORMAT_RGB565A8:
                    case LV_COLOR_FORMAT_AL88:
                        res = decoder_open_file_rgb(dest_buf, &file, &src_header, premultiply);
                        break;

                    default:
                        LV_LOG_WARN("NOT Supported color format: %d, src: %s", src_header.cf, (const char *)dsc->src);
                        res = LV_RESULT_INVALID;
                        break;
                }

                if(res == LV_RESULT_OK) {
                    set_premultiplied_flag_if_needed(dest_buf, src_premultiplied || dsc->args.premultiply);
                    dsc->decoded = dest_buf;
                }
                else {
                    LV_LOG_WARN("read %s failed", (const char *)dsc->src);
                    lv_draw_buf_destroy(dest_buf);
                    dsc->decoded = NULL;
                }

                lv_fs_close(&file);
            }
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
