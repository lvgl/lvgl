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
#include "lv_img_decoder.h"
#include "lv_img_buf.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_color_t chroma_key_color;
    lv_color_t alpha_color;
    const lv_color32_t * palette;
    uint32_t palette_size   : 9;
    uint32_t chroma_keyed   : 1;
} lv_draw_img_sup_t;

typedef struct {

    int16_t angle;
    uint16_t zoom;
    lv_point_t pivot;

    lv_color_t chroma_key_color;
    lv_color_t recolor;
    lv_opa_t recolor_opa;

    lv_opa_t opa;
    lv_blend_mode_t blend_mode : 4;

    int32_t frame_id;
    uint16_t antialias      : 1;
    lv_draw_img_sup_t * sup;
} lv_draw_img_dsc_t;

struct _lv_draw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_img_dsc_init(lv_draw_img_dsc_t * dsc);

/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param dsc pointer to an initialized `lv_draw_img_dsc_t` variable
 */
void lv_draw_img(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords,
                 const void * src);


void lv_draw_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords,
                         const uint8_t * map_p, const lv_draw_img_sup_t * sup, lv_color_format_t color_format);

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. LV_SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 */
lv_img_src_t lv_img_src_get_type(const void * src);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IMG_H*/
