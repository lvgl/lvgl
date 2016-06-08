/**
 * @file lv_draw_vbasic.h
 * 
 */

#ifndef LV_DRAW_VBASIC_H
#define LV_DRAW_VBASIC_H

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"

#if LV_VDB_SIZE != 0

#include "misc/others/color.h"
#include "../lv_misc/2d.h"
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
void lv_vfill(const area_t * cords_p, const area_t * mask_p, 
                   color_t color, opa_t opa);

void lv_vletter(const point_t * pos_p, const area_t * mask_p, 
                     const font_t * font_p, uint8_t letter,
                     color_t color, opa_t opa);

void lv_vmap(const area_t * cords_p, const area_t * mask_p, 
             const color_t * map_p, opa_t opa, bool transp);


/**********************
 *      MACROS
 **********************/

#endif

#endif
