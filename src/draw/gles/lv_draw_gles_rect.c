/**
 * @file lv_draw_gles_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include "../lv_draw.h"
#include "lv_draw_gles_utils.h"
#include "lv_draw_gles.h"

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
void opengl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_gles_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    /* Do software drawing */
    lv_draw_sw_rect(draw_ctx, dsc, coords);

    lv_draw_gles_utils_upload_texture(draw_ctx);
    /* Do opengl drawing */
    opengl_draw_rect(draw_ctx, dsc, coords);
    lv_draw_gles_utils_download_texture(draw_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void opengl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{

    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);

    glClearColor(.2f, .3f, .2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

#endif /*LV_USE_GPU_SDL_GLES*/
