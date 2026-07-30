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
 * Callback invoked by the provider when a completion event fires (in interrupt
 * context).
 *
 * @param user_data  a provider-supplied pointer. Plain hardware-IRQ providers
 *                    (DMA2D, PXP) pass NULL. The NemaGFX GPU2D provider passes a
 *                    pointer to the completed command-list id (cast to
 *                    `uint32_t *` and dereference). `attach` does not take a
 *                    registration-time user_data yet; keeping this `void *`
 *                    leaves room to forward one later without changing callers.
 */
typedef void (*lv_irq_cb_t)(void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* One attach/detach pair per known GPU completion source, used mainly by LVGL's
 * own GPU draw units. `attach` arranges for the callback to be invoked (in
 * interrupt context) when the source signals completion; `detach` stops it.
 * `cb` must not be NULL. The declarations are unconditional, but a definition
 * exists only when something provides that source, so calling an unprovided one
 * is a link error.
 *
 * Providers differ by source:
 *  - DMA2D / PXP: raw hardware IRQs wired by the selected LV_USE_IRQ backend
 *    (device tree on Zephyr, NVIC on CMSIS); the priority is sourced there, and
 *    the callback's user_data is NULL. Supply your own by selecting LV_IRQ_NONE
 *    and defining the attach/detach functions you need (see src/irqal to copy).
 *  - NemaGFX GPU2D: registered through the vendor GPU2D HAL callback API; its
 *    irqal backend (src/irqal, built for the NemaGFX STM32 HAL) provides it. The
 *    callback's user_data is a pointer to the completed command-list id. */

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb);
lv_result_t lv_irq_detach_dma2d(void);

lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb);
lv_result_t lv_irq_detach_pxp(void);

lv_result_t lv_irq_attach_nema_gpu2d(lv_irq_cb_t cb);
lv_result_t lv_irq_detach_nema_gpu2d(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_H*/
