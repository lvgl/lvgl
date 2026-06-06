/**
 * @file lv_sifli_epic_osa.h
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_SIFLI_EPIC_OSA_H
#define LV_SIFLI_EPIC_OSA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#include "../../../misc/lv_types.h"

#if LV_USE_SIFLI_EPIC

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * SiFli EPIC Operating System Abstraction configuration.
 * This structure provides OS-specific callbacks for interrupt handling and synchronization.
 */
typedef struct {
    /** Callback for EPIC interrupt initialization */
    void (*epic_interrupt_init)(void);

    /** Callback for EPIC interrupt de-initialization */
    void (*epic_interrupt_deinit)(void);

    /** Callback for EPIC start (trigger hardware) */
    void (*epic_run)(void);

    /** Callback for waiting for EPIC completion */
    void (*epic_wait)(void);
} epic_osa_cfg_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * EPIC device interrupt handler exported by the adapter.
 */
void EPIC_IRQHandler(void);

/**
 * EZIP device interrupt handler exported by the adapter.
 */
void EZIP_IRQHandler(void);

/**
 * Get the EPIC default OSA configuration.
 * @return Pointer to the default EPIC OSA configuration structure
 */
epic_osa_cfg_t * epic_get_default_cfg(void);

/**
 * Initialize EPIC OS abstraction layer.
 * This function sets up interrupt handlers and synchronization primitives.
 * @return LV_RESULT_OK on success, LV_RESULT_INVALID otherwise
 */
lv_result_t lv_epic_osa_init(void);

/**
 * De-initialize EPIC OS abstraction layer.
 * This function cleans up interrupt handlers and synchronization primitives.
 */
void lv_epic_osa_deinit(void);

/**
 * Initialize thread synchronization primitive (semaphore/event).
 * @return LV_RESULT_OK on success, LV_RESULT_INVALID otherwise
 */
lv_result_t lv_epic_osa_thread_sync_init(void);

/**
 * Wait for EPIC operation completion (blocking).
 * @return LV_RESULT_OK on success, LV_RESULT_INVALID on timeout
 */
lv_result_t lv_epic_osa_thread_sync_wait(void);

/**
 * Signal EPIC operation completion from ISR.
 * This function should be called from the EPIC interrupt handler.
 */
void lv_epic_osa_thread_sync_signal_isr(void);

/**
 * Restore the EPIC job state to idle without signaling waiters.
 * Used when an async kickoff fails before any IRQ can be generated.
 */
void lv_epic_osa_set_idle(void);

/**
 * Delete thread synchronization primitive.
 */
void lv_epic_osa_thread_sync_delete(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SIFLI_EPIC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SIFLI_EPIC_OSA_H*/
