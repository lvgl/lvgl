/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "blend/lv_draw_sw_blend.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_txt_ap.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_assert.h"
#include "../lv_draw_mask.h"

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

void lv_draw_sw_bg_img(lv_draw_unit_t * draw_unit, const lv_draw_bg_img_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->src == NULL) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_area_t clip_area;
    if(!_lv_area_intersect(&clip_area, coords, draw_unit->clip_area)) {
        return;
    }

    const lv_area_t * clip_area_ori = draw_unit->clip_area;
    draw_unit->clip_area = &clip_area;

    lv_img_src_t src_type = lv_img_src_get_type(dsc->src);
    if(src_type == LV_IMG_SRC_SYMBOL) {
        lv_point_t size;
        lv_txt_get_size(&size, dsc->src, dsc->font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        lv_area_t a;
        a.x1 = coords->x1 + lv_area_get_width(coords) / 2 - size.x / 2;
        a.x2 = a.x1 + size.x - 1;
        a.y1 = coords->y1 + lv_area_get_height(coords) / 2 - size.y / 2;
        a.y2 = a.y1 + size.y - 1;

        lv_draw_label_dsc_t label_draw_dsc;
        lv_draw_label_dsc_init(&label_draw_dsc);
        label_draw_dsc.font = dsc->font;
        label_draw_dsc.color = dsc->recolor;
        label_draw_dsc.opa = dsc->opa;
        label_draw_dsc.text = dsc->src;
        lv_draw_sw_label(draw_unit, &label_draw_dsc, &a);
    }
    else {
        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        img_dsc.recolor = dsc->recolor;
        img_dsc.recolor_opa = dsc->recolor_opa;
        img_dsc.opa = dsc->opa;
        img_dsc.src = dsc->src;

        /*Center align*/
        if(dsc->tiled == false) {
            lv_area_t area;
            area.x1 = coords->x1 + lv_area_get_width(coords) / 2 - dsc->img_header.w / 2;
            area.y1 = coords->y1 + lv_area_get_height(coords) / 2 - dsc->img_header.h / 2;
            area.x2 = area.x1 + dsc->img_header.w - 1;
            area.y2 = area.y1 + dsc->img_header.h - 1;

            lv_draw_sw_img(draw_unit, &img_dsc, &area);
        }
        else {
            lv_area_t area;
            area.y1 = coords->y1;
            area.y2 = area.y1 + dsc->img_header.h - 1;

            for(; area.y1 <= coords->y2; area.y1 += dsc->img_header.h, area.y2 += dsc->img_header.h) {

                area.x1 = coords->x1;
                area.x2 = area.x1 + dsc->img_header.w - 1;
                for(; area.x1 <= coords->x2; area.x1 += dsc->img_header.w, area.x2 += dsc->img_header.w) {
                    lv_draw_sw_img(draw_unit, &img_dsc, &area);
                }
            }
        }
    }

    draw_unit->clip_area = clip_area_ori;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
