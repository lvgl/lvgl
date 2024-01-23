/**
 * @file lv_draw_vg_lite_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include "lv_draw_vg_lite_type.h"

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

void lv_draw_vg_lite_mask_rect(lv_draw_unit_t * draw_unit, const lv_draw_mask_rect_dsc_t * dsc,
                               const lv_area_t * coords)
{
    LV_UNUSED(coords);

    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &dsc->area, draw_unit->clip_area)) {
        return;
    }

    lv_draw_sw_mask_radius_param_t param;
    lv_draw_sw_mask_radius_init(&param, &dsc->area, dsc->radius, false);

    void * masks[2] = {0};
    masks[0] = &param;

    uint32_t area_w = lv_area_get_width(&draw_area);
    lv_opa_t * mask_buf = lv_malloc(area_w);

    int32_t y;
    for(y = draw_area.y1; y <= draw_area.y2; y++) {
        lv_memset(mask_buf, 0xff, area_w);
        lv_draw_sw_mask_res_t res = lv_draw_sw_mask_apply(masks, mask_buf, draw_area.x1, y, area_w);
        if(res == LV_DRAW_SW_MASK_RES_FULL_COVER) continue;

        lv_layer_t * target_layer = draw_unit->target_layer;
        lv_color32_t * c32_buf = lv_draw_layer_go_to_xy(target_layer, draw_area.x1 - target_layer->buf_area.x1,
                                                        y - target_layer->buf_area.y1);

        if(res == LV_DRAW_SW_MASK_RES_TRANSP) {
            lv_memzero(c32_buf, area_w * sizeof(lv_color32_t));
        }
        else {
            uint32_t i;
            for(i = 0; i < area_w; i++) {
                if(mask_buf[i] != LV_OPA_COVER) {
                    c32_buf[i].alpha = LV_OPA_MIX2(c32_buf[i].alpha, mask_buf[i]);
                }

                /*Pre-multiply the alpha*/
                lv_color_premultiply(&c32_buf[i]);
            }
        }
    }

    lv_free(mask_buf);
    lv_draw_sw_mask_free_param(&param);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
