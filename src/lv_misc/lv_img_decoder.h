/**
 * @file lv_img_decoder.h
 *
 */

#ifndef LV_IMG_DEOCER_H
#define LV_IMG_DEOCER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdint.h>
#include "lv_fs.h"

/*********************
 *      DEFINES
 *********************/
#define LV_IMG_DECODER_OPEN_FAIL ((void *)(-1));

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{

    /* The first 8 bit is very important to distinguish the different source types.
     * For more info see `lv_img_get_src_type()` in lv_img.c */
    uint32_t cf : 5;          /* Color format: See `lv_img_color_format_t`*/
    uint32_t always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/

    uint32_t reserved : 2; /*Reserved to be used later*/

    uint32_t w : 11; /*Width of the image map*/
    uint32_t h : 11; /*Height of the image map*/
} lv_img_header_t;

/*Image color format*/
enum {
    LV_IMG_CF_UNKNOWN = 0,

    LV_IMG_CF_RAW,       /*Contains the file as it is. Needs custom decoder function*/
    LV_IMG_CF_RAW_ALPHA, /*Contains the file as it is. The image has alpha. Needs custom decoder
                            function*/
    LV_IMG_CF_RAW_CHROMA_KEYED, /*Contains the file as it is. The image is chroma keyed. Needs
                                   custom decoder function*/

    LV_IMG_CF_TRUE_COLOR,       /*Color format and depth should match with LV_COLOR settings*/
    LV_IMG_CF_TRUE_COLOR_ALPHA, /*Same as `LV_IMG_CF_TRUE_COLOR` but every pixel has an alpha byte*/
    LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED, /*Same as `LV_IMG_CF_TRUE_COLOR` but LV_COLOR_TRANSP pixels
                                          will be transparent*/

    LV_IMG_CF_INDEXED_1BIT, /*Can have 2 different colors in a palette (always chroma keyed)*/
    LV_IMG_CF_INDEXED_2BIT, /*Can have 4 different colors in a palette (always chroma keyed)*/
    LV_IMG_CF_INDEXED_4BIT, /*Can have 16 different colors in a palette (always chroma keyed)*/
    LV_IMG_CF_INDEXED_8BIT, /*Can have 256 different colors in a palette (always chroma keyed)*/

    LV_IMG_CF_ALPHA_1BIT, /*Can have one color and it can be drawn or not*/
    LV_IMG_CF_ALPHA_2BIT, /*Can have one color but 4 different alpha value*/
    LV_IMG_CF_ALPHA_4BIT, /*Can have one color but 16 different alpha value*/
    LV_IMG_CF_ALPHA_8BIT, /*Can have one color but 256 different alpha value*/
};
typedef uint8_t lv_img_cf_t;

/* Image header it is compatible with
 * the result image converter utility*/
typedef struct
{
    lv_img_header_t header;
    uint32_t data_size;
    const uint8_t * data;
} lv_img_dsc_t;

/* Decoder function definitions */

struct _lv_img_decoder;
struct _lv_img_decoder_dsc;

/**
 * Get info from an image and store in the `header`
 * @param src the image source. Can be a pointer to a C array or a file name (Use
 * `lv_img_src_get_type` to determine the type)
 * @param header store the info here
 * @return LV_RES_OK: info written correctly; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_info_f_t)(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param src the image source. Can be a pointer to a C array or a file name (Use
 * `lv_img_src_get_type` to determine the type)
 * @param style the style of image (maybe it will be required to determine a color or something)
 * @return there are 3 possible return values:
 *    1) buffer with the decoded image
 *    2) if can decode the whole image NULL. decoder_read_line will be called to read the image
 * line-by-line
 *    3) LV_IMG_DECODER_OPEN_FAIL if the image format is unknown to the decoder or an
 * error occurred
 */
typedef const uint8_t * (*lv_img_decoder_open_f_t)(struct _lv_img_decoder * decoder, struct _lv_img_decoder_dsc * dsc);

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RES_OK: ok; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_read_line_f_t)(struct _lv_img_decoder * decoder, struct _lv_img_decoder_dsc * dsc,
                                                 lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

/**
 * Close the pending decoding. Free resources etc.
 */
typedef void (*lv_img_decoder_close_f_t)(struct _lv_img_decoder * decoder, struct _lv_img_decoder_dsc * dsc);

typedef struct _lv_img_decoder_dsc {
    lv_img_decoder_info_f_t info_cb;
    lv_img_decoder_open_f_t open_cb;
    lv_img_decoder_read_line_f_t read_line_cb;
    lv_img_decoder_close_f_t close_cb;

#if LV_USE_USER_DATA_SINGLE
    lv_img_decoder_user_data_t user_data;
#endif
}lv_img_decoder_t;


typedef struct {
    lv_img_decoder_t * decoder;
    const lv_style_t * style;
    const void * src;
    lv_img_src_t src_type;
    lv_img_header_t header;

#if LV_USE_USER_DATA_SINGLE
    void * user_data;
#endif
}lv_img_decoder_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/
