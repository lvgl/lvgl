/**
 * @file lv_eve5_flash.c
 *
 * EVE5 (BT820) Flash Filesystem Driver for LVGL
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_eve5_flash.h"

#if LV_USE_EVE5 && LV_USE_FS_EVE5_FLASH && defined(EVE_SUPPORT_FLASH)
#include "lv_eve5.h"
#include "lv_eve5_image_private.h"

#include "EVE_Hal.h"
#include "EVE_CoCmd.h"
#include "EVE_GpuAlloc.h"
#include "EVE_ResourceProbe.h"
#include "EVE_ResourceQuery.h"

/*********************
 *      DEFINES
 *********************/

#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t flash_addr;   /**< Base flash byte address */
    uint32_t size;         /**< Remaining flash from base address */
    uint32_t pos;          /**< Current read position */
} eve5_flash_file_t;

typedef struct {
    lv_display_t * disp;
    EVE_HalContext * hal;
    EVE_GpuAlloc * alloc;
    bool flash_ready;
    uint32_t flash_size_bytes;
    lv_fs_drv_t fs_drv;
} eve5_flash_ctx_t;

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

static bool ensure_flash_ready(eve5_flash_ctx_t * ctx);
static uint32_t parse_flash_addr(const char * path);

/* Hook EVE_queryResource_flash's fault recovery into the driver's narrow
 * coprocessor reset (retires deferred frees, drops cached bitmap-handle
 * bindings on the connected draw unit). */
static void flash_query_reset_cb(EVE_HalContext * phost, void * userdata)
{
    (void)phost;
    lv_eve5_reset_coprocessor((lv_display_t *)userdata);
}

/**********************
 *  STATIC VARIABLES
 **********************/
static eve5_flash_ctx_t s_ctx;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_eve5_flash_init(lv_display_t * disp)
{
    if(disp == NULL) return;

    EVE_HalContext *hal = lv_eve5_get_hal(disp);
    EVE_GpuAlloc *alloc = lv_eve5_get_allocator(disp);

    if(hal == NULL || alloc == NULL) {
        LV_LOG_ERROR("EVE5 HAL or allocator not available for flash driver");
        return;
    }

    lv_memzero(&s_ctx, sizeof(s_ctx));
    s_ctx.disp = disp;
    s_ctx.hal = hal;
    s_ctx.alloc = alloc;
    s_ctx.flash_ready = false;

    lv_fs_drv_init(&s_ctx.fs_drv);

    s_ctx.fs_drv.letter = LV_FS_EVE5_FLASH_LETTER;
    s_ctx.fs_drv.cache_size = LV_FS_EVE5_FLASH_CACHE_SIZE;
    s_ctx.fs_drv.user_data = &s_ctx;

    s_ctx.fs_drv.ready_cb = fs_ready;
    s_ctx.fs_drv.open_cb = fs_open;
    s_ctx.fs_drv.close_cb = fs_close;
    s_ctx.fs_drv.read_cb = fs_read;
    s_ctx.fs_drv.write_cb = fs_write;
    s_ctx.fs_drv.seek_cb = fs_seek;
    s_ctx.fs_drv.tell_cb = fs_tell;

    s_ctx.fs_drv.dir_open_cb = NULL;
    s_ctx.fs_drv.dir_read_cb = NULL;
    s_ctx.fs_drv.dir_close_cb = NULL;

    lv_fs_drv_register(&s_ctx.fs_drv);

    LV_LOG_INFO("EVE5 flash filesystem registered with letter '%c'", LV_FS_EVE5_FLASH_LETTER);
}

bool lv_eve5_flash_ready(void)
{
#if LV_USE_OS
    if(s_ctx.disp) lv_eve5_hal_lock(s_ctx.disp);
#endif
    bool ready = ensure_flash_ready(&s_ctx);
#if LV_USE_OS
    if(s_ctx.disp) lv_eve5_hal_unlock(s_ctx.disp);
#endif
    return ready;
}

void lv_fs_eve5_flash_deinit(void)
{
    lv_memzero(&s_ctx, sizeof(s_ctx));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Parse flash address from path. Format: "/123456" or "123456"
 * Returns UINT32_MAX on parse failure.
 */
static uint32_t parse_flash_addr(const char * path)
{
    while(*path == '/' || *path == '\\') path++;

    if(*path == '\0') return UINT32_MAX;

    uint32_t addr = 0;
    while(*path >= '0' && *path <= '9') {
        uint32_t prev = addr;
        addr = addr * 10 + (*path - '0');
        if(addr < prev) return UINT32_MAX;
        path++;
    }

    return addr;
}

/**
 * Ensure flash is attached and in fast mode.
 * Caller must hold HAL mutex if LV_USE_OS.
 */
static bool ensure_flash_ready(eve5_flash_ctx_t * ctx)
{
    if(ctx == NULL || ctx->hal == NULL) return false;
    if(ctx->flash_ready) return true;

#if (EVE_SUPPORT_CHIPID >= EVE_BT815)
    /* Flash commands (CMD_FLASHATTACH, CMD_FLASHREAD, CMD_FLASHSOURCE,
     * CMD_LOADIMAGE+OPT_FLASH) are BT815+. In a multi-target build they're
     * compiled in, but earlier gens (FT80X, FT81X, BT88X) would fault on
     * the unrecognized coprocessor commands. */
    if(!EVE_Hal_supportFlash(ctx->hal)) {
        return false;
    }

    EVE_HalContext *phost = ctx->hal;

    uint32_t status = EVE_Hal_rd32(phost, REG_FLASH_STATUS);

    if(status == FLASH_STATUS_DETACHED) {
        status = EVE_CoCmd_flashAttach(phost);
    }

    if(status < FLASH_STATUS_BASIC) {
        LV_LOG_WARN("Flash attach failed (status=%u)", status);
        return false;
    }

    /* Fast mode required for CMD_LOADIMAGE with OPT_FLASH */
    if(status < FLASH_STATUS_FULL) {
        uint32_t result = 0;
        status = EVE_CoCmd_flashFast(phost, &result);
        if(status < FLASH_STATUS_FULL) {
            LV_LOG_WARN("Flash fast mode failed (status=%u, result=%u)", status, result);
            /* CMD_FLASHREAD still works in basic mode */
        }
    }

    ctx->flash_size_bytes = EVE_Hal_rd32(phost, REG_FLASH_SIZE) * 1024UL * 1024UL;
    if(ctx->flash_size_bytes == 0) {
        LV_LOG_WARN("Flash size is 0");
        return false;
    }

    ctx->flash_ready = true;
    LV_LOG_INFO("Flash ready: %u MB", ctx->flash_size_bytes / (1024 * 1024));
    return true;
#else
    return false;
#endif
}

static bool fs_ready(lv_fs_drv_t * drv)
{
    eve5_flash_ctx_t * ctx = (eve5_flash_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return false;

    /* Fast path: lv_fs_open probes ALL registered drivers via ready_cb.
     * Avoid HAL lock when already ready to reduce unnecessary lock contention. */
    if(ctx->flash_ready) return true;

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif
    bool ready = ensure_flash_ready(ctx);
#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif
    return ready;
}

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    eve5_flash_ctx_t * ctx = (eve5_flash_ctx_t *)drv->user_data;
    if(ctx == NULL || ctx->hal == NULL) return NULL;

    if(mode != LV_FS_MODE_RD) {
        LV_LOG_WARN("EVE5 flash driver only supports read mode");
        return NULL;
    }

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif
    if(!ensure_flash_ready(ctx)) {
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        return NULL;
    }
#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif

    uint32_t addr = parse_flash_addr(path);
    if(addr == UINT32_MAX || addr >= ctx->flash_size_bytes) {
        LV_LOG_WARN("Invalid flash address: %s", path);
        return NULL;
    }

    eve5_flash_file_t * file = lv_malloc(sizeof(eve5_flash_file_t));
    if(file == NULL) return NULL;

    file->flash_addr = addr;
    file->size = ctx->flash_size_bytes - addr;
    file->pos = 0;

    LV_LOG_INFO("Opened flash file at address %u (max size %u)", addr, file->size);
    return file;
}

static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    if(file_p != NULL) {
        lv_free(file_p);
    }
    return LV_FS_RES_OK;
}

/**
 * Read from flash via CMD_FLASHREAD.
 * Handles alignment: dest must be 4-byte aligned, src must be 64-byte aligned,
 * size must be multiple of 4.
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    eve5_flash_ctx_t * ctx = (eve5_flash_ctx_t *)drv->user_data;
    eve5_flash_file_t * file = (eve5_flash_file_t *)file_p;

    if(file == NULL || ctx == NULL || ctx->hal == NULL || ctx->alloc == NULL) {
        if(br) *br = 0;
        return LV_FS_RES_INV_PARAM;
    }

    uint32_t available = file->size - file->pos;
    uint32_t to_read = btr < available ? btr : available;

    if(to_read == 0) {
        if(br) *br = 0;
        return LV_FS_RES_OK;
    }

    uint32_t abs_addr = file->flash_addr + file->pos;
    uint32_t aligned_src = ALIGN_DOWN(abs_addr, 64);
    uint32_t head_pad = abs_addr - aligned_src;
    uint32_t aligned_num = ALIGN_UP(head_pad + to_read, 4);

#if LV_USE_OS
    lv_eve5_hal_lock(ctx->disp);
#endif

    EVE_GpuHandle handle = EVE_GpuAlloc_Alloc(ctx->alloc, aligned_num, GA_ALIGN_4);
    uint32_t ramg_addr = EVE_GpuAlloc_Get(ctx->alloc, handle);

    if(ramg_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate %u bytes in RAM_G for flash read", aligned_num);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        if(br) *br = 0;
        return LV_FS_RES_FS_ERR;
    }

    /* Pre-flush + SuppressErrorOverlay isolate a CMD_FLASHREAD fault to
     * this read so lv_eve5_reset_coprocessor can recover narrowly. */
    if(!EVE_Cmd_waitFlush(ctx->hal)) {
        EVE_GpuAlloc_Free(ctx->alloc, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        if(br) *br = 0;
        return LV_FS_RES_FS_ERR;
    }
    ctx->hal->SuppressErrorOverlay = true;

    EVE_CoCmd_flashRead(ctx->hal, ramg_addr, aligned_src, aligned_num);
    if(!EVE_Cmd_waitFlush(ctx->hal)) {
        LV_LOG_ERROR("CMD_FLASHREAD failed (src=0x%08X, num=%u)", aligned_src, aligned_num);
        if(ctx->hal->CmdFault) lv_eve5_reset_coprocessor(ctx->disp);
        EVE_GpuAlloc_Free(ctx->alloc, handle);
        ctx->hal->SuppressErrorOverlay = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        if(br) *br = 0;
        return LV_FS_RES_FS_ERR;
    }
    ctx->hal->SuppressErrorOverlay = false;

    EVE_Hal_rdMem(ctx->hal, buf, ramg_addr + head_pad, to_read);
    EVE_GpuAlloc_Free(ctx->alloc, handle);

#if LV_USE_OS
    lv_eve5_hal_unlock(ctx->disp);
#endif

    file->pos += to_read;
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
    eve5_flash_file_t * file = (eve5_flash_file_t *)file_p;
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
    eve5_flash_file_t * file = (eve5_flash_file_t *)file_p;
    if(file == NULL || pos_p == NULL) return LV_FS_RES_INV_PARAM;

    *pos_p = file->pos;
    return LV_FS_RES_OK;
}

/**********************
 * PATH CHECKING
 **********************/

bool lv_eve5_flash_is_path(const char * path)
{
    if(path == NULL || path[0] == '\0') return false;
    if(s_ctx.hal == NULL) return false;

    if(path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        char letter = path[0];
        if(letter >= 'a' && letter <= 'z') letter -= ('a' - 'A');
        char flash_letter = LV_FS_EVE5_FLASH_LETTER;
        if(flash_letter >= 'a' && flash_letter <= 'z') flash_letter -= ('a' - 'A');
        return letter == flash_letter;
    }
    return false;
}

/**********************
 * IMAGE LOADING
 **********************/

bool lv_eve5_flash_load_image(const char * path, EVE_GpuHandle *handle,
                              uint32_t * width, uint32_t * height, uint32_t * format,
                              uint32_t * image_offset, uint32_t * palette_offset)
{
    if(path == NULL || handle == NULL || width == NULL || height == NULL || format == NULL) {
        return false;
    }
    if(s_ctx.hal == NULL || s_ctx.alloc == NULL) {
        LV_LOG_ERROR("EVE5 flash driver not initialized");
        return false;
    }

    const char * addr_str = path;
    if(path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        addr_str = path + 2;
    }

    uint32_t flash_addr = parse_flash_addr(addr_str);
    if(flash_addr == UINT32_MAX) {
        LV_LOG_ERROR("Invalid flash address in path: %s", path);
        return false;
    }
    if((flash_addr & 63) != 0) {
        LV_LOG_ERROR("Flash address 0x%08X not 64-byte aligned (required for CMD_FLASHSOURCE)",
                     flash_addr);
        return false;
    }

    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        LV_LOG_WARN("Unsupported image format (not JPEG/PNG): %s", path);
        return false;
    }

    EVE_HalContext * phost = s_ctx.hal;
    EVE_GpuAlloc * alloc = s_ctx.alloc;

#if LV_USE_OS
    lv_eve5_hal_lock(s_ctx.disp);
#endif

    if(!ensure_flash_ready(&s_ctx)) {
        LV_LOG_ERROR("Flash not ready");
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    if(flash_addr >= s_ctx.flash_size_bytes) {
        LV_LOG_ERROR("Flash address 0x%08X exceeds flash size 0x%08X",
                     flash_addr, s_ctx.flash_size_bytes);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Query: dimensions, exact decoded size, EVE format, palette size,
     * HW-loadable gate. preferCmd=true picks CMD_QUERYIMAGE_flash on BT820+
     * with the patch_queryassets firmware (no RAM_G round-trip); otherwise
     * the wrapper chunks CMD_FLASHREAD into a small RAM_G staging buffer and
     * drives EVE_probeResource on the bytes. `scan_limit` bounds the SW scan
     * to the flash extent that's actually addressable — the probe normally
     * completes well within the first chunk for any real JPEG/PNG header. */
    uint32_t opts = OPT_TRUECOLOR;
    uint32_t scan_limit = s_ctx.flash_size_bytes - flash_addr;
    EVE_ResourceType hint = is_jpeg ? EVE_RESOURCE_JPEG : EVE_RESOURCE_PNG;

    EVE_ResourceInfo info;
    if(!EVE_queryResource_flash(phost, alloc, (int32_t)flash_addr, scan_limit,
                                hint, opts, /*preferCmd*/ true,
                                flash_query_reset_cb, s_ctx.disp,
                                &info)) {
        LV_LOG_WARN("EVE5 flash: queryResource failed for %s", path);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Grayscale JPEG auto-promote: re-query with OPT_MONO so info reflects the
     * L8 decode and opts carries OPT_MONO into the load. Silent fall-back when
     * the chip rejects the option. */
    if(info.Type == EVE_RESOURCE_JPEG && info.Channels == 1) {
        EVE_ResourceInfo info_mono;
        if(EVE_queryResource_flash(phost, alloc, (int32_t)flash_addr, scan_limit,
                                   hint, OPT_MONO, /*preferCmd*/ true,
                                   flash_query_reset_cb, s_ctx.disp,
                                   &info_mono)
           && (info_mono.Flags & EVE_RESOURCE_FLAG_HARDWARE_LOADABLE)) {
            info = info_mono;
            opts = OPT_MONO;
        }
    }

    if(info.Type != EVE_RESOURCE_JPEG && info.Type != EVE_RESOURCE_PNG) {
        LV_LOG_WARN("EVE5 flash: %s isn't a JPEG/PNG (type=%u)", path, (unsigned)info.Type);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    if(!(info.Flags & EVE_RESOURCE_FLAG_HARDWARE_LOADABLE)) {
        LV_LOG_INFO("EVE5 flash: %s not HW-decodable on this chip", path);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* GC-flagged: decoded images self-heal through the decoder cache when
     * the handle goes invalid (sweep on pre-BT820, pressure eviction on BT820+). */
    EVE_GpuHandle final_handle = EVE_GpuAlloc_Alloc(alloc, info.Size, GA_ALIGN_4 | GA_GC_FLAG);
    uint32_t final_addr = EVE_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 flash: failed to allocate %u bytes for %s",
                     (unsigned)info.Size, path);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Pre-flush + SuppressErrorOverlay isolate the CMD_LOADIMAGE_flash_ex fault. */
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_GpuAlloc_Free(alloc, final_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = true;

    /* Zero-copy decode straight from flash into the destination. The wrapper
     * bakes in OPT_FLASH | OPT_NODL and the BT820 CMD_NOP early-return workaround. */
    bool ok = EVE_CoCmd_loadImage_flash_ex(phost, final_addr, flash_addr, opts,
                                           NULL, NULL, NULL, NULL, NULL);
    if(!ok) {
        LV_LOG_ERROR("EVE5 flash: CMD_LOADIMAGE failed for %s", path);
        if(phost->CmdFault) lv_eve5_reset_coprocessor(s_ctx.disp);
        EVE_GpuAlloc_Free(alloc, final_handle);
        phost->SuppressErrorOverlay = false;
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

    /* Paletted layout is palette-front: palette (PaletteSize bytes) at base+0,
     * decoded bitmap at base+PaletteSize. PaletteSize == 0 for non-paletted. */
    *handle = final_handle;
    *width = info.Width;
    *height = info.Height;
    *format = info.Format;
    if(image_offset) *image_offset = info.PaletteSize;
    if(palette_offset) *palette_offset = (info.PaletteSize > 0) ? 0u : GA_INVALID;

    LV_LOG_INFO("EVE5 flash: loaded %s (%ux%u fmt=%u size=%u palette=%u)",
                path, (unsigned)info.Width, (unsigned)info.Height,
                (unsigned)info.Format, (unsigned)info.Size, (unsigned)info.PaletteSize);
    return true;
}

EVE_GpuAlloc * lv_eve5_flash_get_allocator(void)
{
    return s_ctx.alloc;
}

#elif LV_USE_EVE5 && LV_USE_FS_EVE5_FLASH

/* Linker stubs for chips without EVE_SUPPORT_FLASH (pre-BT815). */

void lv_fs_eve5_flash_init(lv_display_t * disp)          { (void)disp; }
void lv_fs_eve5_flash_deinit(void)                        {}
bool lv_eve5_flash_ready(void)                            { return false; }
bool lv_eve5_flash_is_path(const char * path)             { (void)path; return false; }
EVE_GpuAlloc * lv_eve5_flash_get_allocator(void)          { return NULL; }

bool lv_eve5_flash_load_image(const char * path, EVE_GpuHandle *handle,
                              uint32_t * width, uint32_t * height, uint32_t * format,
                              uint32_t * image_offset, uint32_t * palette_offset)
{
    (void)path; (void)handle; (void)width; (void)height;
    (void)format; (void)image_offset; (void)palette_offset;
    return false;
}

#endif /* LV_USE_EVE5 && LV_USE_FS_EVE5_FLASH && defined(EVE_SUPPORT_FLASH) */
