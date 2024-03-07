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
#include "../../draw/lv_image_decoder.h"

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
 * @param src the image source: pointer to an `lv_image_dsc_t` variable, a file path or a symbol
 * @param header store the image data here
 * @return LV_RESULT_OK: the info is successfully stored in `header`; LV_RESULT_INVALID: unknown format or other error.
 */
lv_result_t lv_bin_decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);

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

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BIN_DECODER_H*/
