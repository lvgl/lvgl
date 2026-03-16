/**
 * @file lv_eve5_sdcard.c
 *
 * EVE5 (BT820) SD Card Filesystem Driver for LVGL
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD

#include "lv_eve5_sdcard.h"
#include "lv_eve5.h"
#include "../../../core/lv_global.h"
#include "../../../misc/lv_fs.h"
#include "../../../stdlib/lv_mem.h"
#include "../../../stdlib/lv_string.h"

#include "EVE_Hal.h"
#include "EVE_CoCmd.h"
#include "EVE_MediaFifo.h"
#include "Esd_GpuAlloc.h"

/*********************
 *      DEFINES
 *********************/

/** Maximum file size we can load (limited by RAM_G allocation) */
#ifndef LV_EVE5_SDCARD_MAX_FILE_SIZE
#define LV_EVE5_SDCARD_MAX_FILE_SIZE (16 * 1024 * 1024)  /* 16 MB */
#endif

/**********************
 *      TYPEDEFS
 **********************/

/** File handle - holds file data in RAM_G */
typedef struct {
    Esd_GpuHandle gpu_handle;  /**< GPU allocation handle (GA_HANDLE_INVALID if not loaded or stolen) */
    uint32_t size;             /**< Total file size */
    uint32_t pos;              /**< Current read position */
    char *path;                /**< File path on SD card (non-NULL = not yet loaded into RAM_G) */
} eve5_file_t;

/** Directory handle */
typedef struct {
    uint8_t *listing;    /**< Directory listing buffer */
    uint32_t size;       /**< Size of listing data */
    uint32_t pos;        /**< Current position in listing */
} eve5_dir_t;

/** Driver context */
typedef struct {
    lv_display_t *disp;
    EVE_HalContext *hal;
    Esd_GpuAlloc *alloc;
    bool sd_attached;
    lv_fs_drv_t fs_drv;
} eve5_sdcard_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool fs_ready(lv_fs_drv_t *drv);
static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);
static void *fs_dir_open(lv_fs_drv_t *drv, const char *path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *dir_p, char *fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *dir_p);

static bool ensure_sd_attached(eve5_sdcard_ctx_t *ctx);
static bool ensure_file_loaded(eve5_sdcard_ctx_t *ctx, eve5_file_t *file);

/**********************
 *  STATIC VARIABLES
 **********************/
static eve5_sdcard_ctx_t s_ctx;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_eve5_sdcard_init(lv_display_t *disp)
{
    if(disp == NULL) return;

    EVE_HalContext *hal = lv_eve5_get_hal(disp);
    Esd_GpuAlloc *alloc = lv_eve5_get_allocator(disp);

    if(hal == NULL || alloc == NULL) {
        LV_LOG_ERROR("EVE5 HAL or allocator not available");
        return;
    }

    /* Store context */
    lv_memzero(&s_ctx, sizeof(s_ctx));
    s_ctx.disp = disp;
    s_ctx.hal = hal;
    s_ctx.alloc = alloc;
    s_ctx.sd_attached = false;

    /* Initialize and register the filesystem driver */
    lv_fs_drv_init(&s_ctx.fs_drv);

    s_ctx.fs_drv.letter = LV_FS_EVE5_SDCARD_LETTER;
    s_ctx.fs_drv.cache_size = LV_FS_EVE5_SDCARD_CACHE_SIZE;
    s_ctx.fs_drv.user_data = &s_ctx;

    s_ctx.fs_drv.ready_cb = fs_ready;
    s_ctx.fs_drv.open_cb = fs_open;
    s_ctx.fs_drv.close_cb = fs_close;
    s_ctx.fs_drv.read_cb = fs_read;
    s_ctx.fs_drv.write_cb = fs_write;
    s_ctx.fs_drv.seek_cb = fs_seek;
    s_ctx.fs_drv.tell_cb = fs_tell;

    s_ctx.fs_drv.dir_open_cb = fs_dir_open;
    s_ctx.fs_drv.dir_read_cb = fs_dir_read;
    s_ctx.fs_drv.dir_close_cb = fs_dir_close;

    lv_fs_drv_register(&s_ctx.fs_drv);

    LV_LOG_INFO("EVE5 SD card filesystem driver registered with letter '%c'", LV_FS_EVE5_SDCARD_LETTER);
}

bool lv_eve5_sdcard_ready(void)
{
#if LV_USE_OS
    if(s_ctx.disp) lv_eve5_hal_lock(s_ctx.disp);
#endif
    bool ready = ensure_sd_attached(&s_ctx);
#if LV_USE_OS
    if(s_ctx.disp) lv_eve5_hal_unlock(s_ctx.disp);
#endif
    return ready;
}

void lv_fs_eve5_sdcard_deinit(void)
{
    /* Note: LVGL doesn't provide a way to unregister filesystem drivers,
     * so we just clear our context. The driver will return errors if used. */
    lv_memzero(&s_ctx, sizeof(s_ctx));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Check if the filesystem is ready
 */
static bool fs_ready(lv_fs_drv_t *drv)
{
    eve5_sdcard_ctx_t *ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return false;

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif
    bool ready = ensure_sd_attached(ctx);
#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif
    return ready;
}

/**
 * Open a file
 * Gets file size and stores path. Actual loading into RAM_G is deferred to first read.
 * Use lv_eve5_sdcard_steal_ramg() to take ownership for direct GPU use.
 */
static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    eve5_sdcard_ctx_t *ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return NULL;

    /* Only read mode is supported */
    if(mode != LV_FS_MODE_RD) {
        LV_LOG_WARN("EVE5 SD card driver only supports read mode");
        return NULL;
    }

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif

    /* Ensure SD card is attached */
    if(!ensure_sd_attached(ctx)) {
        LV_LOG_ERROR("SD card not attached");
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return NULL;
    }

    EVE_HalContext *phost = ctx->hal;

    /* Get file size */
    uint32_t file_size = EVE_CoCmd_fsSize(phost, path);
    if(file_size == 0xFFFFFFFF || file_size == 0) {
        LV_LOG_WARN("File not found or empty: %s", path);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return NULL;
    }

    if(file_size > LV_EVE5_SDCARD_MAX_FILE_SIZE) {
        LV_LOG_ERROR("File too large: %u bytes (max %u)", file_size, LV_EVE5_SDCARD_MAX_FILE_SIZE);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return NULL;
    }

#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif

    /* Allocate file handle with stored path for deferred loading */
    size_t path_len = lv_strlen(path);
    eve5_file_t *file = lv_malloc(sizeof(eve5_file_t));
    if(file == NULL) {
        LV_LOG_ERROR("Failed to allocate file handle");
        return NULL;
    }

    file->path = lv_malloc(path_len + 1);
    if(file->path == NULL) {
        LV_LOG_ERROR("Failed to allocate path string");
        lv_free(file);
        return NULL;
    }
    lv_memcpy(file->path, path, path_len + 1);

    file->gpu_handle = GA_HANDLE_INVALID;
    file->size = file_size;
    file->pos = 0;

    LV_LOG_INFO("Opened file: %s (%u bytes, deferred load)", path, file->size);
    return file;
}

/**
 * Close a file
 */
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    eve5_sdcard_ctx_t *ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    eve5_file_t *file = (eve5_file_t *)file_p;

    if(file != NULL) {
        /* Free RAM_G allocation if loaded and still owned */
        if(ctx != NULL && ctx->alloc != NULL
           && Esd_GpuAlloc_Get(ctx->alloc, file->gpu_handle) != GA_INVALID) {
            Esd_GpuAlloc_Free(ctx->alloc, file->gpu_handle);
        }
        if(file->path != NULL) {
            lv_free(file->path);
        }
        lv_free(file);
    }

    return LV_FS_RES_OK;
}

/**
 * Read from a file
 * On first read, loads the file from SD card into RAM_G. Subsequent reads fetch directly from RAM_G.
 */
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    eve5_sdcard_ctx_t *ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    eve5_file_t *file = (eve5_file_t *)file_p;

    if(file == NULL || ctx == NULL || ctx->hal == NULL || ctx->alloc == NULL) {
        if(br) *br = 0;
        return LV_FS_RES_INV_PARAM;
    }

    /* Deferred load: load file into RAM_G on first read */
    if(file->path != NULL) {
        if(!ensure_file_loaded(ctx, file)) {
            if(br) *br = 0;
            return LV_FS_RES_FS_ERR;
        }
    }

    /* Get current RAM_G address (returns GA_INVALID if handle was stolen/freed) */
    uint32_t ramg_addr = Esd_GpuAlloc_Get(ctx->alloc, file->gpu_handle);
    if(ramg_addr == GA_INVALID) {
        if(br) *br = 0;
        return LV_FS_RES_FS_ERR;
    }

    /* Calculate how much we can actually read */
    uint32_t available = file->size - file->pos;
    uint32_t to_read = btr < available ? btr : available;

    if(to_read > 0) {
#if LV_USE_OS
        lv_eve5_hal_lock(ctx->disp);
#endif
        EVE_Hal_rdMem(ctx->hal, buf, ramg_addr + file->pos, to_read);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        file->pos += to_read;
    }

    if(br) *br = to_read;

    return LV_FS_RES_OK;
}

/**
 * Write to a file (not supported)
 */
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    LV_UNUSED(drv);
    LV_UNUSED(file_p);
    LV_UNUSED(buf);
    LV_UNUSED(btw);

    if(bw) *bw = 0;
    return LV_FS_RES_NOT_IMP;
}

/**
 * Seek to a position in a file
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    eve5_file_t *file = (eve5_file_t *)file_p;

    if(file == NULL) return LV_FS_RES_INV_PARAM;

    int32_t new_pos;
    switch(whence) {
        case LV_FS_SEEK_SET:
            new_pos = (int32_t)pos;
            break;
        case LV_FS_SEEK_CUR:
            new_pos = (int32_t)file->pos + (int32_t)pos;
            break;
        case LV_FS_SEEK_END:
            new_pos = (int32_t)file->size + (int32_t)pos;
            break;
        default:
            return LV_FS_RES_INV_PARAM;
    }

    /* Clamp to valid range */
    if(new_pos < 0) new_pos = 0;
    if((uint32_t)new_pos > file->size) new_pos = (int32_t)file->size;

    file->pos = (uint32_t)new_pos;
    return LV_FS_RES_OK;
}

/**
 * Get current position in a file
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    LV_UNUSED(drv);
    eve5_file_t *file = (eve5_file_t *)file_p;

    if(file == NULL || pos_p == NULL) return LV_FS_RES_INV_PARAM;

    *pos_p = file->pos;
    return LV_FS_RES_OK;
}

/**
 * Open a directory for reading
 */
static void *fs_dir_open(lv_fs_drv_t *drv, const char *path)
{
    eve5_sdcard_ctx_t *ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return NULL;

    /* Allocate directory handle */
    eve5_dir_t *dir = lv_malloc(sizeof(eve5_dir_t));
    if(dir == NULL) {
        LV_LOG_ERROR("Failed to allocate directory handle");
        return NULL;
    }

    /* Allocate buffer for directory listing */
    dir->listing = lv_malloc(LV_EVE5_SDCARD_DIR_BUFFER_SIZE);
    if(dir->listing == NULL) {
        LV_LOG_ERROR("Failed to allocate directory listing buffer");
        lv_free(dir);
        return NULL;
    }

    dir->size = 0;
    dir->pos = 0;

    /* Build the path - ensure it ends with a separator */
    char full_path[LV_FS_MAX_PATH_LENGTH];
    size_t path_len = lv_strlen(path);

    if(path_len == 0 || (path[0] == '/' && path_len == 1)) {
        /* Root directory */
        lv_strlcpy(full_path, "/", sizeof(full_path));
    }
    else {
        lv_strlcpy(full_path, path, sizeof(full_path));
        /* Ensure path ends with separator */
        if(full_path[path_len - 1] != '/' && full_path[path_len - 1] != '\\') {
            if(path_len + 1 < sizeof(full_path)) {
                full_path[path_len] = '/';
                full_path[path_len + 1] = '\0';
            }
        }
    }

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif

    if(!ensure_sd_attached(ctx)) {
        LV_LOG_ERROR("SD card not attached");
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        lv_free(dir->listing);
        lv_free(dir);
        return NULL;
    }

    EVE_HalContext *phost = ctx->hal;

    /* Allocate temporary RAM_G space for directory listing.
     * CMD_FSDIR likely requires 32-byte alignment like CMD_FSREAD (BT820). */
    Esd_GpuHandle gpu_handle = Esd_GpuAlloc_AlignedAlloc(ctx->alloc, LV_EVE5_SDCARD_DIR_BUFFER_SIZE, 0, 32);
    uint32_t ramg_addr = Esd_GpuAlloc_Get(ctx->alloc, gpu_handle);
    if(ramg_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate RAM_G space for directory listing");
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        lv_free(dir->listing);
        lv_free(dir);
        return NULL;
    }

    /* Read directory listing */
    uint32_t result = EVE_CoCmd_fsDir(phost, ramg_addr, LV_EVE5_SDCARD_DIR_BUFFER_SIZE, full_path);
    if(result != 0) {
        LV_LOG_WARN("Directory read failed with code %u: %s", result, full_path);
        Esd_GpuAlloc_Free(ctx->alloc, gpu_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        lv_free(dir->listing);
        lv_free(dir);
        return NULL;
    }

    /* Copy listing from RAM_G to host memory - get current address in case of defrag */
    ramg_addr = Esd_GpuAlloc_Get(ctx->alloc, gpu_handle);
    EVE_Hal_rdMem(phost, dir->listing, ramg_addr, LV_EVE5_SDCARD_DIR_BUFFER_SIZE);

    /* Free temporary RAM_G space */
    Esd_GpuAlloc_Free(ctx->alloc, gpu_handle);

#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif

    /* Find the actual size of the listing (it's null-terminated strings) */
    uint32_t i = 0;
    while(i < LV_EVE5_SDCARD_DIR_BUFFER_SIZE - 1) {
        if(dir->listing[i] == '\0') {
            /* Check for double-null termination (end of listing) */
            if(dir->listing[i + 1] == '\0') {
                dir->size = i + 2;
                break;
            }
        }
        i++;
    }
    if(i >= LV_EVE5_SDCARD_DIR_BUFFER_SIZE - 1) {
        dir->size = LV_EVE5_SDCARD_DIR_BUFFER_SIZE;
    }

    LV_LOG_INFO("Opened directory: %s", full_path);
    return dir;
}

/**
 * Read the next directory entry
 * CMD_FSDIR returns entries as null-terminated strings.
 * Directory names are indicated by a trailing '/' or '\' in the BT820 output.
 * LVGL expects directory names to start with '/'.
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *dir_p, char *fn, uint32_t fn_len)
{
    LV_UNUSED(drv);
    eve5_dir_t *dir = (eve5_dir_t *)dir_p;

    if(dir == NULL || fn == NULL || fn_len == 0) return LV_FS_RES_INV_PARAM;

    fn[0] = '\0';

    if(dir->listing == NULL || dir->pos >= dir->size) {
        return LV_FS_RES_OK;  /* End of directory */
    }

    /* Get next entry (null-terminated string) */
    const char *entry = (const char *)(dir->listing + dir->pos);
    size_t entry_len = lv_strlen(entry);

    if(entry_len == 0) {
        /* End of listing */
        return LV_FS_RES_OK;
    }

    /* Move position to next entry */
    dir->pos += (uint32_t)(entry_len + 1);

    /* Skip "." and ".." entries */
    if(lv_strcmp(entry, ".") == 0 || lv_strcmp(entry, "..") == 0) {
        /* Recurse to get next entry */
        return fs_dir_read(drv, dir_p, fn, fn_len);
    }

    /* Check if this is a directory (ends with '/' or '\') */
    bool is_dir = false;
    if(entry_len > 0) {
        char last_char = entry[entry_len - 1];
        if(last_char == '/' || last_char == '\\') {
            is_dir = true;
            entry_len--;  /* Don't include the trailing separator */
        }
    }

    /* Format output for LVGL: directories start with '/' */
    if(is_dir) {
        if(fn_len > 1) {
            fn[0] = '/';
            size_t copy_len = entry_len < fn_len - 2 ? entry_len : fn_len - 2;
            lv_memcpy(fn + 1, entry, copy_len);
            fn[copy_len + 1] = '\0';
        }
    }
    else {
        size_t copy_len = entry_len < fn_len - 1 ? entry_len : fn_len - 1;
        lv_memcpy(fn, entry, copy_len);
        fn[copy_len] = '\0';
    }

    return LV_FS_RES_OK;
}

/**
 * Close a directory
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *dir_p)
{
    LV_UNUSED(drv);
    eve5_dir_t *dir = (eve5_dir_t *)dir_p;

    if(dir != NULL) {
        if(dir->listing != NULL) {
            lv_free(dir->listing);
        }
        lv_free(dir);
    }

    return LV_FS_RES_OK;
}

/**
 * Load file from SD card into RAM_G (deferred from open).
 * Caller must NOT hold the HAL mutex.
 */
static bool ensure_file_loaded(eve5_sdcard_ctx_t *ctx, eve5_file_t *file)
{
    if(file->path == NULL) return true;  /* Already loaded */

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif

    if(!ensure_sd_attached(ctx)) {
        LV_LOG_ERROR("SD card not attached");
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return false;
    }

    EVE_HalContext *phost = ctx->hal;

    /* Allocate RAM_G space.
     * CMD_FSREAD requires 32-byte alignment on the destination address (BT820). */
    Esd_GpuHandle gpu_handle = Esd_GpuAlloc_AlignedAlloc(ctx->alloc, file->size, 0, 32);
    uint32_t ramg_addr = Esd_GpuAlloc_Get(ctx->alloc, gpu_handle);

    if(ramg_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate %u bytes in RAM_G for %s", file->size, file->path);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return false;
    }

    /* Read file from SD card to RAM_G */
    LV_LOG_USER("Loading file to RAM_G: %s (size=%u, addr=0x%08X)", file->path, file->size, ramg_addr);
    uint32_t result = EVE_CoCmd_fsRead(phost, ramg_addr, file->path);
    if(result == 0xFFFFFFFF) {
        LV_LOG_ERROR("Failed to read file from SD card: %s", file->path);
        Esd_GpuAlloc_Free(ctx->alloc, gpu_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return false;
    }
    LV_LOG_USER("Loaded file to RAM_G: %s (result=%u)", file->path, result);

#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif

    file->gpu_handle = gpu_handle;
    lv_free(file->path);
    file->path = NULL;

    return true;
}

/**
 * Ensure SD card is attached
 */
static bool ensure_sd_attached(eve5_sdcard_ctx_t *ctx)
{
    if(ctx == NULL || ctx->hal == NULL) return false;

    if(ctx->sd_attached) return true;

#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    EVE_HalContext *phost = ctx->hal;

    /* Try to attach the SD card */
    uint32_t result = EVE_CoCmd_sdAttach(phost, 0);
    if(result == 0) {
        ctx->sd_attached = true;
        LV_LOG_INFO("SD card attached successfully");
        return true;
    }
    else {
        LV_LOG_WARN("SD card attach failed with code %u", result);
        return false;
    }
#else
    return false;
#endif
}

/**
 * Steal the RAM_G allocation from an open file.
 * After this call, the caller owns the GPU memory handle and must free it.
 * The file handle should be closed after stealing (no further reads possible).
 *
 * @param file_p    File handle returned by lv_fs_open()
 * @param alloc     Pointer to receive the GPU allocator (needed for freeing)
 * @param handle    Pointer to receive the GPU handle
 * @param size      Pointer to receive the file size
 * @return          true if successful, false if already stolen
 */
bool lv_eve5_sdcard_steal_ramg(void *file_p, Esd_GpuAlloc **alloc, Esd_GpuHandle *handle, uint32_t *size)
{
    eve5_file_t *file = (eve5_file_t *)file_p;

    if(file == NULL || alloc == NULL || handle == NULL || size == NULL) return false;
    if(s_ctx.alloc == NULL) return false;

    /* Trigger deferred load if not yet loaded */
    if(file->path != NULL) {
        if(!ensure_file_loaded(&s_ctx, file)) {
            LV_LOG_ERROR("Cannot steal RAM_G: deferred load failed");
            return false;
        }
    }

    /* Can't steal if already stolen (handle resolves to GA_INVALID) */
    if(Esd_GpuAlloc_Get(s_ctx.alloc, file->gpu_handle) == GA_INVALID) {
        LV_LOG_WARN("Cannot steal RAM_G: already stolen or freed");
        return false;
    }

    /* Transfer ownership */
    *alloc = s_ctx.alloc;
    *handle = file->gpu_handle;
    *size = file->size;

    /* Clear file's ownership */
    file->gpu_handle = GA_HANDLE_INVALID;

    return true;
}

/**********************
 * IMAGE LOADING
 **********************/

/**
 * Check if a path is for the EVE5 SD card filesystem.
 * Paths should be in the format "X:/path" where X is the drive letter.
 */
bool lv_eve5_sdcard_is_path(const char *path)
{
    if(path == NULL || path[0] == '\0') return false;
    if(s_ctx.hal == NULL) return false;

    /* Check for drive letter format: "X:/" */
    if(path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        char letter = path[0];
        /* Case-insensitive comparison */
        if(letter >= 'a' && letter <= 'z') letter -= ('a' - 'A');
        char sd_letter = LV_FS_EVE5_SDCARD_LETTER;
        if(sd_letter >= 'a' && sd_letter <= 'z') sd_letter -= ('a' - 'A');
        return letter == sd_letter;
    }
    return false;
}

/**
 * Check if a file extension matches (case-insensitive).
 */
static bool has_extension(const char *path, const char *ext)
{
    size_t path_len = lv_strlen(path);
    size_t ext_len = lv_strlen(ext);
    if(path_len < ext_len + 1) return false;

    const char *path_ext = path + path_len - ext_len;
    for(size_t i = 0; i < ext_len; i++) {
        char c1 = path_ext[i];
        char c2 = ext[i];
        if(c1 >= 'A' && c1 <= 'Z') c1 += ('a' - 'A');
        if(c2 >= 'A' && c2 <= 'Z') c2 += ('a' - 'A');
        if(c1 != c2) return false;
    }
    return true;
}

/**
 * Parse JPEG header to extract image dimensions.
 * Looks for SOF0 (0xFFC0) or SOF2 (0xFFC2) markers.
 * Returns true if dimensions were found.
 */
static bool parse_jpeg_header(const uint8_t *data, uint32_t size, uint32_t *width, uint32_t *height)
{
    if(size < 11) return false;

    /* Check JPEG magic */
    if(data[0] != 0xFF || data[1] != 0xD8) return false;

    uint32_t pos = 2;
    while(pos + 4 < size) {
        if(data[pos] != 0xFF) {
            pos++;
            continue;
        }

        uint8_t marker = data[pos + 1];

        /* SOF0 (baseline) or SOF2 (progressive) */
        if(marker == 0xC0 || marker == 0xC2) {
            if(pos + 9 > size) return false;
            /* SOF segment: length(2) + precision(1) + height(2) + width(2) */
            *height = ((uint32_t)data[pos + 5] << 8) | data[pos + 6];
            *width = ((uint32_t)data[pos + 7] << 8) | data[pos + 8];
            return true;
        }

        /* Skip other markers */
        if(marker == 0xD8 || marker == 0xD9 || (marker >= 0xD0 && marker <= 0xD7)) {
            /* Standalone markers (no length) */
            pos += 2;
        }
        else if(pos + 4 <= size) {
            /* Markers with length */
            uint32_t len = ((uint32_t)data[pos + 2] << 8) | data[pos + 3];
            pos += 2 + len;
        }
        else {
            break;
        }
    }
    return false;
}

/**
 * Parse PNG header to extract image dimensions.
 * Dimensions are in the IHDR chunk immediately after the signature.
 * Returns true if dimensions were found.
 */
static bool parse_png_header(const uint8_t *data, uint32_t size, uint32_t *width, uint32_t *height)
{
    /* PNG signature (8 bytes) + IHDR length (4) + "IHDR" (4) + width (4) + height (4) = 24 bytes minimum */
    if(size < 24) return false;

    /* Check PNG signature */
    static const uint8_t png_sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    for(int i = 0; i < 8; i++) {
        if(data[i] != png_sig[i]) return false;
    }

    /* IHDR chunk should be first - check chunk type */
    if(data[12] != 'I' || data[13] != 'H' || data[14] != 'D' || data[15] != 'R') return false;

    /* Width and height are big-endian 32-bit values */
    *width = ((uint32_t)data[16] << 24) | ((uint32_t)data[17] << 16) |
             ((uint32_t)data[18] << 8) | data[19];
    *height = ((uint32_t)data[20] << 24) | ((uint32_t)data[21] << 16) |
              ((uint32_t)data[22] << 8) | data[23];

    return true;
}

/**
 * Load and decode a JPEG/PNG image from the SD card directly to RAM_G.
 *
 * This provides a zero-copy decode path:
 * 1. Load compressed file from SD card to temporary RAM_G (via CMD_FSREAD)
 * 2. Parse header to get dimensions (read header bytes to host)
 * 3. Allocate final decoded buffer in RAM_G
 * 4. Decode via CMD_LOADIMAGE with OPT_MEDIAFIFO (RAM_G to RAM_G)
 * 5. Free temporary allocation
 *
 * @param path      Full path including drive letter (e.g., "S:/image.jpg")
 * @param handle    Pointer to receive the GPU handle (caller must free via Esd_GpuAlloc_Free)
 * @param width     Pointer to receive image width
 * @param height    Pointer to receive image height
 * @param format    Pointer to receive EVE bitmap format (e.g., RGB565)
 * @return          true on success, false on failure
 */
bool lv_eve5_sdcard_load_image(const char *path, Esd_GpuHandle *handle,
                                uint32_t *width, uint32_t *height, uint32_t *format)
{
    if(path == NULL || handle == NULL || width == NULL || height == NULL || format == NULL) {
        return false;
    }

    if(s_ctx.hal == NULL || s_ctx.alloc == NULL) {
        LV_LOG_ERROR("EVE5 SD card driver not initialized");
        return false;
    }

    /* Check extension */
    bool is_jpeg = has_extension(path, ".jpg") || has_extension(path, ".jpeg");
    bool is_png = has_extension(path, ".png");
    if(!is_jpeg && !is_png) {
        LV_LOG_WARN("Unsupported image format (not JPEG/PNG): %s", path);
        return false;
    }

    /* Skip drive letter prefix for SD card commands */
    const char *sd_path = path;
    if(path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        sd_path = path + 2;
    }

    EVE_HalContext *phost = s_ctx.hal;
    Esd_GpuAlloc *alloc = s_ctx.alloc;

#if LV_USE_OS
    lv_eve5_hal_lock(s_ctx.disp);
#endif

    /* Ensure SD card is attached */
    if(!ensure_sd_attached(&s_ctx)) {
        LV_LOG_ERROR("SD card not attached");
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Get file size */
    uint32_t file_size = EVE_CoCmd_fsSize(phost, sd_path);
    if(file_size == 0xFFFFFFFF || file_size == 0) {
        LV_LOG_ERROR("File not found or empty: %s", sd_path);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Allocate temporary space for compressed file.
     * CMD_FSREAD requires 32-byte alignment on the destination address (BT820).
     * MEDIAFIFO requires size to be 4-byte aligned. */
    uint32_t fifo_size = (file_size + 3) & ~3U;  /* Round up to 4-byte alignment */
    Esd_GpuHandle temp_handle = Esd_GpuAlloc_AlignedAlloc(alloc, fifo_size, 0, 32);
    uint32_t temp_addr = Esd_GpuAlloc_Get(alloc, temp_handle);
    if(temp_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate temporary RAM_G for compressed file (%u bytes)", fifo_size);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Read compressed file from SD card to RAM_G */
    uint32_t result = EVE_CoCmd_fsRead(phost, temp_addr, sd_path);
    if(result != 0) {
        LV_LOG_ERROR("CMD_FSREAD failed with code %u: %s", result, sd_path);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Read header bytes to host for parsing (first 1KB should be enough) */
    uint8_t header_buf[1024];
    uint32_t header_size = file_size < sizeof(header_buf) ? file_size : sizeof(header_buf);
    temp_addr = Esd_GpuAlloc_Get(alloc, temp_handle);  /* Re-fetch in case of defrag */
    EVE_Hal_rdMem(phost, header_buf, temp_addr, header_size);

    /* Parse header to get dimensions */
    uint32_t img_width = 0, img_height = 0;
    bool parsed;
    if(is_jpeg) {
        parsed = parse_jpeg_header(header_buf, header_size, &img_width, &img_height);
    }
    else {
        parsed = parse_png_header(header_buf, header_size, &img_width, &img_height);
    }

    if(!parsed || img_width == 0 || img_height == 0) {
        LV_LOG_ERROR("Failed to parse image header: %s", path);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Calculate decoded size - CMD_LOADIMAGE outputs RGB565 by default */
    uint32_t decoded_stride = ((img_width * 2) + 3) & ~3U;  /* RGB565, 4-byte aligned */
    uint32_t decoded_size = decoded_stride * img_height;

    /* Allocate final buffer for decoded image */
    Esd_GpuHandle final_handle = Esd_GpuAlloc_Alloc(alloc, decoded_size, GA_ALIGN_4);
    uint32_t final_addr = Esd_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate decoded image buffer (%u bytes)", decoded_size);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Close any existing managed media FIFO before setting up ours */
    EVE_MediaFifo_close(phost);

    /* Set up MEDIAFIFO pointing at the compressed data in RAM_G.
     * Use EVE_MediaFifo_set() to properly manage host-side state. */
    temp_addr = Esd_GpuAlloc_Get(alloc, temp_handle);  /* Re-fetch in case of defrag */
    if(!EVE_MediaFifo_set(phost, temp_addr, fifo_size)) {
        LV_LOG_ERROR("Failed to set up media FIFO");
        Esd_GpuAlloc_Free(alloc, final_handle);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Set write pointer to indicate all data is present.
     * Normally data is streamed in via EVE_MediaFifo_wrMem(), but our data
     * is already in RAM_G from CMD_FSREAD, so we just set the pointer. */
    EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, file_size);

    /* Decode image via CMD_LOADIMAGE with OPT_MEDIAFIFO */
    EVE_CoCmd_loadImage(phost, final_addr, OPT_MEDIAFIFO | OPT_NODL);
    if(!EVE_Cmd_waitFlush(phost)) {
        LV_LOG_ERROR("CMD_LOADIMAGE failed");
        EVE_MediaFifo_close(phost);
        Esd_GpuAlloc_Free(alloc, final_handle);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Get image properties from coprocessor */
    uint32_t out_source, out_fmt, out_w, out_h, out_palette;
    if(!EVE_CoCmd_getImage(phost, &out_source, &out_fmt, &out_w, &out_h, &out_palette)) {
        LV_LOG_WARN("CMD_GETIMAGE failed, using parsed dimensions");
        out_w = img_width;
        out_h = img_height;
        out_fmt = RGB565;
    }

    /* Clean up */
    EVE_MediaFifo_close(phost);
    Esd_GpuAlloc_Free(alloc, temp_handle);

#if LV_USE_OS
    lv_eve5_hal_unlock(s_ctx.disp);
#endif

    /* Return results */
    *handle = final_handle;
    *width = out_w;
    *height = out_h;
    *format = out_fmt;

    LV_LOG_INFO("Loaded image: %s (%ux%u, format=%u)", path, out_w, out_h, out_fmt);
    return true;
}

/**
 * Get the GPU allocator used by the SD card driver.
 */
Esd_GpuAlloc *lv_eve5_sdcard_get_allocator(void)
{
    return s_ctx.alloc;
}

#endif /* LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD */
