/**
 * @file lv_sifli_epic_osa.c
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_sifli_epic_osa.h"
#include "lv_sifli_epic_cfg.h"

#if LV_USE_SIFLI_EPIC
#include "../../../misc/lv_log.h"
#include "../../../osal/lv_os_private.h"

#if defined(__ZEPHYR__)
    #include <zephyr/kernel.h>
    #include <zephyr/irq.h>
#endif

#if LV_USE_OS == LV_OS_RTTHREAD
    #include "rtthread.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _epic_interrupt_init(void);
static void _epic_interrupt_deinit(void);
static void _epic_run(void);
static void _epic_wait(void);
static void _epic_irq_enter(void);
static void _epic_irq_leave(void);

#if defined(__ZEPHYR__)
    static void _epic_zephyr_irq_handler(const void * arg);
    #ifdef HAL_EZIP_MODULE_ENABLED
        static void _ezip_zephyr_irq_handler(const void * arg);
    #endif
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OS
    static lv_thread_sync_t epic_sync;
#endif
static volatile bool epic_idle = true;

static epic_osa_cfg_t _epic_default_cfg = {
    .epic_interrupt_init = _epic_interrupt_init,
    .epic_interrupt_deinit = _epic_interrupt_deinit,
    .epic_run = _epic_run,
    .epic_wait = _epic_wait,
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if defined(__ZEPHYR__)
static void _epic_zephyr_irq_handler(const void * arg)
{
    LV_UNUSED(arg);
    EPIC_IRQHandler();
}

#ifdef HAL_EZIP_MODULE_ENABLED
static void _ezip_zephyr_irq_handler(const void * arg)
{
    LV_UNUSED(arg);
    EZIP_IRQHandler();
}
#endif
#endif

void EPIC_IRQHandler(void)
{
    _epic_irq_enter();

    if(lv_epic_is_initialized()) {
        EPIC_HandleTypeDef * epic_handle = lv_epic_get_handle();
        if(epic_handle != NULL) {
            HAL_EPIC_IRQHandler(epic_handle);
        }
    }

    _epic_irq_leave();
}

#ifdef HAL_EZIP_MODULE_ENABLED
void EZIP_IRQHandler(void)
{
    _epic_irq_enter();

    if(lv_epic_is_initialized()) {
        EZIP_HandleTypeDef * ezip_handle = lv_ezip_get_handle();
        if(ezip_handle != NULL) {
            HAL_EZIP_IRQHandler(ezip_handle);
        }
    }

    _epic_irq_leave();
}
#endif

epic_osa_cfg_t * epic_get_default_cfg(void)
{
    return &_epic_default_cfg;
}

lv_result_t lv_epic_osa_init(void)
{
#if LV_USE_OS
    if(lv_epic_osa_thread_sync_init() != LV_RESULT_OK) {
        return LV_RESULT_INVALID;
    }
#endif
    _epic_interrupt_init();
    return LV_RESULT_OK;
}

void lv_epic_osa_deinit(void)
{
    _epic_interrupt_deinit();
#if LV_USE_OS
    lv_epic_osa_thread_sync_delete();
#endif
}

lv_result_t lv_epic_osa_thread_sync_init(void)
{
#if LV_USE_OS
    if(lv_thread_sync_init(&epic_sync) != LV_RESULT_OK) {
        return LV_RESULT_INVALID;
    }
#endif
    return LV_RESULT_OK;
}

lv_result_t lv_epic_osa_thread_sync_wait(void)
{
#if LV_USE_OS
    if(lv_thread_sync_wait(&epic_sync) != LV_RESULT_OK) {
        return LV_RESULT_INVALID;
    }
#endif
    return LV_RESULT_OK;
}

void lv_epic_osa_thread_sync_signal_isr(void)
{
    epic_idle = true;
#if LV_USE_OS
    (void)lv_thread_sync_signal_isr(&epic_sync);
#endif
}

void lv_epic_osa_set_idle(void)
{
    epic_idle = true;
}

void lv_epic_osa_thread_sync_delete(void)
{
#if LV_USE_OS
    (void)lv_thread_sync_delete(&epic_sync);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _epic_irq_enter(void)
{
#if LV_USE_OS == LV_OS_RTTHREAD
    rt_interrupt_enter();
#endif
}

static void _epic_irq_leave(void)
{
#if LV_USE_OS == LV_OS_RTTHREAD
    rt_interrupt_leave();
#endif
}

static void _epic_interrupt_init(void)
{
#if defined(__ZEPHYR__)
    IRQ_CONNECT(LV_SIFLI_EPIC_IRQn, LV_SIFLI_EPIC_IRQ_PRIORITY, _epic_zephyr_irq_handler, NULL,
                LV_SIFLI_EPIC_ZEPHYR_IRQ_FLAGS);
    irq_enable(LV_SIFLI_EPIC_IRQn);
#elif LV_USE_OS == LV_OS_FREERTOS && defined(LV_SIFLI_EPIC_FREERTOS_IRQ_PRIORITY)
    HAL_NVIC_SetPriority(LV_SIFLI_EPIC_IRQn, LV_SIFLI_EPIC_FREERTOS_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(LV_SIFLI_EPIC_IRQn);
#else
    HAL_NVIC_SetPriority(LV_SIFLI_EPIC_IRQn, LV_SIFLI_EPIC_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(LV_SIFLI_EPIC_IRQn);
#endif

#ifdef HAL_EZIP_MODULE_ENABLED
#if defined(__ZEPHYR__)
    IRQ_CONNECT(LV_SIFLI_EZIP_IRQn, LV_SIFLI_EPIC_IRQ_PRIORITY, _ezip_zephyr_irq_handler, NULL,
                LV_SIFLI_EPIC_ZEPHYR_IRQ_FLAGS);
    irq_enable(LV_SIFLI_EZIP_IRQn);
#elif LV_USE_OS == LV_OS_FREERTOS && defined(LV_SIFLI_EPIC_FREERTOS_IRQ_PRIORITY)
    HAL_NVIC_SetPriority(LV_SIFLI_EZIP_IRQn, LV_SIFLI_EPIC_FREERTOS_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(LV_SIFLI_EZIP_IRQn);
#else
    HAL_NVIC_SetPriority(LV_SIFLI_EZIP_IRQn, LV_SIFLI_EPIC_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(LV_SIFLI_EZIP_IRQn);
#endif
#endif

    epic_idle = true;
}

static void _epic_interrupt_deinit(void)
{
#if defined(__ZEPHYR__)
    irq_disable(LV_SIFLI_EPIC_IRQn);
#else
    HAL_NVIC_DisableIRQ(LV_SIFLI_EPIC_IRQn);
#endif

#ifdef HAL_EZIP_MODULE_ENABLED
#if defined(__ZEPHYR__)
    irq_disable(LV_SIFLI_EZIP_IRQn);
#else
    HAL_NVIC_DisableIRQ(LV_SIFLI_EZIP_IRQn);
#endif
#endif

    epic_idle = true;
}

static void _epic_run(void)
{
#if LV_USE_OS
    /* Reset completion token from any previous EPIC job so the next wait only
     * observes the current run's completion signal. Using LVGL's thread-sync
     * API ensures cross-platform compatibility without accessing OS internals. */
    (void)lv_thread_sync_delete(&epic_sync);
    (void)lv_thread_sync_init(&epic_sync);
#endif
    epic_idle = false;
}

static void _epic_wait(void)
{
    if(epic_idle) {
        return;
    }

#if LV_USE_OS
    if(lv_epic_osa_thread_sync_wait() == LV_RESULT_OK) {
        epic_idle = true;
    }
#else
    while(!epic_idle) {
    }
#endif
}

#endif /*LV_USE_SIFLI_EPIC*/
