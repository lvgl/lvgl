/**
 * @file lv_libpng.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder_private.h"
#include "../../../lvgl.h"
#if LV_USE_LIBPNG

#include "lv_libpng.h"
#include <png.h>
#include <string.h>
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "PNG"

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_draw_buf_t * decode_png(lv_image_decoder_dsc_t * dsc);

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
 * Register the PNG decoder functions in LVGL
 */
void lv_libpng_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);

    dec->name = DECODER_NAME;
}

void lv_libpng_deinit(void)
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
 * Get info about a PNG image
 * @param dsc can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_image_src_t src_type = dsc->src_type;          /*Get the source type*/

    if(src_type == LV_IMAGE_SRC_FILE || src_type == LV_IMAGE_SRC_VARIABLE) {
        uint32_t * size;
        static const uint8_t magic[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
        uint8_t buf[24];

        /*If it's a PNG file...*/
        if(src_type == LV_IMAGE_SRC_FILE) {
            /* Read the width and height from the file. They have a constant location:
            * [16..19]: width
            * [20..23]: height
            */
            uint32_t rn;
            lv_fs_read(&dsc->file, buf, sizeof(buf), &rn);

            if(rn != sizeof(buf)) return LV_RESULT_INVALID;

            if(lv_memcmp(buf, magic, sizeof(magic)) != 0) return LV_RESULT_INVALID;

            size = (uint32_t *)&buf[16];
        }
        /*If it's a PNG file in a  C array...*/
        else {
            const lv_image_dsc_t * img_dsc = dsc->src;
            const uint32_t data_size = img_dsc->data_size;
            size = ((uint32_t *)img_dsc->data) + 4;

            if(data_size < sizeof(magic)) return LV_RESULT_INVALID;
            if(lv_memcmp(img_dsc->data, magic, sizeof(magic)) != 0) return LV_RESULT_INVALID;
        }

        /*Save the data in the header*/
        header->cf = LV_COLOR_FORMAT_ARGB8888;
        /*The width and height are stored in Big endian format so convert them to little endian*/
        header->w = (int32_t)((size[0] & 0xff000000) >> 24) + ((size[0] & 0x00ff0000) >> 8);
        header->h = (int32_t)((size[1] & 0xff000000) >> 24) + ((size[1] & 0x00ff0000) >> 8);

        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;         /*If didn't succeeded earlier then it's an error*/
}

/**
 * Open a PNG image and return the decoded image
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_draw_buf_t * decoded;
    decoded = decode_png(dsc);

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

    /*Add the decoded image to the cache*/
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

/**
 * Free the allocated resources
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->args.no_cache ||
       !lv_image_cache_is_enabled()) lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
}

static lv_draw_buf_t * decode_png(lv_image_decoder_dsc_t * dsc)
{
    LV_PROFILER_DECODER_BEGIN;
    int ret;
    uint8_t * png_data;
    uint32_t png_data_size;
    /*Prepare png_image*/
    png_image image;
    lv_memzero(&image, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        png_data = lv_fs_load_with_alloc((const char *)dsc->src, &png_data_size);
        if(png_data == NULL) {
            LV_LOG_WARN("can't load file: %s", (const char *)dsc->src);
            LV_PROFILER_DECODER_END;
            return NULL;
        }
    }
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = dsc->src;
        png_data = (uint8_t *)img_dsc->data;
        png_data_size = img_dsc->data_size;
    }
    else {
        LV_PROFILER_DECODER_END;
        return NULL;
    }

    /*Ready to read file*/
    LV_PROFILER_DECODER_BEGIN_TAG("png_image_begin_read_from_memory");
    ret = png_image_begin_read_from_memory(&image, png_data, png_data_size);
    LV_PROFILER_DECODER_END_TAG("png_image_begin_read_from_memory");
    if(!ret) {
        LV_LOG_ERROR("png read failed: %d", ret);
        if(dsc->src_type == LV_IMAGE_SRC_FILE)
            lv_free(png_data);
        LV_PROFILER_DECODER_END;
        return NULL;
    }

    lv_color_format_t cf;
    if(dsc->args.use_indexed && (image.format & PNG_FORMAT_FLAG_COLORMAP)) {
        cf = LV_COLOR_FORMAT_I8;
        image.format = PNG_FORMAT_BGRA_COLORMAP;
    }
    else {
        cf = LV_COLOR_FORMAT_ARGB8888;
        image.format = PNG_FORMAT_BGRA;
    }

    /*Alloc image buffer*/
    lv_draw_buf_t * decoded;
    decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, image.width, image.height, cf, LV_STRIDE_AUTO);
    if(decoded == NULL) {

        if(dsc->src_type == LV_IMAGE_SRC_FILE) {
            LV_LOG_ERROR("alloc PNG_IMAGE_SIZE(%" LV_PRIu32 ") failed: %s", (uint32_t)PNG_IMAGE_SIZE(image),
                         (const char *)dsc->src);
            lv_free(png_data);
        }
        else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE)
            LV_LOG_ERROR("alloc PNG_IMAGE_SIZE(%" LV_PRIu32 ")", (uint32_t)PNG_IMAGE_SIZE(image));

        LV_PROFILER_DECODER_BEGIN_TAG("png_image_free");
        png_image_free(&image);
        LV_PROFILER_DECODER_END_TAG("png_image_free");
        LV_PROFILER_DECODER_END;
        return NULL;
    }

    void * palette = decoded->data;
    void * map = decoded->data + LV_COLOR_INDEXED_PALETTE_SIZE(cf) * sizeof(lv_color32_t);

    /*Start decoding*/
    LV_PROFILER_DECODER_BEGIN_TAG("png_image_finish_read");
    ret = png_image_finish_read(&image, NULL, map, decoded->header.stride, palette);
    LV_PROFILER_DECODER_END_TAG("png_image_finish_read");
    LV_PROFILER_DECODER_BEGIN_TAG("png_image_free");
    png_image_free(&image);
    LV_PROFILER_DECODER_END_TAG("png_image_free");
    if(dsc->src_type == LV_IMAGE_SRC_FILE)
        lv_free(png_data);
    if(!ret) {
        LV_LOG_ERROR("png decode failed: %s", image.message);
        lv_draw_buf_destroy(decoded);
        LV_PROFILER_DECODER_END;
        return NULL;
    }

    LV_PROFILER_DECODER_END;
    return decoded;
}

#endif /*LV_USE_LIBPNG*/
