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

#if defined(SDK_OS_FREE_RTOS)
    static SemaphoreHandle_t xPXPIdleSemaphore;
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
#if defined(SDK_OS_FREE_RTOS)
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif

    if(kPXP_CompleteFlag & PXP_GetStatusFlags(PXP_ID)) {
        PXP_ClearStatusFlags(PXP_ID, kPXP_CompleteFlag);
#if defined(SDK_OS_FREE_RTOS)
        xSemaphoreGiveFromISR(xPXPIdleSemaphore, &xHigherPriorityTaskWoken);

        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
        should be performed to ensure the interrupt returns directly to the highest
        priority task.  The macro used for this purpose is dependent on the port in
        use and may be called portEND_SWITCHING_ISR(). */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#else
        ucPXPIdle = true;
#endif
    }
}

pxp_cfg_t * pxp_get_default_cfg(void)
{
    return &_pxp_default_cfg;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _pxp_interrupt_init(void)
{
#if defined(SDK_OS_FREE_RTOS)
    xPXPIdleSemaphore = xSemaphoreCreateBinary();
    PXP_ASSERT_MSG(xPXPIdleSemaphore, "xSemaphoreCreateBinary failed!");

    NVIC_SetPriority(PXP_IRQ_ID, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
#endif
    ucPXPIdle = true;

    NVIC_EnableIRQ(PXP_IRQ_ID);
}

static void _pxp_interrupt_deinit(void)
{
    NVIC_DisableIRQ(PXP_IRQ_ID);
#if defined(SDK_OS_FREE_RTOS)
    vSemaphoreDelete(xPXPIdleSemaphore);
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
#if defined(SDK_OS_FREE_RTOS)
    /* Return if PXP was never started, otherwise the semaphore will lock forever. */
    if(ucPXPIdle == true)
        return;

    if(xSemaphoreTake(xPXPIdleSemaphore, portMAX_DELAY) == pdTRUE)
        ucPXPIdle = true;
#else
    while(ucPXPIdle == false) {
    }
#endif
}

#endif /*LV_USE_DRAW_PXP*/
