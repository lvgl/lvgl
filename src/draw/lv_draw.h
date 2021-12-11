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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void * user_data;
}lv_draw_mask_t;


typedef struct _lv_draw_t  {
    lv_color_t * dest_buf;          /**< pointer to a buffer to fill*/
    lv_area_t * dest_area;    /**< The the position and size of `dest_buf` (absolute coordinates)*/
    const lv_area_t * clip_area;

#if LV_USE_USER_DATA
    void * user_data;
#endif

    void (*draw_rect)(struct _lv_draw_t * draw, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

    void (*draw_arc)(struct _lv_draw_t * draw, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center, uint16_t radius,  uint16_t start_angle, uint16_t end_angle);

    void (*draw_img)(struct _lv_draw_t * draw, const lv_draw_img_dsc_t * dsc,
                     const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format);

    lv_res_t (*draw_img_core)(struct _lv_draw_t * draw, const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords, const void * src);

    void (*draw_letter)(struct _lv_draw_t * draw, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p, uint32_t letter);


    void (*draw_line)(struct _lv_draw_t * draw, const lv_draw_line_dsc_t * dsc, const lv_point_t * point1, const lv_point_t * point2);


    void (*draw_polygon)(struct _lv_draw_t * draw, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t points[], uint16_t point_cnt);

    lv_draw_mask_t * (*add_rect_mask)(struct _lv_draw_t * draw, const lv_area_t * coords, lv_coord_t * radius);
    void (*remove_rect_mask)(struct _lv_draw_t * draw, lv_draw_mask_t * mask);

    /**
     * Wait until all background operation are finished. (E.g. GPU opertions)
     */
    void (*wait_for_finish)(struct _lv_draw_t * draw);

} lv_draw_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_init(void);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/
#include "sw/lv_draw_sw.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_H*/
