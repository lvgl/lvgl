/**
 * @file lv_draw_rbasic..h
 * 
 */

#ifndef LV_DRAW_RBASIC_H
#define LV_DRAW_RBASIC_H

/*********************
 *      INCLUDES
 *********************/
#include "misc/others/color.h"
#include "../lv_misc/area.h"
#include "../lv_misc/font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_rfill(const area_t * area_p, const area_t * mask_p, 
                   color_t color, opa_t opa);
void lv_rletter(const point_t * pos_p, const area_t * mask_p, 
                     const font_t * font_p, uint8_t letter,
                     color_t color, opa_t opa);
void lv_rmap(const area_t * cords_p, const area_t * mask_p,
             const color_t * map_p, opa_t opa, bool transp,
			 color_t recolor, opa_t recolor_opa);
/**********************
 *      MACROS
 **********************/

#endif
