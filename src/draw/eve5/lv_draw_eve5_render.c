/**
 * @file lv_draw_eve5_render.c
 *
 * EVE5 (BT820) Render Processing Loops
 *
 * Contains the task iteration loops that drive the rendering pipeline:
 * - RGB render pass: dispatches QUEUED tasks to HW or SW
 * - Opaque area pre-pass: finds largest opaque fill for alpha skip optimization
 * - Alpha recovery check: determines if L8 render-target path is needed
 * - L8 render-target alpha pass: renders alpha coverage as grayscale luminance
 * - Direct-to-alpha correction pass: re-iterates tasks writing correct alpha
 *
 * See lv_draw_eve5.c header for alpha recovery architecture explanation.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"

/*********************
 * DEFINES
 *********************/
#define DRAW_UNIT_ID_EVE5 82

#if 0
#define EVE5_LOG(...) LV_LOG_INFO(__VA_ARGS__)
static const char *task_type_str(lv_draw_task_type_t type)
{
    switch(type) {
        case LV_DRAW_TASK_TYPE_FILL:       return "FILL";
        case LV_DRAW_TASK_TYPE_BORDER:     return "BORDER";
        case LV_DRAW_TASK_TYPE_LINE:       return "LINE";
        case LV_DRAW_TASK_TYPE_TRIANGLE:   return "TRIANGLE";
        case LV_DRAW_TASK_TYPE_LABEL:      return "LABEL";
        case LV_DRAW_TASK_TYPE_LETTER:     return "LETTER";
        case LV_DRAW_TASK_TYPE_IMAGE:      return "IMAGE";
        case LV_DRAW_TASK_TYPE_ARC:        return "ARC";
        case LV_DRAW_TASK_TYPE_LAYER:      return "LAYER";
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: return "BOX_SHADOW";
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE: return "MASK_RECT";
        default:                           return "OTHER";
    }
}
#else
#define EVE5_LOG(...) do {} while(0)
#endif

/**********************
 * HELPER UTILITIES
 **********************/

/**
 * Check if a task's visible area is fully inside the opaque region.
 * Uses conservative cross-shaped containment (rect minus 4 corner r*r squares).
 * Insets the opaque boundary by 1px to account for EVE's ~1px AA feathering.
 */
bool lv_draw_eve5_is_fully_inside_opaque(lv_draw_eve5_unit_t *u, const lv_area_t *task_area,
                                    const lv_area_t *clip_area, const lv_area_t *layer_area)
{
    if(!u->has_alpha_opaque) return false;

    lv_area_t visible;
    if(!lv_area_intersect(&visible, task_area, clip_area)) return true;

    int32_t vx1 = visible.x1 - layer_area->x1;
    int32_t vy1 = visible.y1 - layer_area->y1;
    int32_t vx2 = visible.x2 - layer_area->x1;
    int32_t vy2 = visible.y2 - layer_area->y1;

    const lv_area_t *oa = &u->alpha_opaque_area;
    int32_t r = u->alpha_opaque_radius;

    /* Inset by 1px for AA feathering at edges */
    if(vx1 < oa->x1 + 1 || vy1 < oa->y1 + 1 ||
       vx2 > oa->x2 - 1 || vy2 > oa->y2 - 1) return false;

    if(r <= 0) return true;

    /* Check cross-shaped interior: reject if overlapping any corner square */
    bool in_left = (vx1 < oa->x1 + r + 1);
    bool in_right = (vx2 > oa->x2 - r - 1);
    bool in_top = (vy1 < oa->y1 + r + 1);
    bool in_bottom = (vy2 > oa->y2 - r - 1);

    if(in_left && in_top) return false;
    if(in_right && in_top) return false;
    if(in_left && in_bottom) return false;
    if(in_right && in_bottom) return false;

    return true;
}

/**
 * Check if a line task should use the H/V sharp line optimization.
 *
 * Returns false if the line is part of a polyline with diagonal segments,
 * preventing visual inconsistency where some segments get sharp edges
 * while others don't.
 */
bool lv_draw_eve5_line_should_use_hv_opt(const lv_draw_task_t *t, const lv_draw_task_t *prev)
{
    const lv_draw_line_dsc_t *dsc = t->draw_dsc;

    int32_t dx = dsc->p2.x - dsc->p1.x;
    int32_t dy = dsc->p2.y - dsc->p1.y;
    bool this_is_hv = (dx == 0 || dy == 0);
    if(!this_is_hv) return true;

    /* Check previous task */
    if(prev && prev->type == LV_DRAW_TASK_TYPE_LINE) {
        const lv_draw_line_dsc_t *prev_dsc = prev->draw_dsc;
        bool connects = (prev_dsc->p1.x == dsc->p1.x && prev_dsc->p1.y == dsc->p1.y) ||
                        (prev_dsc->p1.x == dsc->p2.x && prev_dsc->p1.y == dsc->p2.y) ||
                        (prev_dsc->p2.x == dsc->p1.x && prev_dsc->p2.y == dsc->p1.y) ||
                        (prev_dsc->p2.x == dsc->p2.x && prev_dsc->p2.y == dsc->p2.y);
        if(connects) {
            int32_t prev_dx = prev_dsc->p2.x - prev_dsc->p1.x;
            int32_t prev_dy = prev_dsc->p2.y - prev_dsc->p1.y;
            if(prev_dx != 0 && prev_dy != 0) {
                return false;
            }
        }
    }

    /* Check next task */
    const lv_draw_task_t *next = t->next;
    if(next && next->type == LV_DRAW_TASK_TYPE_LINE) {
        const lv_draw_line_dsc_t *next_dsc = next->draw_dsc;
        bool connects = (next_dsc->p1.x == dsc->p1.x && next_dsc->p1.y == dsc->p1.y) ||
                        (next_dsc->p1.x == dsc->p2.x && next_dsc->p1.y == dsc->p2.y) ||
                        (next_dsc->p2.x == dsc->p1.x && next_dsc->p2.y == dsc->p1.y) ||
                        (next_dsc->p2.x == dsc->p2.x && next_dsc->p2.y == dsc->p2.y);
        if(connects) {
            int32_t next_dx = next_dsc->p2.x - next_dsc->p1.x;
            int32_t next_dy = next_dsc->p2.y - next_dsc->p1.y;
            if(next_dx != 0 && next_dy != 0) {
                return false;
            }
        }
    }

    return true;
}

/**********************
 * RGB RENDER PASS
 **********************/

/**
 * RGB render pass: process all QUEUED tasks for a layer.
 *
 * MASK_RECTANGLE tasks are deferred on non-screen layers until after alpha
 * correction; they scale all premultiplied RGBA channels and must run after
 * alpha is corrected. Tasks are left IN_PROGRESS so the alpha pass can find them.
 */
int lv_draw_eve5_render_tasks(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen, bool finish_tasks,
                              const lv_draw_eve5_slice_t *slice)
{
    lv_draw_task_t *t = eve5_slice_first(slice, layer);
    lv_draw_task_t *prev_task = NULL;
    int rendered_count = 0;

    while(t && t != slice->end) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_QUEUED) {
            prev_task = t;
            t = t->next;
            continue;
        }

        if(!is_screen && t->type == LV_DRAW_TASK_TYPE_MASK_RECTANGLE) {
            prev_task = t;
            t = t->next;
            continue;
        }

        t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;

        EVE5_LOG("EVE5: Render task: type=%-10s area=(%d,%d)-(%d,%d)",
                 task_type_str(t->type),
                 t->area.x1, t->area.y1, t->area.x2, t->area.y2);

        switch(t->type) {
            case LV_DRAW_TASK_TYPE_FILL: {
                /* Check for matching BORDER from same lv_draw_rect() for unified rendering */
                lv_draw_task_t *next = t->next;
                bool has_matching_border = false;

                if(next &&
                   next->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
                   next->state == LV_DRAW_TASK_STATE_QUEUED &&
                   next->type == LV_DRAW_TASK_TYPE_BORDER &&
                   next->target_layer == t->target_layer) {

                    const lv_draw_fill_dsc_t *fill_dsc = t->draw_dsc;
                    const lv_draw_border_dsc_t *border_dsc = next->draw_dsc;

                    has_matching_border = eve5_fill_border_area_match(&t->area, &next->area) &&
                                          (fill_dsc->radius == border_dsc->radius);
                }

                if(has_matching_border) {
                    EVE5_LOG("EVE5: Unified FILL+BORDER rendering");

                    next->state = LV_DRAW_TASK_STATE_IN_PROGRESS;

#if LV_DRAW_EVE5_SW_FILL || LV_DRAW_EVE5_SW_BORDER
                    lv_draw_eve5_sw_render_task(u, t);
                    lv_draw_eve5_sw_render_task(u, next);
#else
                    lv_draw_eve5_hal_draw_fill_with_border(u, t, next);
#endif
                    if(finish_tasks) next->state = LV_DRAW_TASK_STATE_FINISHED;
                    rendered_count++;
                }
                else {
#if LV_DRAW_EVE5_SW_FILL
                    lv_draw_eve5_sw_render_task(u, t);
#else
                    lv_draw_eve5_hal_draw_fill(u, t, false);
#endif
                }
                break;
            }

            case LV_DRAW_TASK_TYPE_BORDER:
#if LV_DRAW_EVE5_SW_BORDER
                lv_draw_eve5_sw_render_task(u, t);
#else
                lv_draw_eve5_hal_draw_border(u, t, false);
#endif
                break;

            case LV_DRAW_TASK_TYPE_LINE:
#if LV_DRAW_EVE5_SW_LINE
                lv_draw_eve5_sw_render_task(u, t);
#else
                lv_draw_eve5_hal_draw_line(u, t, false, lv_draw_eve5_line_should_use_hv_opt(t, prev_task));
#endif
                break;

            case LV_DRAW_TASK_TYPE_TRIANGLE:
#if LV_DRAW_EVE5_SW_TRIANGLE
                lv_draw_eve5_sw_render_task(u, t);
#else
                lv_draw_eve5_hal_draw_triangle(u, t, false);
#endif
                break;

            case LV_DRAW_TASK_TYPE_LABEL:
#if LV_DRAW_EVE5_SW_LABEL
                lv_draw_eve5_sw_render_task(u, t);
#else
                lv_draw_eve5_hal_draw_label(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_LETTER:
#if LV_DRAW_EVE5_SW_LABEL
                lv_draw_eve5_sw_render_task(u, t);
#else
                lv_draw_eve5_hal_draw_letter(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_IMAGE:
                lv_draw_eve5_hal_draw_image(u, t, false);
                break;

            case LV_DRAW_TASK_TYPE_ARC:
#if LV_DRAW_EVE5_SW_ARC
                lv_draw_eve5_sw_render_task(u, t);
#else
                lv_draw_eve5_hal_draw_arc(u, t, false);
#endif
                break;

            case LV_DRAW_TASK_TYPE_LAYER: {
                lv_draw_image_dsc_t *dsc = t->draw_dsc;
                lv_layer_t *child = (lv_layer_t *)dsc->src;

                if(eve5_get_vram_res(child) == NULL) {
                    if(child->draw_buf == NULL) {
                        break;
                    }
                    LV_LOG_WARN("EVE5: CPU-rendered layer not supported, skipping child %p", (void *)child);
                    break;
                }

                lv_draw_eve5_hal_draw_image(u, t, false);
                break;
            }

            case LV_DRAW_TASK_TYPE_BOX_SHADOW:
#if LV_DRAW_EVE5_SW_BOX_SHADOW
                lv_draw_eve5_sw_render_task(u, t);
#else
                lv_draw_eve5_hal_draw_box_shadow(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
                lv_draw_eve5_hal_draw_mask_rect(u, t);
                break;

            default:
                EVE5_LOG("EVE5:   Unhandled task type %d, skipping", t->type);
                break;
        }

        if(finish_tasks) t->state = LV_DRAW_TASK_STATE_FINISHED;
        rendered_count++;
        prev_task = t;
        t = t->next;
    }

    return rendered_count;
}

/**********************
 * OPAQUE AREA PRE-PASS
 **********************/

/**
 * Opaque area pre-pass: find the largest opaque fill for alpha skip optimization.
 * Tasks fully inside this area can skip individual alpha correction since the
 * opaque fill will overwrite their alpha to 255 anyway.
 */
void lv_draw_eve5_opaque_prepass(lv_draw_eve5_unit_t *u, lv_layer_t *layer, const lv_draw_eve5_slice_t *slice)
{
    const lv_area_t *layer_area = &layer->buf_area;
    lv_draw_task_t *t = eve5_slice_first(slice, layer);

    while(t && t != slice->end) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_QUEUED) {
            t = t->next;
            continue;
        }

        switch(t->type) {
            case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t *dsc = t->draw_dsc;
                if(dsc->opa <= LV_OPA_MIN) break;

                int32_t x1 = t->area.x1 - layer_area->x1;
                int32_t y1 = t->area.y1 - layer_area->y1;
                int32_t x2 = t->area.x2 - layer_area->x1;
                int32_t y2 = t->area.y2 - layer_area->y1;
                int32_t w = x2 - x1 + 1;
                int32_t h = y2 - y1 + 1;
                int32_t real_radius = LV_MIN3((w - 1) / 2, (h - 1) / 2, dsc->radius);

                /* Determine if fill is fully opaque (gradients need all stops opaque) */
                uint8_t fill_opa = dsc->opa;
                if(dsc->grad.dir != LV_GRAD_DIR_NONE && dsc->grad.stops_count > 0) {
                    if(dsc->opa >= LV_OPA_MAX) {
                        for(uint8_t i = 0; i < dsc->grad.stops_count; i++) {
                            if(dsc->grad.stops[i].opa < LV_OPA_MAX) {
                                fill_opa = 0;
                                break;
                            }
                        }
                    }
                    else {
                        fill_opa = 0;
                    }
                }

                /* Fill+border pairing: use border's outer area when matched */
                lv_draw_task_t *next = t->next;
                if(next &&
                   next->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
                   next->state == LV_DRAW_TASK_STATE_QUEUED &&
                   next->type == LV_DRAW_TASK_TYPE_BORDER &&
                   next->target_layer == t->target_layer) {

                    const lv_draw_border_dsc_t *border_dsc = next->draw_dsc;

                    if(eve5_fill_border_area_match(&t->area, &next->area) &&
                       dsc->radius == border_dsc->radius) {
                        if(fill_opa >= LV_OPA_MAX) {
                            int32_t bx1 = next->area.x1 - layer_area->x1;
                            int32_t by1 = next->area.y1 - layer_area->y1;
                            int32_t bx2 = next->area.x2 - layer_area->x1;
                            int32_t by2 = next->area.y2 - layer_area->y1;
                            int32_t bw = bx2 - bx1 + 1;
                            int32_t bh = by2 - by1 + 1;
                            int32_t rout = LV_MIN3((bw - 1) / 2, (bh - 1) / 2,
                                                   border_dsc->radius);
                            lv_draw_eve5_track_alpha_opaque(u, bx1, by1, bx2, by2, rout);
                        }
                        t = next->next;
                        continue;
                    }
                }

                if(fill_opa >= LV_OPA_MAX) {
                    lv_draw_eve5_track_alpha_opaque(u, x1, y1, x2, y2, real_radius);
                }
                break;
            }

            case LV_DRAW_TASK_TYPE_BORDER: {
                /* Standalone border can upgrade an existing opaque area */
                const lv_draw_border_dsc_t *dsc = t->draw_dsc;
                if(dsc->opa >= LV_OPA_MAX &&
                   dsc->side == LV_BORDER_SIDE_FULL &&
                   u->has_alpha_opaque) {
                    int32_t x1 = t->area.x1 - layer_area->x1;
                    int32_t y1 = t->area.y1 - layer_area->y1;
                    int32_t x2 = t->area.x2 - layer_area->x1;
                    int32_t y2 = t->area.y2 - layer_area->y1;
                    int32_t w = x2 - x1 + 1;
                    int32_t h = y2 - y1 + 1;
                    int32_t rout = LV_MIN3((w - 1) / 2, (h - 1) / 2, dsc->radius);

                    lv_area_t border_local = { x1, y1, x2, y2 };
                    if(eve5_fill_border_area_match(&u->alpha_opaque_area, &border_local)) {
                        lv_draw_eve5_track_alpha_opaque(u, x1, y1, x2, y2, rout);
                    }
                }
                break;
            }

            default:
                break;
        }

        t = t->next;
    }
}

#if EVE5_USE_RENDERTARGET_ALPHA

/**
 * Check whether any task requires L8 render-target alpha recovery.
 *
 * Some effects use the alpha channel as scratch space during RGB rendering
 * (rounded gradient fills, image clip_radius, border masking). For these,
 * direct-to-alpha replay cannot recover correct alpha because the values are
 * irrecoverably trashed. Sets u->alpha_needs_rendertarget when found.
 */
void lv_draw_eve5_check_alpha_recovery(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                       const lv_draw_eve5_slice_t *slice)
{
    const lv_area_t *layer_area = &layer->buf_area;
    lv_draw_task_t *t = eve5_slice_first(slice, layer);

    while(t && t != slice->end) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_QUEUED) {
            t = t->next;
            continue;
        }

        /* Tasks inside opaque area will have alpha=255 anyway */
        if(lv_draw_eve5_is_fully_inside_opaque(u, &t->_real_area, &t->clip_area, layer_area)) {
            t = t->next;
            continue;
        }

        bool needs_rt = false;
        switch(t->type) {
            case LV_DRAW_TASK_TYPE_FILL:
                needs_rt = lv_draw_eve5_fill_needs_alpha_rendertarget(t);
                break;
            case LV_DRAW_TASK_TYPE_BORDER:
                needs_rt = lv_draw_eve5_border_needs_alpha_rendertarget(t);
                break;
            case LV_DRAW_TASK_TYPE_LINE:
                needs_rt = lv_draw_eve5_line_needs_alpha_rendertarget(t);
                break;
            case LV_DRAW_TASK_TYPE_ARC:
                needs_rt = lv_draw_eve5_arc_needs_alpha_rendertarget(t);
                break;
            case LV_DRAW_TASK_TYPE_IMAGE:
            case LV_DRAW_TASK_TYPE_LAYER:
                needs_rt = lv_draw_eve5_image_needs_alpha_rendertarget(t);
                break;
            default:
                break;
        }

        if(needs_rt) {
            u->alpha_needs_rendertarget = true;
            EVE5_LOG("EVE5: alpha_needs_rendertarget=true (task type=%s)",
                     task_type_str(t->type));
            return;
        }

        t = t->next;
    }
}

/**********************
 * RENDER-TARGET ALPHA PASS
 **********************/

/**
 * Render alpha coverage into an L8 render target.
 *
 * Dispatches all QUEUED tasks with alpha_to_rgb=true (white shapes at task
 * opacity). With blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA), the L8 luminance
 * accumulates Porter-Duff "over" alpha coverage correctly.
 *
 * BT820's render engine processes all draw commands through internal ARGB
 * line buffers regardless of output format, so alpha-as-scratch masking
 * techniques work correctly since the internal pipeline has alpha for all
 * intermediate phases.
 *
 * Returns GpuHandle for the L8 texture (or GA_HANDLE_INVALID on failure).
 */
Esd_GpuHandle lv_draw_eve5_render_alpha_to_l8(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                              int32_t aligned_w, int32_t aligned_h,
                                              int32_t w, int32_t h,
                                              const lv_draw_eve5_slice_t *slice)
{
    Esd_GpuHandle l8_handle = lv_draw_eve5_hal_init_l8_rendertarget(u,
                                  aligned_w, aligned_h, w, h);
    if(Esd_GpuAlloc_Get(u->allocator, l8_handle) == GA_INVALID) {
        return GA_HANDLE_INVALID;
    }

    const lv_area_t *layer_area = &layer->buf_area;
    lv_draw_task_t *t = eve5_slice_first(slice, layer);
    lv_draw_task_t *prev_task = NULL;

    while(t && t != slice->end) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_QUEUED) {
            prev_task = t;
            t = t->next;
            continue;
        }

        if(t->type == LV_DRAW_TASK_TYPE_MASK_RECTANGLE) {
            prev_task = t;
            t = t->next;
            continue;
        }

        /* Skip tasks inside opaque area; alpha will be overwritten to 255 */
        if(lv_draw_eve5_is_fully_inside_opaque(u, &t->_real_area, &t->clip_area, layer_area)) {
            prev_task = t;
            t = t->next;
            continue;
        }

        switch(t->type) {
            case LV_DRAW_TASK_TYPE_FILL:
                lv_draw_eve5_hal_draw_fill(u, t, true);
                break;

            case LV_DRAW_TASK_TYPE_BORDER:
                lv_draw_eve5_hal_draw_border(u, t, true);
                break;

            case LV_DRAW_TASK_TYPE_LINE:
                lv_draw_eve5_hal_draw_line(u, t, true, lv_draw_eve5_line_should_use_hv_opt(t, prev_task));
                break;

            case LV_DRAW_TASK_TYPE_TRIANGLE:
                lv_draw_eve5_hal_draw_triangle(u, t, true);
                break;

            case LV_DRAW_TASK_TYPE_ARC:
                lv_draw_eve5_hal_draw_arc(u, t, true);
                break;

            case LV_DRAW_TASK_TYPE_IMAGE:
            case LV_DRAW_TASK_TYPE_LAYER:
                lv_draw_eve5_hal_draw_image(u, t, true);
                break;

            case LV_DRAW_TASK_TYPE_LABEL:
                lv_draw_eve5_alpha_draw_label(u, (lv_draw_task_t *)t, true);
                break;

            case LV_DRAW_TASK_TYPE_LETTER:
                lv_draw_eve5_alpha_draw_letter(u, (lv_draw_task_t *)t, true);
                break;

            case LV_DRAW_TASK_TYPE_BOX_SHADOW:
                lv_draw_eve5_alpha_draw_box_shadow(u, t, true);
                break;

            default:
                break;
        }

        prev_task = t;
        t = t->next;
    }

    lv_draw_eve5_hal_finish_l8_rendertarget(u);

    return l8_handle;
}

#endif /* EVE5_USE_RENDERTARGET_ALPHA */

/**********************
 * DIRECT-TO-ALPHA PASS
 **********************/

/**
 * Direct-to-alpha correction pass: re-iterate tasks and write correct alpha.
 *
 * Clears layer alpha to 0, then re-draws each task's shape with
 * blend(ONE, ONE_MINUS_SRC_ALPHA) and colorMask(0,0,0,1), producing correct
 * Porter-Duff "over" alpha: result.a = src.a + dst.a * (1 - src.a/255).
 *
 * This corrects the squared-alpha from the RGB pass, but cannot recover
 * alpha that was used as scratch space by masking operations.
 */
void lv_draw_eve5_alpha_pass(lv_draw_eve5_unit_t *u, lv_layer_t *layer, const lv_draw_eve5_slice_t *slice)
{
    EVE_HalContext *phost = u->hal;
    const lv_area_t *layer_area = &layer->buf_area;
    int32_t layer_w = lv_area_get_width(layer_area);
    int32_t layer_h = lv_area_get_height(layer_area);

    /* Clear entire layer alpha to 0.
     * Reset scissor first since the main pass may have left it restricted. */
    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, layer_w, layer_h);
    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_saveContext(phost);
    EVE_CoDl_colorA(phost, 0);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    EVE_CoDl_lineWidth(phost, 16);
    EVE_CoDl_begin(phost, RECTS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_vertex2f_0(phost, layer_w, layer_h);
    EVE_CoDl_end(phost);
    EVE_CoDl_restoreContext(phost);

    /* Canvas: blit existing content's alpha as background before compositing new tasks */
    if(u->canvas_orig_addr != GA_INVALID) {
        EVE_CoDl_saveContext(phost);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_bitmapSource(phost, u->canvas_orig_addr);
        EVE_CoDl_bitmapLayout(phost, (uint8_t)u->canvas_orig_format, u->canvas_orig_stride, u->canvas_orig_h);
        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, u->canvas_orig_w, u->canvas_orig_h);
        if(u->canvas_orig_palette != GA_INVALID) {
            EVE_CoDl_paletteSource(phost, u->canvas_orig_palette);
        }
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_end(phost);
        EVE_CoDl_restoreContext(phost);
    }

    /* Set alpha pass blend mode: Porter-Duff "over" for alpha accumulation */
    EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);

    lv_draw_task_t *t = eve5_slice_first(slice, layer);
    lv_draw_task_t *prev_task = NULL;

    while(t && t != slice->end) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_IN_PROGRESS) {
            prev_task = t;
            t = t->next;
            continue;
        }

        if(lv_draw_eve5_is_fully_inside_opaque(u, &t->_real_area, &t->clip_area, layer_area)) {
            t->state = LV_DRAW_TASK_STATE_FINISHED;
            prev_task = t;
            t = t->next;
            continue;
        }

        switch(t->type) {
            case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t *fill_dsc = t->draw_dsc;

                /* Opaque fill+border pair: draw single rect at alpha=255 */
                lv_draw_task_t *next = t->next;
                if(fill_dsc->opa >= LV_OPA_MAX &&
                   fill_dsc->grad.dir == LV_GRAD_DIR_NONE &&
                   next &&
                   next->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
                   next->state == LV_DRAW_TASK_STATE_IN_PROGRESS &&
                   next->type == LV_DRAW_TASK_TYPE_BORDER &&
                   next->target_layer == t->target_layer) {

                    const lv_draw_border_dsc_t *border_dsc = next->draw_dsc;

                    if(eve5_fill_border_area_match(&t->area, &next->area) &&
                       fill_dsc->radius == border_dsc->radius) {
                        lv_draw_eve5_alpha_draw_fill_with_border(u, t, next);
                        t->state = LV_DRAW_TASK_STATE_FINISHED;
                        t = next;
                        break;
                    }
                }

                lv_draw_eve5_alpha_draw_fill(u, t);
                break;
            }

            case LV_DRAW_TASK_TYPE_BORDER:
                lv_draw_eve5_alpha_draw_border(u, t);
                break;

            case LV_DRAW_TASK_TYPE_LINE:
                lv_draw_eve5_alpha_draw_line(u, t, lv_draw_eve5_line_should_use_hv_opt(t, prev_task));
                break;

            case LV_DRAW_TASK_TYPE_TRIANGLE:
                lv_draw_eve5_alpha_draw_triangle(u, t);
                break;

            case LV_DRAW_TASK_TYPE_IMAGE:
            case LV_DRAW_TASK_TYPE_LAYER:
                lv_draw_eve5_hal_alpha_draw_image(u, t);
                break;

            case LV_DRAW_TASK_TYPE_LABEL:
                lv_draw_eve5_alpha_draw_label(u, t, false);
                break;

            case LV_DRAW_TASK_TYPE_LETTER:
                lv_draw_eve5_alpha_draw_letter(u, t, false);
                break;

            case LV_DRAW_TASK_TYPE_BOX_SHADOW:
                lv_draw_eve5_alpha_draw_box_shadow(u, t, false);
                break;

            case LV_DRAW_TASK_TYPE_ARC:
                lv_draw_eve5_alpha_draw_arc(u, t);
                break;

            case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
                /* Deferred: processed after alpha pass */
                break;

            default:
                break;
        }

        t->state = LV_DRAW_TASK_STATE_FINISHED;
        prev_task = t;
        t = t->next;
    }

    /* Fill opaque area with alpha=255 */
    if(u->has_alpha_opaque) {
        EVE_CoDl_colorA(phost, 255);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        lv_draw_eve5_set_scissor(u, &(lv_area_t){
            u->alpha_opaque_area.x1 + layer_area->x1,
            u->alpha_opaque_area.y1 + layer_area->y1,
            u->alpha_opaque_area.x2 + layer_area->x1,
            u->alpha_opaque_area.y2 + layer_area->y1
        }, layer_area);
        lv_draw_eve5_draw_rect(u,
            u->alpha_opaque_area.x1, u->alpha_opaque_area.y1,
            u->alpha_opaque_area.x2, u->alpha_opaque_area.y2,
            u->alpha_opaque_radius,
            &(lv_area_t){
                u->alpha_opaque_area.x1 + layer_area->x1,
                u->alpha_opaque_area.y1 + layer_area->y1,
                u->alpha_opaque_area.x2 + layer_area->x1,
                u->alpha_opaque_area.y2 + layer_area->y1
            }, layer_area);
    }

    EVE_CoDl_blendFunc_default(phost);
    EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
}

#endif /* LV_USE_DRAW_EVE5 */
