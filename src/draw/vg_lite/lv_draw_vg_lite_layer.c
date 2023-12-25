/**
 * @file lv_draw_vg_lite_layer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include "lv_draw_vg_lite_type.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vg_lite_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                           const lv_area_t * coords)
{
    lv_layer_t * layer = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if(layer->buf == NULL)
        return;

    lv_image_dsc_t img_dsc;
    lv_memzero(&img_dsc, sizeof(lv_image_dsc_t));
    img_dsc.header.w = lv_area_get_width(&layer->buf_area);
    img_dsc.header.h = lv_area_get_height(&layer->buf_area);
    img_dsc.header.cf = layer->color_format;
    img_dsc.data = layer->buf;

    /* The GPU output is premultiplied RGB */
    img_dsc.header.flags = LV_IMAGE_FLAGS_PREMULTIPLIED;

    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = &img_dsc;

    lv_draw_vg_lite_img(draw_unit, &new_draw_dsc, coords);
    lv_image_cache_drop(&img_dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
