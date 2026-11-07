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
#elif defined(CONFIG_ESP32_CACHE_L2_LINE_SIZE)
#define LV_DRAW_PPA_CACHE_LINE_SIZE CONFIG_ESP32_CACHE_L2_LINE_SIZE
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
    bool img_sw_fallback;
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

static inline int32_t lv_ppa_pic_w(uint32_t stride, int32_t w, lv_color_format_t cf)
{
    if(stride == LV_STRIDE_AUTO) return w;

    uint8_t px_size = lv_color_format_get_size(cf);
    if(px_size == 0 || (stride % px_size) != 0) return 0;

    return (int32_t)(stride / px_size);
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
 * Whether the v5.5 PPA driver would hang on this block at 90 or 270 degrees.
 *
 * DIG-734, esp-idf #19023 and #19096: the driver decides whether to bypass the
 * macro-block order from the block as it is BEFORE the turn, while the hardware
 * needs that decision taken on the block as it is AFTER it. When the two differ,
 * the last macro block leaves the DMA short of its FIFO and the transfer never
 * completes - a blocking submit then never returns. The fix is on master and
 * release/v6.0, but not on release/v5.5 or release/v6.1.
 *
 * ppa_dig734_bypass() reproduces the driver's own test from ppa_srm.c. Asking
 * for the bypass when it is not needed is harmless; withholding it when it is,
 * is the hang - so the predicate only reports the one asymmetric case.
 *
 * Ported from the lv_draw_ortho work in espressif/esp-iot-solution#818, which
 * splits the offending block into pieces that do not hang. This unit takes the
 * simpler route and leaves the draw to software.
 */
static inline bool ppa_dig734_bypass(uint32_t w_in, uint32_t h_in, uint32_t bits, uint32_t mb)
{
    const uint32_t FIFO = 12 * 128;        /* dma2d_fifo_depth_bits */
    const uint32_t w_out = w_in;           /* scale 1.0: this unit never scales while rotating,
                                              so the driver's fractional term drops out */
    const uint32_t wdiv = bits >= 24 ? 32 : 64;

    uint32_t w_left = w_out % wdiv;
    if(w_left == 0) w_left = wdiv;

    uint32_t h_in_left = h_in % mb;
    if(h_in_left == 0) h_in_left = mb;

    return (w_out > wdiv || h_in > mb) && w_left * h_in_left * bits < FIFO;
}

static inline bool lv_ppa_rotate_hangs(uint32_t block_w, uint32_t block_h, int32_t angle, lv_color_format_t cf)
{
    if(angle != 900 && angle != 2700) return false;

    const uint32_t bits = (uint32_t)lv_color_format_get_size(cf) * 8;
    if(bits == 0) return false;

    /* 16x16 macro blocks below silicon v3.0, 32x32 selectable on it: check both. */
    for(uint32_t mb = 16; mb <= 32; mb += 16) {
        const bool needed = ppa_dig734_bypass(block_h, block_w, bits, mb);  /* after the turn */
        const bool given  = ppa_dig734_bypass(block_w, block_h, bits, mb);  /* what v5.5 gives */
        if(needed && !given) return true;
    }
    return false;
}

#define PPA_ALIGN_UP(x, align)  ((((x) + (align) - 1) / (align)) * (align))

#endif /* LV_USE_PPA */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_PPA_PRIVATE_H */
