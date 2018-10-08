/**
 * @file lv_draw_img.h
 *
 */

#ifndef LV_DRAW_IMG_H
#define LV_DRAW_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define LV_IMG_DECODER_OPEN_FAIL    ((void*)(-1))

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_img_t;

typedef struct {

    /* The first 8 bit is very important to distinguish the different source types.
     * For more info see `lv_img_get_src_type()` in lv_img.c */
    uint32_t cf           :5;    /* Color format: See `lv_img_color_format_t`*/
    uint32_t always_zero  :3;    /*It the upper bits of the first byte. Always zero to look like a non-printable character*/

    uint32_t reserved     :2;   /*Reserved to be used later*/

    uint32_t w:11;              /*Width of the image map*/
    uint32_t h:11;              /*Height of the image map*/
} lv_img_header_t;

/*Image color format*/
enum {
    LV_IMG_CF_UNKOWN = 0,

    LV_IMG_CF_RAW,                  /*Contains the file as it is. Needs custom decoder function*/
    LV_IMG_CF_RAW_ALPHA,            /*Contains the file as it is. The image has alpha. Needs custom decoder function*/
    LV_IMG_CF_RAW_CHROMA_KEYED,     /*Contains the file as it is. The image is chroma keyed. Needs custom decoder function*/

    LV_IMG_CF_TRUE_COLOR,           /*Color format and depth should match with LV_COLOR settings*/
    LV_IMG_CF_TRUE_COLOR_ALPHA,     /*Same as `LV_IMG_CF_TRUE_COLOR` but every pixel has an alpha byte*/
    LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,  /*Same as `LV_IMG_CF_TRUE_COLOR` but LV_COLOR_TRANSP pixels will be transparent*/

    LV_IMG_CF_INDEXED_1BIT,         /*Can have 2 different colors in a palette (always chroma keyed)*/
    LV_IMG_CF_INDEXED_2BIT,         /*Can have 4 different colors in a palette (always chroma keyed)*/
    LV_IMG_CF_INDEXED_4BIT,         /*Can have 16 different colors in a palette (always chroma keyed)*/
    LV_IMG_CF_INDEXED_8BIT,         /*Can have 256 different colors in a palette (always chroma keyed)*/

    LV_IMG_CF_ALPHA_1BIT,           /*Can have one color and it can be drawn or not*/
    LV_IMG_CF_ALPHA_2BIT,           /*Can have one color but 4 different alpha value*/
    LV_IMG_CF_ALPHA_4BIT,           /*Can have one color but 16 different alpha value*/
    LV_IMG_CF_ALPHA_8BIT,           /*Can have one color but 256 different alpha value*/
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


/**
 * Get info from an image and store in the `header`
 * @param src the image source. Can be a pointer to a C array or a file name (Use `lv_img_src_get_type` to determine the type)
 * @param header store the info here
 * @return LV_RES_OK: info written correctly; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_info_f_t)(const void * src, lv_img_header_t * header);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param src the image source. Can be a pointer to a C array or a file name (Use `lv_img_src_get_type` to determine the type)
 * @param style the style of image (maybe it will be required to determine a color or something)
 * @return there are 3 possible return values:
 *    1) buffer with the decoded image
 *    2) if can decode the whole image NULL. decoder_read_line will be called to read the image line-by-line
 *    3) LV_IMG_DECODER_OPEN_FAIL if the image format is unknown to the decoder or an error occurred
 */
typedef const uint8_t * (*lv_img_decoder_open_f_t)(const void * src, const lv_style_t * style);

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param x start x coordinate
 * @param y startt y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RES_OK: ok; LV_RES_INV: failed
 */
typedef lv_res_t (*lv_img_decoder_read_line_f_t)(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

/**
 * Close the pending decoding. Free resources etc.
 */
typedef void (*lv_img_decoder_close_f_t)(void);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param style style of the image
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask,
                 const void * src, const lv_style_t * style, lv_opa_t opa_scale);


/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKOWN
 */
lv_img_src_t lv_img_src_get_type(const void * src);

/**
 * Set custom decoder functions. See the typdefs of the function typed above for more info about them
 * @param info_fp info get function
 * @param open_fp open function
 * @param read_fp read line function
 * @param close_fp clode function
 */
void lv_img_decoder_set_custom(lv_img_decoder_info_f_t  info_fp, lv_img_decoder_open_f_t  open_fp,
                               lv_img_decoder_read_line_f_t read_fp, lv_img_decoder_close_f_t close_fp);

lv_res_t lv_img_dsc_get_info(const char * src, lv_img_header_t * header);

uint8_t lv_img_color_format_get_px_size(lv_img_cf_t cf);

bool lv_img_color_format_is_chroma_keyed(lv_img_cf_t cf);

bool lv_img_color_format_has_alpha(lv_img_cf_t cf);


/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/
