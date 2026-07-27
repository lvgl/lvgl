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

/* One attach/detach pair per known peripheral IRQ. The declarations are
 * unconditional (public API); a definition exists only for peripherals the
 * selected backend actually provides (see LV_IRQ_HAS_* in the backend). Guard
 * call sites with the matching LV_IRQ_HAS_* macro so you never reference a
 * function the backend didn't define.
 *
 * `attach` registers/enables the line so `cb()` is invoked when the IRQ fires;
 * `detach` disables it. The interrupt priority is sourced by the backend
 * (device tree on Zephyr, vendor config on CMSIS), not passed here. */

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
