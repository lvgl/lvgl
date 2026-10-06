/**
 * @file lv_draw_eve5_dl_bound.c
 *
 * EVE5 (BT820) display list budget: worst-case entry counts of the draw tasks.
 *
 * Each render target cycle is one display list, which holds EVE_DL_COUNT
 * entries. The RGB pass ends a slice before a task whose bound would not fit
 * (see lv_draw_eve5_render_tasks). The bounds are cheap to compute, so the
 * decision needs no coprocessor read and does not stall the command pipeline.
 *
 * The constants are the maximum over all paths through each draw function,
 * with every CoDl call counted as if the optimizer wrote it, and coprocessor
 * commands at the entries they write (EVE_CO_DL_ENTRIES_*). Loops are
 * bounded by the counts computed here: polyline segments, dash gaps, tile
 * stamps and label characters. They were derived with a branch-aware static
 * analysis of the preprocessed draw sources (eve_apps
 * tests/scripts/eve5_dl_bound.py); rerun it when a draw function changes.
 * Builds with EVE5_DL_STATS=1 log every task whose measured entries exceed
 * its bound.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

/*********************
 * DEFINES
 *********************/

/* RGB pass (lv_draw_eve5_hal_draw_*), direct-to-alpha pass (lv_draw_eve5_alpha_draw_*).
 * The L8 alpha render-target pass calls the RGB functions, or alpha functions
 * that stay within the RGB bound. The SW fallback (lv_draw_eve5_sw_render_task,
 * 22) is within every RGB bound. */
#define DL_FILL                 65
#define DL_FILL_ALPHA           55
#define DL_FILL_BORDER          121 /* Unified FILL+BORDER */
#define DL_FILL_BORDER_ALPHA    13  /* Unified opaque pair; others take DL_FILL_ALPHA + DL_BORDER_ALPHA */
#define DL_BORDER               56
#define DL_BORDER_ALPHA         43
#define DL_TRIANGLE             67
#define DL_TRIANGLE_ALPHA       39
#define DL_ARC                  183
#define DL_ARC_ALPHA            63
#define DL_IMAGE                167 /* Plus DL_IMAGE_STAMP per tile stamp */
#define DL_IMAGE_ALPHA          97
#define DL_IMAGE_STAMP          2
#define DL_BOX_SHADOW           151
#define DL_BOX_SHADOW_ALPHA     151
#define DL_MASK_RECT            31
#define DL_LETTER               72  /* 16 around the glyph callback (56, a placeholder box); CMD_TEXT path 32 */
#define DL_SW_TASK              22
#define DL_SW_TASK_ALPHA        22  /* lv_draw_eve5_sw_alpha_draw_task_texture */

/*********************
 * GLOBAL FUNCTIONS
 *********************/

uint32_t lv_draw_eve5_dl_budget(EVE_HalContext * phost)
{
    LV_UNUSED(phost); /* EVE_DL_COUNT only reads it in multi-target builds */
    uint32_t count = EVE5_DL_BUDGET_TEST ? EVE5_DL_BUDGET_TEST : EVE_DL_COUNT;
    return count - EVE5_DL_MARGIN;
}

/* Tile stamps of an image task: non-power-of-two tiled sources are drawn by
 * stamping the source across the tile extent (eve5_image_tile_emit_stamps) */
static uint32_t image_stamps(const lv_draw_task_t * t)
{
    const lv_draw_image_dsc_t * dsc = t->draw_dsc;
    if(!dsc->tile) return 1;

    int32_t src_w, src_h;
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        const lv_layer_t * child = (const lv_layer_t *)dsc->src;
        src_w = lv_area_get_width(&child->buf_area);
        src_h = lv_area_get_height(&child->buf_area);
    }
    else {
        src_w = dsc->header.w;
        src_h = dsc->header.h;
    }
    if(src_w <= 0 || src_h <= 0) return 1;
    if(!eve5_image_tile_needs_stamps(src_w, src_h)) return 1;

    int32_t nx = (t->area.x2 - dsc->image_area.x1 + 1 + src_w - 1) / src_w;
    int32_t ny = (t->area.y2 - dsc->image_area.y1 + 1 + src_h - 1) / src_h;
    return (uint32_t)LV_MAX(nx, 1) * (uint32_t)LV_MAX(ny, 1);
}

/**
 * Worst-case entries of task `t`, and for a FILL of the BORDER it unifies with.
 * `end` is the exclusive end of the slice, which the unified pair must not cross.
 */
void lv_draw_eve5_task_dl_bound(lv_draw_eve5_unit_t * u, lv_draw_task_t * t, const lv_draw_task_t * end,
                                lv_draw_eve5_dl_bound_t * bound)
{
    bound->rgb = 0;
    bound->alpha = 0;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            if(lv_draw_eve5_fill_matching_border(t, end)) {
                bound->rgb = DL_FILL_BORDER;
                bound->alpha = LV_MAX(DL_FILL_BORDER_ALPHA, DL_FILL_ALPHA + DL_BORDER_ALPHA);
            }
            else {
                bound->rgb = DL_FILL;
                bound->alpha = DL_FILL_ALPHA;
            }
            break;

        case LV_DRAW_TASK_TYPE_BORDER:
            bound->rgb = DL_BORDER;
            bound->alpha = DL_BORDER_ALPHA;
            break;

        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_eve5_line_dl_bound(t, bound);
            break;

        case LV_DRAW_TASK_TYPE_TRIANGLE:
            bound->rgb = DL_TRIANGLE;
            bound->alpha = DL_TRIANGLE_ALPHA;
            break;

        case LV_DRAW_TASK_TYPE_LABEL:
            if(LV_DRAW_EVE5_SW_LABEL || lv_draw_eve5_label_sw_texture(t)) {
                bound->rgb = DL_SW_TASK;
                bound->alpha = DL_SW_TASK_ALPHA;
                break;
            }
            bound->rgb = lv_draw_eve5_label_dl_bound(t);
            bound->alpha = bound->rgb;
            break;

        case LV_DRAW_TASK_TYPE_LETTER:
            bound->rgb = DL_LETTER;
            bound->alpha = DL_LETTER;
            break;

        case LV_DRAW_TASK_TYPE_LAYER: {
                /* A child without GPU content draws nothing */
                const lv_draw_image_dsc_t * dsc = t->draw_dsc;
                lv_eve5_vram_res_t * child_vr = eve5_get_vram_res(u, (lv_layer_t *)dsc->src);
                if(child_vr == NULL) break;
            }
            /* fallthrough */
        case LV_DRAW_TASK_TYPE_IMAGE: {
                uint32_t stamps = image_stamps(t);
                bound->rgb = DL_IMAGE + DL_IMAGE_STAMP * stamps;
                bound->alpha = DL_IMAGE_ALPHA + DL_IMAGE_STAMP * stamps;
                break;
            }

        case LV_DRAW_TASK_TYPE_ARC:
            bound->rgb = DL_ARC;
            bound->alpha = DL_ARC_ALPHA;
            break;

        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            bound->rgb = DL_BOX_SHADOW;
            bound->alpha = DL_BOX_SHADOW_ALPHA;
            break;

        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            bound->rgb = DL_MASK_RECT;
            break;

#if LV_USE_VECTOR_GRAPHIC && LV_DRAW_EVE5_SW_VECTOR
        case LV_DRAW_TASK_TYPE_VECTOR:
            bound->rgb = DL_SW_TASK;
            bound->alpha = DL_SW_TASK_ALPHA;
            break;
#endif

        default:
            break;
    }
}

/**
 * Whether the queued EVE5 tasks in [start, end) fit one display list by their
 * bounds alone, with `overhead` entries around them. For targets that cannot
 * be split once their display list has started (the full-mode swapchain).
 */
bool lv_draw_eve5_range_fits_dl(lv_draw_eve5_unit_t * u, lv_draw_task_t * start, const lv_draw_task_t * end,
                                uint32_t overhead)
{
    uint32_t budget = lv_draw_eve5_dl_budget(u->hal);
    uint32_t total = overhead;
    lv_draw_task_t * merged = NULL;

    for(lv_draw_task_t * t = start; t && t != end; t = t->next) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_QUEUED ||
           t == merged) {
            continue;
        }
        lv_draw_eve5_dl_bound_t bound;
        lv_draw_eve5_task_dl_bound(u, t, end, &bound);
        if(t->type == LV_DRAW_TASK_TYPE_FILL) merged = lv_draw_eve5_fill_matching_border(t, end);
        total += bound.rgb;
        if(total > budget) return false;
    }

    return true;
}

#endif /* LV_USE_DRAW_EVE5 */
