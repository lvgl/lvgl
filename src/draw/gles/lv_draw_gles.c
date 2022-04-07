/**
 * @file lv_draw_gles.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

#include "lv_draw_gles.h"
#include <src/draw/sw/lv_draw_sw.h>
#include "lv_draw_gles_utils.h"
#include "lv_draw_gles_priv.h"
#include "lv_draw_gles_texture_cache.h"

/*********************
 *      DEFINES
 *********************/
void lv_draw_gles_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_gles_draw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                             uint32_t letter);

lv_res_t lv_draw_gles_draw_img(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                               const lv_area_t * coords, const void * src);

void lv_draw_gles_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                         const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format);
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
    lv_draw_gles_ctx_t * draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_memset_00(draw_ctx, sizeof(lv_draw_gles_ctx_t));

    draw_gles_ctx->internals = lv_mem_alloc(sizeof(lv_draw_gles_context_internals_t));
    /* TODO(tan): maybe use the standard memset? */
    lv_memset_00(draw_gles_ctx->internals, sizeof(lv_draw_gles_context_internals_t));
    draw_gles_ctx->internals->hor = disp_drv->hor_res;
    draw_gles_ctx->internals->ver = disp_drv->ver_res;

    lv_draw_gles_utils_internals_init(draw_gles_ctx->internals);
#if !LV_USE_GPU_GLES_SW_MIXED
    draw_gles_ctx->internals->framebuffer = *(GLuint*)disp_drv->user_data;
#endif /* !LV_USE_GPU_GLES_SW_MIXED */
    draw_gles_ctx->base_draw.draw_rect = lv_draw_gles_draw_rect;
    draw_gles_ctx->base_draw.draw_img = lv_draw_gles_draw_img;
    draw_gles_ctx->base_draw.draw_letter = lv_draw_gles_draw_letter;
    draw_gles_ctx->base_draw.draw_line = lv_draw_gles_draw_line;
    draw_gles_ctx->base_draw.draw_arc = lv_draw_gles_draw_arc;
    draw_gles_ctx->base_draw.draw_polygon = lv_draw_gles_polygon;
    draw_gles_ctx->base_draw.draw_bg = lv_draw_gles_draw_bg;
    lv_draw_gles_texture_cache_init(draw_gles_ctx);
#if LV_USE_GPU_GLES_SW_MIXED
    draw_gles_ctx->blend = lv_draw_sw_blend_basic;
#endif /* LV_USE_GPU_GLES_SW_MIXED */

}

void lv_draw_gles_deinit_ctx(lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx)
{

    /*TODO: deinit texture cache */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_GLES*/
