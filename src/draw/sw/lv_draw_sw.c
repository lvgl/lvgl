/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_private.h"
#include "../lv_draw_private.h"
#if LV_USE_DRAW_SW

#include "../../display/lv_display_private.h"
#include "../../core/lv_global.h"
#include "../../misc/lv_area_private.h"

#if LV_USE_THORVG
    #if LV_USE_THORVG_INTERNAL
        #include "../../libs/thorvg/thorvg_capi.h"
    #else
        #include <thorvg_capi.h>
    #endif
#endif

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "arm2d/lv_draw_sw_helium.h"
#endif

#if LV_DRAW_SW_DRAW_UNIT_CNT > 1 && LV_USE_OS == LV_OS_NONE
    #error "OS support is required when more than one SW rendering units are enabled"
#endif

/*********************
 *      DEFINES
 *********************/
#define DRAW_UNIT_ID_SW     1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);
static void draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);
static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);
static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc);
static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc);
static void draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_mask_rect_dsc_t * dsc);
static void draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc);
#endif

static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = draw_fill,
    .draw_border_cb = draw_border,
    .draw_box_shadow_cb = draw_box_shadow,
    .draw_letter_cb = draw_letter,
    .draw_label_cb = draw_label,
    .draw_image_cb = draw_image,
    .can_draw_image_cb = can_draw_image,
    .draw_layer_cb = draw_layer,
    .can_draw_layer_cb = can_draw_image,
    .draw_arc_cb = draw_arc,
    .draw_line_cb = draw_line,
    .draw_triangle_cb = draw_triangle,
    .draw_mask_rect_cb = draw_mask_rect,
    .draw_blur_cb = draw_blur,
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    .draw_vector_cb = draw_vector,
#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_draw_unit_t * lv_draw_sw_get_draw_unit(void)
{
    /*Find the unit by its callback table, so no global has to store it*/
    lv_draw_unit_t * u = LV_GLOBAL_DEFAULT()->draw_info.unit_head;
    while(u && u->draw_cbs != &draw_cbs) u = u->next;
    return u;
}

void lv_draw_sw_init(void)
{

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_init();
#endif

    lv_draw_sw_unit_t * draw_sw_unit = lv_draw_create_unit(sizeof(lv_draw_sw_unit_t));
    draw_sw_unit->base_unit.draw_cbs = &draw_cbs;
    draw_sw_unit->base_unit.id = DRAW_UNIT_ID_SW;

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON
    draw_sw_unit->base_unit.name = "SW_NEON";
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && LV_USE_DRAW_ARM2D_SYNC
    draw_sw_unit->base_unit.name = "SW_ARM2D";
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    draw_sw_unit->base_unit.name = "SW_HELIUM";
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V
    draw_sw_unit->base_unit.name = "SW_RISCV_V";
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_SVE2
    draw_sw_unit->base_unit.name = "SW_SVE2";
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    draw_sw_unit->base_unit.name = "SW_CUSTOM";
#else
    draw_sw_unit->base_unit.name = "SW";
#endif

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    if(LV_DRAW_SW_DRAW_UNIT_CNT > 1) {
        tvg_engine_init(TVG_ENGINE_SW, LV_DRAW_SW_DRAW_UNIT_CNT);
    }
    else {
        tvg_engine_init(TVG_ENGINE_SW, 0);
    }
#endif

    lv_ll_init(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, sizeof(lv_draw_sw_custom_blend_handler_t));
}

void lv_draw_sw_deinit(void)
{
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    tvg_engine_term(TVG_ENGINE_SW);
#endif

#if LV_DRAW_SW_COMPLEX == 1
    lv_draw_sw_mask_deinit();
#endif
}

bool lv_draw_sw_register_blend_handler(lv_draw_sw_custom_blend_handler_t * handler)
{
    lv_draw_sw_custom_blend_handler_t * existing_handler = NULL;
    lv_draw_sw_custom_blend_handler_t * new_handler = NULL;

    // Check if a handler is already registered for the color format
    LV_LL_READ(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, existing_handler) {
        if(existing_handler->dest_cf == handler->dest_cf) {
            new_handler = existing_handler;
            break;
        }
    }

    if(new_handler == NULL) {
        new_handler = lv_ll_ins_head(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll);
        if(new_handler == NULL) {
            LV_ASSERT_MALLOC(new_handler);
            return false;
        }
    }

    lv_memcpy(new_handler, handler, sizeof(lv_draw_sw_custom_blend_handler_t));
    return true;
}

bool lv_draw_sw_unregister_blend_handler(lv_color_format_t dest_cf)
{
    lv_draw_sw_custom_blend_handler_t * handler;

    LV_LL_READ(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, handler) {
        if(handler->dest_cf == dest_cf) {
            lv_ll_remove(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, handler);
            lv_free(handler);
            return true;
        }
    }

    return false;
}

lv_draw_sw_blend_handler_t lv_draw_sw_get_blend_handler(lv_color_format_t dest_cf)
{
    lv_draw_sw_custom_blend_handler_t * handler;

    LV_LL_READ(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, handler) {
        if(handler->dest_cf == dest_cf) {
            return handler->handler;
        }
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);

    /* not support skew */
    if(dsc->skew_x != 0 || dsc->skew_y != 0) return false;

    lv_color_format_t cf = dsc->header.cf;
    bool masked = dsc->bitmap_mask_src != NULL;
    if(masked && (cf == LV_COLOR_FORMAT_A8 || cf == LV_COLOR_FORMAT_RGB565A8)) return false;

    return cf < LV_COLOR_FORMAT_PROPRIETARY_START;
}

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_fill(layer, clip_area, dsc, coords);
}

static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_border(layer, clip_area, dsc, coords);
}

static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_box_shadow(layer, clip_area, dsc, coords);
}

static void draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_letter(layer, clip_area, dsc, coords);
}

static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_label(layer, clip_area, dsc, coords);
}

static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_image(layer, clip_area, dsc, coords);
}

static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_layer(layer, clip_area, dsc, coords);
}

static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_arc(layer, clip_area, dsc, coords);
}

static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    lv_draw_line_iterate(layer, clip_area, dsc, lv_draw_sw_line, NULL);
}

static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_triangle(layer, clip_area, dsc);
}

static void draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_mask_rect_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_mask_rect(layer, clip_area, dsc);
}

static void draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_blur(layer, clip_area, dsc, coords);
}

#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    lv_draw_sw_vector(layer, clip_area, dsc);
}
#endif

#else /*LV_USE_DRAW_SW*/

lv_draw_unit_t * lv_draw_sw_get_draw_unit(void)
{
    return NULL;
}

#endif /*LV_USE_DRAW_SW*/
