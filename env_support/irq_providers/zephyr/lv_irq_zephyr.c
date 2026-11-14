/**
 * @file lv_irq_zephyr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

#define ZEPHYR_DMA2D (LV_IRQ_DMA2D_PROVIDER == LV_IRQ_PROVIDER_ZEPHYR)
#define ZEPHYR_PXP   (LV_IRQ_PXP_PROVIDER == LV_IRQ_PROVIDER_ZEPHYR)

#if ZEPHYR_DMA2D || ZEPHYR_PXP

#include <zephyr/kernel.h>
#include <zephyr/irq.h>
#include <zephyr/devicetree.h>

#if ZEPHYR_DMA2D && !DT_NODE_EXISTS(DT_NODELABEL(dma2d))
    #error "The Zephyr IRQ provider for DMA2D needs a dma2d node in the device tree"
#endif
#if ZEPHYR_PXP && !DT_NODE_EXISTS(DT_NODELABEL(pxp))
    #error "The Zephyr IRQ provider for PXP needs a pxp node in the device tree"
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/* Written from thread context (attach/detach), read in the trampoline: volatile,
 * read once into locals there, and user_data stored before cb so the trampoline
 * never pairs a new cb with an old user_data. A pointer-sized store is atomic
 * on Cortex-M, and detach disables the line before clearing the pointer. */
#if ZEPHYR_DMA2D
    static volatile lv_irq_cb_t dma2d_cb;
    static void * volatile dma2d_user_data;
#endif
#if ZEPHYR_PXP
    static volatile lv_irq_cb_t pxp_cb;
    static void * volatile pxp_user_data;
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* IRQ_CONNECT needs a compile-time-constant ISR, so it cannot take the runtime
 * callback passed to attach: each source registers a fixed trampoline that
 * calls the stored callback. */

#if ZEPHYR_DMA2D
static void dma2d_trampoline(const void * arg)
{
    lv_irq_cb_t cb = dma2d_cb;
    void * user_data = dma2d_user_data;
    LV_UNUSED(arg);
    if(cb) cb(user_data);
}
#endif

#if ZEPHYR_PXP
static void pxp_trampoline(const void * arg)
{
    lv_irq_cb_t cb = pxp_cb;
    void * user_data = pxp_user_data;
    LV_UNUSED(arg);
    if(cb) cb(user_data);
}
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if ZEPHYR_DMA2D

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    dma2d_user_data = user_data;
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

#endif /*ZEPHYR_DMA2D*/

#if ZEPHYR_PXP

lv_result_t lv_irq_attach_pxp(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    pxp_user_data = user_data;
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

#endif /*ZEPHYR_PXP*/

#endif /*ZEPHYR_DMA2D || ZEPHYR_PXP*/
