/**
 * @file lv_draw_img.h
 * 
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

/*********************
 *      INCLUDES
 *********************/
#include "../lv_objx/lv_btn.h"
#include "../lv_objx/lv_rect.h"
#include "../lv_objx/lv_line.h"
#include "../lv_objx/lv_img.h"
#include "../lv_objx/lv_label.h"

#include "misc_conf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/ 

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_draw_rect(const area_t * cords_p, const area_t * mask_p,
                  const lv_rects_t * rects_p, opa_t opa);
void lv_draw_label(const area_t * cords_p,const area_t * mask_p,
                   const lv_labels_t * labels_p, opa_t opa, const char * txt);
void lv_draw_line(const point_t * p1, const point_t * p2, const area_t * mask_p, 
                  const lv_lines_t * lines_p, opa_t opa);

#if USE_LV_IMG != 0 && USE_FSINT != 0 && USE_UFS != 0
void lv_draw_img(const area_t * cords_p, const area_t * mask_p, 
                 const lv_imgs_t * imgs_p,  opa_t opa, const char * fn);
#endif
/**********************
 *      MACROS
 **********************/

#endif
