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

#include "lv_draw_buf.h"
#include "../misc/lv_fs.h"
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Source of image.*/
typedef enum {
    LV_IMAGE_SRC_VARIABLE, /** Binary/C variable*/
    LV_IMAGE_SRC_FILE, /** File in filesystem*/
    LV_IMAGE_SRC_SYMBOL, /** Symbol (@ref lv_symbol_def.h)*/
    LV_IMAGE_SRC_UNKNOWN, /** Unknown source*/
} lv_image_src_t;

/**
 * Get info from an image and store in the `header`
 * @param decoder  pointer to decoder object
 * @param dsc      pointer to decoder descriptor
 * @param header   store the info here
 * @return LV_RESULT_OK: info written correctly; LV_RESULT_INVALID: failed
 */
typedef lv_result_t (*lv_image_decoder_info_f_t)(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                                 lv_image_header_t * header);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param decoder  pointer to the decoder the function associated with
 * @param dsc      pointer to decoder descriptor. `src`, `color` are already initialized in it.
 */
typedef lv_result_t (*lv_image_decoder_open_f_t)(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**
 * Decode `full_area` pixels incrementally by calling in a loop. Set `decoded_area` values to `LV_COORD_MIN` on first call.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 * @param full_area input parameter. the full area to decode after enough subsequent calls
 * @param decoded_area input+output parameter. set the values to `LV_COORD_MIN` for the first call and to reset decoding.
 *                     the decoded area is stored here after each call.
 * @return LV_RESULT_OK: ok; LV_RESULT_INVALID: failed or there is nothing left to decode
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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

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
 * @param dsc    describes a decoding session. Simply a pointer to an `lv_image_decoder_dsc_t` variable.
 * @param src    the image source. Can be
 *                 1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `lv_fs_drv_register())`)
 *                 2) Variable: Pointer to an `lv_image_dsc_t` variable
 *                 3) Symbol: E.g. `LV_SYMBOL_OK`
 * @param args   args about how the image should be opened.
 * @return LV_RESULT_OK: opened the image. `dsc->decoded` and `dsc->header` are set.
 *         LV_RESULT_INVALID: none of the registered image decoders were able to open the image.
 */
lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t * dsc, const void * src, const lv_image_decoder_args_t * args);

/**
 * Decode `full_area` pixels incrementally by calling in a loop. Set `decoded_area` to `LV_COORD_MIN` on first call.
 * @param dsc           image decoder descriptor
 * @param full_area     input parameter. the full area to decode after enough subsequent calls
 * @param decoded_area  input+output parameter. set the values to `LV_COORD_MIN` for the first call and to reset decoding.
 *                      the decoded area is stored here after each call.
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: an error occurred or there is nothing left to decode
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

lv_cache_entry_t * lv_image_decoder_add_to_cache(lv_image_decoder_t * decoder,
                                                 lv_image_cache_data_t * search_key,
                                                 const lv_draw_buf_t * decoded, void * user_data);

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
