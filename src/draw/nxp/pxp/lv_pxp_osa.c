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
#include "../../../misc/lv_log.h"
#include "fsl_pxp.h"

#if defined(SDK_OS_FREE_RTOS)
    #include "FreeRTOS.h"
    #include "semphr.h"
#endif

#if defined(__ZEPHYR__)
    #include <zephyr/kernel.h>
    #include <zephyr/irq.h>
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

void PXP_IRQHandler(void)
{
    if(kPXP_CompleteFlag & PXP_GetStatusFlags(PXP_ID)) {
        PXP_ClearStatusFlags(PXP_ID, kPXP_CompleteFlag);
#if LV_USE_OS
        lv_thread_sync_signal(&pxp_sync);
#else
        ucPXPIdle = true;
#endif
    }
}

#if defined(__ZEPHYR__)
static void PXP_ZephyrIRQHandler(void *)
{
    PXP_IRQHandler();
}
#endif

pxp_cfg_t * pxp_get_default_cfg(void)
{
    return &_pxp_default_cfg;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _pxp_interrupt_init(void)
{
#if LV_USE_OS
    if(lv_thread_sync_init(&pxp_sync) != LV_RESULT_OK) {
        PXP_ASSERT_MSG(false, "Failed to init thread_sync.");
    }
#endif

#if defined(SDK_OS_FREE_RTOS)
    NVIC_SetPriority(PXP_IRQ_ID, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(PXP_IRQ_ID);
#elif defined(__ZEPHYR__)
    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(pxp)), CONFIG_LV_Z_PXP_INTERRUPT_PRIORITY, PXP_ZephyrIRQHandler, NULL, 0);
    irq_enable(DT_IRQN(DT_NODELABEL(pxp)));
#endif
    ucPXPIdle = true;
}

static void _pxp_interrupt_deinit(void)
{
#if defined(SDK_OS_FREE_RTOS)
    NVIC_DisableIRQ(PXP_IRQ_ID);
#elif defined(__ZEPHYR__)
    irq_disable(DT_IRQN(DT_NODELABEL(pxp)));
#endif

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
