/**
 * @file lv_eve5_flash.h
 *
 * EVE5 (BT820) Flash Filesystem Driver for LVGL
 *
 * Provides LVGL filesystem access to the QSPI flash connected to EVE (BT815+).
 * Uses EVE's coprocessor commands (CMD_FLASHREAD, CMD_FLASHSOURCE, CMD_LOADIMAGE)
 * to read data and decode images from flash memory.
 *
 * Architecture:
 * - Flash is a raw byte array — no filesystem, no directories
 * - Path format: "F:/123456" where 123456 is the decimal flash byte address
 * - Reads use CMD_FLASHREAD with temporary RAM_G allocations (freed after copy to host)
 * - Image loading uses CMD_FLASHSOURCE + CMD_LOADIMAGE with OPT_FLASH for zero-copy decode
 * - Write operations are not supported through this driver
 * - Directory enumeration is not supported (flash has no filesystem)
 *
 * Alignment constraints (from EVE HAL):
 * - CMD_FLASHREAD: dest 4-byte aligned, src 64-byte aligned, num multiple of 4
 * - CMD_FLASHSOURCE: address must be 64-byte aligned
 * - Image loading (OPT_FLASH) requires 64-byte aligned flash address
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_EVE5_FLASH_H
#define LV_EVE5_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_EVE5 && LV_USE_FS_EVE5_FLASH

#include "../../../display/lv_display.h"
#include "Esd_GpuAlloc.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_FS_EVE5_FLASH_LETTER
#define LV_FS_EVE5_FLASH_LETTER 'F'
#endif

#ifndef LV_FS_EVE5_FLASH_CACHE_SIZE
#define LV_FS_EVE5_FLASH_CACHE_SIZE 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the EVE5 flash filesystem driver.
 * Must be called after lv_eve5_create().
 *
 * This registers an LVGL filesystem driver with the configured drive letter
 * (default 'F'). Files can then be accessed as "F:/123456" where 123456
 * is the decimal flash byte address.
 *
 * @param disp pointer to the EVE5 display created by lv_eve5_create()
 */
void lv_fs_eve5_flash_init(lv_display_t *disp);

/**
 * Check if flash is attached and ready.
 * Attempts to attach and enter fast mode if not already done.
 *
 * @return true if flash is ready, false otherwise
 */
bool lv_eve5_flash_ready(void);

/**
 * Deinitialize the EVE5 flash filesystem driver.
 */
void lv_fs_eve5_flash_deinit(void);

/**
 * Check if a path is for the EVE5 flash filesystem.
 * Paths should be in the format "X:/address" where X is the configured drive letter.
 *
 * @param path      Path to check
 * @return          true if path uses the EVE5 flash driver
 */
bool lv_eve5_flash_is_path(const char *path);

/**
 * Load and decode a JPEG/PNG image from flash directly to RAM_G.
 *
 * Uses CMD_FLASHSOURCE + CMD_LOADIMAGE with OPT_FLASH for zero-copy decode.
 * The flash address must be 64-byte aligned.
 *
 * The caller owns the returned GPU handle and must free it via
 * Esd_GpuAlloc_Free(lv_eve5_flash_get_allocator(), handle).
 *
 * @param path            Full path including drive letter (e.g., "F:/4096")
 * @param handle          Pointer to receive the GPU handle
 * @param width           Pointer to receive image width
 * @param height          Pointer to receive image height
 * @param format          Pointer to receive EVE bitmap format (e.g., RGB8, ARGB8, PALETTEDARGB8)
 * @param image_offset    Pointer to receive bitmap data offset from handle base
 * @param palette_offset  Pointer to receive palette LUT offset from handle base (GA_INVALID if non-paletted)
 * @return                true on success, false on failure
 */
bool lv_eve5_flash_load_image(const char *path, Esd_GpuHandle *handle,
                               uint32_t *width, uint32_t *height, uint32_t *format,
                               uint32_t *image_offset, uint32_t *palette_offset);

/**
 * Get the GPU allocator used by the flash driver.
 * Useful for freeing handles returned by lv_eve5_flash_load_image().
 *
 * @return          Pointer to the GPU allocator, or NULL if not initialized
 */
Esd_GpuAlloc *lv_eve5_flash_get_allocator(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_EVE5 && LV_USE_FS_EVE5_FLASH */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_FLASH_H */
