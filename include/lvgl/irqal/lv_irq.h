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

/* Whether an IRQ provider serves each completion source: LV_IRQ_<SOURCE>_PROVIDER
 * holds the provider's ID (LV_IRQ_PROVIDER_*, see src/irqal/Kconfig). */
#define LV_IRQ_PROVIDES_DMA2D       (LV_IRQ_DMA2D_PROVIDER != LV_IRQ_PROVIDER_NONE)
#define LV_IRQ_PROVIDES_PXP         (LV_IRQ_PXP_PROVIDER != LV_IRQ_PROVIDER_NONE)
#define LV_IRQ_PROVIDES_NEMA_GPU2D  (LV_IRQ_NEMA_GPU2D_PROVIDER != LV_IRQ_PROVIDER_NONE)

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Callback invoked by the provider when a completion source fires (in interrupt
 * context).
 *
 * @param arg  for DMA2D and PXP, the `user_data` given to attach. For the
 *             NemaGFX GPU2D, an `lv_irq_nema_gpu2d_event_t *` whose `user_data`
 *             is the one given to attach.
 */
typedef void (*lv_irq_cb_t)(void * arg);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* One attach/detach pair per known GPU completion source, used mainly by LVGL's
 * own GPU draw units. `attach` arranges for `cb` to be invoked (in interrupt
 * context) when the source signals completion, handing `user_data` back;
 * `detach` stops it. `cb` must not be NULL.
 *
 * LVGL itself defines none of these: the IRQ provider serving a source
 * (LV_IRQ_<SOURCE>_PROVIDER) defines its pair. A pair is declared only while a
 * provider is picked for its source. */

#if LV_IRQ_PROVIDES_DMA2D
lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_dma2d(void);
#endif

#if LV_IRQ_PROVIDES_PXP
lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_pxp(void);
#endif

#if LV_IRQ_PROVIDES_NEMA_GPU2D
/** What the NemaGFX GPU2D completion passes to the callback. */
typedef struct {
    void * user_data;       /**< The `user_data` given to lv_irq_attach_nema_gpu2d() */
    uint32_t cmd_list_id;   /**< The command list that completed */
} lv_irq_nema_gpu2d_event_t;

lv_result_t lv_irq_attach_nema_gpu2d(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_nema_gpu2d(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_H*/
