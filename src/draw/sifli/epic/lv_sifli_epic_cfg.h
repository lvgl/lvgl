/**
 * @file lv_sifli_epic_cfg.h
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_SIFLI_EPIC_CFG_H
#define LV_SIFLI_EPIC_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_SIFLI_EPIC

/* Include SiFli HAL headers - these should be provided by the platform */
#include "bf0_hal.h"
#include "../../../draw/lv_image_dsc.h"

#include "../../../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

/** EPIC module instance to use (platform-specific) */
#ifndef LV_SIFLI_EPIC_INSTANCE
#define LV_SIFLI_EPIC_INSTANCE EPIC
#endif

/** EPIC interrupt line ID (platform-specific) */
#ifndef LV_SIFLI_EPIC_IRQn
#define LV_SIFLI_EPIC_IRQn EPIC_IRQn
#endif

/** EPIC interrupt priority (platform-specific) */
#ifndef LV_SIFLI_EPIC_IRQ_PRIORITY
#define LV_SIFLI_EPIC_IRQ_PRIORITY 5
#endif

#ifdef HAL_EZIP_MODULE_ENABLED
/** EZIP interrupt line ID (platform-specific) */
#ifndef LV_SIFLI_EZIP_IRQn
#define LV_SIFLI_EZIP_IRQn EZIP_IRQn
#endif
#endif

#if LV_USE_OS == LV_OS_FREERTOS
#if defined(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)
#ifndef LV_SIFLI_EPIC_FREERTOS_IRQ_PRIORITY
#define LV_SIFLI_EPIC_FREERTOS_IRQ_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#endif
#endif
#endif

#ifdef __ZEPHYR__
#ifndef LV_SIFLI_EPIC_ZEPHYR_IRQ_FLAGS
#define LV_SIFLI_EPIC_ZEPHYR_IRQ_FLAGS 0
#endif
#endif

/** Maximum pixels for single EPIC operation */
#define EPIC_MAX_PIXELS (1000 * 1000)

/*
 * Keep SiFli image extension flags local to the adapter if the host LVGL
 * integration hasn't provided them already.
 */
#ifndef LV_IMAGE_FLAGS_EZIP
#define LV_IMAGE_FLAGS_EZIP LV_IMAGE_FLAGS_USER1
#endif

#ifndef LV_IMAGE_FLAGS_JPEG
#define LV_IMAGE_FLAGS_JPEG LV_IMAGE_FLAGS_USER2
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_epic_cplt_cbk)(EPIC_HandleTypeDef * epic);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Reset and initialize EPIC device. This function should be called as part
 * of display init sequence.
 */
void lv_epic_init(void);

/**
 * Disable EPIC device. Should be called during display deinit sequence.
 */
void lv_epic_deinit(void);

/**
 * Reset EPIC device.
 */
void lv_epic_reset(void);

/**
 * Check whether the external EPIC adapter has completed initialization.
 * @return true if initialized, false otherwise
 */
bool lv_epic_is_initialized(void);

/**
 * Check whether the external EPIC adapter currently owns an in-flight job.
 * @return true if EPIC/EZIP is busy, false otherwise
 */
bool lv_epic_is_busy(void);

/**
 * Clear cache and start EPIC.
 */
void lv_epic_run(void);

/**
 * Wait for EPIC completion.
 */
void lv_epic_wait(void);

/**
 * Get EPIC HAL handle.
 * @return Pointer to EPIC handle
 */
EPIC_HandleTypeDef * lv_epic_get_handle(void);

#ifdef HAL_EZIP_MODULE_ENABLED
EZIP_HandleTypeDef * lv_ezip_get_handle(void);
#endif

/**
 * Flush a cache range before EPIC reads CPU-generated data.
 * @param data Start address
 * @param size Range size in bytes
 */
void lv_epic_flush_cache_range(const void * data, uint32_t size);

/**
 * Clean and invalidate a cache range before EPIC reads/writes a shared buffer.
 * @param data Start address
 * @param size Range size in bytes
 */
void lv_epic_invalidate_cache_range(const void * data, uint32_t size);

/**
 * Check if memory region is cached.
 * @param start Start address
 * @param len Memory length
 * @return true if cached, false otherwise
 */
bool lv_epic_is_cached_ram(uint32_t start, uint32_t len);

HAL_StatusTypeDef lv_epic_fill(EPIC_LayerConfigTypeDef * input_layers, uint8_t input_layer_cnt,
                               EPIC_LayerConfigTypeDef * output_layer);
HAL_StatusTypeDef lv_epic_blend(EPIC_LayerConfigTypeDef * input_layers, uint8_t input_layer_cnt,
                                EPIC_LayerConfigTypeDef * output_layer);
HAL_StatusTypeDef lv_epic_fill_grad(EPIC_GradCfgTypeDef * param);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SIFLI_EPIC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SIFLI_EPIC_CFG_H*/
