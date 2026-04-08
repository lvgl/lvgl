/**
 * @file lv_draw_eve5_canvas.c
 *
 * EVE5 (BT820) Canvas Layer Support
 *
 * Handles canvas layer optimization:
 * - Direct image loading to canvas GPU allocation in native format
 * - Format conversion when rendering on top of native format content
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
 * Check if an image draw task is "simple", with no options requiring the render pipeline.
 * Simple images can be loaded directly to canvas GPU allocation.
 */
static bool is_simple_image_draw(const lv_draw_image_dsc_t *dsc, const lv_draw_task_t *t)
{
    if(dsc == NULL) return false;

    if(dsc->opa < LV_OPA_MAX) return false;
    if(dsc->clip_radius > 0) return false;
    if(dsc->rotation != 0) return false;
    if(dsc->scale_x != LV_SCALE_NONE) return false;
    if(dsc->scale_y != LV_SCALE_NONE) return false;
    if(dsc->skew_x != 0) return false;
    if(dsc->skew_y != 0) return false;
    if(dsc->recolor_opa > LV_OPA_MIN) return false;
    if(dsc->colorkey != NULL) return false;
    if(dsc->bitmap_mask_src != NULL) return false;
    if(dsc->blend_mode != LV_BLEND_MODE_NORMAL) return false;
    if(dsc->tile) return false;

    if(t->type != LV_DRAW_TASK_TYPE_IMAGE) return false;

    lv_image_src_t src_type = lv_image_src_get_type(dsc->src);
    if(src_type != LV_IMAGE_SRC_VARIABLE && src_type != LV_IMAGE_SRC_FILE) return false;

    return true;
}

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * Try to handle a canvas layer with a single simple image task directly.
 * Loads the image to the canvas GPU allocation in native format, bypassing
 * the full render pipeline.
 *
 * Returns true if handled (task marked finished), false to continue with normal render.
 */
bool lv_draw_eve5_try_canvas_direct_image(lv_draw_eve5_unit_t *u, lv_layer_t *layer)
{
    if(!layer->draw_buf) return false;

    /* Skip direct load for canvases with existing GPU content.
     * The render path handles incremental updates; direct load replaces everything. */
    lv_eve5_vram_res_t *existing_vr = eve5_get_vram_res(layer);
    if(existing_vr != NULL && layer->draw_buf != NULL) {
        if(lv_draw_buf_has_flag(layer->draw_buf, LV_IMAGE_FLAGS_CLEARZERO | LV_IMAGE_FLAGS_DISCARDABLE)) {
            existing_vr->has_content = false;
            lv_draw_buf_clear_flag(layer->draw_buf, LV_IMAGE_FLAGS_CLEARZERO | LV_IMAGE_FLAGS_DISCARDABLE);
        }
    }
    if(existing_vr != NULL && existing_vr->has_content) {
        return false;
    }

    /* Find exactly one simple image task */
    lv_draw_task_t *image_task = NULL;
    int task_count = 0;

    for(lv_draw_task_t *t = layer->draw_task_head; t != NULL; t = t->next) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5) continue;
        if(t->state != LV_DRAW_TASK_STATE_QUEUED) continue;

        task_count++;
        if(task_count > 1) return false;

        if(t->type == LV_DRAW_TASK_TYPE_IMAGE) {
            image_task = t;
        }
    }

    if(task_count != 1 || image_task == NULL) return false;

    const lv_draw_image_dsc_t *dsc = image_task->draw_dsc;
    if(!is_simple_image_draw(dsc, image_task)) return false;

    /* Require image at canvas origin with exact size match */
    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);
    int32_t img_x = image_task->area.x1 - layer->buf_area.x1;
    int32_t img_y = image_task->area.y1 - layer->buf_area.y1;

    if(img_x != 0 || img_y != 0) return false;

    int32_t img_w = lv_area_get_width(&image_task->area);
    int32_t img_h = lv_area_get_height(&image_task->area);

    if(img_w != layer_w || img_h != layer_h) return false;

    /* Resolve image to GPU */
    lv_eve5_vram_res_t *src_vr = lv_draw_eve5_resolve_to_gpu(u, dsc->src);
    if(!src_vr) return false;

    if(src_vr->width != layer_w || src_vr->height != layer_h) {
        return false;
    }

    /* Copy the GPU allocation descriptor to the canvas draw_buf.
     * The handle is shared with the source (image cache or decoder cache).
     * This is safe: if either side frees via PendingFree, the other detects
     * the dead handle via vram_check_cb and self-heals. */
    {
        lv_eve5_vram_res_t *vr = eve5_get_vram_res(layer);
        if(vr == NULL) {
            vr = lv_malloc(sizeof(lv_eve5_vram_res_t));
            if(vr == NULL) {
                return false;
            }
            layer->draw_buf->vram_res = (lv_draw_buf_vram_res_t *)vr;
        }
        else {
            /* PendingFree: previous canvas content may be in an in-flight display list */
            Esd_GpuAlloc_PendingFree(u->allocator, vr->gpu_handle);
        }

        *vr = *src_vr;
        vr->base.unit = (lv_draw_unit_t *)u;
        vr->is_premultiplied = false;
        vr->has_content = true;
    }

    LV_LOG_INFO("EVE5: Canvas direct image %p: fmt=%d stride=%u %dx%d handle=%d",
                (void *)layer, src_vr->eve_format, src_vr->stride,
                (int)layer_w, (int)layer_h, (int)src_vr->gpu_handle.Id);

    /* QUEUED → FINISHED (skipping IN_PROGRESS since handled synchronously) */
    image_task->state = LV_DRAW_TASK_STATE_FINISHED;

    return true;
}

#endif /* LV_USE_DRAW_EVE5 */
