/**
 * @file lv_draw_dma2d_private.h
 *
 */

#ifndef LV_DRAW_DMA2D_PRIVATE_H
#define LV_DRAW_DMA2D_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_dma2d.h"
#if LV_USE_DRAW_DMA2D

#include "../lv_draw_private.h"
#include "../sw/lv_draw_sw.h"
#include LV_DRAW_DMA2D_HAL_INCLUDE

/*********************
 *      DEFINES
 *********************/

#if LV_USE_DRAW_DMA2D_INTERRUPT && LV_USE_OS
#define LV_DRAW_DMA2D_ASYNC 1
#else
#define LV_DRAW_DMA2D_ASYNC 0
#endif

#if defined(__CORTEX_M) && ((__CORTEX_M == 7) || (__CORTEX_M == 55))
#define LV_DRAW_DMA2D_CACHE 1
#else
#define LV_DRAW_DMA2D_CACHE 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DRAW_DMA2D_OUTPUT_CF_ARGB8888 = 0,
    LV_DRAW_DMA2D_OUTPUT_CF_RGB888,
    LV_DRAW_DMA2D_OUTPUT_CF_RGB565,
    LV_DRAW_DMA2D_OUTPUT_CF_ARGB1555,
    LV_DRAW_DMA2D_OUTPUT_CF_ARGB4444
} lv_draw_dma2d_output_cf_t;

typedef enum {
    LV_DRAW_DMA2D_FGBG_CF_ARGB8888 = 0,
    LV_DRAW_DMA2D_FGBG_CF_RGB888,
    LV_DRAW_DMA2D_FGBG_CF_RGB565,
    LV_DRAW_DMA2D_FGBG_CF_ARGB1555,
    LV_DRAW_DMA2D_FGBG_CF_ARGB4444,
    LV_DRAW_DMA2D_FGBG_CF_L8,
    LV_DRAW_DMA2D_FGBG_CF_AL44,
    LV_DRAW_DMA2D_FGBG_CF_AL88,
    LV_DRAW_DMA2D_FGBG_CF_L4,
    LV_DRAW_DMA2D_FGBG_CF_A8,
    LV_DRAW_DMA2D_FGBG_CF_A4,
    LV_DRAW_DMA2D_FGBG_CF_YCBCR
} lv_draw_dma2d_fgbg_cf_t;

typedef enum {
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY = 0,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_PFC,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING,
    LV_DRAW_DMA2D_MODE_REGISTER_TO_MEMORY,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIXED_COLOR_FG,
    LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING_AND_FIXED_COLOR_BG
} lv_draw_dma2d_mode_t;

typedef enum {
    LV_DRAW_DMA2D_ALPHA_MODE_NO_MODIFY_IMAGE_ALPHA_CHANNEL = 0,
    LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL,
    LV_DRAW_DMA2D_ALPHA_MODE_MULTIPLY_IMAGE_ALPHA_CHANNEL
} lv_draw_dma2d_alpha_mode_t;

typedef struct {
    lv_draw_dma2d_mode_t mode;
    uint32_t w;
    uint32_t h;

    void * output_address;
    uint32_t output_offset;
    lv_draw_dma2d_output_cf_t output_cf;

    uint32_t reg_to_mem_mode_color;

    const void * fg_address;
    uint32_t fg_offset;
    lv_draw_dma2d_fgbg_cf_t fg_cf;
    uint32_t fg_color;
    uint32_t fg_alpha_mode;
    uint32_t fg_alpha;

    const void * bg_address;
    uint32_t bg_offset;
    lv_draw_dma2d_fgbg_cf_t bg_cf;
    uint32_t bg_color;
    uint32_t bg_alpha_mode;
    uint32_t bg_alpha;

} lv_draw_dma2d_configuration_t;

typedef struct {
    const void * first_byte;
    uint32_t width_bytes;
    uint32_t height;
    uint32_t stride;
} lv_draw_dma2d_cache_area_t;

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * volatile task_act;
#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t writing_area;
#endif
#if LV_DRAW_DMA2D_ASYNC
    lv_thread_t thread;
    lv_thread_sync_t interrupt_signal;
#endif
} lv_draw_dma2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_dma2d_opaque_fill(lv_draw_task_t * t, void * first_pixel, int32_t w, int32_t h, int32_t stride);
void lv_draw_dma2d_fill(lv_draw_task_t * t, void * first_pixel, int32_t w, int32_t h, int32_t stride);
void lv_draw_dma2d_opaque_image(lv_draw_task_t * t, void * dest_first_pixel, lv_area_t * clipped_coords,
                                int32_t dest_stride);
void lv_draw_dma2d_image(lv_draw_task_t * t, void * dest_first_pixel, lv_area_t * clipped_coords,
                         int32_t dest_stride);
lv_draw_dma2d_output_cf_t lv_draw_dma2d_cf_to_dma2d_output_cf(lv_color_format_t cf);
uint32_t lv_draw_dma2d_color_to_dma2d_color(lv_draw_dma2d_output_cf_t cf, lv_color_t color);
void lv_draw_dma2d_configure_and_start_transfer(const lv_draw_dma2d_configuration_t * conf);
#if LV_DRAW_DMA2D_CACHE
void lv_draw_dma2d_invalidate_cache(const lv_draw_dma2d_cache_area_t * mem_area);
void lv_draw_dma2d_clean_cache(const lv_draw_dma2d_cache_area_t * mem_area);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_DMA2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_DMA2D_PRIVATE_H*/
