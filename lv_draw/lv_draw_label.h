/**
 * @file lv_draw_label.h
 *
 */

#ifndef LV_DRAW_LABEL_H
#define LV_DRAW_LABEL_H

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Write a text
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 * @param txt 0 terminated text to write
 * @param flag settings for the text from 'txt_flag_t' enum
 * @param offset text offset in x and y direction (NULL if unused)
 *
 */
void lv_draw_label(const lv_area_t * coords,const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale,
                   const char * txt, lv_txt_flag_t flag, lv_point_t * offset);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_LABEL_H*/
