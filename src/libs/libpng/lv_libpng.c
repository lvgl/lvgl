/**
 * @file lv_libpng.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_LIBPNG

#include "lv_libpng.h"
#include <png.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_draw_buf_t * decode_png_file(const char * filename);

static void png_decoder_cache_free_cb(lv_image_cache_data_t * cached_data, void * user_data);
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
    lv_image_decoder_set_cache_free_cb(dec, (lv_cache_free_cb_t)png_decoder_cache_free_cb);
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
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/
    lv_image_src_t src_type = lv_image_src_get_type(src);          /*Get the source type*/

    /*If it's a PNG file...*/
    if(src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = src;

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return LV_RESULT_INVALID;

        /* Read the width and height from the file. They have a constant location:
         * [16..19]: width
         * [20..23]: height
         */
        uint8_t buf[24];
        uint32_t rn;
        lv_fs_read(&f, buf, sizeof(buf), &rn);
        lv_fs_close(&f);

        if(rn != sizeof(buf)) return LV_RESULT_INVALID;

        const uint8_t magic[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
        if(memcmp(buf, magic, sizeof(magic)) != 0) return LV_RESULT_INVALID;

        uint32_t * size = (uint32_t *)&buf[16];
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
 * Open a PNG image and return the decided image
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    /*If it's a PNG file...*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;
        lv_draw_buf_t * decoded = decode_png_file(fn);
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

        if(dsc->args.no_cache) return LV_RES_OK;

#if LV_CACHE_DEF_SIZE > 0
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
#endif
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

    if(dsc->args.no_cache || LV_CACHE_DEF_SIZE == 0)
        lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
    else
        lv_cache_release(dsc->cache, dsc->cache_entry, NULL);
}

static uint8_t * alloc_file(const char * filename, uint32_t * size)
{
    uint8_t * data = NULL;
    lv_fs_file_t f;
    uint32_t data_size;
    uint32_t rn;
    lv_fs_res_t res;

    *size = 0;

    res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("can't open %s", filename);
        return NULL;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_tell(&f, &data_size);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    /*Read file to buffer*/
    data = lv_malloc(data_size);
    if(data == NULL) {
        LV_LOG_WARN("malloc failed for data");
        goto failed;
    }

    res = lv_fs_read(&f, data, data_size, &rn);

    if(res == LV_FS_RES_OK && rn == data_size) {
        *size = rn;
    }
    else {
        LV_LOG_WARN("read file failed");
        lv_free(data);
        data = NULL;
    }

failed:
    lv_fs_close(&f);

    return data;
}

static lv_draw_buf_t * decode_png_file(const char * filename)
{
    int ret;

    /*Prepare png_image*/
    png_image image;
    lv_memzero(&image, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    uint32_t data_size;
    uint8_t * data = alloc_file(filename, &data_size);
    if(data == NULL) {
        LV_LOG_WARN("can't load file: %s", filename);
        return NULL;
    }

    /*Ready to read file*/
    ret = png_image_begin_read_from_memory(&image, data, data_size);
    if(!ret) {
        LV_LOG_ERROR("png file: %s read failed: %d", filename, ret);
        lv_free(data);
        return NULL;
    }

    /*Set color format*/
    image.format = PNG_FORMAT_BGRA;

    /*Alloc image buffer*/
    lv_draw_buf_t * decoded;
    decoded = lv_draw_buf_create(image.width, image.height, LV_COLOR_FORMAT_ARGB8888, PNG_IMAGE_ROW_STRIDE(image));
    if(decoded == NULL) {
        LV_LOG_ERROR("alloc PNG_IMAGE_SIZE(%" LV_PRIu32 ") failed: %s", (uint32_t)PNG_IMAGE_SIZE(image), filename);
        lv_free(data);
        return NULL;
    }

    /*Start decoding*/
    ret = png_image_finish_read(&image, NULL, decoded->data, 0, NULL);
    png_image_free(&image);
    lv_free(data);
    if(!ret) {
        LV_LOG_ERROR("png decode failed: %d", ret);
        lv_draw_buf_destroy(decoded);
        return NULL;
    }

    return decoded;
}

static void png_decoder_cache_free_cb(lv_image_cache_data_t * cached_data, void * user_data)
{
    LV_UNUSED(user_data);

    if(cached_data->src_type == LV_IMAGE_SRC_FILE) lv_free((void *)cached_data->src);
    lv_draw_buf_destroy((lv_draw_buf_t *)cached_data->decoded);
}

#endif /*LV_USE_LIBPNG*/
