/**
 * @file lv_sifli_epic_cfg.c
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

#include "lv_sifli_epic_cfg.h"
#include "lv_sifli_epic_osa.h"

#if LV_USE_SIFLI_EPIC
#include "../../../misc/lv_log.h"
#include "system_bf0_ap.h"
#include <string.h>

#if defined(__ZEPHYR__)
    #include <zephyr/arch/cache.h>
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

/**********************
 *  STATIC VARIABLES
 **********************/

static EPIC_HandleTypeDef epic_handle;
#ifdef HAL_EZIP_MODULE_ENABLED
    static EZIP_HandleTypeDef ezip_handle;
#endif
static bool epic_initialized = false;
static lv_epic_cplt_cbk epic_async_cb;
static uint32_t epic_hw_start_log_cnt;
static uint32_t epic_irq_log_cnt;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void epic_xfer_cplt_callback(EPIC_HandleTypeDef * epic);
static HAL_StatusTypeDef epic_wait_async_result(HAL_StatusTypeDef start_status);
static HAL_StatusTypeDef epic_prepare_start(EPIC_HandleTypeDef * epic,
                                            lv_epic_cplt_cbk cb);
static void epic_start_failed_cleanup(void);
static void lv_epic_flush_cache(void);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_epic_init(void)
{
    if(epic_initialized) {
        return;
    }

    memset(&epic_handle, 0, sizeof(epic_handle));
    epic_handle.Instance = LV_SIFLI_EPIC_INSTANCE;

#ifdef HAL_EZIP_MODULE_ENABLED
    memset(&ezip_handle, 0, sizeof(ezip_handle));
    ezip_handle.Instance = EZIP;
    epic_handle.hezip = &ezip_handle;

    if(HAL_EZIP_Init(epic_handle.hezip) != HAL_OK) {
        return;
    }
#endif

    if(HAL_EPIC_Init(&epic_handle) != HAL_OK) {
        return;
    }

    if(lv_epic_osa_init() != LV_RESULT_OK) {
        memset(&epic_handle, 0, sizeof(epic_handle));
#ifdef HAL_EZIP_MODULE_ENABLED
        memset(&ezip_handle, 0, sizeof(ezip_handle));
#endif
        return;
    }

    epic_initialized = true;
}

void lv_epic_deinit(void)
{
    if(!epic_initialized) {
        return;
    }

    lv_epic_wait();
    lv_epic_osa_deinit();

    memset(&epic_handle, 0, sizeof(epic_handle));
#ifdef HAL_EZIP_MODULE_ENABLED
    memset(&ezip_handle, 0, sizeof(ezip_handle));
#endif
    epic_async_cb = NULL;
    epic_initialized = false;
}

void lv_epic_reset(void)
{
    if(!epic_initialized) {
        return;
    }

    (void)HAL_EPIC_Init(&epic_handle);
}

bool lv_epic_is_initialized(void)
{
    return epic_initialized;
}

bool lv_epic_is_busy(void)
{
    if(!epic_initialized) {
        return false;
    }

    if(epic_handle.State == HAL_EPIC_STATE_BUSY) {
        return true;
    }

#ifdef HAL_EZIP_MODULE_ENABLED
    if(ezip_handle.State == HAL_EZIP_STATE_BUSY) {
        return true;
    }
#endif

    return false;
}

void lv_epic_run(void)
{
    epic_osa_cfg_t * cfg = epic_get_default_cfg();
    if(cfg && cfg->epic_run) {
        cfg->epic_run();
    }
}

void lv_epic_wait(void)
{
    epic_osa_cfg_t * cfg = epic_get_default_cfg();
    if(cfg && cfg->epic_wait) {
        cfg->epic_wait();
    }
}

EPIC_HandleTypeDef * lv_epic_get_handle(void)
{
    if(!epic_initialized) {
        return NULL;
    }

    return &epic_handle;
}

#ifdef HAL_EZIP_MODULE_ENABLED
EZIP_HandleTypeDef * lv_ezip_get_handle(void)
{
    if(!epic_initialized) {
        return NULL;
    }

    return &ezip_handle;
}
#endif

static void lv_epic_flush_cache(void)
{
#if defined(__ZEPHYR__)
    (void)arch_dcache_flush_all();
#else
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    /* Flush D-Cache if present */
    SCB_CleanDCache();
#endif
#endif
}

void lv_epic_flush_cache_range(const void * data, uint32_t size)
{
    if(data == NULL || size == 0U) {
        return;
    }

#if defined(__ZEPHYR__)
    (void)arch_dcache_flush_range((void *)data, size);
#else
    mpu_dcache_clean((void *)data, size);
#endif
}

void lv_epic_invalidate_cache_range(const void * data, uint32_t size)
{
    if(data == NULL || size == 0U) {
        return;
    }

#if defined(__ZEPHYR__)
    (void)arch_dcache_invd_range((void *)data, size);
#else
    mpu_dcache_invalidate((void *)data, size);
#endif
}

bool lv_epic_is_cached_ram(uint32_t start, uint32_t len)
{
    /* Platform-specific implementation
     * This should check if the memory region is in cached RAM.
     * For now, return a conservative default.
     */
    LV_UNUSED(start);
    LV_UNUSED(len);

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    /* If D-Cache is present, assume memory might be cached */
    return true;
#else
    return false;
#endif
}

HAL_StatusTypeDef lv_epic_fill(EPIC_LayerConfigTypeDef * input_layers, uint8_t input_layer_cnt,
                               EPIC_LayerConfigTypeDef * output_layer)
{
    EPIC_HandleTypeDef * epic = lv_epic_get_handle();

    if(epic_prepare_start(epic, NULL) != HAL_OK) {
        return HAL_ERROR;
    }
    HAL_StatusTypeDef start_status = HAL_EPIC_BlendStartEx_IT(epic, input_layers, input_layer_cnt, output_layer);
    return epic_wait_async_result(start_status);
}

HAL_StatusTypeDef lv_epic_blend(EPIC_LayerConfigTypeDef * input_layers, uint8_t input_layer_cnt,
                                EPIC_LayerConfigTypeDef * output_layer)
{
    EPIC_HandleTypeDef * epic = lv_epic_get_handle();

    if(epic_prepare_start(epic, NULL) != HAL_OK) {
        return HAL_ERROR;
    }
    HAL_StatusTypeDef start_status = HAL_EPIC_BlendStartEx_IT(epic, input_layers, input_layer_cnt, output_layer);
    return epic_wait_async_result(start_status);
}

HAL_StatusTypeDef lv_epic_fill_grad(EPIC_GradCfgTypeDef * param)
{
    EPIC_HandleTypeDef * epic = lv_epic_get_handle();

    if(epic_prepare_start(epic, NULL) != HAL_OK) {
        return HAL_ERROR;
    }
    HAL_StatusTypeDef start_status = HAL_EPIC_FillGrad_IT(epic, param);
    return epic_wait_async_result(start_status);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void epic_xfer_cplt_callback(EPIC_HandleTypeDef * epic)
{
    if(epic_irq_log_cnt < 8) {
        LV_LOG_WARN("EPIC irq done[%u]: state=%d",
                    (unsigned int)epic_irq_log_cnt,
                    (int)epic->State);
        epic_irq_log_cnt++;
    }

    lv_epic_osa_thread_sync_signal_isr();

    if(epic_async_cb != NULL) {
        lv_epic_cplt_cbk cb = epic_async_cb;
        epic_async_cb = NULL;
        cb(epic);
    }
}

static HAL_StatusTypeDef epic_wait_async_result(HAL_StatusTypeDef start_status)
{
    if(start_status != HAL_OK) {
        LV_LOG_WARN("EPIC start failed: status=%d", (int)start_status);
        epic_start_failed_cleanup();
        return start_status;
    }

    lv_epic_wait();
    return HAL_OK;
}

static HAL_StatusTypeDef epic_prepare_start(EPIC_HandleTypeDef * epic,
                                            lv_epic_cplt_cbk cb)
{
    if(epic == NULL) {
        return HAL_ERROR;
    }

    if(lv_epic_is_busy()) {
        lv_epic_wait();
    }

    lv_epic_flush_cache();
    epic_async_cb = cb;
    epic->XferCpltCallback = epic_xfer_cplt_callback;
    lv_epic_run();
    if(epic_hw_start_log_cnt < 8) {
        LV_LOG_WARN("EPIC hw start[%u]: state=%d",
                    (unsigned int)epic_hw_start_log_cnt,
                    (int)epic->State);
        epic_hw_start_log_cnt++;
    }
    return HAL_OK;
}

static void epic_start_failed_cleanup(void)
{
    epic_handle.XferCpltCallback = NULL;
    epic_async_cb = NULL;
    lv_epic_osa_set_idle();
}

#endif /*LV_USE_SIFLI_EPIC*/
