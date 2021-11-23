/**
 * @file lv_draw.h
 *
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_style.h"
#include "../misc/lv_txt.h"
#include "lv_img_decoder.h"
#include "lv_img_cache.h"

#include "lv_draw_rect.h"
#include "lv_draw_label.h"
#include "lv_draw_img.h"
#include "lv_draw_line.h"
#include "lv_draw_triangle.h"
#include "lv_draw_arc.h"
#include "lv_draw_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_draw_backend_t{
    struct _lv_draw_backend_t * base;

    void * ctx;

    void (*draw_rect)(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);

    void (*draw_arc)(lv_coord_t center_x, lv_coord_t center_y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle,
                     const lv_area_t * clip_area, const lv_draw_arc_dsc_t * dsc);

    void (*draw_img)(const lv_area_t * map_area, const lv_area_t * clip_area,
                                                  const uint8_t * map_p,
                                                  const lv_draw_img_dsc_t * draw_dsc,
                                                  bool chroma_key, bool alpha_byte);

   void (*draw_letter)(const lv_point_t * pos_p, const lv_area_t * clip_area,
                       const lv_font_t * font_p, uint32_t letter,
                       lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);


   void (*draw_line)(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
                                           const lv_draw_line_dsc_t * dsc);


   void (*draw_polygon)(const lv_point_t points[], uint16_t point_cnt, const lv_area_t * clip_area,
                        const lv_draw_rect_dsc_t * draw_dsc);


   /**
    * Fill an arae of a buffer with a color
    * @param dest_buf       pointer to a buffer to fill
    * @param dest_stride    stride of `dest_buf` (number of pixel in a line)
    * @param fill_area      the area to fill on `dest_buf`
    * @param color          fill color
    * @param mask           NULL if ignored, or an alpha mask to apply on `fill_area`
    * @param opa            overall opacity
    * @param blend_mode     e.g. LV_BLEND_MODE_ADDITIVE
    */
   void (*blend_fill)(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                      lv_color_t color, lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode);

   /**
    * Blend a source buffer to a destination buffer
    * @param dest_buf       pointer to the destination buffer
    * @param dest_stride    stride of `dest_buf` (number of pixel in a line)
    * @param clip_area      clip the blending to this area
    * @param src_buf        pointer to the destination buffer
    * @param src_area       coordinates of the `src_buf` relative to the `dest_buf`
    * @param mask           NULL if ignored, or an alpha mask to apply on `clip_area` (it's size is equal to the `clip_area`)
    * @param opa            overall opacity
    * @param blend_mode     e.g. LV_BLEND_MODE_ADDITIVE
    */
   void (*blend_map)(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * clip_area,
                     const lv_color_t * src_buf, const lv_area_t * src_area,
                     lv_opa_t * mask, lv_opa_t opa, lv_blend_mode_t blend_mode);

}lv_draw_backend_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_init(void);

void lv_draw_backend_init(lv_draw_backend_t * backend);

void lv_draw_backend_add(lv_draw_backend_t * backend);

const lv_draw_backend_t * lv_draw_backend_get(void);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
