/**
 * @file lv_irq_private.h
 *
 * Provides the compile-time per-peripheral capability macros (LV_IRQ_HAS_*).
 *
 * This is NOT part of the public API and is deliberately NOT included from
 * lvgl_private.h: irqal exposes no types embedded in LVGL structs, so it does
 * not need umbrella-wide visibility. Only the irqal backend .c files and the
 * few draw units that use irqal (e.g. dma2d, pxp) include this header, because
 * they need LV_IRQ_HAS_* at compile time (to gate call sites / async paths).
 * The public API (lv_irq_attach_*, lv_irq_cb_t) lives in include/lvgl/irqal/lv_irq.h.
 */

#ifndef LV_IRQ_PRIVATE_H
#define LV_IRQ_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

/* Include every backend header; each self-guards on LV_USE_IRQ, so only the
 * selected one contributes its LV_IRQ_HAS_* definitions and vendor includes. */
#include "lv_irq_none.h"
#include "lv_irq_cmsis.h"
#include "lv_irq_zephyr.h"

/*********************
 *      DEFINES
 *********************/

/* Capability macros default to 0 when the selected backend didn't define them.
 * A custom backend (LV_IRQ_NONE + own implementation) can pre-define these to 1
 * in lv_conf.h; those definitions win because lv_conf is included above. */
#ifndef LV_IRQ_HAS_DMA2D
#define LV_IRQ_HAS_DMA2D 0
#endif

#ifndef LV_IRQ_HAS_PXP
#define LV_IRQ_HAS_PXP 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_PRIVATE_H*/
