/**
 * @file lv_rle.h
 *
 */

#ifndef LV_RLE_H
#define LV_RLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_RLE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

uint32_t lv_rle_decompress(const uint8_t * input,
                           uint32_t input_buff_len, uint8_t * output,
                           uint32_t output_buff_len, uint8_t blk_size);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLE_H*/
