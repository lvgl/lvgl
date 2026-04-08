/**
 * @file lv_draw_eve5_canvas.c
 *
 * EVE5 (BT820) Canvas Layer Support
 *
 * Handles canvas layer optimization:
 * - Direct image loading to canvas GPU allocation in native format
 * - Format conversion when rendering on top of native format content
 * - Canvas cache management for persistent GPU allocations
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw_image.h"

/**********************
 * DEFINES
 **********************/
#define DRAW_UNIT_ID_EVE5 82

/**********************
 * STATIC PROTOTYPES
 **********************/
static bool is_simple_image_draw(const lv_draw_image_dsc_t *dsc, const lv_draw_task_t *t);

/**********************
 * STATIC FUNCTIONS
 **********************/

/**
 * Check if an image draw task is "simple" — no special options that require
 * the render pipeline. Simple images can be loaded directly to the canvas
 * GPU allocation without going through the full render path.
 */
static bool is_simple_image_draw(const lv_draw_image_dsc_t *dsc, const lv_draw_task_t *t)
{
    if(dsc == NULL) return false;

    /* Check for any special options that require rendering */
    if(dsc->opa < LV_OPA_MAX) return false;           /* Opacity */
    if(dsc->clip_radius > 0) return false;            /* Clip radius */
    if(dsc->rotation != 0) return false;              /* Rotation */
    if(dsc->scale_x != LV_SCALE_NONE) return false;   /* Scale X */
    if(dsc->scale_y != LV_SCALE_NONE) return false;   /* Scale Y */
    if(dsc->skew_x != 0) return false;                /* Skew X */
    if(dsc->skew_y != 0) return false;                /* Skew Y */
    if(dsc->recolor_opa > LV_OPA_MIN) return false;   /* Recolor */
    if(dsc->colorkey != NULL) return false;           /* Colorkey */
    if(dsc->bitmap_mask_src != NULL) return false;    /* Bitmap mask */
    if(dsc->blend_mode != LV_BLEND_MODE_NORMAL) return false; /* Blend mode */
    if(dsc->tile) return false;                       /* Tiling */

    /* Check task type */
    if(t->type != LV_DRAW_TASK_TYPE_IMAGE) return false;

    /* Ensure source is a variable (in-memory image) or file */
    lv_image_src_t src_type = lv_image_src_get_type(dsc->src);
    if(src_type != LV_IMAGE_SRC_VARIABLE && src_type != LV_IMAGE_SRC_FILE) return false;

    return true;
}

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Try to handle a canvas layer with a single simple image task directly.
 * If the canvas has exactly one IMAGE task with no special options, load
 * the image directly to the canvas GPU allocation in native format.
 *
 * Returns true if handled (task marked finished), false to continue with normal render.
 */
bool lv_draw_eve5_try_canvas_direct_image(lv_draw_eve5_unit_t *u, lv_layer_t *layer)
{
    if(!layer->draw_buf) return false;

    /* Skip direct load for canvases that already have GPU content.
     * The full render path handles incremental canvas updates correctly
     * (blit existing content, render new tasks on top). The direct load
     * path does not support incremental updates — it replaces everything. */
    lv_draw_eve5_vram_res_t *existing_vr = eve5_get_vram_res(layer);
    if(existing_vr != NULL && existing_vr->has_content) {
        return false;
    }

    /* Count tasks and find the single image task */
    lv_draw_task_t *image_task = NULL;
    int task_count = 0;

    for(lv_draw_task_t *t = layer->draw_task_head; t != NULL; t = t->next) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5) continue;
        if(t->state != LV_DRAW_TASK_STATE_QUEUED) continue;

        task_count++;
        if(task_count > 1) return false;  /* Multiple tasks */

        if(t->type == LV_DRAW_TASK_TYPE_IMAGE) {
            image_task = t;
        }
    }

    if(task_count != 1 || image_task == NULL) return false;

    /* Check if it's a simple image draw */
    const lv_draw_image_dsc_t *dsc = image_task->draw_dsc;
    if(!is_simple_image_draw(dsc, image_task)) return false;

    /* Check if the image fills the entire canvas (no offset) */
    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);
    int32_t img_x = image_task->area.x1 - layer->buf_area.x1;
    int32_t img_y = image_task->area.y1 - layer->buf_area.y1;

    /* For now, require the image to start at (0,0) of the canvas */
    if(img_x != 0 || img_y != 0) return false;

    /* Get image dimensions */
    int32_t img_w = lv_area_get_width(&image_task->area);
    int32_t img_h = lv_area_get_height(&image_task->area);

    /* Require exact size match */
    if(img_w != layer_w || img_h != layer_h) return false;

    /* Try to load the image directly */
    uint16_t eve_format = ARGB8;
    int32_t stride = 0;
    int32_t img_loaded_w = 0, img_loaded_h = 0;
    Esd_GpuHandle handle = GA_HANDLE_INVALID;
    uint32_t palette_addr = GA_INVALID;

    uint32_t addr = lv_draw_eve5_load_image(u, dsc->src, &eve_format, &stride,
                                             &img_loaded_w, &img_loaded_h, &handle,
                                             &palette_addr);
    if(addr == GA_INVALID) {
        return false;  /* Direct load failed - fall back to normal render */
    }

    /* Verify loaded dimensions match canvas expectations */
    if(img_loaded_w != layer_w || img_loaded_h != layer_h) {
        /* Dimensions mismatch - free the allocation and fall back */
        track_frame_alloc(u, handle);
        return false;
    }

    /* Store GPU allocation in vram_res on the draw_buf */
    uint32_t alloc_base = Esd_GpuAlloc_Get(u->allocator, handle);
    uint32_t source_offset = (alloc_base != GA_INVALID) ? (addr - alloc_base) : 0;

    {
        /* Create or update vram_res with the loaded image info */
        lv_draw_eve5_vram_res_t *vr = (lv_draw_eve5_vram_res_t *)layer->draw_buf->vram_res;
        if(vr == NULL) {
            vr = lv_malloc(sizeof(lv_draw_eve5_vram_res_t));
            if(vr == NULL) {
                track_frame_alloc(u, handle);
                return false;
            }
            vr->base.unit = (lv_draw_unit_t *)u;
            layer->draw_buf->vram_res = (lv_draw_buf_vram_res_t *)vr;
            layer->draw_buf->header.flags |= LV_IMAGE_FLAGS_VRAM_RESIDENT;
        }
        else {
            /* Free old GPU allocation if present */
            Esd_GpuAlloc_Free(u->allocator, vr->gpu_handle);
        }
        vr->base.size = Esd_GpuAlloc_Get(u->allocator, handle) != GA_INVALID
                       ? (uint32_t)(stride * layer_h) : 0;
        vr->gpu_handle = handle;
        vr->eve_format = eve_format;
        vr->stride = (uint32_t)stride;
        vr->source_offset = source_offset;
        vr->palette_offset = (palette_addr != GA_INVALID && alloc_base != GA_INVALID)
                            ? (palette_addr - alloc_base) : GA_INVALID;
        vr->is_premultiplied = false;
        vr->has_content = true;
    }

    LV_LOG_INFO("EVE5: Canvas direct image %p: addr=0x%08x fmt=%d stride=%d %dx%d handle=%d buf=%p",
                (void *)layer, addr, eve_format, (int)stride,
                (int)layer_w, (int)layer_h, (int)handle.Id,
                (void *)layer->draw_buf->data);

    /* Mark task as complete - goes from QUEUED → FINISHED (skipping IN_PROGRESS
     * since we handled it synchronously) */
    image_task->state = LV_DRAW_TASK_STATE_FINISHED;

    return true;
}

#endif /* LV_USE_DRAW_EVE5 */
