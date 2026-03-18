/**
 * @file lv_draw_eve5_sw_fallback.c
 *
 * EVE5 (BT820) Software Fallback Rendering
 *
 * When LV_DRAW_EVE5_SW_* flags are enabled for QA testing, individual task
 * types are rendered via LVGL's software renderer instead of EVE hardware.
 * The SW-rendered ARGB8 buffer is uploaded to RAM_G and blitted as a texture.
 *
 * Contains:
 * - Per-task-type SW rendering with coordinate normalization
 * - SW texture cache integration (render once, reuse across frames)
 * - Upload and blit of SW-rendered textures via HAL
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#if LV_USE_OS
#include "../../drivers/display/eve5/lv_eve5.h"
#endif

/*
 * QA Configuration: Force SW rendering for specific task types.
 * Set to 1 to render via software fallback, 0 for hardware rendering.
 * Useful for comparing HW vs SW output during QA testing.
 */
#ifndef LV_DRAW_EVE5_SW_FILL
#define LV_DRAW_EVE5_SW_FILL 0
#endif

#ifndef LV_DRAW_EVE5_SW_BORDER
#define LV_DRAW_EVE5_SW_BORDER 0
#endif

#ifndef LV_DRAW_EVE5_SW_LINE
#define LV_DRAW_EVE5_SW_LINE 0
#endif

#ifndef LV_DRAW_EVE5_SW_TRIANGLE
#define LV_DRAW_EVE5_SW_TRIANGLE 0
#endif

#ifndef LV_DRAW_EVE5_SW_LABEL
#define LV_DRAW_EVE5_SW_LABEL 0
#endif

#ifndef LV_DRAW_EVE5_SW_ARC
#define LV_DRAW_EVE5_SW_ARC 0
#endif

#ifndef LV_DRAW_EVE5_SW_BOX_SHADOW
#define LV_DRAW_EVE5_SW_BOX_SHADOW 0
#endif

/**********************
 * SW FALLBACK HELPERS
 **********************/

/**
 * Get the descriptor data pointer and size for cache comparison.
 * Returns pointer to data AFTER the base descriptor.
 */
const void *lv_draw_eve5_sw_get_dsc_cache_data(const lv_draw_task_t *t, uint32_t *out_size)
{
    const lv_draw_dsc_base_t *base = t->draw_dsc;
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
 * Helper: render task to a CPU buffer using SW fallback.
 * Caller must free returned buffer.
 */
uint8_t *lv_draw_eve5_sw_render_to_buffer(lv_draw_eve5_unit_t *u,
                                     const lv_draw_task_t *t,
                                     int32_t buf_w, int32_t buf_h)
{
    LV_UNUSED(u);

    uint32_t buf_stride = buf_w * 4;
    uint32_t buf_size = buf_stride * buf_h;
    uint8_t *buf_data = lv_malloc(buf_size);

    if(!buf_data) {
        LV_LOG_ERROR("EVE5: Failed to allocate SW buffer (%"PRIu32" bytes)", buf_size);
        return NULL;
    }
    lv_memzero(buf_data, buf_size);

    lv_draw_buf_t sw_buf;
    lv_draw_buf_init(&sw_buf, buf_w, buf_h,
                     LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO,
                     buf_data, buf_size);

    /* Normalized coordinates (0,0 origin) */
    lv_area_t norm_area;
    lv_area_set(&norm_area, 0, 0, buf_w - 1, buf_h - 1);

    lv_layer_t temp_layer;
    lv_memzero(&temp_layer, sizeof(temp_layer));
    temp_layer.draw_buf = &sw_buf;
    temp_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    temp_layer.buf_area = norm_area;
    temp_layer._clip_area = norm_area;
    temp_layer.phy_clip_area = norm_area;

    int32_t ofs_x = t->_real_area.x1;
    int32_t ofs_y = t->_real_area.y1;
    (void)ofs_x; (void)ofs_y;

    bool render_ok = false;

    switch(t->type) {
#if LV_DRAW_EVE5_SW_FILL
        case LV_DRAW_TASK_TYPE_FILL: {
            lv_draw_fill_dsc_t *src_dsc = t->draw_dsc;

            lv_area_t norm_task_area;
            norm_task_area.x1 = t->area.x1 - ofs_x;
            norm_task_area.y1 = t->area.y1 - ofs_y;
            norm_task_area.x2 = t->area.x2 - ofs_x;
            norm_task_area.y2 = t->area.y2 - ofs_y;

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.base.user_data = (void *)1;  /* SW fallback marker */
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
            lv_draw_border_dsc_t *src_dsc = t->draw_dsc;

            lv_area_t norm_task_area;
            norm_task_area.x1 = t->area.x1 - ofs_x;
            norm_task_area.y1 = t->area.y1 - ofs_y;
            norm_task_area.x2 = t->area.x2 - ofs_x;
            norm_task_area.y2 = t->area.y2 - ofs_y;

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.base.user_data = (void *)1;  /* SW fallback marker */
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
            line_dsc.base.user_data = (void *)1;  /* SW fallback marker */

            /* Normalize coordinates */
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
            tri_dsc.base.user_data = (void *)1;  /* SW fallback marker */

            /* Normalize coordinates */
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

#if LV_DRAW_EVE5_SW_LABEL
        case LV_DRAW_TASK_TYPE_LABEL: {
            lv_draw_label_dsc_t label_dsc;
            lv_memcpy(&label_dsc, t->draw_dsc, sizeof(label_dsc));
            label_dsc.base.user_data = (void *)1;  /* SW fallback marker */

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
            arc_dsc.base.user_data = (void *)1;  /* SW fallback marker */

            /* Normalize center coordinates */
            arc_dsc.center.x -= ofs_x;
            arc_dsc.center.y -= ofs_y;

            lv_draw_arc(&temp_layer, &arc_dsc);
            render_ok = true;
            break;
        }
#endif

#if LV_DRAW_EVE5_SW_BOX_SHADOW
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: {
            lv_draw_box_shadow_dsc_t *src_dsc = t->draw_dsc;

            lv_area_t norm_task_area;
            norm_task_area.x1 = t->area.x1 - ofs_x;
            norm_task_area.y1 = t->area.y1 - ofs_y;
            norm_task_area.x2 = t->area.x2 - ofs_x;
            norm_task_area.y2 = t->area.y2 - ofs_y;

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.base.user_data = (void *)1;  /* SW fallback marker */
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

        default:
            LV_LOG_WARN("EVE5: No SW fallback for task type %d", t->type);
            break;
    }

    if(!render_ok) {
        lv_free(buf_data);
        return NULL;
    }

    /* Dispatch to SW renderer.
     * Unlock HAL mutex while SW threads run — they may trigger image
     * decoders or filesystem access that needs the HAL mutex. */
    lv_display_t *disp = lv_refr_get_disp_refreshing();
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
 * Render a task via SW fallback, with caching.
 */
Esd_GpuHandle lv_draw_eve5_sw_render_cached(lv_draw_eve5_unit_t *u,
                                            const lv_draw_task_t *t,
                                            int32_t *out_w, int32_t *out_h,
                                            uint32_t *out_stride,
                                            bool *out_from_cache)
{
    *out_from_cache = false;

    int32_t buf_w = lv_area_get_width(&t->_real_area);
    int32_t buf_h = lv_area_get_height(&t->_real_area);

    if(buf_w <= 0 || buf_h <= 0) {
        return GA_HANDLE_INVALID;
    }

    /* Get descriptor data for cache key (excluding base) */
    uint32_t dsc_size;
    const void *dsc_data = lv_draw_eve5_sw_get_dsc_cache_data(t, &dsc_size);

    /* Check cache first */
    Esd_GpuHandle cached_handle;
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

    /* Cache miss - render via SW */
    uint8_t *buf_data = lv_draw_eve5_sw_render_to_buffer(u, t, buf_w, buf_h);
    if(!buf_data) {
        return GA_HANDLE_INVALID;
    }

    /* Upload to RAM_G via HAL */
    uint32_t eve_stride;
    Esd_GpuHandle handle = lv_draw_eve5_hal_upload_texture(u, buf_data,
                                                           buf_w, buf_h,
                                                           &eve_stride);
    lv_free(buf_data);

    if(handle.Id == GA_HANDLE_INVALID.Id) {
        return GA_HANDLE_INVALID;
    }

    /* Insert into cache */
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

/**
 * Helper: render a task via SW fallback and blit to screen.
 */
void lv_draw_eve5_sw_render_task(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    int32_t tex_w, tex_h;
    uint32_t tex_stride;
    bool from_cache;

    Esd_GpuHandle handle = lv_draw_eve5_sw_render_cached(u, t, &tex_w, &tex_h, &tex_stride, &from_cache);

    if(handle.Id == GA_HANDLE_INVALID.Id) {
        LV_LOG_WARN("EVE5: SW fallback failed for task type %d", t->type);
        return;
    }

    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, handle);
    lv_draw_eve5_hal_draw_texture(u, t, addr, tex_w, tex_h, tex_stride, &t->_real_area);
}

#endif /* LV_USE_DRAW_EVE5 */
