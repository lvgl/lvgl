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
#include "lv_draw_gles_utils.h"
#include "lv_draw_gles_priv.h"

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

lv_res_t lv_draw_gles_img_core(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                              const lv_area_t * coords, const void * src)
{
    LV_LOG_INFO("");

}

void lv_draw_gles_draw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                             uint32_t letter)
{
    LV_LOG_INFO("");

}

void lv_draw_gles_draw_line(lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc, const lv_point_t * point1,
                           const lv_point_t * point2)
{
    LV_LOG_INFO("");

}

void lv_draw_gles_draw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                          uint16_t radius, uint16_t start_angle, uint16_t end_angle)
{
    LV_LOG_INFO("");

}

void lv_draw_gles_polygon(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t * points,
                         uint16_t point_cnt)
{
    LV_LOG_INFO("");

}


void lv_draw_gles_draw_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    LV_LOG_INFO("");

}

void lv_draw_gles_init_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)
{
    _lv_draw_gles_utils_init();
    lv_memset_00(draw_ctx, sizeof(lv_draw_gles_ctx_t));
    draw_ctx->draw_rect = lv_draw_gles_draw_rect;
    draw_ctx->draw_img = lv_draw_gles_img_core;
    draw_ctx->draw_letter = lv_draw_gles_draw_letter;
    draw_ctx->draw_line = lv_draw_gles_draw_line;
    draw_ctx->draw_arc = lv_draw_gles_draw_arc;
    draw_ctx->draw_polygon = lv_draw_gles_polygon;
    draw_ctx->draw_bg = lv_draw_gles_draw_bg;
    lv_draw_gles_ctx_t * draw_ctx_gles = (lv_draw_gles_ctx_t *) draw_ctx;
    draw_ctx_gles->framebuffer = (GLuint*)disp_drv->user_data;
    draw_ctx_gles->internals = lv_mem_alloc(sizeof(lv_draw_gles_context_internals_t));
    lv_memset_00(draw_ctx_gles->internals, sizeof(lv_draw_gles_context_internals_t));
    lv_draw_gles_utils_internals_init(draw_ctx_gles->internals);
}

void lv_draw_gles_deinit_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)
{

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_GLES*/
