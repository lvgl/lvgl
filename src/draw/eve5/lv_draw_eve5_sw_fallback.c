/**
 * @file lv_draw_eve5_sw_fallback.c
 *
 * EVE5 (BT820) Software Fallback Rendering
 *
 * When LV_DRAW_EVE5_SW_* flags are enabled for QA testing, individual task
 * types are rendered via LVGL's software renderer instead of EVE hardware.
 * The SW-rendered ARGB8 buffer is uploaded to RAM_G and blitted as a texture.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5 && LV_DRAW_EVE5_SW_FALLBACK

#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#if LV_DRAW_EVE5_SW_VECTOR
    #include "../lv_draw_vector_private.h"
#endif
#if LV_DRAW_EVE5_SW_OUTLINE_FONT
    #include "../../font/freetype/lv_freetype_private.h"
#endif

/**********************
 * SW FALLBACK HELPERS
 **********************/

#if LV_DRAW_EVE5_SW_VECTOR
/**
 * Choose the software buffer area for a vector task. The software vector renderer adjusts the
 * initial task clip only by partial_y_offset, and paths cannot widen that clip. Start the buffer at
 * x = 0 so the clip and buffer coordinates agree.
 */
static void eve5_sw_vector_area(const lv_draw_task_t * t, lv_area_t * area)
{
    *area = t->_real_area;
    if(area->x1 > 0) area->x1 = 0;
}
#endif

/**
 * Get descriptor data pointer and size for cache comparison.
 * Returns pointer to data AFTER the base descriptor.
 */
const void * lv_draw_eve5_sw_get_dsc_cache_data(const lv_draw_task_t * t, uint32_t * out_size)
{
    const lv_draw_dsc_base_t * base = t->draw_dsc;
    uint32_t full_size = (uint32_t)base->dsc_size;
    uint32_t base_size = sizeof(lv_draw_dsc_base_t);

    if(full_size <= base_size) {
        *out_size = 0;
        return NULL;
    }

    *out_size = full_size - base_size;
    return (const uint8_t *)base + base_size;
}

/**
 * Render task to a CPU buffer using SW fallback. Caller must free returned buffer.
 */
uint8_t * lv_draw_eve5_sw_render_to_buffer(lv_draw_eve5_unit_t * u,
                                           const lv_draw_task_t * t,
                                           int32_t buf_w, int32_t buf_h)
{
    LV_UNUSED(u);

    uint32_t buf_stride = buf_w * 4;
    uint32_t buf_size = buf_stride * buf_h;
    uint8_t * buf_data = lv_malloc(buf_size);

    if(!buf_data) {
        LV_LOG_ERROR("EVE5: Failed to allocate SW buffer (%"PRIu32" bytes)", buf_size);
        return NULL;
    }
    lv_memzero(buf_data, buf_size);

    lv_draw_buf_t sw_buf;
    /* Packed rows, as lv_draw_eve5_hal_upload_texture reads them. The
     * automatic stride can be wider with LV_DRAW_BUF_STRIDE_ALIGN. */
    lv_draw_buf_init(&sw_buf, buf_w, buf_h,
                     LV_COLOR_FORMAT_ARGB8888, buf_stride,
                     buf_data, buf_size);

    lv_area_t norm_area;
    lv_area_set(&norm_area, 0, 0, buf_w - 1, buf_h - 1);

    /*
     * Copy the original task's opacity to the layer so tasks added by lv_draw_add_task inherit it.
     */
    lv_layer_t temp_layer;
    lv_layer_init(&temp_layer);
    temp_layer.opa = t->opa;
    temp_layer.draw_buf = &sw_buf;
    temp_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    temp_layer.buf_area = norm_area;
    temp_layer._clip_area = norm_area;
    temp_layer.phy_clip_area = norm_area;

    int32_t ofs_x = t->_real_area.x1;
    int32_t ofs_y = t->_real_area.y1;
    (void)ofs_x;
    (void)ofs_y;

    bool render_ok = false;

    /* user_data = (void *)1 marks task for SW fallback, preventing EVE5 from reclaiming it */
    switch(t->type) {
#if LV_DRAW_EVE5_SW_FILL
        case LV_DRAW_TASK_TYPE_FILL: {
                lv_draw_fill_dsc_t * src_dsc = t->draw_dsc;

                lv_area_t norm_task_area;
                norm_task_area.x1 = t->area.x1 - ofs_x;
                norm_task_area.y1 = t->area.y1 - ofs_y;
                norm_task_area.x2 = t->area.x2 - ofs_x;
                norm_task_area.y2 = t->area.y2 - ofs_y;

                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.base.user_data = (void *)1;
                rect_dsc.bg_color = src_dsc->color;
                rect_dsc.bg_grad = src_dsc->grad;
                rect_dsc.radius = src_dsc->radius;
                rect_dsc.bg_opa = src_dsc->opa;

                lv_draw_rect(&temp_layer, &rect_dsc, &norm_task_area);
                render_ok = true;
                break;
            }
#endif

#if LV_DRAW_EVE5_SW_BORDER
        case LV_DRAW_TASK_TYPE_BORDER: {
                lv_draw_border_dsc_t * src_dsc = t->draw_dsc;

                lv_area_t norm_task_area;
                norm_task_area.x1 = t->area.x1 - ofs_x;
                norm_task_area.y1 = t->area.y1 - ofs_y;
                norm_task_area.x2 = t->area.x2 - ofs_x;
                norm_task_area.y2 = t->area.y2 - ofs_y;

                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.base.user_data = (void *)1;
                rect_dsc.bg_opa = LV_OPA_TRANSP;
                rect_dsc.radius = src_dsc->radius;
                rect_dsc.border_color = src_dsc->color;
                rect_dsc.border_opa = src_dsc->opa;
                rect_dsc.border_side = src_dsc->side;
                rect_dsc.border_width = src_dsc->width;

                lv_draw_rect(&temp_layer, &rect_dsc, &norm_task_area);
                render_ok = true;
                break;
            }
#endif

#if LV_DRAW_EVE5_SW_LINE
        case LV_DRAW_TASK_TYPE_LINE: {
                lv_draw_line_dsc_t line_dsc;
                lv_memcpy(&line_dsc, t->draw_dsc, sizeof(line_dsc));
                line_dsc.base.user_data = (void *)1;

                line_dsc.p1.x -= ofs_x;
                line_dsc.p1.y -= ofs_y;
                line_dsc.p2.x -= ofs_x;
                line_dsc.p2.y -= ofs_y;

                lv_draw_line(&temp_layer, &line_dsc);
                render_ok = true;
                break;
            }
#endif

#if LV_DRAW_EVE5_SW_TRIANGLE
        case LV_DRAW_TASK_TYPE_TRIANGLE: {
                lv_draw_triangle_dsc_t tri_dsc;
                lv_memcpy(&tri_dsc, t->draw_dsc, sizeof(tri_dsc));
                tri_dsc.base.user_data = (void *)1;

                tri_dsc.p[0].x -= ofs_x;
                tri_dsc.p[0].y -= ofs_y;
                tri_dsc.p[1].x -= ofs_x;
                tri_dsc.p[1].y -= ofs_y;
                tri_dsc.p[2].x -= ofs_x;
                tri_dsc.p[2].y -= ofs_y;

                lv_draw_triangle(&temp_layer, &tri_dsc);
                render_ok = true;
                break;
            }
#endif

#if LV_DRAW_EVE5_SW_LABEL || LV_DRAW_EVE5_SW_OUTLINE_FONT
        case LV_DRAW_TASK_TYPE_LABEL: {
                lv_draw_label_dsc_t label_dsc;
                lv_memcpy(&label_dsc, t->draw_dsc, sizeof(label_dsc));
                label_dsc.base.user_data = (void *)1;

                lv_area_t norm_task_area;
                norm_task_area.x1 = t->area.x1 - ofs_x;
                norm_task_area.y1 = t->area.y1 - ofs_y;
                norm_task_area.x2 = t->area.x2 - ofs_x;
                norm_task_area.y2 = t->area.y2 - ofs_y;

                lv_draw_label(&temp_layer, &label_dsc, &norm_task_area);
                render_ok = true;
                break;
            }
#endif

#if LV_DRAW_EVE5_SW_ARC
        case LV_DRAW_TASK_TYPE_ARC: {
                lv_draw_arc_dsc_t arc_dsc;
                lv_memcpy(&arc_dsc, t->draw_dsc, sizeof(arc_dsc));
                arc_dsc.base.user_data = (void *)1;

                arc_dsc.center.x -= ofs_x;
                arc_dsc.center.y -= ofs_y;

                lv_draw_arc(&temp_layer, &arc_dsc);
                render_ok = true;
                break;
            }
#endif

#if LV_DRAW_EVE5_SW_BOX_SHADOW
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: {
                lv_draw_box_shadow_dsc_t * src_dsc = t->draw_dsc;

                lv_area_t norm_task_area;
                norm_task_area.x1 = t->area.x1 - ofs_x;
                norm_task_area.y1 = t->area.y1 - ofs_y;
                norm_task_area.x2 = t->area.x2 - ofs_x;
                norm_task_area.y2 = t->area.y2 - ofs_y;

                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.base.user_data = (void *)1;
                rect_dsc.bg_opa = LV_OPA_TRANSP;
                rect_dsc.radius = src_dsc->radius;
                rect_dsc.shadow_color = src_dsc->color;
                rect_dsc.shadow_opa = src_dsc->opa;
                rect_dsc.shadow_width = src_dsc->width;
                rect_dsc.shadow_spread = src_dsc->spread;
                rect_dsc.shadow_offset_x = src_dsc->ofs_x;
                rect_dsc.shadow_offset_y = src_dsc->ofs_y;

                lv_draw_rect(&temp_layer, &rect_dsc, &norm_task_area);
                render_ok = true;
                break;
            }
#endif

#if LV_DRAW_EVE5_SW_VECTOR
        case LV_DRAW_TASK_TYPE_VECTOR: {
                /*
                 * Vector paths use screen coordinates, so place the layer in the same coordinate
                 * system (see eve5_sw_vector_area).
                 */
                eve5_sw_vector_area(t, &temp_layer.buf_area);
                temp_layer._clip_area = t->clip_area;
                temp_layer.phy_clip_area = t->clip_area;
                temp_layer.partial_y_offset = temp_layer.buf_area.y1;

                /*
                 * Transfer ownership of the vector task list to the software task. Rendering
                 * consumes and destroys the list, so it can be rendered only once.
                 */
                lv_draw_vector_dsc_t * src_dsc = t->draw_dsc;
                lv_draw_vector_dsc_t vector_dsc;
                lv_memcpy(&vector_dsc, src_dsc, sizeof(vector_dsc));
                vector_dsc.base.layer = &temp_layer;
                vector_dsc.base.user_data = (void *)1;
                src_dsc->task_list = NULL;

                lv_draw_vector(&vector_dsc);
                render_ok = true;
                break;
            }
#endif

        default:
            LV_LOG_WARN("EVE5: No SW fallback for task type %d", t->type);
            break;
    }

    if(!render_ok) {
        lv_free(buf_data);
        return NULL;
    }

    /* Dispatch to SW renderer.
     * Unlock HAL mutex while SW threads run, as they may trigger image
     * decoders or filesystem access that needs the HAL mutex. */
    lv_display_t * disp = lv_refr_get_disp_refreshing();
#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif
    while(temp_layer.draw_task_head) {
        lv_draw_dispatch_layer(disp, &temp_layer);
        if(temp_layer.draw_task_head) {
            lv_draw_dispatch_wait_for_request();
        }
    }
#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif

    return buf_data;
}

/**
 * Render a task via SW fallback with caching.
 */
EVE_GpuHandle lv_draw_eve5_sw_render_cached(lv_draw_eve5_unit_t * u,
                                            const lv_draw_task_t * t,
                                            int32_t * out_w, int32_t * out_h,
                                            uint32_t * out_stride,
                                            bool *out_from_cache)
{
    *out_from_cache = false;

    int32_t buf_w = lv_area_get_width(&t->_real_area);
    int32_t buf_h = lv_area_get_height(&t->_real_area);

    if(buf_w <= 0 || buf_h <= 0) {
        return GA_HANDLE_INVALID;
    }

    uint32_t dsc_size;
    const void * dsc_data = lv_draw_eve5_sw_get_dsc_cache_data(t, &dsc_size);

    EVE_GpuHandle cached_handle;
    uint32_t cached_stride;

    if(lv_draw_eve5_sw_cache_lookup(u, t->type, buf_w, buf_h,
                                    dsc_data, dsc_size,
                                    &cached_handle, &cached_stride)) {
        *out_w = buf_w;
        *out_h = buf_h;
        *out_stride = cached_stride;
        *out_from_cache = true;
        return cached_handle;
    }

    uint8_t * buf_data = lv_draw_eve5_sw_render_to_buffer(u, t, buf_w, buf_h);
    if(!buf_data) {
        return GA_HANDLE_INVALID;
    }

    uint32_t eve_stride;
    EVE_GpuHandle handle = lv_draw_eve5_hal_upload_texture(u, buf_data,
                                                           buf_w, buf_h,
                                                           &eve_stride);
    lv_free(buf_data);

    if(EVE_GpuAlloc_Get(u->allocator, handle) == GA_INVALID) {
        return GA_HANDLE_INVALID;
    }

    lv_draw_eve5_sw_cache_insert(u, t->type, buf_w, buf_h,
                                 dsc_data, dsc_size, handle, eve_stride);

    *out_w = buf_w;
    *out_h = buf_h;
    *out_stride = eve_stride;
    *out_from_cache = true;

    LV_LOG_INFO("EVE5: SW fallback rendered and cached %"PRId32"x%"PRId32" type=%d",
                buf_w, buf_h, t->type);

    return handle;
}

#if LV_DRAW_EVE5_SW_OUTLINE_FONT
/* A label in a FreeType outline font, whose glyphs are vector paths */
bool lv_draw_eve5_label_needs_sw(const lv_draw_task_t * t)
{
    const lv_draw_label_dsc_t * dsc = t->draw_dsc;
    for(const lv_font_t * f = dsc->font; f != NULL; f = f->fallback) {
        if(lv_freetype_is_outline_font(f)) return true;
    }
    return false;
}
#endif

#if LV_DRAW_EVE5_SW_TEXTURES
/**
 * Render a software fallback texture on first use and retain it for the slice's other passes.
 * Vector descriptors contain path pointers that cannot identify the drawing for cache lookup, and
 * rendering consumes those paths, so reuse the texture instead of rendering them again.
 */
static const lv_draw_eve5_sw_texture_t * sw_task_texture(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    for(uint32_t i = 0; i < u->sw_texture_count; i++) {
        if(u->sw_textures[i].task == t) return &u->sw_textures[i];
    }

    if(u->sw_texture_count == u->sw_texture_capacity) {
        uint32_t capacity = u->sw_texture_capacity ? u->sw_texture_capacity * 2 : 4;
        lv_draw_eve5_sw_texture_t * textures = lv_realloc(u->sw_textures, capacity * sizeof(*textures));
        if(textures == NULL) return NULL;
        u->sw_textures = textures;
        u->sw_texture_capacity = capacity;
    }

    lv_area_t area = t->_real_area;
    /*
     * ThorVG produces premultiplied pixels with TVG_COLORSPACE_ARGB8888. LVGL's software blend path
     * produces straight-alpha pixels in this transparent buffer.
     */
    bool premultiplied = false;
#if LV_DRAW_EVE5_SW_VECTOR
    if(t->type == LV_DRAW_TASK_TYPE_VECTOR) {
        eve5_sw_vector_area(t, &area);
        premultiplied = true;
    }
#endif
    int32_t w = lv_area_get_width(&area);
    int32_t h = lv_area_get_height(&area);
    if(w <= 0 || h <= 0) return NULL;

    uint8_t * buf_data = lv_draw_eve5_sw_render_to_buffer(u, t, w, h);
    if(!buf_data) return NULL;
    uint32_t stride;
    EVE_GpuHandle handle = lv_draw_eve5_hal_upload_texture(u, buf_data, w, h, &stride);
    lv_free(buf_data);
    if(EVE_GpuAlloc_Get(u->allocator, handle) == GA_INVALID) {
        LV_LOG_WARN("EVE5: SW fallback failed for task type %d", t->type);
        return NULL;
    }

    lv_draw_eve5_sw_texture_t * tex = &u->sw_textures[u->sw_texture_count++];
    tex->task = t;
    tex->handle = handle;
    tex->area = area;
    tex->stride = stride;
    tex->premultiplied = premultiplied;
    return tex;
}

/* Draw the SW texture of a task */
void lv_draw_eve5_sw_draw_task_texture(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    const lv_draw_eve5_sw_texture_t * tex = sw_task_texture(u, t);
    if(tex == NULL) return;
    uint32_t addr = EVE_GpuAlloc_Get(u->allocator, tex->handle);
    lv_draw_eve5_hal_draw_texture(u, t, addr, lv_area_get_width(&tex->area), lv_area_get_height(&tex->area),
                                  tex->stride, &tex->area, tex->premultiplied);
}

/**
 * Draw the software texture's alpha coverage in white. The L8 pass stores the coverage as
 * luminance; the direct-to-alpha pass composites it with the correct over alpha equation.
 */
void lv_draw_eve5_sw_alpha_draw_task_texture(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    const lv_draw_eve5_sw_texture_t * tex = sw_task_texture(u, t);
    if(tex == NULL) return;
    uint32_t addr = EVE_GpuAlloc_Get(u->allocator, tex->handle);
    if(addr == GA_INVALID) return;

    EVE_HalContext * phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    int32_t h = lv_area_get_height(&tex->area);

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_bitmapTransform_identity(phost);
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(phost, addr);
    EVE_CoDl_bitmapLayout(phost, GLFORMAT, tex->stride, h);
    EVE_CoDl_bitmapExtFormat(phost, ARGB8);
    EVE_CoDl_bitmapSwizzle(phost, ONE, ONE, ONE, ALPHA);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, lv_area_get_width(&tex->area), h);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, tex->area.x1 - layer->buf_area.x1, tex->area.y1 - layer->buf_area.y1);
    EVE_CoDl_end(phost);
#else
    LV_UNUSED(u);
    LV_UNUSED(t);
#endif
}

/* Release the SW textures once the slice that draws them is finished */
void lv_draw_eve5_sw_release_textures(lv_draw_eve5_unit_t * u)
{
    for(uint32_t i = 0; i < u->sw_texture_count; i++) {
        EVE_GpuAlloc_ScopedFree(u->allocator, u->sw_textures[i].handle);
    }
    lv_free(u->sw_textures);
    u->sw_textures = NULL;
    u->sw_texture_count = 0;
    u->sw_texture_capacity = 0;
}
#endif

/**
 * Render a task via SW fallback and blit to current layer.
 */
void lv_draw_eve5_sw_render_task(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    int32_t tex_w, tex_h;
    uint32_t tex_stride;
    bool from_cache;

#if LV_DRAW_EVE5_SW_TEXTURES
    if(t->type == LV_DRAW_TASK_TYPE_VECTOR || t->type == LV_DRAW_TASK_TYPE_LABEL) {
        lv_draw_eve5_sw_draw_task_texture(u, t);
        return;
    }
#endif

    EVE_GpuHandle handle = lv_draw_eve5_sw_render_cached(u, t, &tex_w, &tex_h, &tex_stride, &from_cache);

    uint32_t addr = EVE_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: SW fallback failed for task type %d", t->type);
        return;
    }
    lv_draw_eve5_hal_draw_texture(u, t, addr, tex_w, tex_h, tex_stride, &t->_real_area, false);
}

#endif /* LV_USE_DRAW_EVE5 && LV_DRAW_EVE5_SW_FALLBACK */
