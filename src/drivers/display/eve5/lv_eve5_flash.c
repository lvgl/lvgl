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

    EVE_CoCmd_flashRead(ctx->hal, ramg_addr, aligned_src, aligned_num);
    if(!EVE_Cmd_waitFlush(ctx->hal)) {
        LV_LOG_ERROR("CMD_FLASHREAD failed (src=0x%08X, num=%u)", aligned_src, aligned_num);
        EVE_GpuAlloc_Free(ctx->alloc, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(ctx->disp);
#endif
        if(br) *br = 0;
        return LV_FS_RES_FS_ERR;
    }

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
        LV_LOG_ERROR("Flash address 0x%08X not 64-byte aligned (required for CMD_FLASHSOURCE)", flash_addr);
        return false;
    }

    bool is_jpeg = eve5_has_extension(path, ".jpg") || eve5_has_extension(path, ".jpeg");
    bool is_png = eve5_has_extension(path, ".png");
    if(!is_jpeg && !is_png) {
        LV_LOG_WARN("Unsupported image format (not JPEG/PNG): %s", path);
        return false;
    }

    EVE_HalContext *phost = s_ctx.hal;
    EVE_GpuAlloc *alloc = s_ctx.alloc;

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
        LV_LOG_ERROR("Flash address 0x%08X exceeds flash size 0x%08X", flash_addr, s_ctx.flash_size_bytes);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Read header for dimension parsing */
    uint32_t header_read_size = 1024;
    if(flash_addr + header_read_size > s_ctx.flash_size_bytes) {
        header_read_size = ALIGN_DOWN(s_ctx.flash_size_bytes - flash_addr, 4);
    }

    EVE_GpuHandle temp_handle = EVE_GpuAlloc_Alloc(alloc, header_read_size, GA_ALIGN_4);
    uint32_t temp_addr = EVE_GpuAlloc_Get(alloc, temp_handle);
    if(temp_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate temp RAM_G for flash header read");
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    EVE_CoCmd_flashRead(phost, temp_addr, flash_addr, header_read_size);
    if(!EVE_Cmd_waitFlush(phost)) {
        LV_LOG_ERROR("CMD_FLASHREAD failed for header at 0x%08X", flash_addr);
        EVE_GpuAlloc_Free(alloc, temp_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    uint8_t header_buf[1024];
    uint32_t header_size = header_read_size < sizeof(header_buf) ? header_read_size : sizeof(header_buf);
    EVE_Hal_rdMem(phost, header_buf, temp_addr, header_size);
    EVE_GpuAlloc_Free(alloc, temp_handle);

    uint32_t img_w = 0, img_h = 0;
    bool parsed;
    if(is_jpeg) {
        parsed = eve5_parse_jpeg_dimensions(header_buf, header_size, &img_w, &img_h);
    }
    else {
        parsed = eve5_parse_png_dimensions(header_buf, header_size, &img_w, &img_h);
    }

    if(is_png && header_size >= 26) {
        LV_LOG_INFO("PNG %s: %ux%u depth=%u color_type=%u",
                    path, img_w, img_h, header_buf[24], header_buf[25]);
    }
    else if(is_jpeg) {
        LV_LOG_INFO("JPEG %s: %ux%u", path, img_w, img_h);
    }

    if(!parsed || img_w == 0 || img_h == 0) {
        LV_LOG_ERROR("Failed to parse image header from flash at 0x%08X", flash_addr);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* Allocate decoded image buffer: worst case ARGB8, or paletted output
     * (palette plus w*h indices; 1024-byte palette for PALETTEDARGB8) —
     * larger for images under ~342 pixels. */
    uint32_t decoded_size = img_w * 4 * img_h;
    uint32_t paletted_size = 256 * 4 + img_w * img_h;
    if(paletted_size > decoded_size) decoded_size = paletted_size;

    uint32_t alloc_flags = GA_ALIGN_4 | (EVE_Hal_supportRenderTarget(phost) ? 0 : GA_GC_FLAG);
    EVE_GpuHandle final_handle = EVE_GpuAlloc_Alloc(alloc, decoded_size, alloc_flags);
    uint32_t final_addr = EVE_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_ERROR("Failed to allocate decoded image buffer (%u bytes)", decoded_size);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    /* OPT_TRUECOLOR is BT820-only — earlier gens decode to RGB565/ARGB4.
     * The wrapper bakes in OPT_FLASH | OPT_NODL plus the BT820 CMD_NOP
     * early-return workaround, and chooses CMD_GETIMAGE vs CMD_GETIMAGE_FORMAT
     * based on which outputs are requested. */
    uint32_t loadimage_opts = 0;
    if(EVE_Hal_supportRenderTarget(phost)) loadimage_opts |= OPT_TRUECOLOR;

    uint32_t out_source = final_addr;     /* BT815/816: CMD_GETIMAGE unavailable, source = alloc base */
    uint32_t out_fmt = 0;
    uint32_t out_w = img_w;               /* BT815/816: dims from JPEG/PNG header parse */
    uint32_t out_h = img_h;
    uint32_t out_palette = GA_INVALID;
    bool ok;

#if (EVE_SUPPORT_CHIPID >= EVE_BT817) || defined(EVE_MULTI_GRAPHICS_TARGET)
    if(EVE_CHIPID >= EVE_BT817) {
        ok = EVE_CoCmd_loadImage_flash_ex(phost, final_addr, flash_addr, loadimage_opts,
                                          &out_fmt, &out_source, &out_w, &out_h, &out_palette);
    }
    else
#endif
    {
        ok = EVE_CoCmd_loadImage_flash_ex(phost, final_addr, flash_addr, loadimage_opts,
                                          &out_fmt, NULL, NULL, NULL, NULL);
    }

    if(!ok) {
        LV_LOG_ERROR("CMD_LOADIMAGE from flash failed for %s", path);
        EVE_GpuAlloc_Free(alloc, final_handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(s_ctx.disp);
#endif
        return false;
    }

    EVE_Hal_requestFenceBeforeSwap(phost);

    uint32_t alloc_base = EVE_GpuAlloc_Get(alloc, final_handle);
    uint32_t img_ofs = (out_source >= alloc_base) ? (out_source - alloc_base) : 0;
    uint32_t pal_ofs = GA_INVALID;
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    if(out_fmt == PALETTEDARGB8 && out_palette >= alloc_base) {
        pal_ofs = out_palette - alloc_base;
    }
#endif

    /* Trim allocation to actual decoded size */
    int32_t bpp = eve5_format_bpp(out_fmt);
    uint32_t index_size = out_w * (uint32_t)bpp * out_h;
    uint32_t img_end = img_ofs + index_size;
    uint32_t pal_end = (pal_ofs != GA_INVALID) ? (pal_ofs + 256 * 4) : 0;
    uint32_t actual_size = img_end > pal_end ? img_end : pal_end;
    if(actual_size < decoded_size) {
        EVE_GpuAlloc_Truncate(alloc, final_handle, actual_size);
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

    LV_LOG_INFO("Loaded flash image %s (%ux%u, format=%u)", path, out_w, out_h, out_fmt);
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
