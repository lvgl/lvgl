/**
 * @file lv_irq_cmsis.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

#define CMSIS_DMA2D (LV_IRQ_DMA2D_PROVIDER == LV_IRQ_PROVIDER_CMSIS)
#define CMSIS_PXP   (LV_IRQ_PXP_PROVIDER == LV_IRQ_PROVIDER_CMSIS)

#if CMSIS_DMA2D || CMSIS_PXP

#if LV_IRQ_CMSIS_USE_INCLUDE
    #include LV_IRQ_CMSIS_INCLUDE
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void cmsis_enable_irq(IRQn_Type irqn);

/**********************
 *  STATIC VARIABLES
 **********************/

/* Written from thread context (attach/detach), read in the vector: volatile,
 * read once into locals there, and user_data stored before cb so the vector
 * never pairs a new cb with an old user_data. A pointer-sized store is atomic
 * on Cortex-M, and detach disables the line before clearing the pointer. */
#if CMSIS_DMA2D
    static volatile lv_irq_cb_t dma2d_cb;
    static void * volatile dma2d_user_data;
#endif
#if CMSIS_PXP
    static volatile lv_irq_cb_t pxp_cb;
    static void * volatile pxp_user_data;
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* A static-vector Cortex-M target cannot register a handler at runtime, so this
 * provider defines the named vectors (overriding the startup code's weak
 * defaults at link time); attach only stores the callback and enables the line. */

#if CMSIS_DMA2D

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    dma2d_user_data = user_data;
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
    lv_irq_cb_t cb = dma2d_cb;
    void * user_data = dma2d_user_data;
    if(cb) cb(user_data);
}

#endif /*CMSIS_DMA2D*/

#if CMSIS_PXP

lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    pxp_user_data = user_data;
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
    lv_irq_cb_t cb = pxp_cb;
    void * user_data = pxp_user_data;
    if(cb) cb(user_data);
}

#endif /*CMSIS_PXP*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* The ISR calls a ...FromISR API under an RTOS, which needs the priority at or
 * below the kernel's syscall ceiling. That value is OS-specific, so it comes
 * from config instead of an RTOS header. */
static void cmsis_enable_irq(IRQn_Type irqn)
{
#if LV_IRQ_CMSIS_SET_PRIORITY
    NVIC_SetPriority(irqn, LV_IRQ_CMSIS_PRIORITY);
#endif
    NVIC_EnableIRQ(irqn);
}

#endif /*CMSIS_DMA2D || CMSIS_PXP*/
