/**
 * @file lv_irq.h
 *
 */

#ifndef LV_IRQ_H
#define LV_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Callback invoked by the selected IRQ backend when a registered hardware
 * interrupt fires. The callback runs in interrupt context.
 *
 * No context pointer is passed: each `lv_irq_attach_*()` is peripheral-specific
 * and every peripheral is a singleton, so the callback already knows its context
 * and reads whatever state it needs from its own static storage.
 */
typedef void (*lv_irq_cb_t)(void);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* One attach/detach pair per known peripheral IRQ, implemented by the selected
 * IRQ backend (LV_USE_IRQ) for the peripherals the target actually provides.
 * These are used mainly by LVGL's own GPU draw units. The declarations are
 * unconditional, but a definition exists only when the selected backend covers
 * that peripheral, so calling one the backend does not provide is a link error.
 * To supply your own backend, select LV_IRQ_NONE and define the attach/detach
 * functions you need (see src/irqal for the reference backends to copy).
 *
 * `attach` registers/enables the line so `cb()` is invoked when the IRQ fires
 * (in interrupt context); `detach` disables it. `cb` must not be NULL. The
 * interrupt priority is sourced by the backend (device tree on Zephyr, vendor
 * config on CMSIS), not passed here. */

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb);
lv_result_t lv_irq_detach_dma2d(void);

lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb);
lv_result_t lv_irq_detach_pxp(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_H*/
