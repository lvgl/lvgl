/**
 * @file lv_bin_decoder.h
 *
 */

#ifndef LV_BIN_DECODER_H
#define LV_BIN_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_decoder.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the binary image decoder module
 */
void lv_bin_decoder_init(void);

/**
 * Get info about a lvgl binary image
 * @param decoder the decoder where this function belongs
 * @param dsc image descriptor containing the source and type of the image and other info.
 * @param header store the image data here
 * @return LV_RESULT_OK: the info is successfully stored in `header`; LV_RESULT_INVALID: unknown format or other error.
 */
lv_result_t lv_bin_decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header);

lv_result_t lv_bin_decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area);

/**
 * Open a lvgl binary image
 * @param decoder the decoder where this function belongs
 * @param dsc pointer to decoder descriptor. `src`, `style` are already initialized in it.
 * @return LV_RESULT_OK: the info is successfully stored in `header`; LV_RESULT_INVALID: unknown format or other error.
 */
lv_result_t lv_bin_decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
void lv_bin_decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

/**
 * Decompress an RLE/LZ4 compressed bin image into a new draw buf.
 * The data layout is the same as the uncompressed image (palette first for indexed formats).
 * @param src       pointer to an `lv_image_dsc_t` variable or a file path
 * @param src_type  type of `src`
 * @param header    the image header as stored in the source, with `LV_IMAGE_FLAGS_COMPRESSED` and a valid `stride`
 * @return          the decompressed draw buf to be freed by `lv_draw_buf_destroy`, or NULL on failure
 */
lv_draw_buf_t * lv_bin_decoder_decompress(const void * src, lv_image_src_t src_type, const lv_image_header_t * header);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BIN_DECODER_H*/
