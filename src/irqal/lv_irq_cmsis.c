/**
 * @file lv_irq_cmsis.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_irq_private.h"

#if LV_USE_IRQ == LV_IRQ_CMSIS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* On a static-vector Cortex-M target the handler cannot be registered at runtime,
 * so this backend *defines* the named IRQ vector (overriding the vendor startup's
 * weak default at link time) and `attach` only stores the callback and enables the
 * NVIC line. A project using this backend for a peripheral must NOT also define the
 * corresponding *_IRQHandler vector. */

/* Enable an IRQ line, first lowering its NVIC priority to a kernel-safe value when
 * requested. On an RTOS the ISR calls a ...FromISR API (via lv_thread_sync_signal_isr),
 * which requires the priority to sit at or below the syscall ceiling; that value is
 * OS-specific, so it comes from config (LV_IRQ_CMSIS_PRIORITY) rather than from any
 * RTOS header, keeping this backend environment-agnostic. */
#if LV_IRQ_HAS_DMA2D || LV_IRQ_HAS_PXP
static void cmsis_enable_irq(IRQn_Type irqn)
{
#if LV_IRQ_CMSIS_SET_PRIORITY
    NVIC_SetPriority(irqn, LV_IRQ_CMSIS_PRIORITY);
#endif
    NVIC_EnableIRQ(irqn);
}
#endif

#if LV_IRQ_HAS_DMA2D

static lv_irq_cb_t dma2d_cb;

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb)
{
    dma2d_cb = cb;
    cmsis_enable_irq(DMA2D_IRQn);
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_dma2d(void)
{
    NVIC_DisableIRQ(DMA2D_IRQn);
    dma2d_cb = NULL;
    return LV_RESULT_OK;
}

void DMA2D_IRQHandler(void);
void DMA2D_IRQHandler(void)
{
    if(dma2d_cb) dma2d_cb();
}

#endif /*LV_IRQ_HAS_DMA2D*/

#if LV_IRQ_HAS_PXP

static lv_irq_cb_t pxp_cb;

lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb)
{
    pxp_cb = cb;
    cmsis_enable_irq(PXP_IRQn);
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_pxp(void)
{
    NVIC_DisableIRQ(PXP_IRQn);
    pxp_cb = NULL;
    return LV_RESULT_OK;
}

void PXP_IRQHandler(void);
void PXP_IRQHandler(void)
{
    if(pxp_cb) pxp_cb();
}

#endif /*LV_IRQ_HAS_PXP*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_IRQ == LV_IRQ_CMSIS*/
