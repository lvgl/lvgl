/**
 * @file lv_gpu_nxp_pxp_osa.c
 *
 */

/**
 * MIT License
 *
 * Copyright (c) 2020 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"

#if LV_USE_GPU_NXP_PXP && LV_USE_GPU_NXP_PXP_AUTO_INIT

#include "lv_gpu_nxp_pxp.h"
#include "fsl_pxp.h"

#if defined(FSL_RTOS_FREE_RTOS)
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
static lv_res_t _lv_gpu_nxp_pxp_interrupt_init(void);
static void _lv_gpu_nxp_pxp_interrupt_deinit(void);
static void _lv_gpu_nxp_pxp_run(void);

/**********************
 *  STATIC VARIABLES
 **********************/

#if defined(FSL_RTOS_FREE_RTOS)
    static SemaphoreHandle_t s_pxpIdle;
#else
    static volatile bool s_pxpIdle;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * PXP device interrupt handler. Used to check PXP task completion status.
 */
void PXP_IRQHandler(void)
{
#if defined(FSL_RTOS_FREE_RTOS)
    BaseType_t taskAwake = pdFALSE;
#endif

    if(kPXP_CompleteFlag & PXP_GetStatusFlags(LV_GPU_NXP_PXP_ID)) {
        PXP_ClearStatusFlags(LV_GPU_NXP_PXP_ID, kPXP_CompleteFlag);
#if defined(FSL_RTOS_FREE_RTOS)
        xSemaphoreGiveFromISR(s_pxpIdle, &taskAwake);
        portYIELD_FROM_ISR(taskAwake);
#else
        s_pxpIdle = true;
#endif

    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * PXP interrupt initialization.
 */
static lv_res_t _lv_gpu_nxp_pxp_interrupt_init(void)
{
#if defined(FSL_RTOS_FREE_RTOS)
    s_pxpIdle = xSemaphoreCreateBinary();
    if(s_pxpIdle == NULL) {
        return LV_RES_INV;
    }

    NVIC_SetPriority(LV_GPU_NXP_PXP_IRQ_ID, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
#else
    s_pxpIdle = true;
#endif

    NVIC_EnableIRQ(LV_GPU_NXP_PXP_IRQ_ID);

    return LV_RES_OK;
}

/**
 * PXP interrupt de-initialization.
 */
static void _lv_gpu_nxp_pxp_interrupt_deinit(void)
{
    NVIC_DisableIRQ(LV_GPU_NXP_PXP_IRQ_ID);
#if defined(FSL_RTOS_FREE_RTOS)
    vSemaphoreDelete(s_pxpIdle);
#endif
}

/**
 * Function to start PXP job. This function must wait for task complete.
 */
static void _lv_gpu_nxp_pxp_run(void)
{
#if !defined(FSL_RTOS_FREE_RTOS)
    s_pxpIdle = false;
#endif

    PXP_EnableInterrupts(LV_GPU_NXP_PXP_ID, kPXP_CompleteInterruptEnable);
    PXP_Start(LV_GPU_NXP_PXP_ID);

#if defined(FSL_RTOS_FREE_RTOS)
    if(xSemaphoreTake(s_pxpIdle, portMAX_DELAY) != pdTRUE) {
        LV_LOG_ERROR("xSemaphoreTake error. Task halted.");
        for(; ;) ;
    }
#else
    while(s_pxpIdle == false) {
    }
#endif
}

lv_nxp_pxp_cfg_t pxp_default_cfg = {
    .pxp_interrupt_init = _lv_gpu_nxp_pxp_interrupt_init,
    .pxp_interrupt_deinit = _lv_gpu_nxp_pxp_interrupt_deinit,
    .pxp_run = _lv_gpu_nxp_pxp_run
};

#endif /* LV_USE_GPU_NXP_PXP && LV_USE_GPU_NXP_PXP_AUTO_INIT */
