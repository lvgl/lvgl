/**
 * @file lv_draw_eve5_hal.c
 *
 * EVE5 (BT820) Hardware Abstraction Layer for LVGL Draw Unit
 *
 * Ported from BT81x EVE implementation with adaptations for:
 * - Render-to-texture architecture
 * - EVE5 coprocessor command interface
 * - Esd_GpuAlloc memory management
 *
 * This file contains:
 * - Layer initialization and finalization
 * - Image/bitmap drawing and format conversion
 * - Label/text drawing with font support
 * - Child layer compositing
 * - Software fallback buffer compositing
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_label.h"
#include "../lv_draw_image.h"

/**********************
 * STATIC VARIABLES
 **********************/

/* Used to pass unit context to label glyph callback (LVGL is single-threaded) */
static lv_draw_eve5_unit_t *s_current_unit = NULL;
static lv_layer_t *s_current_layer = NULL;

/**********************
 * STATIC PROTOTYPES
 **********************/

/* Image format conversion helpers */
static void convert_rgb565a8_to_argb8(const uint8_t *rgb, const uint8_t *alpha,
                                       uint8_t *dst, uint32_t w);
static void convert_xrgb8888_to_rgb8(const uint8_t *src, uint8_t *dst, uint32_t w);
static bool get_eve_format_info(lv_color_format_t src_cf,
                                 uint8_t *eve_format,
                                 uint8_t *bytes_per_pixel,
                                 bool *needs_conversion);
static uint32_t upload_image(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                              uint8_t *out_eve_format, int32_t *out_eve_stride);
static Esd_GpuHandle track_frame_alloc(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle);

/* Font/glyph helpers */
static void glyph_bitmap_to_ramg_aligned(lv_draw_eve5_unit_t *u, uint32_t addr,
                                          const uint8_t *src, uint32_t width,
                                          uint32_t height, uint32_t eve_stride,
                                          uint8_t src_stride_align);
static uint32_t upload_glyph(lv_draw_eve5_unit_t *u, const lv_font_fmt_txt_dsc_t *font_dsc,
                              uint32_t gid, uint16_t *out_stride);
static void draw_glyph_cb(lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                          lv_draw_fill_dsc_t *fill_dsc, const lv_area_t *fill_area);

/**********************
 * HELPER FUNCTIONS
 **********************/

/**
 * Set scissor region for clipping (used by all drawing functions)
 * Non-static so primitives.c can use it
 */
void lv_draw_eve5_set_scissor(lv_draw_eve5_unit_t *u, const lv_area_t *clip, const lv_area_t *layer_area)
{
    int32_t x = clip->x1 - layer_area->x1;
    int32_t y = clip->y1 - layer_area->y1;
    int32_t w = lv_area_get_width(clip);
    int32_t h = lv_area_get_height(clip);

    if(x < 0) { w += x; x = 0; }
    if(y < 0) { h += y; y = 0; }
    if(w <= 0 || h <= 0) { w = 0; h = 0; }

    EVE_CoDl_scissorXY(u->hal, x, y);
    EVE_CoDl_scissorSize(u->hal, w, h);
}

static Esd_GpuHandle track_frame_alloc(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle)
{
    if(handle.Id == GA_HANDLE_INVALID.Id) {
        return handle;
    }

    if(u->frame_alloc_count >= EVE5_MAX_FRAME_ALLOCS) {
        LV_LOG_WARN("EVE5: Frame allocation tracking overflow");
        return handle;
    }

    u->frame_allocs[u->frame_alloc_count++] = handle;
    return handle;
}

/**********************
 * LAYER MANAGEMENT
 **********************/

void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen)
{
    Esd_GpuHandle handle = { 0 };
    uint32_t ram_g_addr = GA_INVALID;

    /* Check if layer already has allocation */
    if(layer->user_data != NULL) {
        handle = Esd_GpuHandle_FromPtrType(layer->user_data);
        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);
    }

    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aligned_w = ALIGN_UP(w, 16);
    int32_t aligned_h = ALIGN_UP(h, 16);

    /* Allocate texture if needed */
    if(ram_g_addr == GA_INVALID) {
		uint32_t size = aligned_w * aligned_h * 4; /* ARGB8 */
        handle = Esd_GpuAlloc_Alloc(u->allocator, size, GA_ALIGN_128);
        layer->user_data = Esd_GpuHandle_ToPtrType(handle);

        ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);
        if(ram_g_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate layer texture (%"PRId32"x%"PRId32")", aligned_w, aligned_h);
            layer->user_data = NULL;
            return;
        }

        LV_LOG_INFO("EVE5: Allocated layer %p at RAM_G 0x%08X (%"PRId32"x%"PRId32")",
                    (void *)layer, ram_g_addr, aligned_w, aligned_h);
    }

    /* Set render target */
    EVE_CoCmd_renderTarget(u->hal, ram_g_addr, ARGB8, aligned_w, aligned_h);
    LV_LOG_INFO("EVE5: Set render target for layer %p at RAM_G0x%08X (%"PRId32"x%"PRId32")",
                (void *)layer, ram_g_addr, aligned_w, aligned_h);

    /* Start display list */
    EVE_CoCmd_dlStart(u->hal);

	/* Clear FULL aligned texture first */
	EVE_CoDl_scissorXY(u->hal, 0, 0);
	EVE_CoDl_scissorSize(u->hal, aligned_w, aligned_h);

	EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
	if(is_screen) {
		/* EVE_CoDl_clearColorRgb(u->hal, 0, 128, 0); */ /* TEST COLOR */
		EVE_CoDl_clearColorA(u->hal, 255);
	} else {
		/* EVE_CoDl_clearColorRgb(u->hal, 0, 0, 128); */ /* TEST COLOR */
		EVE_CoDl_clearColorA(u->hal, 0);
	}
	EVE_CoDl_clear(u->hal, 1, 1, 1);

	/* Restrict scissor to actual content */
	EVE_CoDl_scissorXY(u->hal, 0, 0);
	EVE_CoDl_scissorSize(u->hal, w, h);

    /* Set default state */
    EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
    EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen)
{
    LV_UNUSED(layer);
    LV_UNUSED(is_screen);

    EVE_CoDl_display(u->hal);
    EVE_CoCmd_swap(u->hal);
    EVE_CoCmd_graphicsFinish(u->hal);

	/* Get sync marker for deferred free */
    EVE_CmdSync sync = EVE_Cmd_sync(u->hal);

	/* Queue all tracked allocations for deferred free */
    for(uint16_t i = 0; i < u->frame_alloc_count; i++) {
        Esd_GpuAlloc5_DeferredFree(u->allocator, u->frame_allocs[i], sync);
    }
    u->frame_alloc_count = 0;

    // EVE_Cmd_waitFlush(u->hal);
}

/**********************
 * IMAGE FORMAT CONVERSION
 **********************/

static void convert_rgb565a8_to_argb8(const uint8_t *rgb, const uint8_t *alpha,
                                       uint8_t *dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        uint16_t rgb565 = ((const uint16_t *)rgb)[x];
        
        /* Expand 5/6/5 bits to 8 bits with proper bit replication for accuracy */
        uint8_t r5 = (rgb565 >> 11) & 0x1F;
        uint8_t g6 = (rgb565 >> 5) & 0x3F;
        uint8_t b5 = rgb565 & 0x1F;
        
        uint8_t r = (r5 << 3) | (r5 >> 2);  /* 5-bit to 8-bit */
        uint8_t g = (g6 << 2) | (g6 >> 4);  /* 6-bit to 8-bit */
        uint8_t b = (b5 << 3) | (b5 >> 2);  /* 5-bit to 8-bit */
        
        /* EVE ARGB8 is BGRA in memory (little-endian) */
        dst[4 * x + 0] = b;
        dst[4 * x + 1] = g;
        dst[4 * x + 2] = r;
        dst[4 * x + 3] = alpha[x];
    }
}

static void convert_xrgb8888_to_rgb8(const uint8_t *src, uint8_t *dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        /* LVGL XRGB8888 is BGRX in memory */
        dst[3 * x + 0] = src[4 * x + 0];  /* B */
        dst[3 * x + 1] = src[4 * x + 1];  /* G */
        dst[3 * x + 2] = src[4 * x + 2];  /* R */
        /* Skip X (alpha) byte */
    }
}

static bool get_eve_format_info(lv_color_format_t src_cf,
                                 uint8_t *eve_format,
                                 uint8_t *bytes_per_pixel,
                                 bool *needs_conversion)
{
    *needs_conversion = false;

    switch(src_cf) {
        case LV_COLOR_FORMAT_L8:
            *eve_format = L8;
            *bytes_per_pixel = 1;
            break;

        case LV_COLOR_FORMAT_A8:
            *eve_format = L8;
            *bytes_per_pixel = 1;
            break;

        case LV_COLOR_FORMAT_RGB565:
            *eve_format = RGB565;
            *bytes_per_pixel = 2;
            break;

        case LV_COLOR_FORMAT_RGB888:
            *eve_format = RGB8;
            *bytes_per_pixel = 3;
            break;

        case LV_COLOR_FORMAT_XRGB8888:
            *eve_format = RGB8;
            *bytes_per_pixel = 3;
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_ARGB8888:
            *eve_format = ARGB8;
            *bytes_per_pixel = 4;
            break;

        case LV_COLOR_FORMAT_RGB565A8:
            *eve_format = ARGB8;
            *bytes_per_pixel = 4;
            *needs_conversion = true;
            break;

        default:
            LV_LOG_WARN("EVE5: Unsupported image format %d", src_cf);
            return false;
    }

    return true;
}

/**********************
 * IMAGE UPLOAD
 **********************/

static uint32_t upload_image(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                              uint8_t *out_eve_format, int32_t *out_eve_stride)
{
    const uint8_t *src_buf = img_dsc->data;
    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    if(src_stride == 0) {
        src_stride = src_w * lv_color_format_get_size(src_cf);
    }

    uint8_t eve_format;
    uint8_t bpp;
    bool needs_conversion;

    if(!get_eve_format_info(src_cf, &eve_format, &bpp, &needs_conversion)) {
        return GA_INVALID;
    }

    int32_t eve_stride = ALIGN_UP(src_w * bpp, 4);
    int32_t eve_size = eve_stride * src_h;

    *out_eve_format = eve_format;
    *out_eve_stride = eve_stride;

    /* Check cache first */
    uint32_t cached_addr = lv_draw_eve5_image_cache_lookup(u, img_dsc, out_eve_format, out_eve_stride);
    if(cached_addr != GA_INVALID) {
        return cached_addr;
    }

    /* Allocate RAM_G space */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, eve_size, GA_ALIGN_4);
    uint32_t ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate image in RAM_G (%d bytes)", eve_size);
        return GA_INVALID;
    }

    /* Upload based on format */
    switch(src_cf) {
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_A8: {
            /* 1 byte per pixel, simple copy */
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, src_w);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_RGB565: {
            /* 2 bytes per pixel, direct copy */
            int32_t row_bytes = src_w * 2;
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, row_bytes);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_RGB888: {
            /* 3 bytes per pixel, direct copy */
            int32_t row_bytes = src_w * 3;
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, row_bytes);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_ARGB8888: {
            /* 4 bytes per pixel - DIRECT COPY, no conversion needed! */
            int32_t row_bytes = src_w * 4;
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, row_bytes);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_XRGB8888: {
            /* Convert XRGB8888 to RGB8 (strip alpha) */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return GA_INVALID;
            }

            for(int32_t y = 0; y < src_h; y++) {
                convert_xrgb8888_to_rgb8(src_buf + y * src_stride, tmp_buf, src_w);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        case LV_COLOR_FORMAT_RGB565A8: {
            /* Convert RGB565+A8 to ARGB8 */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return GA_INVALID;
            }

            /* Alpha plane follows RGB data */
            const uint8_t *alpha_buf = src_buf + src_h * src_stride;
            int32_t alpha_stride = src_stride / 2;  /* Alpha is 1 byte per pixel vs 2 for RGB565 */

            for(int32_t y = 0; y < src_h; y++) {
                convert_rgb565a8_to_argb8(src_buf + y * src_stride,
                                           alpha_buf + y * alpha_stride,
                                           tmp_buf, src_w);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        default:
            /* Should not reach here due to earlier check */
            Esd_GpuAlloc_Free(u->allocator, handle);
            return GA_INVALID;
    }

    /* Insert into cache (allocator owns the memory, we just index it) */
    lv_draw_eve5_image_cache_insert(u, img_dsc, handle, eve_format, eve_stride);

    LV_LOG_TRACE("EVE5: Uploaded image %dx%d cf=%d as EVE format %d at 0x%08X",
                 src_w, src_h, src_cf, eve_format, ram_g_addr);

    return ram_g_addr;
}

/**********************
 * IMAGE DRAWING
 **********************/

void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    lv_draw_image_dsc_t *dsc = t->draw_dsc;

    /* Check if this is a LAYER composite task */
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_draw_eve5_hal_render_child(u, (lv_draw_task_t *)t, layer); // TODO: This depends on whether it's a GPU or CPU rendered layer
        return;
    }

    /* Regular image */
    if(lv_image_src_get_type(dsc->src) != LV_IMAGE_SRC_VARIABLE) {
        LV_LOG_WARN("EVE5: Only variable images supported");
        return;
    }

    const lv_image_dsc_t *img_dsc = dsc->src;
    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;

    uint8_t eve_format;
    int32_t eve_stride;

    /* Upload image to RAM_G or get cached image */
    uint32_t ram_g_addr = upload_image(u, img_dsc, &eve_format, &eve_stride);
    if(ram_g_addr == GA_INVALID) {
        return;
    }

    /* Calculate position in layer coordinates */
    int32_t x = t->area.x1 - layer->buf_area.x1;
    int32_t y = t->area.y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorA(u->hal, dsc->opa);

    /* Handle recolor */
    if(dsc->recolor_opa > LV_OPA_MIN) {
        lv_color_t mixed = lv_color_mix(dsc->recolor, lv_color_white(), dsc->recolor_opa);
        EVE_CoDl_colorRgb(u->hal, mixed.red, mixed.green, mixed.blue);
    }
    else {
        EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    }

    /* Set up bitmap */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, eve_format, eve_stride, src_h);
    EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, src_w, src_h);

    /* Handle rotation and scaling */
    if(dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
        int32_t clip_x = t->clip_area.x1 - layer->buf_area.x1;
        int32_t clip_y = t->clip_area.y1 - layer->buf_area.y1;

        EVE_CoCmd_loadIdentity(u->hal);
        EVE_CoCmd_translate(u->hal, F16(x - clip_x + dsc->pivot.x), F16(y - clip_y + dsc->pivot.y));

        if(dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE) {
            EVE_CoCmd_scale(u->hal, F16_SCALE_DIV_256(dsc->scale_x), F16_SCALE_DIV_256(dsc->scale_y));
        }
        if(dsc->rotation != 0) {
            EVE_CoCmd_rotate(u->hal, DEGREES(dsc->rotation));
        }

        EVE_CoCmd_translate(u->hal, -F16(dsc->pivot.x), -F16(dsc->pivot.y));
        EVE_CoCmd_setMatrix(u->hal);
        EVE_CoCmd_loadIdentity(u->hal);

        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, clip_x, clip_y);
        EVE_CoDl_end(u->hal);
    }
    else {
        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, x, y);
        EVE_CoDl_end(u->hal);
    }
}

/**********************
 * GLYPH UPLOAD
 **********************/

static void glyph_bitmap_to_ramg_aligned(lv_draw_eve5_unit_t *u, uint32_t addr,
                                          const uint8_t *src, uint32_t width,
                                          uint32_t height, uint32_t eve_stride,
                                          uint8_t src_stride_align)
{
    uint32_t natural_stride = (width + 1) / 2;

    /* Allocate row buffer for aligned stride */
    uint8_t *row_buf = lv_malloc(eve_stride);
    if(!row_buf) {
        LV_LOG_ERROR("EVE5: Failed to allocate glyph row buffer");
        return;
    }

    /* Clear buffer to ensure padding bytes are zero */
    lv_memzero(row_buf, eve_stride);

    /* Simple case: source has no special alignment */
    if(src_stride_align == 1 || (src_stride_align == 0 && width % 2 == 0)) {
        for(uint32_t y = 0; y < height; y++) {
            lv_memcpy(row_buf, src + y * natural_stride, natural_stride);
            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
        return;
    }

    /* Aligned source stride case */
    if(src_stride_align > 0) {
        uint32_t src_stride = ALIGN_UP(natural_stride, src_stride_align);
        for(uint32_t y = 0; y < height; y++) {
            lv_memcpy(row_buf, src + y * src_stride, natural_stride);
            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
        return;
    }

    /* Complex case: need to repack nibbles (src_stride_align == 0 and odd width) */
    uint32_t src_i = 0;
    uint8_t key = 0;

    for(uint32_t y = 0; y < height; y++) {
        lv_memzero(row_buf, eve_stride);  /* Clear for each row */
        
        uint32_t row_i;
        for(row_i = 0; row_i < (width / 2); ++row_i) {
            uint8_t n1, n2;
            if(key == 0) {
                n1 = GET_NIBBLE_HI(src[src_i]);
                n2 = GET_NIBBLE_LO(src[src_i]);
            }
            else {
                n1 = GET_NIBBLE_LO(src[src_i - 1]);
                n2 = GET_NIBBLE_HI(src[src_i]);
            }
            row_buf[row_i] = (n1 << 4) | n2;
            src_i++;
        }

        /* Last nibble if odd width */
        if(width % 2 != 0) {
            row_buf[row_i] = (key == 0) ?
                             (GET_NIBBLE_HI(src[src_i]) << 4) :
                             (GET_NIBBLE_LO(src[src_i - 1]) << 4);
        }

        key = (key == 0) ? 1 : 0;
        src_i += (key == 1) ? 1 : 0;

        EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
    }

    lv_free(row_buf);
}

static uint32_t upload_glyph(lv_draw_eve5_unit_t *u, const lv_font_fmt_txt_dsc_t *font_dsc,
                              uint32_t gid, uint16_t *out_stride)
{
    const lv_font_fmt_txt_glyph_dsc_t *glyph_dsc = &font_dsc->glyph_dsc[gid];
    const uint8_t *glyph_bitmap = &font_dsc->glyph_bitmap[glyph_dsc->bitmap_index];

    uint16_t g_w = glyph_dsc->box_w;
    uint16_t g_h = glyph_dsc->box_h;

    /* L4 format: 4 bits per pixel, so stride = ceil(width / 2) bytes
     * Align stride to 4 bytes for optimal memory access */
    uint16_t g_stride_natural = (g_w + 1) / 2;
    uint16_t g_stride = ALIGN_UP(g_stride_natural, 4);
    uint32_t glyph_size = g_stride * g_h;

    *out_stride = g_stride;

    /* Check cache first */
    uint32_t cached_addr = lv_draw_eve5_glyph_cache_lookup(u, glyph_bitmap, out_stride);
    if(cached_addr != GA_INVALID) {
        return cached_addr;
    }

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, glyph_size, GA_ALIGN_4);
    uint32_t ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(ram_g_addr == GA_INVALID) {
        return GA_INVALID;
    }

    /* Upload glyph data with aligned stride */
    glyph_bitmap_to_ramg_aligned(u, ram_g_addr, glyph_bitmap, g_w, g_h,
                                  g_stride, font_dsc->stride);

    /* Insert into cache */
    lv_draw_eve5_glyph_cache_insert(u, glyph_bitmap, handle, g_w, g_h, g_stride);

    return ram_g_addr;
}

/**********************
 * LABEL DRAWING
 **********************/

static void draw_glyph_cb(lv_draw_task_t *t, lv_draw_glyph_dsc_t *glyph_dsc,
                          lv_draw_fill_dsc_t *fill_dsc, const lv_area_t *fill_area)
{
    LV_UNUSED(t);

    lv_draw_eve5_unit_t *u = s_current_unit;
    lv_layer_t *layer = s_current_layer;

    if(u == NULL || layer == NULL) return;

    /* Draw underline/strikethrough */
    if(fill_dsc && fill_area) {
        int32_t x1 = fill_area->x1 - layer->buf_area.x1;
        int32_t y1 = fill_area->y1 - layer->buf_area.y1;
        int32_t x2 = fill_area->x2 - layer->buf_area.x1;
        int32_t y2 = fill_area->y2 - layer->buf_area.y1;

        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1, y1);
        EVE_CoDl_vertex2f_0(u->hal, x2, y2);
        EVE_CoDl_end(u->hal);
    }

    if(glyph_dsc == NULL) return;

    const lv_font_t *font = glyph_dsc->g->resolved_font;

    /* Check font compatibility */
    if(font->get_glyph_bitmap != lv_font_get_bitmap_fmt_txt) {
        LV_LOG_WARN("EVE5: Only static fonts supported");
        return;
    }

    const lv_font_fmt_txt_dsc_t *font_dsc = font->dsc;
    if(font_dsc->bpp != 4) {
        LV_LOG_WARN("EVE5: Only 4bpp fonts supported");
        return;
    }

    uint32_t gid = glyph_dsc->g->gid.index;
    const lv_font_fmt_txt_glyph_dsc_t *g_dsc = &font_dsc->glyph_dsc[gid];

    uint16_t g_w = g_dsc->box_w;
    uint16_t g_h = g_dsc->box_h;
    uint16_t g_stride;

    uint32_t ram_g_addr = upload_glyph(u, font_dsc, gid, &g_stride);
    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to upload glyph");
        return;
    }

    int32_t x = glyph_dsc->letter_coords->x1 - layer->buf_area.x1;
    int32_t y = glyph_dsc->letter_coords->y1 - layer->buf_area.y1;

    EVE_CoDl_colorRgb(u->hal, glyph_dsc->color.red, glyph_dsc->color.green, glyph_dsc->color.blue);
    EVE_CoDl_colorA(u->hal, glyph_dsc->opa);

    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, L4, g_stride, g_h);
    EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, g_w, g_h);

    EVE_CoDl_vertex2f_0(u->hal, x, y);
}

void lv_draw_eve5_hal_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    lv_draw_label_dsc_t *dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;

    /* Check if font is compatible with bitmap rendering */
    bool use_bitmap_font = false;
    if(dsc->font && dsc->font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
        const lv_font_fmt_txt_dsc_t *font_dsc = dsc->font->dsc;
        if(font_dsc->bpp == 4) {
            use_bitmap_font = true;
        }
    }

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    if(use_bitmap_font) {
        /* Use LVGL's character iteration with our glyph callback */
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_begin(u->hal, BITMAPS);

        /* Store context for callback (LVGL is single-threaded) */
        s_current_unit = u;
        s_current_layer = layer;

        lv_draw_label_iterate_characters(t, dsc, &t->area, draw_glyph_cb);

        s_current_unit = NULL;
        s_current_layer = NULL;

        EVE_CoDl_end(u->hal);
    }
    else {
        /* Fallback to EVE ROM fonts */
        int32_t x = t->area.x1 - layer->buf_area.x1;
        int32_t y = t->area.y1 - layer->buf_area.y1;

        uint8_t eve_font = 31;
        if(dsc->font) {
            int32_t font_h = lv_font_get_line_height(dsc->font);
            if(font_h <= 8) eve_font = 16;
            else if(font_h <= 13) eve_font = 26;
            else if(font_h <= 16) eve_font = 27;
            else if(font_h <= 20) eve_font = 29;
            else if(font_h <= 25) eve_font = 30;
            else eve_font = 31;
        }

        EVE_CoCmd_text(u->hal, x, y, eve_font, 0, dsc->text);
    }
}

/**********************
 * CHILD LAYER COMPOSITING
 **********************/

void lv_draw_eve5_hal_render_child(lv_draw_eve5_unit_t *u, lv_draw_task_t *t, lv_layer_t *parent_layer)
{
    EVE_HalContext *phost = u->hal;
    lv_draw_image_dsc_t *dsc = t->draw_dsc;
    lv_layer_t *child_layer = (lv_layer_t *)dsc->src;

    Esd_GpuHandle child_handle = Esd_GpuHandle_FromPtrType(child_layer->user_data);
    uint32_t child_addr = Esd_GpuAlloc_Get(u->allocator, child_handle);

    if(child_addr == GA_INVALID) {
		LV_LOG_WARN("EVE5: Child layer %p texture invalid, handle id %i", child_layer, (int)child_handle.Id);
        return;
    }

	printf("EVE5: Compositing child layer %p at RAM_G 0x%08X with handle id %i onto parent layer %p\n",
	    child_layer, child_addr, (int)child_handle.Id, parent_layer);

    int32_t child_w = lv_area_get_width(&child_layer->buf_area);
    int32_t child_h = lv_area_get_height(&child_layer->buf_area);
    int32_t stride = ALIGN_UP(child_w, 16) * 4;
    int32_t aligned_h = ALIGN_UP(child_h, 16);

    int32_t x = child_layer->buf_area.x1 - parent_layer->buf_area.x1;
    int32_t y = child_layer->buf_area.y1 - parent_layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &parent_layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(u->hal, child_addr);
    EVE_CoDl_bitmapLayout(u->hal, ARGB8, stride, aligned_h);
    EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, child_w, child_h);

    EVE_CoDl_begin(u->hal, BITMAPS);
    EVE_CoDl_vertex2f_0(u->hal, x, y);
    EVE_CoDl_end(u->hal);

    /* Track child texture for deferred free after parent finishes */
    track_frame_alloc(u, child_handle); /* DEFERRED_FREE */
    child_layer->user_data = NULL; /* Clear to prevent double-free */
}

/**********************
 * TEXTURE DRAWING (for cached SW content)
 **********************/

Esd_GpuHandle lv_draw_eve5_hal_upload_texture(lv_draw_eve5_unit_t *u,
                                               const uint8_t *buf_data,
                                               int32_t buf_w, int32_t buf_h,
                                               uint32_t *out_stride)
{
    uint32_t eve_stride = ALIGN_UP(buf_w * 4, 4);
    uint32_t eve_size = eve_stride * buf_h;

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, eve_size, GA_ALIGN_4);
    uint32_t ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate texture (%"PRIu32" bytes)", eve_size);
        return GA_HANDLE_INVALID;
    }

    uint32_t src_stride = buf_w * 4;
    if(eve_stride == src_stride) {
        EVE_Hal_wrMem(u->hal, ram_g_addr, buf_data, eve_size);
    }
    else {
        for(int32_t y = 0; y < buf_h; y++) {
            EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                          buf_data + y * src_stride, src_stride);
        }
    }

    *out_stride = eve_stride;

    LV_LOG_TRACE("EVE5: Uploaded texture %"PRId32"x%"PRId32" to 0x%08X",
                 buf_w, buf_h, ram_g_addr);

    return handle;
}

void lv_draw_eve5_hal_draw_texture(lv_draw_eve5_unit_t *u,
                                    const lv_draw_task_t *t,
                                    uint32_t ram_g_addr,
                                    int32_t tex_w, int32_t tex_h,
                                    uint32_t eve_stride,
                                    const lv_area_t *draw_area)
{
    lv_layer_t *layer = t->target_layer;

    if(ram_g_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Invalid RAM_G address for texture draw");
        return;
    }

    int32_t x = draw_area->x1 - layer->buf_area.x1;
    int32_t y = draw_area->y1 - layer->buf_area.y1;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);

    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, ARGB8, eve_stride, tex_h);
    EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, tex_w, tex_h);

    EVE_CoDl_begin(u->hal, BITMAPS);
    EVE_CoDl_vertex2f_0(u->hal, x, y);
    EVE_CoDl_end(u->hal);
}

/**********************
 * UTILITIES
 **********************/

bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle)
{
    return (Esd_GpuAlloc_Get(u->allocator, handle) != GA_INVALID);
}

#endif /* LV_USE_DRAW_EVE5 */
