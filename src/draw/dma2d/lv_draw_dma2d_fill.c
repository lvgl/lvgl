/**
 * @file lv_draw_dma2d_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_dma2d_private.h"
#if LV_USE_DRAW_DMA2D

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

void lv_draw_dma2d_opaque_fill(lv_draw_dma2d_unit_t * u, void * first_pixel, int32_t w, int32_t h, int32_t stride)
{
    lv_draw_fill_dsc_t * dsc = u->task_act->draw_dsc;
    lv_color_format_t cf = dsc->base.layer->color_format;

    lv_draw_dma2d_output_cf_t output_cf = lv_draw_dma2d_cf_to_dma2d_output_cf(cf);
    uint32_t cf_size = LV_COLOR_FORMAT_GET_SIZE(cf);
    uint32_t reg_to_mem_color = lv_draw_dma2d_color_to_dma2d_color(output_cf, dsc->color);

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t cache_area = {
        .first_byte = first_pixel,
        .width_bytes = w * cf_size,
        .height = h,
        .stride = stride
    };
    lv_memcpy(&u->writing_area, &cache_area, sizeof(lv_draw_dma2d_cache_area_t));
#endif

    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_REGISTER_TO_MEMORY,
        .w = w,
        .h = h,

        .output_address = first_pixel,
        .output_offset = (stride / cf_size) - w,
        .output_cf = output_cf,

        .reg_to_mem_mode_color = reg_to_mem_color
    };
    lv_draw_dma2d_configure_and_start_transfer(&conf);
}

void lv_draw_dma2d_fill(lv_draw_dma2d_unit_t * u, void * first_pixel, int32_t w, int32_t h, int32_t stride)
{
    lv_draw_fill_dsc_t * dsc = u->task_act->draw_dsc;
    lv_color_t color = dsc->color;
    lv_color_format_t cf = dsc->base.layer->color_format;
    lv_opa_t opa = dsc->opa;

    lv_draw_dma2d_output_cf_t output_cf = lv_draw_dma2d_cf_to_dma2d_output_cf(cf);
    uint32_t cf_size = LV_COLOR_FORMAT_GET_SIZE(cf);

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t cache_area = {
        .first_byte = first_pixel,
        .width_bytes = w * cf_size,
        .height = h,
        .stride = stride
    };
    lv_memcpy(&u->writing_area, &cache_area, sizeof(lv_draw_dma2d_cache_area_t));

    /* make sure the background area DMA2D is blending is up-to-date in main memory */
    lv_draw_dma2d_clean_cache(&cache_area);
#endif

    uint32_t output_offset = (stride / cf_size) - w;
    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIXED_COLOR_FG,
        .w = w,
        .h = h,

        .output_address = first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf,

        .fg_color = lv_color_to_u32(color),
        .fg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL,
        .fg_alpha = opa,

        .bg_address = first_pixel,
        .bg_offset = output_offset,
        .bg_cf = (lv_draw_dma2d_fgbg_cf_t) output_cf
    };

    /* Background alpha channel should be treated as 0xFF if the cf is XRGB */
    if(cf == LV_COLOR_FORMAT_XRGB8888) {
        conf.bg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
        conf.bg_alpha = 0xff;
    }

    lv_draw_dma2d_configure_and_start_transfer(&conf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_DMA2D*/
