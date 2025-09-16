/**
 * @file lv_grad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_grad.h"

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
void lv_grad_init_stops(lv_grad_dsc_t * dsc, const lv_color_t colors[], const lv_opa_t opa[],
                        const uint8_t fracs[], int num_stops)
{
    LV_ASSERT(num_stops <= LV_GRADIENT_MAX_STOPS);
    LV_ASSERT(num_stops > 1);
    LV_ASSERT_NULL(dsc);
    LV_ASSERT_NULL(colors);

    dsc->stops_count = num_stops;
    for(int i = 0; i < num_stops; i++) {
        dsc->stops[i].color = colors[i];
        dsc->stops[i].opa = opa != NULL ? opa[i] : LV_OPA_COVER;
        dsc->stops[i].frac = fracs != NULL ? fracs[i] : 255 * i / (num_stops - 1);
    }
}

void lv_grad_horizontal_init(lv_grad_dsc_t * dsc)
{
    LV_ASSERT_NULL(dsc);

    dsc->dir = LV_GRAD_DIR_HOR;
}

void lv_grad_vertical_init(lv_grad_dsc_t * dsc)
{
    LV_ASSERT_NULL(dsc);

    dsc->dir = LV_GRAD_DIR_VER;
}

void lv_grad_linear_init(lv_grad_dsc_t * dsc, int32_t from_x, int32_t from_y, int32_t to_x, int32_t to_y,
                         lv_grad_extend_t extend)
{
    LV_ASSERT_NULL(dsc);
    dsc->dir = LV_GRAD_DIR_LINEAR;
    dsc->params.linear.start.x = from_x;
    dsc->params.linear.start.y = from_y;
    dsc->params.linear.end.x = to_x;
    dsc->params.linear.end.y = to_y;
    dsc->extend = extend;
}

void lv_grad_radial_init(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t to_x, int32_t to_y,
                         lv_grad_extend_t extend)
{
    LV_ASSERT_NULL(dsc);
    dsc->dir = LV_GRAD_DIR_RADIAL;
    dsc->params.radial.focal.x = center_x;
    dsc->params.radial.focal.y = center_y;
    dsc->params.radial.focal_extent.x = center_x;
    dsc->params.radial.focal_extent.y = center_y;
    dsc->params.radial.end.x = center_x;
    dsc->params.radial.end.y = center_y;
    dsc->params.radial.end_extent.x = to_x;
    dsc->params.radial.end_extent.y = to_y;
    dsc->extend = extend;
}

void lv_grad_conical_init(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t start_angle,
                          int32_t end_angle, lv_grad_extend_t extend)
{
    LV_ASSERT_NULL(dsc);
    dsc->dir = LV_GRAD_DIR_CONICAL;
    dsc->params.conical.center.x = center_x;
    dsc->params.conical.center.y = center_y;
    dsc->params.conical.start_angle = start_angle;
    dsc->params.conical.end_angle = end_angle;
    dsc->extend = extend;
}

void lv_grad_radial_set_focal(lv_grad_dsc_t * dsc, int32_t center_x, int32_t center_y, int32_t radius)
{
    LV_ASSERT_NULL(dsc);
    dsc->params.radial.focal.x = center_x;
    dsc->params.radial.focal.y = center_y;
    dsc->params.radial.focal_extent.x = center_x + radius;
    dsc->params.radial.focal_extent.y = center_y;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
