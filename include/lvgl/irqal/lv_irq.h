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
 * Callback invoked by the provider when a completion source fires (in interrupt
 * context).
 *
 * @param arg  for DMA2D, PXP, EPIC and EZIP, the `user_data` given to attach. For the
 *             NemaGFX GPU2D, an `lv_irq_nemagfx_event_t *` whose `user_data`
 *             is the one given to attach.
 */
typedef void (*lv_irq_cb_t)(void * arg);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* One attach/detach pair per known GPU completion source, used mainly by LVGL's
 * own GPU draw units. `attach` arranges for `cb` to be invoked when the source
 * signals completion, handing `user_data` back, and enables the source's
 * interrupt; `detach` disables it. `cb` must not be NULL.
 *
 * Attach as the last step of initializing the peripheral: from then on its
 * interrupts may arrive, and the caller must be ready to handle them.
 *
 * A provider must invoke `cb` from interrupt context: callbacks may use
 * ISR-only APIs such as lv_thread_sync_signal_isr().
 *
 * An IRQ provider defines the pairs: one bundled with LVGL (picked with
 * LV_IRQ_PROVIDER) or any other code. A pair is declared only while
 * LV_IRQ_PROVIDES_<SOURCE> is enabled. */

#if LV_IRQ_PROVIDES_DMA2D
lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_dma2d(void);
#endif

#if LV_IRQ_PROVIDES_PXP
lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_pxp(void);
#endif

#if LV_IRQ_PROVIDES_NEMAGFX
/** What the NemaGFX GPU2D completion passes to the callback. */
typedef struct {
    void * user_data;       /**< The `user_data` given to lv_irq_attach_nemagfx() */
    uint32_t cmd_list_id;   /**< The command list that completed */
} lv_irq_nemagfx_event_t;

lv_result_t lv_irq_attach_nemagfx(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_nemagfx(void);
#endif

#if LV_IRQ_PROVIDES_EPIC
lv_result_t lv_irq_attach_epic(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_epic(void);
#endif

#if LV_IRQ_PROVIDES_EZIP
lv_result_t lv_irq_attach_ezip(lv_irq_cb_t cb, void * user_data);
lv_result_t lv_irq_detach_ezip(void);
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_H*/
