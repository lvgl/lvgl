/**
 * @file lv_draw_gles.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include "lv_draw_gles.h"

/*********************
 *      DEFINES
 *********************/
void lv_draw_gles_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

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

void lv_draw_gles_init_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)
{
    draw_ctx->draw_rect = lv_draw_gles_draw_rect;
}

void lv_draw_gles_deinit_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)
{

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_GLES*/
