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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_img_t;

typedef struct {

	/* The first 8 bit is very important to distinguish the different source types.
	 * For more info see `lv_img_get_src_type()` in lv_img.c */
	uint32_t color_format      :5;    /*See: lv_img_px_format*/
	uint32_t always_zero :3;		  /*It the upper bits of the first byte*/

	uint32_t compression :2;

	uint32_t w:11;              /*Width of the image map*/
	uint32_t h:11;              /*Height of the image map*/
}lv_img_header_t;


typedef enum {
	LV_IMG_COMPRESSION_NONE,
	LV_IMG_COMPRESSION_RLE,			/*Run length encoded*/
	LV_IMG_COMPRESSION_RESERVED1,
	LV_IMG_COMPRESSION_RESERVED2
}lv_img_compression_t;

typedef enum {
    LV_IMG_FORMAT_UNKOWN = 0,

	LV_IMG_FORMAT_RAW,					/*Contains the file's as it is. Needs custom decoder function*/

	LV_IMG_FORMAT_TRUE_COLOR,			/*Color format and depth should match with LV_COLOR settings*/
	LV_IMG_FORMAT_TRUE_COLOR_ALPHA,		/*Same as `LV_IMG_FORMAT_TRUE_COLOR` but every pixel has an alpha byte*/
	LV_IMG_FORMAT_TRUE_COLOR_CHROMA_KEYED,	/*Same as `LV_IMG_FORMAT_TRUE_COLOR` but LV_COLOR_TRANSP pixels will be transparent*/

	LV_IMG_FORMAT_INDEXED_1BIT,			/*Can have 2 different colors in a palette (always chroma keyed)*/
	LV_IMG_FORMAT_INDEXED_2BIT,			/*Can have 4 different colors in a palette (always chroma keyed)*/
	LV_IMG_FORMAT_INDEXED_4BIT,			/*Can have 16 different colors in a palette (always chroma keyed)*/
	LV_IMG_FORMAT_INDEXED_8BIT,			/*Can have 256 different colors in a palette (always chroma keyed)*/

	LV_IMG_FORMAT_ALPHA_1BIT, 			/*Can have one color and it can be drawn or not*/
	LV_IMG_FORMAT_ALPHA_2BIT, 			/*Can have one color but 4 different alpha value*/
	LV_IMG_FORMAT_ALPHA_4BIT, 			/*Can have one color but 16 different alpha value*/
	LV_IMG_FORMAT_ALPHA_8BIT, 			/*Can have one color but 256 different alpha value*/

	LV_IMG_FORMAT_INDEXED_ALPHA_2BIT, 			/*Can have 2 different colors which are drawn or not*/
	LV_IMG_FORMAT_INDEXED_ALPHA_4BIT, 			/*Can have 4 different colors but 4 different alpha value*/
	LV_IMG_FORMAT_INDEXED_ALPHA_8BIT, 			/*Can have 16 different colors but 16 different alpha value*/
	LV_IMG_FORMAT_INDEXED_ALPHA_16BIT, 			/*Can have 256 different colors but 256 different alpha value*/
} lv_img_color_format_t;

/* Image header it is compatible with
 * the result image converter utility*/
typedef struct
{
	lv_img_header_t header;
    const uint8_t * data;
} lv_img_dsc_t;

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



bool lv_img_dsc_get_info(const char * src, lv_img_header_t * header, const lv_style_t * style);

uint8_t lv_img_color_format_get_px_size(lv_img_color_format_t cf);

bool lv_img_color_format_is_chroma_key(lv_img_color_format_t cf);

bool lv_img_color_format_has_alpha(lv_img_color_format_t cf);

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKOWN
 */
lv_img_src_t lv_img_get_src_type(const void * src);


/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/
