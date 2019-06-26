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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include "../lv_core/lv_style.h"
#include "../lv_misc/lv_txt.h"
#include "lv_img_decoder.h"

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
 * Give a buffer with the given to use during drawing.
 * Be careful to not use the buffer while other processes are using it.
 * @param size the required size
 */
void * lv_draw_get_buf(uint32_t size);

/**
 * Free the draw buffer
 */
void lv_draw_free_buf(void);

#if LV_ANTIALIAS

/**
 * Get the opacity of a pixel based it's position in a line segment
 * @param seg segment length
 * @param px_id position of  of a pixel which opacity should be get [0..seg-1]
 * @param base_opa the base opacity
 * @return the opacity of the given pixel
 */
lv_opa_t lv_draw_aa_get_opa(lv_coord_t seg, lv_coord_t px_id, lv_opa_t base_opa);

/**
 * Add a vertical  anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void lv_draw_aa_ver_seg(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color,
                        lv_opa_t opa);

/**
 * Add a horizontal anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void lv_draw_aa_hor_seg(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color,
                        lv_opa_t opa);
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/
#include "lv_draw_basic.h"
#include "lv_draw_rect.h"
#include "lv_draw_label.h"
#include "lv_draw_img.h"
#include "lv_draw_line.h"
#include "lv_draw_triangle.h"
#include "lv_draw_arc.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_H*/
