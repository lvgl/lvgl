/**
 * @file lv_libwebp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder_private.h"
#include "../../../lvgl.h"
#if LV_USE_LIBWEBP

#include "lv_libwebp.h"
#include <webp/decode.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "LIBWEBP"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

#define WEBP_HEADER_SIZE    64

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_draw_buf_t * decode_webp_file(lv_image_decoder_dsc_t * dsc, const char * filename);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register the WEBP decoder functions in LVGL
 */
void lv_libwebp_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);

    dec->name = DECODER_NAME;
}

void lv_libwebp_deinit(void)
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

/**
 * Get info about a WEBP image
 * @param dsc can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/
    lv_image_src_t src_type = dsc->src_type;          /*Get the source type*/

    /*If it's a webp file...*/
    if(src_type == LV_IMAGE_SRC_FILE) {
        uint8_t buf[WEBP_HEADER_SIZE];
        int width;
        int height;
        uint32_t rn;
        lv_fs_res_t res = lv_fs_read(&dsc->file, buf, sizeof(buf), &rn);

        /* The max header size = RIFF + VP8X + (optional chunks) + VP8(L), 64 bytes is enough to get the width and height.
         * If the file is smaller than 64 bytes, it's maybe a valid webp file,
         * so we don't check the result length here.
         * VP8X : RIFF(12) + VP8X(18) = 30bytes;
         * VP8(L): RIFF(12) + VP8(L) chunk header(8) + VP8(L) frame header(5)  = 23bytes;
         * VP8: RIFF(12) + VP8(L) chunk header(8) + VP8(L) frame header(10) = 28bytes;
         */
        if(res != LV_FS_RES_OK) return LV_RESULT_INVALID;

        if(WebPGetInfo(buf, rn, &width, &height) == 0) {
            return LV_RESULT_INVALID;
        }

        /*Default decoder color format is ARGB8888*/
        header->cf = LV_COLOR_FORMAT_ARGB8888;
        header->w = width;
        header->h = height;
        header->stride = width * sizeof(lv_color32_t);
        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;         /*If it didn't succeed earlier then it's an error*/
}

/**
 * Open a WEBP image and return the decoded image
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    /*If it's a webp file...*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;
        lv_draw_buf_t * decoded = decode_webp_file(dsc, fn);
        if(decoded == NULL) {
            return LV_RESULT_INVALID;
        }

        lv_draw_buf_t * adjusted = lv_image_decoder_post_process(dsc, decoded);
        if(adjusted == NULL) {
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }

        /*The adjusted draw buffer is newly allocated.*/
        if(adjusted != decoded) {
            lv_draw_buf_destroy(decoded);
            decoded = adjusted;
        }

        dsc->decoded = decoded;

        if(dsc->args.no_cache) {
            return LV_RESULT_OK;
        }

        /*If the image cache is disabled, just return the decoded image*/
        if(!lv_image_cache_is_enabled()) {
            return LV_RESULT_OK;
        }

        lv_image_cache_data_t search_key;
        search_key.src_type = dsc->src_type;
        search_key.src = dsc->src;
        search_key.slot.size = decoded->data_size;

        lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, decoded, NULL);

        if(entry == NULL) {
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }
        dsc->cache_entry = entry;

        return LV_RESULT_OK;     /*The image is fully decoded. Return with its pointer*/
    }

    return LV_RESULT_INVALID;    /*If not returned earlier then it failed*/
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->args.no_cache ||
       !lv_image_cache_is_enabled()) lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
}

static lv_draw_buf_t * decode_webp_file(lv_image_decoder_dsc_t * dsc, const char * filename)
{
    LV_PROFILER_DECODER_BEGIN;
    uint32_t data_size;
    uint8_t * data = lv_fs_load_with_alloc(filename, &data_size);
    if(data == NULL) {
        LV_LOG_WARN("can't load file: %s", filename);
        LV_PROFILER_DECODER_END;
        return NULL;
    }

    /*Alloc image buffer*/
    lv_draw_buf_t * decoded;
    decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, dsc->header.w, dsc->header.h, dsc->header.cf,
                                    LV_STRIDE_AUTO);
    if(decoded == NULL) {
        LV_LOG_ERROR("alloc draw buffer failed: %s", filename);
        lv_free(data);
        LV_PROFILER_DECODER_END;
        return NULL;
    }

    WebPDecoderConfig config;
    WebPInitDecoderConfig(&config);

    config.output.colorspace = MODE_BGRA;
    config.output.u.RGBA.rgba = (uint8_t *) decoded->data;
    config.output.u.RGBA.stride = decoded->header.stride;
    config.output.u.RGBA.size = decoded->data_size;
    config.output.is_external_memory = 1;

    LV_PROFILER_DECODER_BEGIN_TAG("WebPDecode");
    int status = WebPDecode(data, data_size, &config);
    LV_PROFILER_DECODER_END_TAG("WebPDecode");

    if(status != VP8_STATUS_OK) {
        LV_LOG_ERROR("decode webp failed: %s, status: %d", filename, status);
        lv_draw_buf_destroy(decoded);
        decoded = NULL;
    }

    lv_free(data);
    LV_PROFILER_DECODER_END;
    return decoded;
}

#endif /*LV_USE_LIBWEBP*/
