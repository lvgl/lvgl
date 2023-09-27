/**
 * @file lv_lodepng.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_LODEPNG

#include "lv_lodepng.h"
#include "lodepng.h"
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(struct _lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * dec, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * dec, lv_image_decoder_dsc_t * dsc);
static void convert_color_depth(uint8_t * img_p, uint32_t px_cnt);
static const void * decode_png_data(const void * png_data, size_t png_data_size);
static lv_result_t try_cache(lv_image_decoder_dsc_t * dsc);

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
void lv_lodepng_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);
}

void lv_lodepng_deinit(void)
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
 * @param decoder   pointer to the decoder where this function belongs
 * @param src       can be file name or pointer to a C array
 * @param header    image information is set in header parameter
 * @return          LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(struct _lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    (void) decoder; /*Unused*/
    lv_image_src_t src_type = lv_image_src_get_type(src);          /*Get the source type*/

    /*If it's a PNG file...*/
    if(src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = src;
        if(strcmp(lv_fs_get_ext(fn), "png") == 0) {              /*Check the extension*/

            /* Read the width and height from the file. They have a constant location:
             * [16..23]: width
             * [24..27]: height
             */
            uint32_t size[2];
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) return LV_RESULT_INVALID;

            lv_fs_seek(&f, 16, LV_FS_SEEK_SET);

            uint32_t rn;
            lv_fs_read(&f, &size, 8, &rn);
            lv_fs_close(&f);

            if(rn != 8) return LV_RESULT_INVALID;

            /*Save the data in the header*/
            header->always_zero = 0;
            header->cf = LV_COLOR_FORMAT_ARGB8888;
            /*The width and height are stored in Big endian format so convert them to little endian*/
            header->w = (lv_coord_t)((size[0] & 0xff000000) >> 24) + ((size[0] & 0x00ff0000) >> 8);
            header->h = (lv_coord_t)((size[1] & 0xff000000) >> 24) + ((size[1] & 0x00ff0000) >> 8);

            return LV_RESULT_OK;
        }
    }
    /*If it's a PNG file in a  C array...*/
    else if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        const uint32_t data_size = img_dsc->data_size;
        const uint32_t * size = ((uint32_t *)img_dsc->data) + 4;
        const uint8_t magic[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
        if(data_size < sizeof(magic)) return LV_RESULT_INVALID;
        if(memcmp(magic, img_dsc->data, sizeof(magic))) return LV_RESULT_INVALID;
        header->always_zero = 0;

        header->cf = LV_COLOR_FORMAT_ARGB8888;

        if(img_dsc->header.w) {
            header->w = img_dsc->header.w;         /*Save the image width*/
        }
        else {
            header->w = (lv_coord_t)((size[0] & 0xff000000) >> 24) + ((size[0] & 0x00ff0000) >> 8);
        }

        if(img_dsc->header.h) {
            header->h = img_dsc->header.h;         /*Save the color height*/
        }
        else {
            header->h = (lv_coord_t)((size[1] & 0xff000000) >> 24) + ((size[1] & 0x00ff0000) >> 8);
        }

        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;         /*If didn't succeeded earlier then it's an error*/
}


/**
 * Open a PNG image and decode it into dsc.img_data
 * @param decoder   pointer to the decoder where this function belongs
 * @param dsc       decoded image descriptor
 * @return          LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    (void) decoder; /*Unused*/

    /*Check the cache first*/
    if(try_cache(dsc) == LV_RESULT_OK) return LV_RESULT_OK;

    const uint8_t * png_data = NULL;
    size_t png_data_size = 0;
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;
        if(strcmp(lv_fs_get_ext(fn), "png") == 0) {              /*Check the extension*/
            unsigned error;
            error = lodepng_load_file((void *)&png_data, &png_data_size, fn);  /*Load the file*/
            if(error) {
                if(png_data != NULL) {
                    lv_free((void *)png_data);
                }
                LV_LOG_WARN("error %u: %s\n", error, lodepng_error_text(error));
                return LV_RESULT_INVALID;
            }
        }
    }
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = dsc->src;
        png_data = img_dsc->data;
        png_data_size = img_dsc->data_size;
    }
    else {
        return LV_RESULT_INVALID;
    }

    lv_cache_lock();
    lv_cache_entry_t * cache = lv_cache_add(dsc->header.w * dsc->header.h * 4);
    if(cache == NULL) {
        lv_cache_unlock();
        return LV_RESULT_INVALID;
    }

    uint32_t t = lv_tick_get();
    const void * decoded_img = decode_png_data(png_data, png_data_size);
    t = lv_tick_elaps(t);
    cache->weight = t;
    cache->data = decoded_img;
    cache->free_data = 1;
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        cache->src = lv_strdup(dsc->src);
        cache->src_type = LV_CACHE_SRC_TYPE_STR;
        cache->free_src = 1;
        lv_free((void *)png_data);
    }
    else {
        cache->src_type = LV_CACHE_SRC_TYPE_PTR;
        cache->src = dsc->src;
    }

    dsc->img_data = lv_cache_get_data(cache);
    dsc->user_data = cache;

    lv_cache_unlock();
    return LV_RESULT_OK;    /*If not returned earlier then it failed*/
}

/**
 * Close PNG image and free data
 * @param decoder   pointer to the decoder where this function belongs
 * @param dsc       decoded image descriptor
 * @return          LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    lv_cache_lock();
    lv_cache_release(dsc->user_data);
    lv_cache_unlock();
}


static lv_result_t try_cache(lv_image_decoder_dsc_t * dsc)
{
    lv_cache_lock();
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;

        lv_cache_entry_t * cache = lv_cache_find(fn, LV_CACHE_SRC_TYPE_STR, 0, 0);
        if(cache) {
            dsc->img_data = lv_cache_get_data(cache);
            dsc->user_data = cache;     /*Save the cache to release it in decoder_close*/
            lv_cache_unlock();
            return LV_RESULT_OK;
        }
    }

    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = dsc->src;

        lv_cache_entry_t * cache = lv_cache_find(img_dsc, LV_CACHE_SRC_TYPE_PTR, 0, 0);
        if(cache) {
            dsc->img_data = lv_cache_get_data(cache);
            dsc->user_data = cache;     /*Save the cache to release it in decoder_close*/
            lv_cache_unlock();
            return LV_RESULT_OK;
        }
    }

    lv_cache_unlock();
    return LV_RESULT_INVALID;
}

static const void * decode_png_data(const void * png_data, size_t png_data_size)
{
    unsigned png_width;             /*Not used, just required by the decoder*/
    unsigned png_height;            /*Not used, just required by the decoder*/
    uint8_t * img_data = NULL;

    /*Decode the image in ARGB8888 */
    unsigned error = lodepng_decode32(&img_data, &png_width, &png_height, png_data, png_data_size);

    if(error) {
        if(img_data != NULL)  lv_free(img_data);
        return NULL;
    }

    /*Convert the image to the system's color depth*/
    convert_color_depth(img_data,  png_width * png_height);

    return img_data;
}


/**
 * If the display is not in 32 bit format (ARGB888) then convert the image to the current color depth
 * @param img the ARGB888 image
 * @param px_cnt number of pixels in `img`
 */
static void convert_color_depth(uint8_t * img_p, uint32_t px_cnt)
{
    lv_color32_t * img_argb = (lv_color32_t *)img_p;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        uint8_t blue = img_argb[i].blue;
        img_argb[i].blue = img_argb[i].red;
        img_argb[i].red = blue;
    }
}

#endif /*LV_USE_LODEPNG*/


