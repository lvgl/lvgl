/**
 * @file lv_pxp_osa.c
 *
 */

/**
 * Copyright 2020, 2022-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_pxp_osa.h"

#if LV_USE_DRAW_PXP
#include "lv_pxp_utils.h"
#include "../../../osal/lv_os_private.h"
#include "../../../irqal/lv_irq_private.h"
#include <fsl_pxp.h>

#if !LV_IRQ_HAS_PXP
    #error "PXP needs an IRQ backend that provides PXP (set LV_USE_IRQ to a backend whose platform exposes PXP - e.g. a Zephyr DT node or CMSIS PXP_IRQn - or supply a custom backend defining LV_IRQ_HAS_PXP)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * PXP interrupt initialization.
 */
static void _pxp_interrupt_init(void);

/**
 * PXP interrupt de-initialization.
 */
static void _pxp_interrupt_deinit(void);

/**
 * Start the PXP job.
 */
static void _pxp_run(void);

/**
 * Wait for PXP completion.
 */
static void _pxp_wait(void);

/**
 * PXP completion callback, invoked by the IRQ abstraction layer (src/irqal).
 */
static void _pxp_irq_cb(void);

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OS
    static lv_thread_sync_t pxp_sync;
#endif
static volatile bool ucPXPIdle;

static pxp_cfg_t _pxp_default_cfg = {
    .pxp_interrupt_init = _pxp_interrupt_init,
    .pxp_interrupt_deinit = _pxp_interrupt_deinit,
    .pxp_run = _pxp_run,
    .pxp_wait = _pxp_wait,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

pxp_cfg_t * pxp_get_default_cfg(void)
{
    return &_pxp_default_cfg;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Called by the IRQ abstraction layer (src/irqal) when the PXP interrupt fires,
 * regardless of the underlying environment (Zephyr, CMSIS, ...). */
static void _pxp_irq_cb(void)
{
    if(kPXP_CompleteFlag & PXP_GetStatusFlags(PXP_ID)) {
        PXP_ClearStatusFlags(PXP_ID, kPXP_CompleteFlag);
#if LV_USE_OS
        lv_thread_sync_signal_isr(&pxp_sync);
#else
        ucPXPIdle = true;
#endif
    }
}

static void _pxp_interrupt_init(void)
{
#if LV_USE_OS
    if(lv_thread_sync_init(&pxp_sync) != LV_RESULT_OK) {
        PXP_ASSERT_MSG(false, "Failed to init thread_sync.");
    }
#endif

    /* register the PXP completion interrupt through the IRQ abstraction layer;
     * the environment-specific wiring lives in src/irqal, not here */
    lv_irq_attach_pxp(_pxp_irq_cb);

    ucPXPIdle = true;
}

static void _pxp_interrupt_deinit(void)
{
    lv_irq_detach_pxp();

#if LV_USE_OS
    lv_thread_sync_delete(&pxp_sync);
#endif
}

/**
 * Function to start PXP job.
 */
static void _pxp_run(void)
{
    ucPXPIdle = false;

    PXP_EnableInterrupts(PXP_ID, kPXP_CompleteInterruptEnable);
    PXP_Start(PXP_ID);
}

/**
 * Function to wait for PXP completion.
 */
static void _pxp_wait(void)
{
    if(ucPXPIdle == true)
        return;
#if LV_USE_OS
    if(lv_thread_sync_wait(&pxp_sync) == LV_RESULT_OK)
        ucPXPIdle = true;
#else
    while(ucPXPIdle == false) {
    }
#endif
}

#endif /*LV_USE_DRAW_PXP*/
