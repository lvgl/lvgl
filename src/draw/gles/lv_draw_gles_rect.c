/**
 * @file lv_draw_gles_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include "lv_draw_gles_priv.h"
#include "lv_draw_gles_blend.h"
#include "lv_draw_gles.h"
#include "../lv_draw_rect.h"
#include "../lv_draw_img.h"
#include "../lv_draw_label.h"
#include "../../core/lv_refr.h"


#include LV_GPU_SDL_GLES_GLAD_INCLUDE_PATH

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
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_gles_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_sw_rect(draw_ctx, dsc, coords);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL_GLES*/
