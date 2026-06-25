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
#include "lv_eve5_sdcard.h"

#if LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD && (EVE_SUPPORT_CHIPID >= EVE_BT820)
#include "lv_eve5.h"
#include "lv_eve5_image_private.h"
#include "../../../core/lv_global.h"

#include "EVE_Hal.h"
#include "EVE_CoCmd.h"
#include "EVE_CoCmd_Ext.h"
#include "EVE_MediaFifo.h"
#include "EVE_GpuAlloc.h"
#include "EVE_ResourceProbe.h"
#include "EVE_ResourceQuery.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_EVE5_SDCARD_MAX_FILE_SIZE
    #define LV_EVE5_SDCARD_MAX_FILE_SIZE (16 * 1024 * 1024)
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    EVE_GpuHandle gpu_handle;  /**< RAM_G handle (GA_HANDLE_INVALID if not loaded or stolen) */
    uint32_t size;
    uint32_t pos;
    char * path;               /**< Non-NULL = deferred load pending */
} eve5_file_t;

typedef struct {
    uint8_t * listing;
    uint32_t size;
    uint32_t pos;
} eve5_dir_t;

typedef struct {
    lv_display_t * disp;
    EVE_HalContext * hal;
    EVE_GpuAlloc * alloc;
    bool sd_attached;
    lv_fs_drv_t fs_drv;
} eve5_sdcard_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool fs_ready(lv_fs_drv_t * drv);
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

static bool ensure_sd_attached(eve5_sdcard_ctx_t * ctx);
static bool ensure_file_loaded(eve5_sdcard_ctx_t * ctx, eve5_file_t * file);

/* Hook EVE_queryResource_fs's fault recovery into the driver's narrow
 * coprocessor reset (which also retires deferred frees and invalidates the
 * draw unit's bitmap-handle bindings). */
static void sdcard_query_reset_cb(EVE_HalContext * phost, void * userdata)
{
    (void)phost;
    lv_eve5_reset_coprocessor((lv_display_t *)userdata);
}

/**********************
 *  STATIC VARIABLES
 **********************/
static eve5_sdcard_ctx_t s_ctx;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_eve5_sdcard_init(lv_display_t * disp)
{
    if(disp == NULL) return;

    EVE_HalContext *hal = lv_eve5_get_hal(disp);
    EVE_GpuAlloc *alloc = lv_eve5_get_allocator(disp);

    if(hal == NULL || alloc == NULL) {
        LV_LOG_ERROR("EVE5 HAL or allocator not available");
        return;
    }

    lv_memzero(&s_ctx, sizeof(s_ctx));
    s_ctx.disp = disp;
    s_ctx.hal = hal;
    s_ctx.alloc = alloc;
    s_ctx.sd_attached = false;

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
    /* LVGL doesn't support unregistering FS drivers, so just clear context */
    lv_memzero(&s_ctx, sizeof(s_ctx));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool fs_ready(lv_fs_drv_t * drv)
{
    eve5_sdcard_ctx_t * ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return false;

    /* Fast path: lv_fs_open probes ALL registered drivers via ready_cb.
     * Avoid HAL lock when already attached to reduce unnecessary lock contention. */
    if(ctx->sd_attached) return true;

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif
    bool ready = ensure_sd_attached(ctx);
#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif
    return ready;
}

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    eve5_sdcard_ctx_t * ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return NULL;

    if(mode != LV_FS_MODE_RD) {
        LV_LOG_WARN("EVE5 SD card driver only supports read mode");
        return NULL;
    }

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif

    if(!ensure_sd_attached(ctx)) {
        LV_LOG_ERROR("SD card not attached");
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return NULL;
    }

    EVE_HalContext *phost = ctx->hal;

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

    size_t path_len = lv_strlen(path);
    eve5_file_t * file = lv_malloc(sizeof(eve5_file_t));
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

static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    eve5_sdcard_ctx_t * ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    eve5_file_t * file = (eve5_file_t *)file_p;

    if(file != NULL) {
        if(ctx != NULL && ctx->alloc != NULL) {
            /* Free validates the handle internally — no-op when the file was
             * never loaded or the allocation was stolen. */
            EVE_GpuAlloc_Free(ctx->alloc, file->gpu_handle);
        }
        if(file->path != NULL) {
            lv_free(file->path);
        }
        lv_free(file);
    }

    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    eve5_sdcard_ctx_t * ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    eve5_file_t * file = (eve5_file_t *)file_p;

    if(file == NULL || ctx == NULL || ctx->hal == NULL || ctx->alloc == NULL) {
        if(br) *br = 0;
        return LV_FS_RES_INV_PARAM;
    }

    /* Deferred load on first read */
    if(file->path != NULL) {
        if(!ensure_file_loaded(ctx, file)) {
            if(br) *br = 0;
            return LV_FS_RES_FS_ERR;
        }
    }

    uint32_t ramg_addr = EVE_GpuAlloc_Get(ctx->alloc, file->gpu_handle);
    if(ramg_addr == GA_INVALID) {
        if(br) *br = 0;
        return LV_FS_RES_FS_ERR;
    }

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

static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    LV_UNUSED(file_p);
    LV_UNUSED(buf);
    LV_UNUSED(btw);

    if(bw) *bw = 0;
    return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    eve5_file_t * file = (eve5_file_t *)file_p;

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

    if(new_pos < 0) new_pos = 0;
    if((uint32_t)new_pos > file->size) new_pos = (int32_t)file->size;

    file->pos = (uint32_t)new_pos;
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    eve5_file_t * file = (eve5_file_t *)file_p;

    if(file == NULL || pos_p == NULL) return LV_FS_RES_INV_PARAM;

    *pos_p = file->pos;
    return LV_FS_RES_OK;
}

static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    eve5_sdcard_ctx_t * ctx = (eve5_sdcard_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return NULL;

    eve5_dir_t * dir = lv_malloc(sizeof(eve5_dir_t));
    if(dir == NULL) {
        LV_LOG_ERROR("Failed to allocate directory handle");
        return NULL;
    }

    dir->listing = lv_malloc(LV_EVE5_SDCARD_DIR_BUFFER_SIZE);
    if(dir->listing == NULL) {
        LV_LOG_ERROR("Failed to allocate directory listing buffer");
        lv_free(dir);
        return NULL;
    }

    dir->size = 0;
    dir->pos = 0;

    char full_path[LV_FS_MAX_PATH_LENGTH];
    size_t path_len = lv_strlen(path);

    if(path_len == 0 || (path[0] == '/' && path_len == 1)) {
        lv_strlcpy(full_path, "/", sizeof(full_path));
    }
    else {
        lv_strlcpy(full_path, path, sizeof(full_path));
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

    /* CMD_FSDIR requires 32-byte alignment (BT820) */
    EVE_GpuHandle gpu_handle = EVE_GpuAlloc_AlignedAlloc(ctx->alloc, LV_EVE5_SDCARD_DIR_BUFFER_SIZE, 0, 32);
    uint32_t ramg_addr = EVE_GpuAlloc_Get(ctx->alloc, gpu_handle);
    if(ramg_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate RAM_G space for directory listing");
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        lv_free(dir->listing);
        lv_free(dir);
        return NULL;
    }

    /* Pre-flush + SuppressErrorOverlay so a CMD_FSDIR fault is isolated to
     * this attempt and lv_eve5_reset_coprocessor can recover narrowly. */
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_GpuAlloc_Free(ctx->alloc, gpu_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        lv_free(dir->listing);
        lv_free(dir);
        return NULL;
    }
    phost->SuppressErrorOverlay = true;

    uint32_t result = EVE_CoCmd_fsDir(phost, ramg_addr, LV_EVE5_SDCARD_DIR_BUFFER_SIZE, full_path);
    if(result != 0) {
        LV_LOG_WARN("Directory read failed with code %u: %s", result, full_path);
        if(phost->CmdFault) lv_eve5_reset_coprocessor(ctx->disp);
        EVE_GpuAlloc_Free(ctx->alloc, gpu_handle);
        phost->SuppressErrorOverlay = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        lv_free(dir->listing);
        lv_free(dir);
        return NULL;
    }
    phost->SuppressErrorOverlay = false;

    ramg_addr = EVE_GpuAlloc_Get(ctx->alloc, gpu_handle);
    EVE_Hal_rdMem(phost, dir->listing, ramg_addr, LV_EVE5_SDCARD_DIR_BUFFER_SIZE);

    EVE_GpuAlloc_Free(ctx->alloc, gpu_handle);

#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif

    /* Find listing size (double-null terminated) */
    uint32_t i = 0;
    while(i < LV_EVE5_SDCARD_DIR_BUFFER_SIZE - 1) {
        if(dir->listing[i] == '\0') {
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
 * Read next directory entry.
 * BT820 marks directories with trailing '/' or '\'.
 * LVGL expects directories to start with '/'.
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len)
{
    LV_UNUSED(drv);
    eve5_dir_t * dir = (eve5_dir_t *)dir_p;

    if(dir == NULL || fn == NULL || fn_len == 0) return LV_FS_RES_INV_PARAM;

    fn[0] = '\0';

    if(dir->listing == NULL || dir->pos >= dir->size) {
        return LV_FS_RES_OK;
    }

    const char * entry = (const char *)(dir->listing + dir->pos);
    size_t entry_len = lv_strlen(entry);

    if(entry_len == 0) {
        return LV_FS_RES_OK;
    }

    dir->pos += (uint32_t)(entry_len + 1);

    if(lv_strcmp(entry, ".") == 0 || lv_strcmp(entry, "..") == 0) {
        return fs_dir_read(drv, dir_p, fn, fn_len);
    }

    bool is_dir = false;
    if(entry_len > 0) {
        char last_char = entry[entry_len - 1];
        if(last_char == '/' || last_char == '\\') {
            is_dir = true;
            entry_len--;
        }
    }

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

static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);
    eve5_dir_t * dir = (eve5_dir_t *)dir_p;

    if(dir != NULL) {
        if(dir->listing != NULL) {
            lv_free(dir->listing);
        }
        lv_free(dir);
    }

    return LV_FS_RES_OK;
}

/**
 * Load file from SD card into RAM_G.
 * Caller must NOT hold the HAL mutex.
 */
static bool ensure_file_loaded(eve5_sdcard_ctx_t * ctx, eve5_file_t * file)
{
    if(file->path == NULL) return true;

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

    /* CMD_FSREAD requires 32-byte alignment (BT820) */
    EVE_GpuHandle gpu_handle = EVE_GpuAlloc_AlignedAlloc(ctx->alloc, file->size, 0, 32);
    uint32_t ramg_addr = EVE_GpuAlloc_Get(ctx->alloc, gpu_handle);

    if(ramg_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate %u bytes in RAM_G for %s", file->size, file->path);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return false;
    }

    LV_LOG_USER("Loading file to RAM_G: %s (size=%u, addr=0x%08X)", file->path, file->size, ramg_addr);

    /* Pre-flush + SuppressErrorOverlay isolate a CMD_FSREAD fault (corrupt
     * FAT, bad sector, ...) to this attempt so lv_eve5_reset_coprocessor
     * can recover narrowly without losing later renders. */
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_GpuAlloc_Free(ctx->alloc, gpu_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = true;

    uint32_t result = EVE_CoCmd_fsRead(phost, ramg_addr, file->path);
    if(result == 0xFFFFFFFF) {
        LV_LOG_ERROR("Failed to read file from SD card: %s", file->path);
        if(phost->CmdFault) lv_eve5_reset_coprocessor(ctx->disp);
        EVE_GpuAlloc_Free(ctx->alloc, gpu_handle);
        phost->SuppressErrorOverlay = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = false;
    LV_LOG_USER("Loaded file to RAM_G: %s (result=%u)", file->path, result);

    /* File may be handed to the graphics engine via lv_eve5_sdcard_steal_ramg() */
    EVE_Hal_requestFenceBeforeSwap(phost);

#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif

    file->gpu_handle = gpu_handle;
    lv_free(file->path);
    file->path = NULL;

    return true;
}

static bool ensure_sd_attached(eve5_sdcard_ctx_t * ctx)
{
    if(ctx == NULL || ctx->hal == NULL) return false;

    if(ctx->sd_attached) return true;

#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    /* CMD_SDATTACH and the CMD_FS* family are BT820-only. In multi-target
     * builds the code compiles in but must not be reached on earlier gens —
     * the chip would fault on the unrecognized coprocessor command. */
    if(!EVE_Hal_supportSdCard(ctx->hal)) {
        return false;
    }

    EVE_HalContext *phost = ctx->hal;

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
 * After this call, the caller owns the GPU handle and must free it.
 * The file handle should be closed afterward (no further reads possible).
 */
bool lv_eve5_sdcard_steal_ramg(void * file_p, EVE_GpuAlloc ** alloc, EVE_GpuHandle *handle, uint32_t * size)
{
    eve5_file_t * file = (eve5_file_t *)file_p;

    if(file == NULL || alloc == NULL || handle == NULL || size == NULL) return false;
    if(s_ctx.alloc == NULL) return false;

    if(file->path != NULL) {
        if(!ensure_file_loaded(&s_ctx, file)) {
            LV_LOG_ERROR("Cannot steal RAM_G: deferred load failed");
            return false;
        }
    }

    if(EVE_GpuAlloc_Get(s_ctx.alloc, file->gpu_handle) == GA_INVALID) {
        LV_LOG_WARN("Cannot steal RAM_G: already stolen or freed");
        return false;
    }

    *alloc = s_ctx.alloc;
    *handle = file->gpu_handle;
    *size = file->size;

    file->gpu_handle = GA_HANDLE_INVALID;

    return true;
}

/**********************
 * IMAGE LOADING
 **********************/

bool lv_eve5_sdcard_is_path(const char * path)
{
    if(path == NULL || path[0] == '\0') return false;
    if(s_ctx.hal == NULL) return false;

    if(path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        char letter = path[0];
        if(letter >= 'a' && letter <= 'z') letter -= ('a' - 'A');
        char sd_letter = LV_FS_EVE5_SDCARD_LETTER;
        if(sd_letter >= 'a' && sd_letter <= 'z') sd_letter -= ('a' - 'A');
        return letter == sd_letter;
    }
    return false;
}

/**
 * Load and decode a JPEG/PNG from SD card directly to RAM_G (zero-copy path).
 *
 * Flow:
 * 1. CMD_FSREAD compressed file to temporary RAM_G
 * 2. Parse header for dimensions
 * 3. Allocate decoded buffer
 * 4. CMD_LOADIMAGE with OPT_MEDIAFIFO (RAM_G to RAM_G decode)
 * 5. Free temporary allocation
 */
bool lv_eve5_sdcard_load_image(const char * path, EVE_GpuHandle *handle,
                               uint32_t * width, uint32_t * height, uint32_t * format,
                               uint32_t * image_offset, uint32_t * palette_offset)
{
    if(path == NULL || handle == NULL || width == NULL || height == NULL || format == NULL) {
        return false;
    }
    if(s_ctx.hal == NULL || s_ctx.alloc == NULL) {
        LV_LOG_ERROR("EVE5 SD card driver not initialized");
        return false;
    }

    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        LV_LOG_WARN("Unsupported image format (not JPEG/PNG): %s", path);
        return false;
    }

    /* Strip drive letter for the coprocessor FS commands */
    const char * sd_path = path;
    if(path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        sd_path = path + 2;
    }

    EVE_HalContext * phost = s_ctx.hal;
    EVE_GpuAlloc * alloc = s_ctx.alloc;

#if LV_USE_OS
    lv_eve5_hal_lock(s_ctx.disp);
#endif

    if(!ensure_sd_attached(&s_ctx)) {
        LV_LOG_ERROR("SD card not attached");
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Pass OPT_TRUECOLOR so the chip predicts the same decoded format it will
     * produce at load time (ARGB8 / RGB8 / PALETTEDARGB8 instead of the
     * default RGB565 / ARGB4). The query then reports the exact RAM_G
     * allocation we need. Single-channel JPEGs are re-queried with OPT_MONO
     * below so they decode to L8 (half the RAM_G of ARGB8) and render as
     * opaque luminance through the EVE5 image decoder's L8 → LV_COLOR_FORMAT_L8
     * mapping. */
    uint32_t opts = OPT_TRUECOLOR;

    /* Query: dimensions, exact decoded size, EVE bitmap format, palette size.
     * preferCmd=true picks CMD_QUERYIMAGE_fs on BT820+ with the patch_queryassets
     * firmware (zero RAM_G round-trip); otherwise the query stages the file via
     * CMD_FSREAD into RAM_G and drives EVE_probeResource on the bytes. When the
     * SW fallback runs the staging handle is handed back so we can reuse the
     * compressed bytes for the CMD_LOADIMAGE pass via OPT_MEDIAFIFO — no
     * second SD read. */
    EVE_ResourceInfo info;
    EVE_GpuHandle staging = GA_HANDLE_INVALID;
    uint32_t staging_size = 0;
    if(!EVE_queryResource_fs(phost, alloc, sd_path, opts, /*preferCmd*/ true,
                             sdcard_query_reset_cb, s_ctx.disp,
                             &staging, &staging_size, &info)) {
        LV_LOG_WARN("EVE5 SD: queryResource failed for %s", path);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Grayscale JPEG auto-promote: re-query with OPT_MONO for the L8 prediction
     * (half the RAM_G of ARGB8). Re-query first; only adopt on success so the
     * first staging stays valid on rejection. CMD path runs CMD_QUERYIMAGE_fs
     * again with no SD round-trip; SW path re-stages the file bytes. */
    if(info.Type == EVE_RESOURCE_JPEG && info.Channels == 1) {
        EVE_ResourceInfo info_mono;
        EVE_GpuHandle staging_mono = GA_HANDLE_INVALID;
        uint32_t staging_size_mono = 0;
        if(EVE_queryResource_fs(phost, alloc, sd_path, OPT_MONO, /*preferCmd*/ true,
                                sdcard_query_reset_cb, s_ctx.disp,
                                &staging_mono, &staging_size_mono, &info_mono)
           && (info_mono.Flags & EVE_RESOURCE_FLAG_HARDWARE_LOADABLE)) {
            if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
            info = info_mono;
            staging = staging_mono;
            staging_size = staging_size_mono;
            opts = OPT_MONO;
        }
        else if(staging_mono.Id != GA_HANDLE_INVALID.Id) {
            EVE_GpuAlloc_Free(alloc, staging_mono);
        }
    }

    if(info.Type != EVE_RESOURCE_JPEG && info.Type != EVE_RESOURCE_PNG) {
        LV_LOG_WARN("EVE5 SD: %s isn't a JPEG/PNG (type=%u)", path, (unsigned)info.Type);
        if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    if(!(info.Flags & EVE_RESOURCE_FLAG_HARDWARE_LOADABLE)) {
        LV_LOG_INFO("EVE5 SD: %s not HW-decodable on this chip", path);
        if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* GC-flagged: decoded images self-heal through the decoder cache when
     * the handle goes invalid (pressure eviction; SD commands are BT820+). */
    EVE_GpuHandle final_handle = EVE_GpuAlloc_Alloc(alloc, info.Size, GA_ALIGN_4 | GA_GC_FLAG);
    uint32_t final_addr = EVE_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 SD: failed to allocate %u bytes for %s", (unsigned)info.Size, path);
        if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Pre-flush + SuppressErrorOverlay isolate a CMD_LOADIMAGE fault to this
     * attempt so lv_eve5_reset_coprocessor can recover narrowly. */
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_GpuAlloc_Free(alloc, final_handle);
        if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = true;

    bool load_ok = false;
    uint32_t loadimage_opts = OPT_NODL | opts;

    if(staging.Id == GA_HANDLE_INVALID.Id) {
        /* CMD path was taken by the query (BT820+ patched firmware) — load
         * directly from SD via CMD_LOADIMAGE_fs. No intermediate buffer. */
#if EVE_COCMD_PATCH_QUERY
        uint32_t r = EVE_CoCmd_loadImage_fs(phost, final_addr, sd_path, loadimage_opts,
                                            NULL, NULL, NULL, NULL, NULL);
        load_ok = (r == 0);
#endif
    }
    else {
        /* SW query path: the staging buffer holds the compressed file. Re-publish
         * it as a MediaFIFO so CMD_LOADIMAGE decodes directly into the
         * destination — no second SD read. */
        uint32_t staging_addr = EVE_GpuAlloc_Get(alloc, staging);
        if(staging_addr != GA_INVALID) {
            uint32_t fifo_size = (staging_size + 3u) & ~3u;
            EVE_MediaFifo_close(phost);
            if(EVE_MediaFifo_set(phost, staging_addr, fifo_size)) {
                EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, staging_size);
                EVE_CoCmd_loadImage(phost, final_addr, OPT_MEDIAFIFO | loadimage_opts);
                load_ok = EVE_Cmd_waitFlush(phost);
                EVE_MediaFifo_close(phost);
            }
        }
        EVE_GpuAlloc_Free(alloc, staging);
    }

    if(!load_ok) {
        LV_LOG_ERROR("EVE5 SD: CMD_LOADIMAGE failed for %s", path);
        if(phost->CmdFault) lv_eve5_reset_coprocessor(s_ctx.disp);
        phost->SuppressErrorOverlay = false;
        EVE_GpuAlloc_Free(alloc, final_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = false;
    EVE_Hal_requestFenceBeforeSwap(phost);

#if LV_USE_OS
    lv_eve5_hal_unlock(s_ctx.disp);
#endif

    /* Paletted layout is generation-dependent. EVE3 (BT815/6) and BT820 use
     * palette-FRONT (palette at base, image at base+PaletteSize). EVE4
     * (BT817/8) uses palette-AFTER (image at base, palette at base+stride*h).
     * The probe / query path encodes the chip's choice via
     * EVE_RESOURCE_FLAG_PALETTE_AFTER_DATA — derive offsets from it instead
     * of hard-coding palette-front. (This BT820-gated function only sees the
     * flag clear in practice, but mirroring the probe's contract keeps the
     * code correct under any future caller / multi-target lift.) */
    *handle = final_handle;
    *width = info.Width;
    *height = info.Height;
    *format = info.Format;
    if(info.PaletteSize == 0) {
        if(image_offset) *image_offset = 0;
        if(palette_offset) *palette_offset = GA_INVALID;
    }
    else if(info.Flags & EVE_RESOURCE_FLAG_PALETTE_AFTER_DATA) {
        if(image_offset) *image_offset = 0;
        if(palette_offset) *palette_offset = info.Stride * info.Height;
    }
    else {
        if(image_offset) *image_offset = info.PaletteSize;
        if(palette_offset) *palette_offset = 0;
    }

    LV_LOG_INFO("EVE5 SD: loaded %s (%ux%u fmt=%u size=%u palette=%u)",
                path, (unsigned)info.Width, (unsigned)info.Height,
                (unsigned)info.Format, (unsigned)info.Size, (unsigned)info.PaletteSize);
    return true;
}

bool lv_eve5_sdcard_load_image_staged(const char * path,
                                      EVE_GpuHandle staging, uint32_t staging_size,
                                      const EVE_ResourceInfo * info,
                                      EVE_GpuHandle * handle,
                                      uint32_t * width, uint32_t * height, uint32_t * format,
                                      uint32_t * image_offset, uint32_t * palette_offset)
{
    if(path == NULL || info == NULL || handle == NULL
       || width == NULL || height == NULL || format == NULL) {
        if(s_ctx.alloc != NULL) EVE_GpuAlloc_Free(s_ctx.alloc, staging);
        return false;
    }
    if(s_ctx.hal == NULL || s_ctx.alloc == NULL) {
        return false;
    }
    if(staging.Id == GA_HANDLE_INVALID.Id || staging_size == 0) {
        return false;
    }

    EVE_HalContext * phost = s_ctx.hal;
    EVE_GpuAlloc * alloc = s_ctx.alloc;

#if LV_USE_OS
    lv_eve5_hal_lock(s_ctx.disp);
#endif

    /* GC-flagged final allocation, sized from the caller's resolved info. */
    EVE_GpuHandle final_handle = EVE_GpuAlloc_Alloc(alloc, info->Size, GA_ALIGN_4 | GA_GC_FLAG);
    uint32_t final_addr = EVE_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 SD staged: failed to allocate %u bytes for %s",
                     (unsigned)info->Size, path);
        EVE_GpuAlloc_Free(alloc, staging);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    uint32_t staging_addr = EVE_GpuAlloc_Get(alloc, staging);
    if(staging_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 SD staged: staging handle invalid for %s", path);
        EVE_GpuAlloc_Free(alloc, final_handle);
        EVE_GpuAlloc_Free(alloc, staging);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Pre-flush + SuppressErrorOverlay so a CMD_LOADIMAGE fault is isolated. */
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_GpuAlloc_Free(alloc, final_handle);
        EVE_GpuAlloc_Free(alloc, staging);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = true;

    bool load_ok = false;
    uint32_t fifo_size = (staging_size + 3u) & ~3u;
    /* Derive the decode option from the caller's resolved info: JPEG predicted
     * to L8 came from a grayscale-JPEG re-probe in info_cb (with OPT_MONO),
     * so load with OPT_MONO to match. Everything else takes OPT_TRUECOLOR. */
    uint32_t decode_opt = (info->Type == EVE_RESOURCE_JPEG && info->Format == L8)
                              ? OPT_MONO : OPT_TRUECOLOR;
    EVE_MediaFifo_close(phost);
    if(EVE_MediaFifo_set(phost, staging_addr, fifo_size)) {
        EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, staging_size);
        EVE_CoCmd_loadImage(phost, final_addr, OPT_NODL | OPT_MEDIAFIFO | decode_opt);
        load_ok = EVE_Cmd_waitFlush(phost);
        EVE_MediaFifo_close(phost);
    }
    EVE_GpuAlloc_Free(alloc, staging);

    if(!load_ok) {
        LV_LOG_ERROR("EVE5 SD staged: CMD_LOADIMAGE failed for %s", path);
        if(phost->CmdFault) lv_eve5_reset_coprocessor(s_ctx.disp);
        phost->SuppressErrorOverlay = false;
        EVE_GpuAlloc_Free(alloc, final_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = false;
    EVE_Hal_requestFenceBeforeSwap(phost);

#if LV_USE_OS
    lv_eve5_hal_unlock(s_ctx.disp);
#endif

    /* Same flag-aware layout resolution as lv_eve5_sdcard_load_image. */
    *handle = final_handle;
    *width = info->Width;
    *height = info->Height;
    *format = info->Format;
    if(info->PaletteSize == 0) {
        if(image_offset) *image_offset = 0;
        if(palette_offset) *palette_offset = GA_INVALID;
    }
    else if(info->Flags & EVE_RESOURCE_FLAG_PALETTE_AFTER_DATA) {
        if(image_offset) *image_offset = 0;
        if(palette_offset) *palette_offset = info->Stride * info->Height;
    }
    else {
        if(image_offset) *image_offset = info->PaletteSize;
        if(palette_offset) *palette_offset = 0;
    }

    LV_LOG_INFO("EVE5 SD staged: loaded %s (%ux%u fmt=%u size=%u palette=%u)",
                path, (unsigned)info->Width, (unsigned)info->Height,
                (unsigned)info->Format, (unsigned)info->Size, (unsigned)info->PaletteSize);
    return true;
}

EVE_GpuAlloc * lv_eve5_sdcard_get_allocator(void)
{
    return s_ctx.alloc;
}

/**********************
 * ESDM SIDECAR LOADING
 **********************/

bool lv_eve5_sdcard_read_esdm(const char * path, uint8_t * buf, uint32_t buf_size, uint32_t * out_read)
{
    if(out_read) *out_read = 0;
    if(path == NULL || buf == NULL || buf_size == 0) return false;
    if(s_ctx.hal == NULL || s_ctx.alloc == NULL) return false;

    /* Build "<path>.esdm", then strip the drive letter for the coprocessor FS. */
    char meta_path[LV_FS_MAX_PATH_LENGTH];
    if(!eve5_esdm_meta_path(path, meta_path, sizeof(meta_path))) return false;
    const char * sd_path = meta_path;
    if(meta_path[0] != '\0' && meta_path[1] == ':' && (meta_path[2] == '/' || meta_path[2] == '\\')) {
        sd_path = meta_path + 2;
    }

    EVE_HalContext * phost = s_ctx.hal;
    bool ok = false;

#if LV_USE_OS
    lv_eve5_hal_lock(s_ctx.disp);
#endif
    /* Pre-flush + SuppressErrorOverlay isolate FSSIZE / FSREAD faults so
     * a missing/corrupt sidecar can't leave the coprocessor in a bad state. */
    if(ensure_sd_attached(&s_ctx) && EVE_Cmd_waitFlush(phost)) {
        phost->SuppressErrorOverlay = true;
        uint32_t fsize = EVE_CoCmd_fsSize(phost, sd_path);
        bool fault_after_size = phost->CmdFault;
        if(fsize != 0xFFFFFFFFu && fsize != 0 && fsize <= buf_size) {
            /* CMD_FSREAD destination must be 32-byte aligned. */
            uint32_t aligned = (fsize + 31u) & ~31u;
            EVE_GpuHandle h = EVE_GpuAlloc_AlignedAlloc(s_ctx.alloc, aligned, 0, 32);
            uint32_t addr = EVE_GpuAlloc_Get(s_ctx.alloc, h);
            if(addr != GA_INVALID) {
                if(EVE_CoCmd_fsRead(phost, addr, sd_path) == 0) {
                    addr = EVE_GpuAlloc_Get(s_ctx.alloc, h);
                    EVE_Hal_rdMem(phost, buf, addr, fsize);
                    if(out_read) *out_read = fsize;
                    ok = true;
                }
                EVE_GpuAlloc_Free(s_ctx.alloc, h);
            }
        }
        if(!ok && (fault_after_size || phost->CmdFault)) {
            lv_eve5_reset_coprocessor(s_ctx.disp);
        }
        phost->SuppressErrorOverlay = false;
    }
#if LV_USE_OS
    lv_eve5_hal_unlock(s_ctx.disp);
#endif

    return ok;
}

bool lv_eve5_sdcard_load_esdm(const char * path, EVE_GpuHandle *handle,
                              uint32_t * width, uint32_t * height, uint32_t * format,
                              int32_t * stride, uint32_t * image_offset, uint32_t * palette_offset)
{
    if(path == NULL || handle == NULL) return false;
    if(s_ctx.hal == NULL || s_ctx.alloc == NULL) return false;

    /* Read + parse the metadata sidecar (raw FS read, no lv_fs). */
    uint8_t meta[EVE5_ESDM_MAX];
    uint32_t meta_read = 0;
    if(!lv_eve5_sdcard_read_esdm(path, meta, sizeof(meta), &meta_read)) return false;
    eve5_esdm_bmp_t bmp;
    if(!eve5_parse_esdm_bmp(meta, meta_read, &bmp)) return false;

    /* JPEG/PNG sidecars load through the regular image path. */
    if(bmp.compression == EVE5_ESDM_IMAGE) return false;
    if(bmp.has_swizzle) {
        LV_LOG_WARN("EVE5 esdm: bitmap swizzle not applied for %s", path);
    }

    EVE_HalContext * phost = s_ctx.hal;
    EVE_GpuAlloc * alloc = s_ctx.alloc;

    /* Strip drive letters for the coprocessor FS. */
    const char * img_path = path;
    if(path[0] != '\0' && path[1] == ':' && (path[2] == '/' || path[2] == '\\')) img_path = path + 2;

    bool paletted = (bmp.palette_size > 0);
    char pal_path_buf[LV_FS_MAX_PATH_LENGTH];
    const char * pal_path = NULL;
    if(paletted) {
        if(!eve5_esdm_palette_path(path, bmp.ext_len, bmp.palette_ext, pal_path_buf, sizeof(pal_path_buf))) {
            LV_LOG_WARN("EVE5 esdm: cannot derive palette path for %s", path);
            return false;
        }
        pal_path = pal_path_buf;
        if(pal_path[0] != '\0' && pal_path[1] == ':' && (pal_path[2] == '/' || pal_path[2] == '\\')) pal_path += 2;
    }

    /* Palette (if any) precedes the bitmap data in one contiguous allocation,
     * matching the driver's PALETTEDARGB8 layout. Keep the bitmap region
     * 32-byte aligned for CMD_FSREAD. */
    uint32_t pal_region = paletted ? (((uint32_t)bmp.palette_size + 31u) & ~31u) : 0;
    uint32_t data_bytes = (bmp.compression == EVE5_ESDM_ASSET && bmp.raw_size)
                          ? bmp.raw_size : (uint32_t)(bmp.stride * bmp.height);
    uint32_t alloc_size = pal_region + data_bytes;

    bool ok = false;
    uint32_t img_ofs = pal_region;
    uint32_t pal_ofs = GA_INVALID;

#if LV_USE_OS
    lv_eve5_hal_lock(s_ctx.disp);
#endif
    /* Pre-flush + SuppressErrorOverlay isolate any palette/body load fault
     * (corrupt deflate stream, mis-relocated asset, ...) so the coprocessor
     * can be recovered narrowly via lv_eve5_reset_coprocessor. */
    if(ensure_sd_attached(&s_ctx) && EVE_Cmd_waitFlush(phost)) {
        phost->SuppressErrorOverlay = true;
        /* GC-flagged: raw assets re-load on demand through the decoder cache. */
        EVE_GpuHandle h = EVE_GpuAlloc_AlignedAlloc(alloc, alloc_size, GA_GC_FLAG, 32);
        uint32_t base = EVE_GpuAlloc_Get(alloc, h);
        if(base != GA_INVALID) {
            bool loaded = true;

            if(paletted) {
                if(EVE_CoCmd_fsRead(phost, base, pal_path) != 0) {
                    LV_LOG_WARN("EVE5 esdm: palette read failed: %s", pal_path);
                    loaded = false;
                }
                else {
                    pal_ofs = 0;
                }
            }

            if(loaded) {
                /* Re-flush so the body's fault is isolated from the
                 * palette FSREAD that preceded it. */
                if(!EVE_Cmd_waitFlush(phost)) {
                    loaded = false;
                }
                else {
                    uint32_t dst = base + pal_region;
                    switch(bmp.compression) {
                        case EVE5_ESDM_RAW:
                            loaded = (EVE_CoCmd_fsRead(phost, dst, img_path) == 0);
                            break;
                        case EVE5_ESDM_DEFLATE:
                            loaded = (EVE_CoCmd_inflate_fs(phost, dst, img_path, 0) == 0);
                            break;
                        case EVE5_ESDM_ASSET:
                            loaded = (EVE_CoCmd_loadAsset_fs(phost, dst, img_path, 0) == 0);
                            break;
                        default:
                            loaded = false;
                            break;
                    }
                }
            }

            if(loaded) {
                EVE_Hal_requestFenceBeforeSwap(phost);
                *handle = h;
                ok = true;
            }
            else {
                if(phost->CmdFault) lv_eve5_reset_coprocessor(s_ctx.disp);
                EVE_GpuAlloc_Free(alloc, h);
            }
        }
        phost->SuppressErrorOverlay = false;
    }
#if LV_USE_OS
    lv_eve5_hal_unlock(s_ctx.disp);
#endif

    if(!ok) return false;

    if(width) *width = (uint32_t)bmp.width;
    if(height) *height = (uint32_t)bmp.height;
    if(format) *format = bmp.format;
    if(stride) *stride = bmp.stride;
    if(image_offset) *image_offset = img_ofs;
    if(palette_offset) *palette_offset = pal_ofs;

    LV_LOG_INFO("EVE5 esdm: loaded %s (%dx%d fmt=%u stride=%d comp=%u%s)",
                path, (int)bmp.width, (int)bmp.height, (unsigned)bmp.format,
                (int)bmp.stride, (unsigned)bmp.compression, paletted ? " paletted" : "");
    return true;
}

#elif LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD

/* Linker stubs for chips without BT820 SD-card command support. */

void lv_fs_eve5_sdcard_init(lv_display_t * disp)         { (void)disp; }
void lv_fs_eve5_sdcard_deinit(void)                       {}
bool lv_eve5_sdcard_ready(void)                           { return false; }
bool lv_eve5_sdcard_is_path(const char * path)            { (void)path; return false; }
EVE_GpuAlloc * lv_eve5_sdcard_get_allocator(void)         { return NULL; }

bool lv_eve5_sdcard_steal_ramg(void * file_p, EVE_GpuAlloc ** alloc,
                               EVE_GpuHandle *handle, uint32_t * size)
{
    (void)file_p; (void)alloc; (void)handle; (void)size;
    return false;
}

bool lv_eve5_sdcard_load_image(const char * path, EVE_GpuHandle *handle,
                               uint32_t * width, uint32_t * height, uint32_t * format,
                               uint32_t * image_offset, uint32_t * palette_offset)
{
    (void)path; (void)handle; (void)width; (void)height;
    (void)format; (void)image_offset; (void)palette_offset;
    return false;
}

bool lv_eve5_sdcard_load_image_staged(const char * path,
                                      EVE_GpuHandle staging, uint32_t staging_size,
                                      const EVE_ResourceInfo * info,
                                      EVE_GpuHandle * handle,
                                      uint32_t * width, uint32_t * height, uint32_t * format,
                                      uint32_t * image_offset, uint32_t * palette_offset)
{
    (void)path; (void)staging; (void)staging_size; (void)info; (void)handle;
    (void)width; (void)height; (void)format; (void)image_offset; (void)palette_offset;
    return false;
}

bool lv_eve5_sdcard_read_esdm(const char * path, uint8_t * buf, uint32_t buf_size, uint32_t * out_read)
{
    (void)path; (void)buf; (void)buf_size;
    if(out_read) *out_read = 0;
    return false;
}

bool lv_eve5_sdcard_load_esdm(const char * path, EVE_GpuHandle *handle,
                              uint32_t * width, uint32_t * height, uint32_t * format,
                              int32_t * stride, uint32_t * image_offset, uint32_t * palette_offset)
{
    (void)path; (void)handle; (void)width; (void)height; (void)format;
    (void)stride; (void)image_offset; (void)palette_offset;
    return false;
}

#endif /* LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD && (EVE_SUPPORT_CHIPID >= EVE_BT820) */
