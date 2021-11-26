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
    dsc->bg_grad_color = lv_color_black();
    dsc->border_color = lv_color_black();
    dsc->shadow_color = lv_color_black();
    dsc->bg_grad_color_stop = 0xFF;
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
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;


    const lv_draw_backend_t * backend = lv_draw_backend_get();
    backend->draw_rect(coords, clip, dsc);

    LV_ASSERT_MEM_INTEGRITY();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
