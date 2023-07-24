/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../core/lv_refr.h"
#include "../misc/lv_math.h"

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

void lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc) LV_ATTRIBUTE_FAST_MEM
{
    lv_memset_00(dsc, sizeof(lv_draw_line_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
}

void lv_draw_line(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                  const lv_point_t * point1, const lv_point_t * point2) LV_ATTRIBUTE_FAST_MEM
{
    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    draw_ctx->draw_line(draw_ctx, dsc, point1, point2);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
