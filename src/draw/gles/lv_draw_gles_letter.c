/**
 * @file lv_draw_gles_letter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES


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
/**
* @file lv_draw_sdl_rect.c
*
*/

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"


#if LV_USE_GPU_SDL_GLES

#include "../lv_draw.h"



#include LV_GPU_SDL_GLES_GLAD_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

void lv_draw_sw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                       uint32_t letter);

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_gles_draw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                              uint32_t letter)
{
    lv_draw_sw_letter(draw_ctx, dsc, pos_p, letter);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL_GLES*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL_GLES*/
