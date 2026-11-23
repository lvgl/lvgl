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
#include "../../../osal/lv_os_private.h"

#if !LV_IRQ_PROVIDES_EPIC
    #error "EPIC IRQ provider missing: enable LV_IRQ_PROVIDES_EPIC"
#endif
#if defined(HAL_EZIP_MODULE_ENABLED) && !LV_IRQ_PROVIDES_EZIP
    #error "EZIP IRQ provider missing: enable LV_IRQ_PROVIDES_EZIP"
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
static void _epic_irq_cb(void * user_data);
#ifdef HAL_EZIP_MODULE_ENABLED
    static void _ezip_irq_cb(void * user_data);
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

static void _epic_irq_cb(void * user_data)
{
    LV_UNUSED(user_data);

    EPIC_HandleTypeDef * epic_handle = lv_epic_get_handle();
    if(epic_handle != NULL) {
        HAL_EPIC_IRQHandler(epic_handle);
    }
}

#ifdef HAL_EZIP_MODULE_ENABLED
static void _ezip_irq_cb(void * user_data)
{
    LV_UNUSED(user_data);

    EZIP_HandleTypeDef * ezip_handle = lv_ezip_get_handle();
    if(ezip_handle != NULL) {
        (void)HAL_EZIP_IRQHandler(ezip_handle);
    }
}
#endif

static void _epic_interrupt_init(void)
{
    epic_idle = true;

#ifdef HAL_EZIP_MODULE_ENABLED
    lv_irq_attach_ezip(_ezip_irq_cb, NULL);
#endif
    lv_irq_attach_epic(_epic_irq_cb, NULL);
}

static void _epic_interrupt_deinit(void)
{
    lv_irq_detach_epic();
#ifdef HAL_EZIP_MODULE_ENABLED
    lv_irq_detach_ezip();
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
