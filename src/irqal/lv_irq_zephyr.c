/**
 * @file lv_irq_zephyr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_irq_private.h"

#if LV_USE_IRQ == LV_IRQ_ZEPHYR

#include <zephyr/kernel.h>
#include <zephyr/irq.h>

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
 *   STATIC FUNCTIONS
 **********************/

/* IRQ_CONNECT is a build-time macro that needs a compile-time-constant ISR symbol,
 * so it can't be handed the runtime callback passed to attach(). Each peripheral
 * therefore registers a small fixed trampoline that calls the stored callback set
 * by the matching attach (the Zephyr analogue of the CMSIS named vector). The
 * interrupt priority comes from the device tree node. */

#if LV_IRQ_HAS_DMA2D

static lv_irq_cb_t dma2d_cb;

static void dma2d_trampoline(void * arg)
{
    LV_UNUSED(arg);
    if(dma2d_cb) dma2d_cb();
}

#endif /*LV_IRQ_HAS_DMA2D*/

#if LV_IRQ_HAS_PXP

static lv_irq_cb_t pxp_cb;

static void pxp_trampoline(void * arg)
{
    LV_UNUSED(arg);
    if(pxp_cb) pxp_cb();
}

#endif /*LV_IRQ_HAS_PXP*/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_IRQ_HAS_DMA2D

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb)
{
    dma2d_cb = cb;
    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(dma2d)), DT_IRQ(DT_NODELABEL(dma2d), priority),
                dma2d_trampoline, NULL, 0);
    irq_enable(DT_IRQN(DT_NODELABEL(dma2d)));
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_dma2d(void)
{
    irq_disable(DT_IRQN(DT_NODELABEL(dma2d)));
    dma2d_cb = NULL;
    return LV_RESULT_OK;
}

#endif /*LV_IRQ_HAS_DMA2D*/

#if LV_IRQ_HAS_PXP

lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb)
{
    pxp_cb = cb;
    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(pxp)), DT_IRQ(DT_NODELABEL(pxp), priority),
                pxp_trampoline, NULL, 0);
    irq_enable(DT_IRQN(DT_NODELABEL(pxp)));
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_pxp(void)
{
    irq_disable(DT_IRQN(DT_NODELABEL(pxp)));
    pxp_cb = NULL;
    return LV_RESULT_OK;
}

#endif /*LV_IRQ_HAS_PXP*/

#endif /*LV_USE_IRQ == LV_IRQ_ZEPHYR*/
