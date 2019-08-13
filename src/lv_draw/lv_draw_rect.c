/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rect.h"
#include "../lv_misc/lv_circ.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_refr.h"
#include "lv_blit.h"
#include "lv_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_bg(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;


    draw_bg(coords, clip, style, opa_scale);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->body.opa;

    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_area_t draw_a;
    bool union_ok;

    /* Get the union of `coords` and `clip`*/
    /* `clip` is already truncated to the `vdb` size
     * in 'lv_refr_area' function */
    union_ok = lv_area_intersect(&draw_a, coords, clip);

    /*If there are common part of `clip` and `vdb` then draw*/
    if(union_ok == false) return;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /*Store the coordinates of the `draw_a` relative to the VDB */
    lv_area_t draw_rel_a;
    draw_rel_a.x1 = draw_a.x1 - vdb->area.x1;
    draw_rel_a.y1 = draw_a.y1 - vdb->area.y1;
    draw_rel_a.x2 = draw_a.x2 - vdb->area.x1;
    draw_rel_a.y2 = draw_a.y2 - vdb->area.y1;

    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    uint32_t vdb_width       = lv_area_get_width(&vdb->area);
    uint32_t draw_a_width    = lv_area_get_width(&draw_rel_a);

    /*Move the vdb_buf_tmp to the first row*/
    vdb_buf_tmp += vdb_width * draw_rel_a.y1;


    lv_color_t line_buf[LV_HOR_RES_MAX];
    lv_opa_t mask_buf[LV_HOR_RES_MAX];
    lv_mask_line_param_t line_mask_param;
    line_mask_param.origo.x = 0;
    line_mask_param.origo.y = 0;
    line_mask_param.steep = 987;
    line_mask_param.flat = 1;


    /*Fill with a color line-by-line*/
    lv_coord_t h;

    /*Fill the first row with 'color'*/
    for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {
        lv_blit_color(line_buf, &vdb_buf_tmp[draw_rel_a.x1], draw_a_width, style->body.main_color, LV_BLIT_MODE_NORMAL);

        if(style->body.main_color.full != style->body.grad_color.full) {
            memset(mask_buf, LV_OPA_COVER, draw_a_width);
            lv_mask_line_left(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width, true, &line_mask_param);
            lv_mask_apply(&vdb_buf_tmp[draw_rel_a.x1], line_buf, mask_buf, draw_a_width);
        } else {
            memcpy(&vdb_buf_tmp[draw_rel_a.x1], line_buf, draw_a_width * sizeof(lv_color_t));
        }

        vdb_buf_tmp += vdb_width;
    }
}
