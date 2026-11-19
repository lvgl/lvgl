/**
 * @file lv_irq_zephyr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

#if LV_IRQ_PROVIDER == LV_IRQ_PROVIDER_ZEPHYR

#include <zephyr/devicetree.h>

/* A source is served when it is enabled and the device tree has its node. */
#define ZEPHYR_DMA2D (LV_IRQ_PROVIDES_DMA2D && DT_NODE_EXISTS(DT_NODELABEL(dma2d)))
#define ZEPHYR_PXP   (LV_IRQ_PROVIDES_PXP && DT_NODE_EXISTS(DT_NODELABEL(pxp)))
#define ZEPHYR_EPIC  (LV_IRQ_PROVIDES_EPIC && DT_NODE_EXISTS(DT_NODELABEL(epic)))
#define ZEPHYR_EZIP  (LV_IRQ_PROVIDES_EZIP && DT_NODE_EXISTS(DT_NODELABEL(ezip)))

#if ZEPHYR_DMA2D || ZEPHYR_PXP || ZEPHYR_EPIC || ZEPHYR_EZIP

#include <zephyr/kernel.h>
#include <zephyr/irq.h>

/**********************
 *  STATIC VARIABLES
 **********************/

/* Written from thread context (attach/detach), read in the trampoline: volatile,
 * read once into locals there. Attach and detach mask the line while they
 * change the pair, so the trampoline never sees a half-updated one, and a
 * completion arriving meanwhile stays pending instead of being lost. */
#if ZEPHYR_DMA2D
    static volatile lv_irq_cb_t dma2d_cb;
    static void * volatile dma2d_user_data;
#endif
#if ZEPHYR_PXP
    static volatile lv_irq_cb_t pxp_cb;
    static void * volatile pxp_user_data;
#endif
#if ZEPHYR_EPIC
    static volatile lv_irq_cb_t epic_cb;
    static void * volatile epic_user_data;
#endif
#if ZEPHYR_EZIP
    static volatile lv_irq_cb_t ezip_cb;
    static void * volatile ezip_user_data;
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

#if ZEPHYR_EPIC
static void epic_trampoline(const void * arg)
{
    lv_irq_cb_t cb = epic_cb;
    void * user_data = epic_user_data;
    LV_UNUSED(arg);
    if(cb) cb(user_data);
}
#endif

#if ZEPHYR_EZIP
static void ezip_trampoline(const void * arg)
{
    lv_irq_cb_t cb = ezip_cb;
    void * user_data = ezip_user_data;
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
    irq_disable(DT_IRQN(DT_NODELABEL(dma2d)));
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
    irq_disable(DT_IRQN(DT_NODELABEL(pxp)));
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

#if ZEPHYR_EPIC

lv_result_t lv_irq_attach_epic(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    irq_disable(DT_IRQN(DT_NODELABEL(epic)));
    epic_user_data = user_data;
    epic_cb = cb;
    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(epic)), DT_IRQ(DT_NODELABEL(epic), priority),
                epic_trampoline, NULL, 0);
    irq_enable(DT_IRQN(DT_NODELABEL(epic)));
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_epic(void)
{
    irq_disable(DT_IRQN(DT_NODELABEL(epic)));
    epic_cb = NULL;
    return LV_RESULT_OK;
}

#endif /*ZEPHYR_EPIC*/

#if ZEPHYR_EZIP

lv_result_t lv_irq_attach_ezip(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    irq_disable(DT_IRQN(DT_NODELABEL(ezip)));
    ezip_user_data = user_data;
    ezip_cb = cb;
    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(ezip)), DT_IRQ(DT_NODELABEL(ezip), priority),
                ezip_trampoline, NULL, 0);
    irq_enable(DT_IRQN(DT_NODELABEL(ezip)));
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_ezip(void)
{
    irq_disable(DT_IRQN(DT_NODELABEL(ezip)));
    ezip_cb = NULL;
    return LV_RESULT_OK;
}

#endif /*ZEPHYR_EZIP*/

#endif /*ZEPHYR_DMA2D || ZEPHYR_PXP || ZEPHYR_EPIC || ZEPHYR_EZIP*/

#endif /*LV_IRQ_PROVIDER == LV_IRQ_PROVIDER_ZEPHYR*/
