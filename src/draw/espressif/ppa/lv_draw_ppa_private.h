/**
 * @file lv_draw_ppa_private.h
 *
 */

#ifndef LV_DRAW_PPA_PRIVATE_H
#define LV_DRAW_PPA_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
*      INCLUDES
*********************/
#include "../../../lvgl_public.h"

#if LV_USE_PPA
#if LV_PPA_NONBLOCKING_OPS
#error "PPA draw in nonblocking is experimental and not supported yet, please make it to 0!"
#endif

#ifndef LV_PPA_NONBLOCKING_OPS
#define LV_PPA_NONBLOCKING_OPS 0
#endif

#include "../../lv_draw_private.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_area_private.h"

/* The ppa driver depends heavily on the esp-idf headers*/
#include <sdkconfig.h>

#if defined(CONFIG_CACHE_L2_CACHE_LINE_SIZE)
#define LV_DRAW_PPA_CACHE_LINE_SIZE CONFIG_CACHE_L2_CACHE_LINE_SIZE
#elif defined(CONFIG_CACHE_L1_DCACHE_LINE_SIZE)
#define LV_DRAW_PPA_CACHE_LINE_SIZE CONFIG_CACHE_L1_DCACHE_LINE_SIZE
#else
#error "Cannot determine the data cache line size for the PPA draw unit"
#endif

#if (CONFIG_LV_DRAW_BUF_ALIGN != LV_DRAW_PPA_CACHE_LINE_SIZE)
#error "CONFIG_LV_DRAW_BUF_ALIGN must be equal to the data cache line size!"
#endif


#ifndef CONFIG_SOC_PPA_SUPPORTED
#error "This SoC does not support PPA"
#endif

#include <driver/ppa.h>
#include <esp_heap_caps.h>
#include <esp_err.h>
#include <hal/color_hal.h>
#include <esp_cache.h>
#include <esp_log.h>
/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/
typedef struct lv_draw_ppa_unit {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    ppa_client_handle_t srm_client;
    ppa_client_handle_t fill_client;
    ppa_client_handle_t blend_client;
    uint8_t * buf;
} lv_draw_ppa_unit_t;

/**********************
*  STATIC PROTOTYPES
**********************/

/**********************
* GLOBAL PROTOTYPES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   STATIC FUNCTIONS
**********************/

static inline bool ppa_src_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline bool ppa_dest_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline ppa_fill_color_mode_t lv_color_format_to_ppa_fill(lv_color_format_t lv_fmt)
{
    switch(lv_fmt) {
        case LV_COLOR_FORMAT_RGB565:
            return PPA_FILL_COLOR_MODE_RGB565;
        case LV_COLOR_FORMAT_RGB888:
            return PPA_FILL_COLOR_MODE_RGB888;
        case LV_COLOR_FORMAT_ARGB8888:
            return PPA_FILL_COLOR_MODE_ARGB8888;
        default:
            return PPA_FILL_COLOR_MODE_RGB565;
    }
}

static inline ppa_blend_color_mode_t lv_color_format_to_ppa_blend(lv_color_format_t lv_fmt)
{
    switch(lv_fmt) {
        case LV_COLOR_FORMAT_RGB565:
            return PPA_BLEND_COLOR_MODE_RGB565;
        case LV_COLOR_FORMAT_RGB888:
            return PPA_BLEND_COLOR_MODE_RGB888;
        case LV_COLOR_FORMAT_ARGB8888:
            return PPA_BLEND_COLOR_MODE_ARGB8888;
        default:
            return PPA_BLEND_COLOR_MODE_RGB565;
    }
}

static inline ppa_srm_color_mode_t lv_color_format_to_ppa_srm(lv_color_format_t lv_fmt)
{
    switch(lv_fmt) {
        case LV_COLOR_FORMAT_RGB565:
            return PPA_SRM_COLOR_MODE_RGB565;
        case LV_COLOR_FORMAT_RGB888:
            return PPA_SRM_COLOR_MODE_RGB888;
        case LV_COLOR_FORMAT_XRGB8888:
            return PPA_SRM_COLOR_MODE_ARGB8888;
        default:
            return PPA_SRM_COLOR_MODE_RGB565;
    }
}

/**
 * Row pitch of a picture, in pixels - what the PPA calls pic_w.
 *
 * The PPA has no stride field: it derives the pitch from pic_w times the pixel
 * size, so a buffer whose stride is padded beyond its width must be described
 * by that stride and not by its width. LVGL's image converter pads it, and every
 * decode path in this unit passes stride_align = false, which keeps that padding
 * rather than normalising it away: the 100 px wide benchmark logo is stored with
 * a 448 byte stride, 112 px, and describing it as 100 px makes every row start
 * 12 px early.
 *
 * Because no path re-strides, a source header stride is also exactly what the
 * decode returns, so ppa_evaluate() can check this without decoding first.
 *
 * Returns 0 when the stride is not a whole number of pixels - RGB888 padded to
 * 320 bytes is 106.67 px - which the PPA cannot describe at all. Callers must
 * treat that as "not for this draw unit".
 */
static inline int32_t lv_ppa_pic_w(uint32_t stride, int32_t w, lv_color_format_t cf)
{
    /* LV_STRIDE_AUTO. lv_image_decoder_open() resolves it to width * pixel size
     * (img_width_to_stride(), no alignment applied), so the pitch is the width. */
    if(stride == LV_STRIDE_AUTO) return w;

    uint8_t px_size = lv_color_format_get_size(cf);
    if(px_size == 0 || (stride % px_size) != 0) return 0;

    return (int32_t)(stride / px_size);
}

#define PPA_ALIGN_UP(x, align)  ((((x) + (align) - 1) / (align)) * (align))

#endif /* LV_USE_PPA */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_PPA_PRIVATE_H */
