/**
 * @file lv_image_decoder.h
 *
 */

#ifndef LV_IMAGE_DECODER_H
#define LV_IMAGE_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include "lv_draw_buf.h"
#include "../misc/lv_fs.h"
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"
#include "../misc/cache/lv_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Source of image.*/
enum _lv_image_src_t {
    LV_IMAGE_SRC_VARIABLE, /** Binary/C variable*/
    LV_IMAGE_SRC_FILE, /** File in filesystem*/
    LV_IMAGE_SRC_SYMBOL, /** Symbol (@ref lv_symbol_def.h)*/
    LV_IMAGE_SRC_UNKNOWN, /** Unknown source*/
};

#ifdef DOXYGEN
typedef _lv_image_src_t lv_image_src_t;
#else
typedef uint8_t lv_image_src_t;
#endif /*DOXYGEN*/

/*Decoder function definitions*/
struct _lv_image_decoder_dsc_t;
typedef struct _lv_image_decoder_dsc_t lv_image_decoder_dsc_t;

/**
 * Image decoder args.
 * It determines how to decoder an image, e.g. whether to premultiply the alpha or not.
 * It should be passed to lv_img_decoder_open() function. If NULL is provided, default
 * args are used.
 *
 * Default args:
 * all field are zero or false.
 */
typedef struct _lv_image_decoder_args_t {
    bool stride_align;      /*Whether stride should be aligned*/
    bool premultiply;       /*Whether image should be premultiplied or not after decoding*/
    bool no_cache;          /*When set, decoded image won't be put to cache, and decoder open will also ignore cache.*/
    bool use_indexed;       /*Decoded indexed image as is. Convert to ARGB8888 if false.*/
} lv_image_decoder_args_t;

/**
 * Get info from an image and store in the `header`
 * @param src the image source. Can be a pointer to a C array or a file name (Use
 * `lv_image_src_get_type` to determine the type)
 * @param header store the info here
 * @return LV_RESULT_OK: info written correctly; LV_RESULT_INVALID: failed
 */
typedef lv_result_t (*lv_image_decoder_info_f_t)(lv_image_decoder_t * decoder, const void * src,
                                                 lv_image_header_t * header);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor. `src`, `color` are already initialized in it.
 */
typedef lv_result_t (*lv_image_decoder_open_f_t)(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RESULT_OK: ok; LV_RESULT_INVALID: failed
 */
typedef lv_result_t (*lv_image_decoder_get_area_cb_t)(lv_image_decoder_t * decoder,
                                                      lv_image_decoder_dsc_t * dsc,
                                                      const lv_area_t * full_area, lv_area_t * decoded_area);

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
typedef void (*lv_image_decoder_close_f_t)(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

struct _lv_image_decoder_t {
    lv_image_decoder_info_f_t info_cb;
    lv_image_decoder_open_f_t open_cb;
    lv_image_decoder_get_area_cb_t get_area_cb;
    lv_image_decoder_close_f_t close_cb;

    lv_cache_free_cb_t cache_free_cb;
    void * user_data;
};

typedef struct _lv_image_decoder_cache_data_t {
    lv_cache_slot_size_t slot;

    const void * src;
    lv_image_src_t src_type;

    const lv_draw_buf_t * decoded;
    const lv_image_decoder_t * decoder;
    void * user_data;
} lv_image_cache_data_t;

typedef struct _lv_image_decoder_header_cache_data_t {
    const void * src;
    lv_image_src_t src_type;

    lv_image_header_t header;
    lv_image_decoder_t * decoder;
} lv_image_header_cache_data_t;

/**Describe an image decoding session. Stores data about the decoding*/
struct _lv_image_decoder_dsc_t {
    /**The decoder which was able to open the image source*/
    lv_image_decoder_t * decoder;

    /*A copy of parameters of how this image is decoded*/
    lv_image_decoder_args_t args;

    /**The image source. A file path like "S:my_img.png" or pointer to an `lv_image_dsc_t` variable*/
    const void * src;

    /**Type of the source: file or variable. Can be set in `open` function if required*/
    lv_image_src_t src_type;

    /**Info about the opened image: color format, size, etc. MUST be set in `open` function*/
    lv_image_header_t header;

    /** Pointer to a draw buffer where the image's data (pixels) are stored in a decoded, plain format.
     *  MUST be set in `open` or `get_area_cb`function*/
    const lv_draw_buf_t * decoded;    /*A draw buffer to described decoded image.*/

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
 */
void _lv_image_decoder_init(void);

/**
 * Deinitialize the image decoder module
 */
void _lv_image_decoder_deinit(void);

/**
 * Get information about an image.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src the image source. Can be
 *  1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `lv_fs_drv_register()`)
 *  2) Variable: Pointer to an `lv_image_dsc_t` variable
 *  3) Symbol: E.g. `LV_SYMBOL_OK`
 * @param header the image info will be stored here
 * @return LV_RESULT_OK: success; LV_RESULT_INVALID: wasn't able to get info about the image
 */
lv_result_t lv_image_decoder_get_info(const void * src, lv_image_header_t * header);

/**
 * Open an image.
 * Try the created image decoders one by one. Once one is able to open the image that decoder is saved in `dsc`
 * @param dsc describes a decoding session. Simply a pointer to an `lv_image_decoder_dsc_t` variable.
 * @param src the image source. Can be
 *  1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `lv_fs_drv_register())`)
 *  2) Variable: Pointer to an `lv_image_dsc_t` variable
 *  3) Symbol: E.g. `LV_SYMBOL_OK`
 * @param color The color of the image with `LV_COLOR_FORMAT_ALPHA_...`
 * @param args args about how the image should be opened.
 * @return LV_RESULT_OK: opened the image. `dsc->decoded` and `dsc->header` are set.
 *         LV_RESULT_INVALID: none of the registered image decoders were able to open the image.
 */
lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t * dsc, const void * src, const lv_image_decoder_args_t * args);

/**
 * Decode an area of the opened image
 * @param dsc           image decoder descriptor
 * @param full_area     start X coordinate (from left)
 * @param decoded_area  start Y coordinate (from top)
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: an error occurred
 */
lv_result_t lv_image_decoder_get_area(lv_image_decoder_dsc_t * dsc, const lv_area_t * full_area,
                                      lv_area_t * decoded_area);

/**
 * Close a decoding session
 * @param dsc pointer to `lv_image_decoder_dsc_t` used in `lv_image_decoder_open`
 */
void lv_image_decoder_close(lv_image_decoder_dsc_t * dsc);

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
lv_image_decoder_t * lv_image_decoder_create(void);

/**
 * Delete an image decoder
 * @param decoder pointer to an image decoder
 */
void lv_image_decoder_delete(lv_image_decoder_t * decoder);

/**
 * Get the next image decoder in the linked list of image decoders
 * @param decoder pointer to an image decoder or NULL to get the first one
 * @return the next image decoder or NULL if no more image decoder exists
 */
lv_image_decoder_t * lv_image_decoder_get_next(lv_image_decoder_t * decoder);

/**
 * Set a callback to get information about the image
 * @param decoder pointer to an image decoder
 * @param info_cb a function to collect info about an image (fill an `lv_image_header_t` struct)
 */
void lv_image_decoder_set_info_cb(lv_image_decoder_t * decoder, lv_image_decoder_info_f_t info_cb);

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void lv_image_decoder_set_open_cb(lv_image_decoder_t * decoder, lv_image_decoder_open_f_t open_cb);

/**
 * Set a callback to a decoded line of an image
 * @param decoder pointer to an image decoder
 * @param read_line_cb a function to read a line of an image
 */
void lv_image_decoder_set_get_area_cb(lv_image_decoder_t * decoder, lv_image_decoder_get_area_cb_t read_line_cb);

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void lv_image_decoder_set_close_cb(lv_image_decoder_t * decoder, lv_image_decoder_close_f_t close_cb);

/**
 * Set a custom method to free cache data.
 * Normally this is not needed. If the custom decoder allocates additional memory other than dsc->decoded
 * draw buffer, then you need to register your own method to free it. By default the cache entry is free'ed
 * in `image_decoder_cache_free_cb`.
 *
 * @param decoder pointer to the image decoder
 * @param cache_free_cb the custom callback to free cache data. Refer to `image_decoder_cache_free_cb`.
 */
void lv_image_decoder_set_cache_free_cb(lv_image_decoder_t * decoder, lv_cache_free_cb_t cache_free_cb);

#if LV_CACHE_DEF_SIZE > 0
lv_cache_entry_t * lv_image_decoder_add_to_cache(lv_image_decoder_t * decoder,
                                                 lv_image_cache_data_t * search_key,
                                                 const lv_draw_buf_t * decoded, void * user_data);
#endif

/**
 * Check the decoded image, make any modification if decoder `args` requires.
 * @note A new draw buf will be allocated if provided `decoded` is not modifiable or stride mismatch etc.
 * @param dsc       pointer to a decoder descriptor
 * @param decoded   pointer to a decoded image to post process to meet dsc->args requirement.
 * @return          post processed draw buffer, when it differs with `decoded`, it's newly allocated.
 */
lv_draw_buf_t * lv_image_decoder_post_process(lv_image_decoder_dsc_t * dsc, lv_draw_buf_t * decoded);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_DECODER_H*/
