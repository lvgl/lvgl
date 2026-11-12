/**
 * @file lv_irq_redox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "lv_irq_redox.h"

#if LV_IRQ_DMA2D_PROVIDER == LV_IRQ_PROVIDER_REDOX

#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#if !defined(LV_IRQ_REDOX_DMA2D_IRQ) || LV_IRQ_REDOX_DMA2D_IRQ < 0
    #error "Set LV_IRQ_REDOX_DMA2D_IRQ to the DMA2D interrupt number"
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    int irq;
    int fd;
    pthread_t thread;
    volatile lv_irq_cb_t cb;
    void * user_data;
} redox_irq_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t redox_attach(redox_irq_t * irq, lv_irq_cb_t cb, void * user_data);
static lv_result_t redox_detach(redox_irq_t * irq);
static void * redox_irq_thread(void * arg);

/**********************
 *  STATIC VARIABLES
 **********************/

static redox_irq_t dma2d_irq = { .irq = LV_IRQ_REDOX_DMA2D_IRQ, .fd = -1 };

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb, void * user_data)
{
    return redox_attach(&dma2d_irq, cb, user_data);
}

lv_result_t lv_irq_detach_dma2d(void)
{
    return redox_detach(&dma2d_irq);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t redox_attach(redox_irq_t * irq, lv_irq_cb_t cb, void * user_data)
{
    char path[32];

    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);

    lv_snprintf(path, sizeof(path), "/scheme/irq/%d", irq->irq);
    irq->fd = open(path, O_RDWR);
    if(irq->fd < 0) return LV_RESULT_INVALID;

    irq->user_data = user_data;
    irq->cb = cb;
    if(pthread_create(&irq->thread, NULL, redox_irq_thread, irq) != 0) {
        irq->cb = NULL;
        close(irq->fd);
        irq->fd = -1;
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

static lv_result_t redox_detach(redox_irq_t * irq)
{
    if(irq->fd < 0) return LV_RESULT_OK;

    irq->cb = NULL;
    /* The thread is blocked in read(), a cancellation point. */
    pthread_cancel(irq->thread);
    pthread_join(irq->thread, NULL);
    close(irq->fd);
    irq->fd = -1;
    return LV_RESULT_OK;
}

/* A read on the IRQ's file blocks until the interrupt fires; writing the count
 * back acknowledges it. The callback therefore runs in this thread, not in an
 * interrupt handler. */
static void * redox_irq_thread(void * arg)
{
    redox_irq_t * irq = arg;
    uint64_t count;

    while(read(irq->fd, &count, sizeof(count)) == (ssize_t)sizeof(count)) {
        lv_irq_cb_t cb = irq->cb;
        if(cb) cb(irq->user_data);
        if(write(irq->fd, &count, sizeof(count)) != (ssize_t)sizeof(count)) break;
    }
    return NULL;
}

#endif /*LV_IRQ_DMA2D_PROVIDER == LV_IRQ_PROVIDER_REDOX*/
