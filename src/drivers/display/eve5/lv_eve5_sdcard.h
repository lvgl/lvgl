/**
 * @file lv_eve5_sdcard.h
 *
 * EVE5 (BT820) SD Card Filesystem Driver for LVGL
 *
 * Provides LVGL filesystem access to the SD card connected to BT820.
 * Uses BT820's coprocessor commands (CMD_FSREAD, CMD_FSSIZE, CMD_FSDIR)
 * to read files from the FAT filesystem on the SD card.
 *
 * Architecture:
 * - Files are loaded into RAM_G on open and stay there (read-only access)
 * - Read operations fetch directly from RAM_G
 * - Use lv_eve5_sdcard_steal_ramg() to take ownership of the RAM_G allocation
 *   for direct GPU use (zero-copy path for textures)
 * - Write operations are not supported (SD card is read-only through this driver)
 * - Directory enumeration uses CMD_FSDIR with temporary RAM_G buffer
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_EVE5_SDCARD_H
#define LV_EVE5_SDCARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD

#include "../../../display/lv_display.h"
#include "Esd_GpuAlloc.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_FS_EVE5_SDCARD_LETTER
#define LV_FS_EVE5_SDCARD_LETTER 'S'
#endif

#ifndef LV_FS_EVE5_SDCARD_CACHE_SIZE
#define LV_FS_EVE5_SDCARD_CACHE_SIZE 0
#endif

/** Temporary RAM_G buffer size for directory enumeration */
#ifndef LV_EVE5_SDCARD_DIR_BUFFER_SIZE
#define LV_EVE5_SDCARD_DIR_BUFFER_SIZE 4096
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the EVE5 SD card filesystem driver.
 * Must be called after lv_eve5_create().
 *
 * This registers an LVGL filesystem driver with the configured drive letter
 * (default 'S'). Files can then be accessed as "S:/path/to/file.ext".
 *
 * @param disp pointer to the EVE5 display created by lv_eve5_create()
 */
void lv_fs_eve5_sdcard_init(lv_display_t *disp);

/**
 * Check if the SD card is attached and ready.
 * Attempts to attach the SD card if not already attached.
 *
 * @return true if SD card is ready, false otherwise
 */
bool lv_eve5_sdcard_ready(void);

/**
 * Deinitialize the EVE5 SD card filesystem driver.
 * Unregisters the filesystem driver. Any open files should be closed first.
 */
void lv_fs_eve5_sdcard_deinit(void);

/**
 * Steal the RAM_G allocation from an open file for direct GPU use.
 *
 * This provides a zero-copy path for loading textures from SD card directly
 * into GPU memory. After calling this function:
 * - The caller owns the GPU handle and must free it via Esd_GpuAlloc_Free()
 * - The file handle should be closed (reads will fail)
 * - Use Esd_GpuAlloc_Get(alloc, handle) to get the current RAM_G address
 *
 * @param file_p    File handle returned by lv_fs_open()
 * @param alloc     Pointer to receive the GPU allocator (needed for address lookup and freeing)
 * @param handle    Pointer to receive the GPU handle
 * @param size      Pointer to receive the file size in bytes
 * @return          true if successful, false if already stolen
 */
bool lv_eve5_sdcard_steal_ramg(void *file_p, Esd_GpuAlloc **alloc, Esd_GpuHandle *handle, uint32_t *size);

/**
 * Check if a path is for the EVE5 SD card filesystem.
 * Paths should be in the format "X:/path" where X is the configured drive letter.
 *
 * @param path      Path to check
 * @return          true if path uses the EVE5 SD card driver
 */
bool lv_eve5_sdcard_is_path(const char *path);

/**
 * Load and decode a JPEG/PNG image from SD card directly to RAM_G.
 *
 * This provides a zero-copy decode path using EVE's hardware decoder:
 * 1. Load compressed file from SD card to temporary RAM_G
 * 2. Parse header to get dimensions
 * 3. Decode via CMD_LOADIMAGE with OPT_MEDIAFIFO
 * 4. Free temporary allocation
 *
 * The caller owns the returned GPU handle and must free it via
 * Esd_GpuAlloc_Free(lv_eve5_sdcard_get_allocator(), handle).
 *
 * @param path      Full path including drive letter (e.g., "S:/image.jpg")
 * @param handle    Pointer to receive the GPU handle
 * @param width     Pointer to receive image width
 * @param height    Pointer to receive image height
 * @param format    Pointer to receive EVE bitmap format (e.g., RGB565)
 * @return          true on success, false on failure
 */
bool lv_eve5_sdcard_load_image(const char *path, Esd_GpuHandle *handle,
                                uint32_t *width, uint32_t *height, uint32_t *format);

/**
 * Get the GPU allocator used by the SD card driver.
 * Useful for freeing handles returned by lv_eve5_sdcard_load_image().
 *
 * @return          Pointer to the GPU allocator, or NULL if not initialized
 */
Esd_GpuAlloc *lv_eve5_sdcard_get_allocator(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_SDCARD_H */
