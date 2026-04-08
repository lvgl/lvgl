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
#include "lv_eve5_image_private.h"
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

#ifndef LV_EVE5_SDCARD_MAX_FILE_SIZE
    #define LV_EVE5_SDCARD_MAX_FILE_SIZE (16 * 1024 * 1024)
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    Esd_GpuHandle gpu_handle;  /**< RAM_G handle (GA_HANDLE_INVALID if not loaded or stolen) */
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
    Esd_GpuAlloc * alloc;
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
    Esd_GpuAlloc *alloc = lv_eve5_get_allocator(disp);

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

    uint32_t ramg_addr = Esd_GpuAlloc_Get(ctx->alloc, file->gpu_handle);
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

    ramg_addr = Esd_GpuAlloc_Get(ctx->alloc, gpu_handle);
    EVE_Hal_rdMem(phost, dir->listing, ramg_addr, LV_EVE5_SDCARD_DIR_BUFFER_SIZE);

    Esd_GpuAlloc_Free(ctx->alloc, gpu_handle);

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
    Esd_GpuHandle gpu_handle = Esd_GpuAlloc_AlignedAlloc(ctx->alloc, file->size, 0, 32);
    uint32_t ramg_addr = Esd_GpuAlloc_Get(ctx->alloc, gpu_handle);

    if(ramg_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate %u bytes in RAM_G for %s", file->size, file->path);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return false;
    }

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

static bool ensure_sd_attached(eve5_sdcard_ctx_t * ctx)
{
    if(ctx == NULL || ctx->hal == NULL) return false;

    if(ctx->sd_attached) return true;

#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
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
bool lv_eve5_sdcard_steal_ramg(void * file_p, Esd_GpuAlloc ** alloc, Esd_GpuHandle *handle, uint32_t * size)
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

    if(Esd_GpuAlloc_Get(s_ctx.alloc, file->gpu_handle) == GA_INVALID) {
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
bool lv_eve5_sdcard_load_image(const char * path, Esd_GpuHandle *handle,
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

    bool is_jpeg = eve5_has_extension(path, ".jpg") || eve5_has_extension(path, ".jpeg");
    bool is_png = eve5_has_extension(path, ".png");
    if(!is_jpeg && !is_png) {
        LV_LOG_WARN("Unsupported image format (not JPEG/PNG): %s", path);
        return false;
    }

    /* Skip drive letter prefix for SD card commands */
    const char * sd_path = path;
    if(path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        sd_path = path + 2;
    }

    EVE_HalContext *phost = s_ctx.hal;
    Esd_GpuAlloc *alloc = s_ctx.alloc;

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

    uint32_t file_size = EVE_CoCmd_fsSize(phost, sd_path);
    if(file_size == 0xFFFFFFFF || file_size == 0) {
        LV_LOG_ERROR("File not found or empty: %s", sd_path);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Allocate temporary space for compressed file.
     * CMD_FSREAD requires 32-byte alignment, MEDIAFIFO requires 4-byte size alignment. */
    uint32_t fifo_size = (file_size + 3) & ~3U;
    Esd_GpuHandle temp_handle = Esd_GpuAlloc_AlignedAlloc(alloc, fifo_size, 0, 32);
    uint32_t temp_addr = Esd_GpuAlloc_Get(alloc, temp_handle);
    if(temp_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate temporary RAM_G for compressed file (%u bytes)", fifo_size);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    uint32_t result = EVE_CoCmd_fsRead(phost, temp_addr, sd_path);
    if(result != 0) {
        LV_LOG_ERROR("CMD_FSREAD failed with code %u: %s", result, sd_path);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Parse header for dimensions */
    uint8_t header_buf[1024];
    uint32_t header_size = file_size < sizeof(header_buf) ? file_size : sizeof(header_buf);
    temp_addr = Esd_GpuAlloc_Get(alloc, temp_handle);
    EVE_Hal_rdMem(phost, header_buf, temp_addr, header_size);

    uint32_t img_width = 0, img_height = 0;
    bool parsed;
    if(is_jpeg) {
        parsed = eve5_parse_jpeg_dimensions(header_buf, header_size, &img_width, &img_height);
    }
    else {
        parsed = eve5_parse_png_dimensions(header_buf, header_size, &img_width, &img_height);
    }

    if(!parsed || img_width == 0 || img_height == 0) {
        LV_LOG_ERROR("Failed to parse image header: %s", path);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Worst case decoded size: ARGB8 (4 bpp), 4-byte aligned stride */
    uint32_t decoded_stride = ((img_width * 4) + 3) & ~3U;
    uint32_t decoded_size = decoded_stride * img_height;

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

    EVE_MediaFifo_close(phost);

    temp_addr = Esd_GpuAlloc_Get(alloc, temp_handle);
    if(!EVE_MediaFifo_set(phost, temp_addr, fifo_size)) {
        LV_LOG_ERROR("Failed to set up media FIFO");
        Esd_GpuAlloc_Free(alloc, final_handle);
        Esd_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Data already in RAM_G from CMD_FSREAD, just set write pointer */
    EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, file_size);

    /* OPT_TRUECOLOR: decode to RGB8/ARGB8 instead of RGB565/ARGB4 to avoid banding */
    EVE_CoCmd_loadImage(phost, final_addr, OPT_MEDIAFIFO | OPT_NODL | OPT_TRUECOLOR);
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

    uint32_t out_source, out_fmt, out_w, out_h, out_palette;
    if(!EVE_CoCmd_getImage(phost, &out_source, &out_fmt, &out_w, &out_h, &out_palette)) {
        LV_LOG_WARN("CMD_GETIMAGE failed, using parsed dimensions");
        out_source = final_addr;
        out_w = img_width;
        out_h = img_height;
        out_fmt = RGB565;
        out_palette = GA_INVALID;
    }

    EVE_MediaFifo_close(phost);
    Esd_GpuAlloc_Free(alloc, temp_handle);

    /* Compute offsets from allocation base */
    uint32_t alloc_base = Esd_GpuAlloc_Get(alloc, final_handle);
    uint32_t img_ofs = (out_source >= alloc_base) ? (out_source - alloc_base) : 0;
    uint32_t pal_ofs = GA_INVALID;
    if(out_fmt == PALETTEDARGB8 && out_palette >= alloc_base) {
        pal_ofs = out_palette - alloc_base;
    }

    /* Trim allocation to actual decoded size */
    int32_t bpp = eve5_format_bpp(out_fmt);
    uint32_t index_size = out_w * (uint32_t)bpp * out_h;
    uint32_t img_end = img_ofs + index_size;
    uint32_t pal_end = (pal_ofs != GA_INVALID) ? (pal_ofs + 256 * 4) : 0;
    uint32_t actual_size = img_end > pal_end ? img_end : pal_end;
    if(actual_size < decoded_size) {
        Esd_GpuAlloc_Truncate(alloc, final_handle, actual_size);
    }

#if LV_USE_OS
    lv_eve5_hal_unlock(s_ctx.disp);
#endif

    *handle = final_handle;
    *width = out_w;
    *height = out_h;
    *format = out_fmt;
    if(image_offset) *image_offset = img_ofs;
    if(palette_offset) *palette_offset = pal_ofs;

    LV_LOG_INFO("Loaded image: %s (%ux%u, format=%u)", path, out_w, out_h, out_fmt);
    return true;
}

Esd_GpuAlloc * lv_eve5_sdcard_get_allocator(void)
{
    return s_ctx.alloc;
}

#endif /* LV_USE_EVE5 && LV_USE_FS_EVE5_SDCARD */
