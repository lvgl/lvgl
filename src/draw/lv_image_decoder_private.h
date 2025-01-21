/**
 * @file lv_image_decoder_private.h
 *
 */

#ifndef LV_IMAGE_DECODER_PRIVATE_H
#define LV_IMAGE_DECODER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_decoder.h"
#include "../misc/cache/lv_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Image decoder args.
 * It determines how to decoder an image, e.g. whether to premultiply the alpha or not.
 * It should be passed to lv_img_decoder_open() function. If NULL is provided, default
 * args are used.
 *
 * Default args:
 * all field are zero or false.
 */
struct _lv_image_decoder_args_t {
    bool stride_align;      /**< Whether stride should be aligned */
    bool premultiply;       /**< Whether image should be premultiplied or not after decoding */
    bool no_cache;          /**< When set, decoded image won't be put to cache, and decoder open will also ignore cache. */
    bool use_indexed;       /**< Decoded indexed image as is. Convert to ARGB8888 if false. */
    bool flush_cache;       /**< Whether to flush the data cache after decoding */
};

struct _lv_image_decoder_t {
    lv_image_decoder_info_f_t info_cb;
    lv_image_decoder_open_f_t open_cb;
    lv_image_decoder_get_area_cb_t get_area_cb;
    lv_image_decoder_close_f_t close_cb;

    lv_image_decoder_custom_draw_t custom_draw_cb;

    const char * name;

    void * user_data;
};

struct _lv_image_cache_data_t {
    lv_cache_slot_size_t slot;

    const void * src;
    lv_image_src_t src_type;

    const lv_draw_buf_t * decoded;
    const lv_image_decoder_t * decoder;
    void * user_data;
};

struct _lv_image_header_cache_data_t {
    const void * src;
    lv_image_src_t src_type;

    lv_image_header_t header;
    lv_image_decoder_t * decoder;
};

/**Describe an image decoding session. Stores data about the decoding*/
struct _lv_image_decoder_dsc_t {
    /**The decoder which was able to open the image source*/
    lv_image_decoder_t * decoder;

    /**A copy of parameters of how this image is decoded*/
    lv_image_decoder_args_t args;

    /**The image source. A file path like "S:my_img.png" or pointer to an `lv_image_dsc_t` variable*/
    const void * src;

    /**Type of the source: file or variable. Can be set in `open` function if required*/
    lv_image_src_t src_type;

    lv_fs_file_t file;

    /**Info about the opened image: color format, size, etc. MUST be set in `open` function*/
    lv_image_header_t header;

    /** Pointer to a draw buffer where the image's data (pixels) are stored in a decoded, plain format.
     *  MUST be set in `open` or `get_area_cb`function*/
    const lv_draw_buf_t * decoded;

    const lv_color32_t * palette;
    uint32_t palette_size;

    /** How much time did it take to open the image. [ms]
     *  If not set `lv_image_cache` will measure and set the time to open*/
    uint32_t time_to_open;

    /**A text to display instead of the image when the image can't be opened.
     * Can be set in `open` function or set NULL.*/
    const char * error_msg;

    lv_cache_t * cache;

    /**Point to cache entry information*/
    lv_cache_entry_t * cache_entry;

    /**Store any custom data here is required*/
    void * user_data;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the image decoder module
 * @param image_cache_size    Image cache size in bytes. 0 to disable cache.
 * @param image_header_count  Number of header cache entries. 0 to disable header cache.
 */
void lv_image_decoder_init(uint32_t image_cache_size, uint32_t image_header_count);

/**
 * Deinitialize the image decoder module
 */
void lv_image_decoder_deinit(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_DECODER_PRIVATE_H*/
