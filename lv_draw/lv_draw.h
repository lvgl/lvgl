/**
 * @file lv_draw.h
 * 
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#include "../lv_misc/text.h"
#include "../lv_obj/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#define LV_RECT_CIRCLE      ((cord_t)-1)    /*A very big radius to always draw as circle*/

/**********************
 *      TYPEDEFS
 **********************/ 

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Draw a rectangle
 * @param cords_p the coordinates of the rectangle
 * @param mask_p the rectangle will be drawn only in this mask
 * @param style_p pointer to a style
 */
#if USE_LV_RECT != 0
void lv_draw_rect(const area_t * cords_p, const area_t * mask_p, const lv_style_t * style_p);
#endif


/*Experimental use for 3D modeling*/
#define USE_LV_TRIANGLE 0
#if USE_LV_TRIANGLE != 0
/**
 *
 * @param points pointer to an array with 3 points
 * @param mask_p the triangle will be drawn only in this mask
 * @param color color of the triangle
 */
void lv_draw_triangle(const point_t * points, const area_t * mask_p, color_t color);
#endif

/**
 * Write a text
 * @param cords_p coordinates of the label
 * @param mask_p the label will be drawn only in this area
 * @param style_p pointer to a style
 * @param txt 0 terminated text to write
 * @param flags settings for the text from 'txt_flag_t' enum
 */
#if USE_LV_LABEL != 0
void lv_draw_label(const area_t * cords_p,const area_t * mask_p, const lv_style_t * style_p,
                    const char * txt, txt_flag_t flag);
#endif

/**
 * Draw an image
 * @param cords_p the coordinates of the image
 * @param mask_p the image will be drawn only in this area
 * @param map_p pointer to a color_t array which contains the pixels of the image
 */
#if USE_LV_IMG != 0 && USE_FSINT != 0 && USE_UFS != 0
void lv_draw_img(const area_t * cords_p, const area_t * mask_p,
                 const lv_style_t * style_p, const char * fn);
#endif

/**
 * Draw a line
 * @param p1 first point of the line
 * @param p2 second point of the line
 * @param mask_pthe line will be drawn only on this area
 * @param style_p pointer to a style
 */
#if USE_LV_LINE != 0
void lv_draw_line(const point_t * p1, const point_t * p2, const area_t * mask_p,
                  const lv_style_t * style_p);
#endif

/**********************
 *      MACROS
 **********************/

#endif
