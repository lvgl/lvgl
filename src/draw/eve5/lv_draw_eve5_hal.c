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
 * - Software fallback buffer compositing
 * - Texture upload/draw utilities
 * - Mask rectangle drawing
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_mask_private.h"

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

void lv_draw_eve5_clear_stencil(lv_draw_eve5_unit_t *u,
                                 int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                 const lv_area_t *clip, const lv_area_t *layer_area)
{
    lv_area_t bbox = { x1 + layer_area->x1, y1 + layer_area->y1,
                       x2 + layer_area->x1, y2 + layer_area->y1 };
    lv_area_t clear_area;
    if(!lv_area_intersect(&clear_area, &bbox, clip)) return;
    lv_draw_eve5_set_scissor(u, &clear_area, layer_area);
    /* clearStencil defaults to 0 and nothing changes it */
    EVE_CoDl_clear(u->hal, 0, 1, 0);
    lv_draw_eve5_set_scissor(u, clip, layer_area);
}

Esd_GpuHandle track_frame_alloc(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle)
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

    /* Reset alpha repair tracking for this layer */
    u->has_alpha_opaque = false;
    u->has_alpha_trashed = false;
}

void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen)
{
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
    /* Bitmap transform may be non-identity from a previous draw */
    EVE_CoDl_bitmapTransform_identity(u->hal);

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

/**********************
 * MASK RECTANGLE
 **********************/

void lv_draw_eve5_hal_draw_mask_rect(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t *layer = t->target_layer;
    const lv_draw_mask_rect_dsc_t *dsc = t->draw_dsc;

    /* Convert mask area from absolute to layer-local coordinates */
    int32_t mask_x1 = dsc->area.x1 - layer->buf_area.x1;
    int32_t mask_y1 = dsc->area.y1 - layer->buf_area.y1;
    int32_t mask_x2 = dsc->area.x2 - layer->buf_area.x1;
    int32_t mask_y2 = dsc->area.y2 - layer->buf_area.y1;

    /* Layer bounds in local coordinates */
    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);

    /* Step 1: Clear stencil (clearStencil defaults to 0) */
    EVE_CoDl_clear(phost, 0, 1, 0);

    /* Step 2: Draw rounded rect mask — multiply existing RGBA AND flag stencil.
     * This runs after both the RGB pass and the alpha correction pass, so all
     * four channels have their correct premultiplied values at this point.
     * We must scale ALL channels (not just alpha) because the layer content is
     * premultiplied — scaling only alpha would leave RGB too bright at partially
     * masked edges, causing white fringing when composited onto the parent.
     * blend(ZERO, SRC_ALPHA): dst.rgba = dst.rgba * src.a / 255
     * stencilOp(KEEP, INCR): stencil++ where the primitive draws. */
    EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 0xFF);
    EVE_CoDl_stencilOp(phost, KEEP, INCR);

    lv_draw_eve5_draw_rect(u, mask_x1, mask_y1, mask_x2, mask_y2,
                            dsc->radius, &t->clip_area, &layer->buf_area);

    /* Step 3: Zero RGBA everywhere the mask primitive didn't draw.
     * stencilFunc(EQUAL, 0): only affect pixels where stencil is still 0.
     * blend(ZERO, ZERO): result = 0 — clears all channels (correct for
     * premultiplied: fully transparent is 0,0,0,0). */
    if(!dsc->keep_outside) {
        EVE_CoDl_stencilFunc(phost, EQUAL, 0, 0xFF);
        EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        EVE_CoDl_colorA(phost, 0);
        EVE_CoDl_blendFunc(phost, ZERO, ZERO);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_vertex2f_0(phost, layer_w, layer_h);
        EVE_CoDl_end(phost);
    }

    EVE_CoDl_restoreContext(phost);
}

#endif /* LV_USE_DRAW_EVE5 */
