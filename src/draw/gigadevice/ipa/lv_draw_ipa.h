/**
 * @file lv_draw_ipa.h
 *
 */

#ifndef LV_DRAW_IPA_H
#define LV_DRAW_IPA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"
#if LV_USE_DRAW_IPA

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize and register the IPA draw unit.
 * Enables the IPA clock on supported GD32 targets and disables IPA dead time.
 * Installs IPA cache handlers when cache support is enabled and creates a
 * synchronization object for asynchronous operation. If LV_USE_DRAW_IPA_INTERRUPT
 * is enabled, also enables IPA_IRQn, using the FreeRTOS syscall-safe priority
 * when FreeRTOS is selected.
 * @note Called by lv_init() when LV_USE_DRAW_IPA is enabled; do not call it again.
 *       The application must provide the IPA ISR when interrupts are enabled.
 */
void lv_draw_ipa_init(void);

/**
 * Shut down the IPA hardware and asynchronous synchronization object.
 * Disables IPA_IRQn and the IPA clock on supported GD32 targets. Does not wait
 * for an active transfer, unregister the draw unit, or restore cache handlers.
 * @note Called by lv_deinit() when LV_USE_DRAW_IPA is enabled. Ensure all IPA
 *       transfers and rendering activity have stopped before shutting down.
 */
void lv_draw_ipa_deinit(void);

#if LV_USE_DRAW_IPA_INTERRUPT
/**
 * Notify the asynchronous draw unit that an IPA transfer has completed.
 * Call from the application's IPA ISR after checking and clearing the hardware
 * transfer-complete interrupt flag. The ISR must handle other interrupt causes
 * separately; this function neither reads nor clears IPA interrupt flags.
 * @note Call only while the IPA draw unit is initialized. This function signals
 *       the waiting thread; task completion and cache invalidation occur outside
 *       the ISR. Without asynchronous operation, this function has no effect.
 */
void lv_draw_ipa_transfer_complete_interrupt_handler(void);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_IPA*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IPA_H*/
