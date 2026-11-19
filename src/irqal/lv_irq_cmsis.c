/**
 * @file lv_irq_cmsis.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

#if LV_IRQ_PROVIDER == LV_IRQ_PROVIDER_CMSIS

#if LV_IRQ_CMSIS_USE_INCLUDE
    #include LV_IRQ_CMSIS_INCLUDE
#endif

#ifndef __NVIC_PRIO_BITS
    #error "The CMSIS IRQ provider needs the device header: set LV_IRQ_CMSIS_INCLUDE"
#endif

/* A source is served when it is enabled and the device has it. The *_IRQn
 * values are enum constants, invisible to the preprocessor, so the
 * base-address macros tell. */
#if LV_IRQ_PROVIDES_DMA2D && defined(DMA2D_BASE)
    #define CMSIS_DMA2D 1
#else
    #define CMSIS_DMA2D 0
#endif
#if LV_IRQ_PROVIDES_PXP && defined(PXP_BASE)
    #define CMSIS_PXP 1
#else
    #define CMSIS_PXP 0
#endif
#if LV_IRQ_PROVIDES_EPIC && defined(EPIC_BASE)
    #define CMSIS_EPIC 1
#else
    #define CMSIS_EPIC 0
#endif
/* SiFli names the base EZIP_BASE or EZIP1_BASE depending on the family. */
#if LV_IRQ_PROVIDES_EZIP && (defined(EZIP_BASE) || defined(EZIP1_BASE))
    #define CMSIS_EZIP 1
#else
    #define CMSIS_EZIP 0
#endif

#if CMSIS_DMA2D || CMSIS_PXP || CMSIS_EPIC || CMSIS_EZIP

/* RT-Thread must be told about interrupt context, or a wakeup from the ISR
 * does not reschedule. */
#if LV_USE_OS == LV_OS_RTTHREAD
    #include <rtthread.h>
    #define CMSIS_ISR_ENTER() rt_interrupt_enter()
    #define CMSIS_ISR_LEAVE() rt_interrupt_leave()
#else
    #define CMSIS_ISR_ENTER()
    #define CMSIS_ISR_LEAVE()
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void cmsis_enable_irq(IRQn_Type irqn);

/**********************
 *  STATIC VARIABLES
 **********************/

/* Written from thread context (attach/detach), read in the vector: volatile,
 * read once into locals there. Attach and detach mask the line while they
 * change the pair, so the vector never sees a half-updated one, and a
 * completion arriving meanwhile stays pending instead of being lost. */
#if CMSIS_DMA2D
    static volatile lv_irq_cb_t dma2d_cb;
    static void * volatile dma2d_user_data;
#endif
#if CMSIS_PXP
    static volatile lv_irq_cb_t pxp_cb;
    static void * volatile pxp_user_data;
#endif
#if CMSIS_EPIC
    static volatile lv_irq_cb_t epic_cb;
    static void * volatile epic_user_data;
#endif
#if CMSIS_EZIP
    static volatile lv_irq_cb_t ezip_cb;
    static void * volatile ezip_user_data;
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
    NVIC_DisableIRQ(DMA2D_IRQn);
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
    CMSIS_ISR_ENTER();
    if(cb) cb(user_data);
    CMSIS_ISR_LEAVE();
}

#endif /*CMSIS_DMA2D*/

#if CMSIS_PXP

lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    NVIC_DisableIRQ(PXP_IRQn);
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
    CMSIS_ISR_ENTER();
    if(cb) cb(user_data);
    CMSIS_ISR_LEAVE();
}

#endif /*CMSIS_PXP*/

#if CMSIS_EPIC

lv_result_t lv_irq_attach_epic(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    NVIC_DisableIRQ(EPIC_IRQn);
    epic_user_data = user_data;
    epic_cb = cb;
    cmsis_enable_irq(EPIC_IRQn);
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_epic(void)
{
    NVIC_DisableIRQ(EPIC_IRQn);
    epic_cb = NULL;
    return LV_RESULT_OK;
}

void EPIC_IRQHandler(void);
void EPIC_IRQHandler(void)
{
    lv_irq_cb_t cb = epic_cb;
    void * user_data = epic_user_data;
    CMSIS_ISR_ENTER();
    if(cb) cb(user_data);
    CMSIS_ISR_LEAVE();
}

#endif /*CMSIS_EPIC*/

#if CMSIS_EZIP

lv_result_t lv_irq_attach_ezip(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    NVIC_DisableIRQ(EZIP_IRQn);
    ezip_user_data = user_data;
    ezip_cb = cb;
    cmsis_enable_irq(EZIP_IRQn);
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_ezip(void)
{
    NVIC_DisableIRQ(EZIP_IRQn);
    ezip_cb = NULL;
    return LV_RESULT_OK;
}

void EZIP_IRQHandler(void);
void EZIP_IRQHandler(void)
{
    lv_irq_cb_t cb = ezip_cb;
    void * user_data = ezip_user_data;
    CMSIS_ISR_ENTER();
    if(cb) cb(user_data);
    CMSIS_ISR_LEAVE();
}

#endif /*CMSIS_EZIP*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* The ISR calls a ...FromISR API under an RTOS, which needs the priority at or
 * below the kernel's syscall ceiling. That value is OS-specific, so it comes
 * from config instead of an RTOS header. */
static void cmsis_enable_irq(IRQn_Type irqn)
{
#if LV_IRQ_CMSIS_SET_NVIC_PRIORITY
    NVIC_SetPriority(irqn, LV_IRQ_CMSIS_NVIC_PRIORITY);
#endif
    NVIC_EnableIRQ(irqn);
}

#endif /*CMSIS_DMA2D || CMSIS_PXP || CMSIS_EPIC || CMSIS_EZIP*/

#endif /*LV_IRQ_PROVIDER == LV_IRQ_PROVIDER_CMSIS*/
