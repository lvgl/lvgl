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
    lv_color_t alpha_color;
    const lv_color32_t * palette;
    uint32_t palette_size   : 9;
} lv_draw_img_sup_t;

typedef struct _lv_draw_img_dsc_t {
    lv_draw_dsc_base_t base;

    const void * src;

    lv_coord_t angle;
    lv_coord_t zoom;
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

struct _lv_layer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_img_dsc_init(lv_draw_img_dsc_t * dsc);

/**
 * Draw an image
 * @param draw_ctx      pointer to the current draw context
 * @param dsc           pointer to an initialized `lv_draw_img_dsc_t` variable
 * @param coords        the coordinates of the image
 * @param src           pointer to a lv_color_t array which contains the pixels of the image
 */
void lv_draw_img(struct _lv_layer_t * layer, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords);


void lv_draw_layer(struct _lv_layer_t * layer, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords);

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
