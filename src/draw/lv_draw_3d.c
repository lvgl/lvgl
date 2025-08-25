/**
 * @file lv_draw_3d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_3d.h"
#if LV_USE_3DTEXTURE

#include "lv_draw_private.h"

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

void lv_draw_3d_dsc_init(lv_draw_3d_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_3d_dsc_t));
    dsc->base.dsc_size = sizeof(lv_draw_3d_dsc_t);
    dsc->tex_id = LV_3DTEXTURE_ID_NULL;
    dsc->h_flip = false;
    dsc->v_flip = false;
    dsc->opa = LV_OPA_COVER;
}

lv_draw_3d_dsc_t * lv_draw_task_get_3d_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_3D ? (lv_draw_3d_dsc_t *)task->draw_dsc : NULL;
}

void lv_draw_3d(lv_layer_t * layer, const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_draw_task_t * t = lv_draw_add_task(layer, coords, LV_DRAW_TASK_TYPE_3D);

    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));

    lv_draw_finalize_task_creation(layer, t);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_3DTEXTURE*/
