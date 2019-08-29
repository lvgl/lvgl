/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/src/lv_draw/lv_blend.h>
#include "lv_draw_rect.h"
#include "../lv_misc/lv_circ.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_refr.h"
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
static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale);
static void shadow_draw_corner_buf(lv_opa_t * sh_buf, lv_coord_t s, lv_coord_t r);

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

    draw_shadow(coords, clip, style, opa_scale);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->body.opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, coords, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

    /*Create a mask if there is a radius*/
    lv_opa_t mask_buf[LV_HOR_RES_MAX];

    uint8_t other_mask_cnt = lv_mask_get_cnt();
    int16_t mask_rout_id = LV_MASK_ID_INV;

    /*Get the real radius*/
    lv_coord_t rout = style->body.radius;
    lv_coord_t short_side = LV_MATH_MIN(lv_area_get_width(coords), lv_area_get_height(coords));
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Most simple case: just a plain rectangle*/
    if(other_mask_cnt == 0 && rout == 0 && style->body.main_color.full == style->body.grad_color.full) {
        lv_blend_fill(clip, coords,
                style->body.main_color, NULL, LV_MASK_RES_FULL_COVER, style->body.opa,
                LV_BLIT_MODE_NORMAL);
    }
    /*More complex case: there is a radius, gradient or mask.*/
    else {

        lv_mask_param_t mask_rout_param;
        if(rout > 0) {
            lv_mask_radius_init(&mask_rout_param, coords, rout, false);
            mask_rout_id = lv_mask_add(lv_mask_radius, &mask_rout_param, NULL);
        }

        /*Draw the background line by line*/
        lv_coord_t h;
        lv_mask_res_t mask_res = LV_MASK_RES_FULL_COVER;
        lv_color_t grad_color = style->body.main_color;

        /*Fill the first row with 'color'*/
        if(opa >= LV_OPA_MIN) {

            lv_area_t fill_area;
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x2;
            fill_area.y1 = disp_area->y1 + draw_area.y1;
            fill_area.y2 = fill_area.y1;
            for(h = draw_area.y1; h <= draw_area.y2; h++) {
                lv_coord_t y = h + vdb->area.y1;

                /*In not corner areas apply the mask only if required*/
                if(y > coords->y1 + rout + 1 &&
                        y < coords->y2 - rout - 1) {
                    mask_res = LV_MASK_RES_FULL_COVER;
                    if(other_mask_cnt != 0) {
                        memset(mask_buf, LV_OPA_COVER, draw_area_w);
                        mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                    }
                }
                /*In corner areas apply the mask anyway*/
                else {
                    memset(mask_buf, LV_OPA_COVER, draw_area_w);
                    mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                }

                /*Get the current line color*/
                if(style->body.main_color.full != style->body.grad_color.full) {
                    lv_opa_t mix = (uint32_t)((uint32_t) (y - coords->y1) * 255) / lv_area_get_height(coords);
                    grad_color = lv_color_mix(style->body.grad_color, style->body.main_color, mix);
                }

                /* If there is not other mask and drawing the corner area split the drawing to corners and middle area
                 * because it the middle mask shuldn't be taken into account (therefore its faster)*/
                if(other_mask_cnt == 0 &&
                        (y < coords->y1 + rout + 1 ||
                                y > coords->y2 - rout - 1)) {

                    lv_area_t fill_area2;
                    fill_area2.x1 = coords->x1;
                    fill_area2.x2 = coords->x1 + rout;
                    fill_area2.y1 = fill_area.y1;
                    fill_area2.y2 = fill_area.y2;

                    lv_blend_fill(clip, &fill_area2,
                            grad_color, mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);


                    fill_area2.x1 = coords->x1 + rout + 1;
                    fill_area2.x2 = coords->x2 - rout - 1;

                    lv_blend_fill(clip, &fill_area2,
                            grad_color, NULL, LV_MASK_RES_FULL_COVER, style->body.opa, LV_BLIT_MODE_NORMAL);

                    fill_area2.x1 = coords->x2 - rout;
                    fill_area2.x2 = coords->x2;

                    lv_coord_t mask_ofs = (coords->x2 - rout) - (vdb->area.x1 + draw_area.x1);
                    if(mask_ofs < 0) mask_ofs = 0;
                    lv_blend_fill(clip, &fill_area2,
                            grad_color, mask_buf + mask_ofs, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);
                } else {
                    lv_blend_fill(clip, &fill_area,
                            grad_color,mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);
                }
                fill_area.y1++;
                fill_area.y2++;
            }
        }
    }

    /*Draw the border if any*/
    lv_coord_t border_width = style->body.border.width;
    if(border_width) {
        /*Move the vdb_buf_tmp to the first row*/
        lv_mask_param_t mask_rsmall_param;

        /*Get the inner radius*/
        lv_coord_t rin = rout - border_width;
        if(rin < 0) rin = 0;

        /*Get the inner area*/
        lv_area_t area_small;
        lv_area_copy(&area_small, coords);
        area_small.x1 += border_width;
        area_small.x2 -= border_width;
        area_small.y1 += border_width;
        area_small.y2 -= border_width;

        /*Create the mask*/
        lv_mask_radius_init(&mask_rsmall_param, &area_small, rout - border_width, true);
        int16_t mask_rsmall_id = lv_mask_add(lv_mask_radius, &mask_rsmall_param, NULL);

        lv_coord_t corner_size = LV_MATH_MAX(rout, border_width - 1);

        lv_coord_t h;
        lv_mask_res_t mask_res;
        lv_area_t fill_area;

        /*Apply some optimization if there is no other mask*/
        if(other_mask_cnt == 0) {
            /*Draw the upper corner area*/
            lv_coord_t upper_corner_end = coords->y1 - disp_area->y1 + corner_size;
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x2;
            fill_area.y1 = disp_area->y1 + draw_area.y1;
            fill_area.y2 = fill_area.y1;
            for(h = draw_area.y1; h <= upper_corner_end; h++) {
                memset(mask_buf, LV_OPA_COVER, draw_area_w);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

                lv_area_t fill_area2;
                fill_area2.x1 = coords->x1;
                fill_area2.x2 = coords->x1 + rout;
                fill_area2.y1 = fill_area.y1;
                fill_area2.y2 = fill_area.y2;

                lv_blend_fill(clip, &fill_area2,
                        style->body.border.color, mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);


                if(fill_area2.y2 < coords->y1 + style->body.border.width) {
                    fill_area2.x1 = coords->x1 + rout + 1;
                    fill_area2.x2 = coords->x2 - rout - 1;

                    lv_blend_fill(clip, &fill_area2,
                            style->body.border.color, NULL, LV_MASK_RES_FULL_COVER, style->body.border.opa, LV_BLIT_MODE_NORMAL);
                }
                fill_area2.x1 = coords->x2 - rout;
                fill_area2.x2 = coords->x2;

                lv_coord_t mask_ofs = (coords->x2 - rout) - (vdb->area.x1 + draw_area.x1);
                if(mask_ofs < 0) mask_ofs = 0;
                lv_blend_fill(clip, &fill_area2,
                        style->body.border.color, mask_buf + mask_ofs, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);

                fill_area.y1++;
                fill_area.y2++;
            }

            /*Draw the lower corner area corner area*/
            lv_coord_t lower_corner_end = coords->y2 - disp_area->y1 - corner_size;
            if(lower_corner_end <= upper_corner_end) lower_corner_end = upper_corner_end + 1;
            fill_area.y1 = disp_area->y1 + lower_corner_end;
            fill_area.y2 = fill_area.y1;
            for(h = lower_corner_end; h <= draw_area.y2; h++) {
                memset(mask_buf, LV_OPA_COVER, draw_area_w);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

                lv_area_t fill_area2;
                fill_area2.x1 = coords->x1;
                fill_area2.x2 = coords->x1 + rout;
                fill_area2.y1 = fill_area.y1;
                fill_area2.y2 = fill_area.y2;

                lv_blend_fill(clip, &fill_area2,
                        style->body.border.color, mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);


                if(fill_area2.y2 > coords->y2 - style->body.border.width) {
                    fill_area2.x1 = coords->x1 + rout + 1;
                    fill_area2.x2 = coords->x2 - rout - 1;

                    lv_blend_fill(clip, &fill_area2,
                            style->body.border.color, NULL, LV_MASK_RES_FULL_COVER, style->body.border.opa, LV_BLIT_MODE_NORMAL);
                }
                fill_area2.x1 = coords->x2 - rout;
                fill_area2.x2 = coords->x2;

                lv_coord_t mask_ofs = (coords->x2 - rout) - (vdb->area.x1 + draw_area.x1);
                if(mask_ofs < 0) mask_ofs = 0;
                lv_blend_fill(clip, &fill_area2,
                        style->body.border.color, mask_buf + mask_ofs, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);


                fill_area.y1++;
                fill_area.y2++;
            }

            /*Draw the left vertical border part*/
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x1 + border_width - 1;
            fill_area.y1 = coords->y1 + corner_size + 1;
            fill_area.y2 = coords->y2 - corner_size - 1;

            lv_blend_fill(clip, &fill_area,
                    style->body.border.color, NULL, LV_MASK_RES_FULL_COVER, style->body.border.opa, LV_BLIT_MODE_NORMAL);

            /*Draw the right vertical border*/
            fill_area.x1 = coords->x2 - border_width + 1;
            fill_area.x2 = coords->x2;

            lv_blend_fill(clip, &fill_area,
                    style->body.border.color, NULL, LV_MASK_RES_FULL_COVER, style->body.border.opa, LV_BLIT_MODE_NORMAL);
        }
        /*Process line by line if there is other mask too*/
        else {
            fill_area.x1 = coords->x1;
            fill_area.x2 = coords->x2;
            fill_area.y1 = disp_area->y1 + draw_area.y1;
            fill_area.y2 = fill_area.y1;
            for(h = draw_area.y1; h <= draw_area.y2; h++) {
                memset(mask_buf, LV_OPA_COVER, draw_area_w);
                mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

                lv_blend_fill( clip, &fill_area,
                        style->body.border.color, mask_buf, mask_res, style->body.border.opa, LV_BLIT_MODE_NORMAL);

                fill_area.y1++;
                fill_area.y2++;

            }
        }
        lv_mask_remove_id(mask_rsmall_id);
    }

    lv_mask_remove_id(mask_rout_id);
}



static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    if(style->body.shadow.width == 0) return;
//    lv_area_t c2;
//    c2.x1 = 20;
//    c2.x2 = 80;
//    c2.y1 = 20;
//    c2.y2 = 80;
//
//    coords = &c2;

    lv_opa_t opa = style->body.opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;
    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, coords, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

    /*Create a mask if there is a radius*/
    lv_opa_t mask_buf[LV_HOR_RES_MAX];

    /*Draw a radius into the shadow buffer*/
    int16_t mask_rout_id = LV_MASK_ID_INV;
    uint8_t other_mask_cnt = lv_mask_get_cnt();


    /*Get the real radius*/
    lv_coord_t rout = style->body.radius;
    lv_coord_t short_side = LV_MATH_MIN(lv_area_get_width(coords), lv_area_get_height(coords));
    if(rout > short_side >> 1) rout = short_side >> 1;


    lv_coord_t corner_size = style->body.shadow.width  + rout;


    lv_opa_t sh_buf[corner_size * corner_size];

    shadow_draw_corner_buf(sh_buf, style->body.shadow.width, rout);


    lv_area_t a;
    a.x1 = coords->x1;
    a.x2 = coords->x1 + corner_size-1;
    a.y1 = coords->y1;
    a.y2 = a.y1;

    lv_opa_t * sh_buf_tmp = sh_buf;

    lv_coord_t y;
    for(y = 0; y < corner_size; y++) {
        lv_blend_fill(clip, &a,
                LV_COLOR_BLACK, sh_buf_tmp, LV_MASK_RES_CHANGED, LV_OPA_COVER, LV_BLIT_MODE_NORMAL);
        a.y1++;
        a.y2++;
        sh_buf_tmp += corner_size;
    }



}

#define SHADOW_UPSACALE_SHIFT   0

static void shadow_draw_corner_buf(lv_opa_t * sh_buf, lv_coord_t sw, lv_coord_t r)
{
    lv_coord_t size = sw  + r;

    lv_area_t sh_area;
    sh_area.x1 = -100;
    sh_area.y1 = sw / 2 + 1;
    sh_area.x2 = sw / 2 + 1 + r - 1;  /*make the end far to not draw the other radius*/
    sh_area.y2 = 100;

    lv_mask_param_t mask_param;
    lv_mask_radius_init(&mask_param, &sh_area, r, false);
    int16_t mask_id = lv_mask_add(lv_mask_radius, &mask_param, NULL);

    lv_mask_res_t mask_res;
    lv_coord_t y;
    lv_opa_t mask_line[size];
    uint32_t sh_ups_buf[size*size];
    uint32_t * sh_ups_tmp_buf = sh_ups_buf;
    lv_coord_t s_left = sw >> 1;
    lv_coord_t s_right = (sw >> 1);
    if((sw & 1) == 0) s_left--;
    for(y = 0; y < size; y++) {
        memset(mask_line, 0xFF, size);
        mask_res = lv_mask_apply(mask_line, 0, y, size);
        if(mask_res == LV_MASK_RES_FULL_TRANSP) {
            memset(sh_ups_tmp_buf, 0x00, size * sizeof(sh_ups_buf[0]));
        } else {
            uint32_t i;
            sh_ups_tmp_buf[0] = (mask_line[0] << SHADOW_UPSACALE_SHIFT) / sw;
            for(i = 1; i < size; i++) {
                if(mask_line[i] == mask_line[i-1]) sh_ups_tmp_buf[i] = sh_ups_tmp_buf[i-1];
                else  sh_ups_tmp_buf[i] = (mask_line[i] << SHADOW_UPSACALE_SHIFT) / sw;
            }
        }

        sh_ups_tmp_buf += size;
    }

    lv_mask_remove_id(mask_id);

    /*Horizontal blur*/
    uint32_t sh_ups_hor_buf[size*size];
    sh_ups_tmp_buf = sh_ups_buf + (size*0);
    uint32_t * sh_ups_hor_buf_tmp = sh_ups_hor_buf + (size*0);

    lv_coord_t x;
    for(y = 0; y < size; y++) {
        int32_t v = sh_ups_tmp_buf[0] * sw;
        for(x = 0; x < size; x++) {
            sh_ups_hor_buf_tmp[x] = v;

            /*Forget the left pixel*/
            uint32_t left_val;
            if(x - s_left <= 0) left_val = sh_ups_tmp_buf[0];//25<< SHADOW_UPSACALE_SHIFT;
            else left_val = sh_ups_tmp_buf[x - s_left];
            v -= left_val;

            /*Add the right pixel*/
            uint32_t right_val = 0;
            if(x + s_right + 1 < size) right_val = sh_ups_tmp_buf[x + s_right + 1];
            v += right_val;
        }
        sh_ups_tmp_buf += size;
        sh_ups_hor_buf_tmp += size;
    }
//
//
//    uint32_t i;
//    for(i = 0; i < size * size; i++) {
//        sh_buf[i] = (sh_ups_hor_buf[i] >> SHADOW_UPSACALE_SHIFT) ;
//    }
//
//    return;



    /*Vertical blur*/
    uint32_t i;
   sh_ups_hor_buf[0] = sh_ups_hor_buf[0] / sw;
   for(i = 1; i < size * size; i++) {
       if(sh_ups_hor_buf[i] == sh_ups_hor_buf[i-1]) sh_ups_hor_buf[i] = sh_ups_hor_buf[i-1];
       else  sh_ups_hor_buf[i] = sh_ups_hor_buf[i] / sw;
   }

    for(x = 0; x < size; x++) {
        sh_ups_hor_buf_tmp = &sh_ups_hor_buf[x];
        lv_opa_t * sh_buf_tmp = &sh_buf[x];
        int32_t v = sh_ups_hor_buf_tmp[0] * sw;
        for(y = 0; y < size; y++, sh_ups_hor_buf_tmp += size, sh_buf_tmp += size) {
            sh_buf_tmp[0] = v >> SHADOW_UPSACALE_SHIFT;

            /*Forget the top pixel*/
            uint32_t top_val;
            if(y - s_left <= 0) top_val = sh_ups_hor_buf_tmp[0];//25<< SHADOW_UPSACALE_SHIFT;
            else top_val = sh_ups_hor_buf[(y - s_left) * size + x];
            v -= top_val;

            /*Add the bottom pixel*/
            uint32_t bottom_val;
            if(y + s_right + 1 < size) bottom_val = sh_ups_hor_buf[(y + s_right + 1) * size + x];
            else bottom_val = sh_ups_hor_buf[(size - 1) * size + x];
            v += bottom_val;
        }
    }


}









