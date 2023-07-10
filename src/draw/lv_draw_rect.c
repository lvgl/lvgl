/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_draw_rect.h"
#include "../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc)
{
    lv_memset_00(dsc, sizeof(lv_draw_rect_dsc_t));
    dsc->bg_color = lv_color_white();
    dsc->bg_grad.stops[0].color = lv_color_white();
    dsc->bg_grad.stops[1].color = lv_color_black();
    dsc->bg_grad.stops[1].frac = 0xFF;
    dsc->bg_grad.stops_count = 2;
    dsc->border_color = lv_color_black();
    dsc->shadow_color = lv_color_black();
    dsc->bg_img_symbol_font = LV_FONT_DEFAULT;
    dsc->bg_opa = LV_OPA_COVER;
    dsc->bg_img_opa = LV_OPA_COVER;
    dsc->outline_opa = LV_OPA_COVER;
    dsc->border_opa = LV_OPA_COVER;
    dsc->shadow_opa = LV_OPA_COVER;
    dsc->border_side = LV_BORDER_SIDE_FULL;
}

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;

    draw_ctx->draw_rect(draw_ctx, dsc, coords);

    LV_ASSERT_MEM_INTEGRITY();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
