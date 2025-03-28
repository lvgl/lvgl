/**
 * @file lv_draw_3d.h
 *
 */

#ifndef LV_DRAW_3D_H
#define LV_DRAW_3D_H

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

typedef struct {
    lv_draw_dsc_base_t base;
    lv_3dtexture_id_t tex_id;
    lv_opa_t opa;
} lv_draw_3d_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_3d_dsc_init(lv_draw_3d_dsc_t * dsc);
lv_draw_3d_dsc_t * lv_draw_task_get_3d_dsc(lv_draw_task_t * task);
void lv_draw_3d(lv_layer_t * layer, const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_3D_H*/
