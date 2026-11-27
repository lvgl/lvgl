/**
 * @file lv_draw_sifli_epic.c
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sifli_epic.h"
#include "../../sw/lv_draw_sw_private.h"

#if LV_USE_SIFLI_EPIC
#include "lv_sifli_epic_utils.h"
#include "lv_sifli_epic_cfg.h"
#include "../../../display/lv_display_private.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Tell whether EPIC can draw on a layer.
 * @param layer Target layer
 * @return true if the layer's color format is supported
 */
static bool _epic_layer_supported(const lv_layer_t * layer);

/**
 * Tell whether EPIC can draw a descriptor on a layer.
 * @param draw_unit Draw unit
 * @param layer Target layer
 * @param dsc Draw descriptor
 * @return true if EPIC can draw it
 */
static bool _epic_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool _epic_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
static bool _epic_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
static bool _epic_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool _epic_can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);

/**
 * Wait for EPIC to finish the current operation.
 * @param draw_unit Draw unit
 * @return 1 on success
 */
static int32_t _epic_wait_for_finish(lv_draw_unit_t * draw_unit);

/**
 * Wait for EPIC and write the CPU cache of the clip area back before it draws.
 */
static void _epic_draw_begin(lv_layer_t * layer, const lv_area_t * clip_area);

/**
 * Wait for EPIC to finish drawing and invalidate the CPU cache of the clip area.
 */
static void _epic_draw_end(lv_layer_t * layer, const lv_area_t * clip_area);

static void _epic_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void _epic_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                              const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
static void _epic_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                             const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);
static void _epic_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                             const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void _epic_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                             const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = _epic_draw_fill,
    .can_draw_fill_cb = _epic_can_draw_fill,
    .draw_border_cb = _epic_draw_border,
    .can_draw_border_cb = _epic_can_draw_border,
    .draw_label_cb = _epic_draw_label,
    .can_draw_label_cb = _epic_can_draw_label,
    .draw_image_cb = _epic_draw_image,
    .can_draw_image_cb = _epic_can_draw_img,
    .draw_layer_cb = _epic_draw_layer,
    .can_draw_layer_cb = _epic_can_draw_layer,
};

static bool epic_draw_unit_initialized = false;
/**********************
 *      MACROS
 **********************/


/* Vector glyphs are not handled by the EPIC label path below.
 * Keep the check lightweight and fall back only when the first glyph is vector. */
static bool _epic_label_has_vector_glyph(const lv_draw_label_dsc_t * draw_dsc)
{
#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    if(draw_dsc == NULL || draw_dsc->font == NULL || draw_dsc->text == NULL || draw_dsc->text[0] == '\0') {
        return false;
    }

    lv_font_glyph_dsc_t glyph_dsc;
    uint32_t txt_ofs = 0;
    uint32_t letter = lv_text_encoded_next(draw_dsc->text, &txt_ofs);

    if(letter == '\0') {
        return false;
    }

    if(lv_font_get_glyph_dsc(draw_dsc->font, &glyph_dsc, letter, '\0') &&
       glyph_dsc.format == LV_FONT_GLYPH_FORMAT_VECTOR) {
        return true;
    }
#else
    LV_UNUSED(draw_dsc);
#endif

    return false;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sifli_epic_init(void)
{
    if(epic_draw_unit_initialized) {
        return;
    }

    /* Initialize EPIC hardware */
    lv_epic_init();
    if(!lv_epic_is_initialized()) {
        return;
    }

    /* Initialize draw buffer handlers */
    lv_draw_buf_sifli_epic_init_handlers();

    /* Create EPIC draw unit */
    lv_draw_sifli_epic_unit_t * draw_epic_unit = lv_draw_create_unit(sizeof(lv_draw_sifli_epic_unit_t));
    draw_epic_unit->base_unit.name = "SiFli_EPIC";
    draw_epic_unit->base_unit.draw_cbs = &draw_cbs;
    draw_epic_unit->base_unit.fallback = lv_draw_sw_get_draw_unit();
    draw_epic_unit->base_unit.wait_for_finish_cb = _epic_wait_for_finish;

    epic_draw_unit_initialized = true;
}

void lv_draw_sifli_epic_deinit(void)
{
    if(!epic_draw_unit_initialized) {
        return;
    }

    lv_epic_deinit();
    epic_draw_unit_initialized = false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool _epic_layer_supported(const lv_layer_t * layer)
{
    return layer != NULL && lv_epic_cf_supported(layer->color_format, 0);
}

static bool _epic_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    if(!_epic_layer_supported(layer)) {
        return false;
    }

    /* EPIC doesn't support rounded corners */
    if(dsc->radius != 0) {
        return false;
    }

    /* Check gradient support */
    if(dsc->grad.dir != LV_GRAD_DIR_NONE) {
        if(dsc->grad.dir != LV_GRAD_DIR_HOR && dsc->grad.dir != LV_GRAD_DIR_VER) {
            return false;
        }

        /* Only support 2-stop gradients */
        if(dsc->grad.stops_count != 2) {
            return false;
        }

        /* Check if output format supports gradient */
        uint32_t epic_cf = lv_img_cf_to_epic_cf(layer->color_format);
        if(!EPIC_SUPPROT_OUT_FORMAT(epic_cf)) {
            return false;
        }
    }

    return true;
}

static bool _epic_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    return _epic_layer_supported(layer) && dsc->radius == 0;
}

static bool _epic_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

#ifdef EPIC_SUPPORT_A8
    if(!_epic_layer_supported(layer)) {
        return false;
    }

    if(dsc->font == NULL || dsc->text == NULL || dsc->text[0] == '\0') {
        return false;
    }

    return !_epic_label_has_vector_glyph(dsc);
#else
    LV_UNUSED(layer);
    LV_UNUSED(dsc);
    return false;
#endif
}

static bool _epic_can_draw_img(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    if(!_epic_layer_supported(layer)) {
        return false;
    }

    if(dsc->src == NULL) {
        return false;
    }

    if(dsc->clip_radius != 0) {
        return false;
    }

    if(dsc->blend_mode != LV_BLEND_MODE_NORMAL) {
        return false;
    }

    /* Check if recolor is used */
    bool has_recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
    if(has_recolor) {
        return false;
    }

    /* Check if transform is used */
    bool has_transform = (dsc->rotation != 0 ||
                          dsc->scale_x != LV_SCALE_NONE ||
                          dsc->scale_y != LV_SCALE_NONE);

    /* Check color format support */
    if(!lv_epic_cf_supported(dsc->header.cf, dsc->header.flags)) {
        return false;
    }

    /* RGB565A8 with transform is not supported */
    return !((LV_COLOR_FORMAT_RGB565A8 == dsc->header.cf) && has_transform);
}

static bool _epic_can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);

    if(!_epic_layer_supported(layer)) {
        return false;
    }

    const lv_layer_t * layer_to_draw = dsc->src;

    if(layer_to_draw == NULL) {
        return false;
    }

    if(layer_to_draw->draw_buf == NULL) {
        return false;
    }

    /* Check if recolor is used */
    bool has_recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
    if(has_recolor) {
        return false;
    }

    /* Check if transform is used */
    bool has_transform = (dsc->rotation != 0 ||
                          dsc->scale_x != LV_SCALE_NONE ||
                          dsc->scale_y != LV_SCALE_NONE);

    /* Check color format support */
    if(!lv_epic_cf_supported(layer_to_draw->color_format, 0)) {
        return false;
    }

    /* RGB565A8 with transform is not supported */
    return !((LV_COLOR_FORMAT_RGB565A8 == layer_to_draw->color_format) && has_transform);
}

static int32_t _epic_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    LV_UNUSED(draw_unit);
    lv_epic_wait();
    return 1;
}

static void _epic_draw_begin(lv_layer_t * layer, const lv_area_t * clip_area)
{
    /* Wait for previous operation to complete */
    lv_epic_wait();

    lv_area_t draw_area = *clip_area;
    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_flush_cache(layer->draw_buf, &draw_area);
}

static void _epic_draw_end(lv_layer_t * layer, const lv_area_t * clip_area)
{
    /* Wait for operation to complete */
    lv_epic_wait();

    lv_area_t draw_area = *clip_area;
    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_invalidate_cache(layer->draw_buf, &draw_area);
}

static void _epic_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    _epic_draw_begin(layer, clip_area);
    lv_draw_sifli_epic_fill(draw_unit, layer, clip_area, dsc, coords);
    _epic_draw_end(layer, clip_area);
}

static void _epic_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                              const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    _epic_draw_begin(layer, clip_area);
    lv_draw_sifli_epic_border(draw_unit, layer, clip_area, dsc, coords);
    _epic_draw_end(layer, clip_area);
}

static void _epic_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                             const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    _epic_draw_begin(layer, clip_area);
    lv_draw_sifli_epic_label(draw_unit, layer, clip_area, dsc, coords);
    _epic_draw_end(layer, clip_area);
}

static void _epic_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                             const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    _epic_draw_begin(layer, clip_area);
    lv_draw_sifli_epic_img(draw_unit, layer, clip_area, dsc, coords);
    _epic_draw_end(layer, clip_area);
}

static void _epic_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                             const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    _epic_draw_begin(layer, clip_area);
    lv_draw_sifli_epic_layer(draw_unit, layer, clip_area, dsc, coords);
    _epic_draw_end(layer, clip_area);
}

#endif /*LV_USE_SIFLI_EPIC*/
