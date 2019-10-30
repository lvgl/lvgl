/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rect.h"
#include "lv_draw_blend.h"
#include "lv_draw_mask.h"
#include "../lv_misc/lv_circ.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/
#define SHADOW_UPSACALE_SHIFT   6
#define SHADOW_ENHANCE          1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_bg(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale);
static void draw_border(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale);
static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale);
static lv_color_t grad_get(const lv_style_t * style, lv_coord_t s, lv_coord_t i);
static void shadow_draw_corner_buf(const lv_area_t * coords,  lv_opa_t * sh_buf, lv_coord_t s, lv_coord_t r);
static void shadow_blur_corner(lv_coord_t size, lv_coord_t sw, lv_opa_t * res_buf, uint16_t * sh_ups_buf);

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

    draw_shadow(coords, clip, style, opa_scale);
    draw_bg(coords, clip, style, opa_scale);
    draw_border(coords, clip, style, opa_scale);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_area_t coords_bg;
    lv_area_copy(&coords_bg, coords);

    /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
    if(style->body.border.width > 1 && style->body.border.opa >= LV_OPA_MAX && style->body.radius != 0) {
        coords_bg.x1++;
        coords_bg.y1++;
        coords_bg.x2--;
        coords_bg.y2--;
    }

    lv_opa_t opa = style->body.opa;
    if(opa_scale != LV_OPA_COVER) opa = (opa * opa_scale) >> 8;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, &coords_bg, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

    /*Create a mask if there is a radius*/
    lv_opa_t * mask_buf = lv_draw_buf_get(draw_area_w);

    bool simple_mode = true;
    if(lv_draw_mask_get_cnt()!= 0) simple_mode = false;
    else if(style->body.border.part != LV_BORDER_PART_FULL) simple_mode = false;
    else if(style->body.grad_dir == LV_GRAD_DIR_HOR) simple_mode = false;

    int16_t mask_rout_id = LV_MASK_ID_INV;

    lv_coord_t coords_w = lv_area_get_width(&coords_bg);
    lv_coord_t coords_h = lv_area_get_height(&coords_bg);

    /*Get the real radius*/
    lv_coord_t rout = style->body.radius;
    lv_coord_t short_side = LV_MATH_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Most simple case: just a plain rectangle*/
    if(simple_mode && rout == 0 && style->body.main_color.full == style->body.grad_color.full) {
        lv_blend_fill(clip, &coords_bg,
                style->body.main_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa,
                style->body.blend_mode);
    }
    /*More complex case: there is a radius, gradient or mask.*/
    else {
        lv_draw_mask_param_t mask_rout_param;
        if(rout > 0) {
            lv_draw_mask_radius_init(&mask_rout_param, &coords_bg, rout, false);
            mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        }

        if(opa >= LV_OPA_MIN) {
            /*Draw the background line by line*/
            lv_coord_t h;
            lv_draw_mask_res_t mask_res = LV_DRAW_MASK_RES_FULL_COVER;
            lv_color_t grad_color = style->body.main_color;


            lv_color_t * grad_map = NULL;
            /*In case of horizontal gradient pre-compute a line with a gradient*/
            if(style->body.grad_dir == LV_GRAD_DIR_HOR && style->body.main_color.full != style->body.grad_color.full) {
                grad_map = lv_draw_buf_get(coords_w * sizeof(lv_color_t));

                lv_coord_t i;
                for(i = 0; i < coords_w; i++) {
                    grad_map[i] = grad_get(style, coords_w, i);
                }
            }

            lv_area_t fill_area;
            fill_area.x1 = coords_bg.x1;
            fill_area.x2 = coords_bg.x2;
            fill_area.y1 = disp_area->y1 + draw_area.y1;
            fill_area.y2 = fill_area.y1;
            for(h = draw_area.y1; h <= draw_area.y2; h++) {
                lv_coord_t y = h + vdb->area.y1;

                /*In not corner areas apply the mask only if required*/
                if(y > coords_bg.y1 + rout + 1 &&
                        y < coords_bg.y2 - rout - 1) {
                    mask_res = LV_DRAW_MASK_RES_FULL_COVER;
                    if(simple_mode == false) {
                        memset(mask_buf, LV_OPA_COVER, draw_area_w);
                        mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                    }
                }
                /*In corner areas apply the mask anyway*/
                else {
                    memset(mask_buf, LV_OPA_COVER, draw_area_w);
                    mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                }

                /*Get the current line color*/
                if(style->body.grad_dir == LV_GRAD_DIR_VER && style->body.main_color.full != style->body.grad_color.full) {
                    grad_color = grad_get(style, lv_area_get_height(&coords_bg), y - coords_bg.y1);
                }

                /* If there is not other mask and drawing the corner area split the drawing to corner and middle areas
                 * because it the middle mask shuldn't be taken into account (therefore its faster)*/
                if(simple_mode &&
                        (y < coords_bg.y1 + rout + 1 ||
                                y > coords_bg.y2 - rout - 1)) {

                    /*Left part*/
                    lv_area_t fill_area2;
                    fill_area2.x1 = coords_bg.x1;
                    fill_area2.x2 = coords_bg.x1 + rout - 1;
                    fill_area2.y1 = fill_area.y1;
                    fill_area2.y2 = fill_area.y2;

                    lv_blend_fill(clip, &fill_area2,
                            grad_color, mask_buf, mask_res, opa, style->body.blend_mode);


                    /*Central part*/
                    fill_area2.x1 = coords_bg.x1 + rout;
                    fill_area2.x2 = coords_bg.x2 - rout;

                    lv_blend_fill(clip, &fill_area2,
                            grad_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, style->body.blend_mode);

                    fill_area2.x1 = coords_bg.x2 - rout + 1;
                    fill_area2.x2 = coords_bg.x2;

                    lv_coord_t mask_ofs = (coords_bg.x2 - rout + 1) - (vdb->area.x1 + draw_area.x1);
                    if(mask_ofs < 0) mask_ofs = 0;
                    lv_blend_fill(clip, &fill_area2,
                            grad_color, mask_buf + mask_ofs, mask_res, opa, style->body.blend_mode);
                } else {
                    if(grad_map == NULL) {
                        lv_blend_fill(clip, &fill_area,
                                grad_color,mask_buf, mask_res, opa, style->body.blend_mode);
                    } else {
                        lv_blend_map(clip, &fill_area, grad_map, mask_buf, mask_res, opa, style->body.blend_mode);
                    }

                }
                fill_area.y1++;
                fill_area.y2++;
            }

            if(grad_map) lv_draw_buf_release(grad_map);
        }

        lv_draw_mask_remove_id(mask_rout_id);
    }

    lv_draw_buf_release(mask_buf);

}

static void draw_border(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_coord_t border_width = style->body.border.width;
    if(border_width == 0) return;

    lv_opa_t opa = style->body.border.opa;
    if(opa_scale != LV_OPA_COVER) opa = (opa * opa_scale) >> 8;

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

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

    /*Create a mask if there is a radius*/
    lv_opa_t * mask_buf = lv_draw_buf_get(draw_area_w);

    bool simple_mode = true;
    if(lv_draw_mask_get_cnt()!= 0) simple_mode = false;
    else if(style->body.border.part != LV_BORDER_PART_FULL) simple_mode = false;
    else if(style->body.grad_dir == LV_GRAD_DIR_HOR) simple_mode = false;

    int16_t mask_rout_id = LV_MASK_ID_INV;

    lv_coord_t coords_w = lv_area_get_width(coords);
    lv_coord_t coords_h = lv_area_get_height(coords);

    /*Get the real radius*/
    lv_coord_t rout = style->body.radius;
    lv_coord_t short_side = LV_MATH_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Get the outer area*/
    lv_draw_mask_param_t mask_rout_param;
    if(rout > 0) {
        lv_draw_mask_radius_init(&mask_rout_param, coords, rout, false);
        mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
    }

    /*Move the vdb_buf_tmp to the first row*/
    lv_draw_mask_param_t mask_rin_param;

    /*Get the inner radius*/
    lv_coord_t rin = rout - border_width;
    if(rin < 0) rin = 0;

    /*Get the inner area*/
    lv_area_t area_small;
    lv_area_copy(&area_small, coords);
    area_small.x1 += ((style->body.border.part & LV_BORDER_PART_LEFT) ? border_width : -border_width);
    area_small.x2 -= ((style->body.border.part & LV_BORDER_PART_RIGHT) ? border_width : -border_width);
    area_small.y1 += ((style->body.border.part & LV_BORDER_PART_TOP) ? border_width : -border_width);
    area_small.y2 -= ((style->body.border.part & LV_BORDER_PART_BOTTOM) ? border_width : -border_width);

    /*Create inner the mask*/
    lv_draw_mask_radius_init(&mask_rin_param, &area_small, rout - border_width, true);
    int16_t mask_rin_id = lv_draw_mask_add(&mask_rin_param, NULL);

    lv_coord_t corner_size = LV_MATH_MAX(rout, border_width - 1);

    lv_coord_t h;
    lv_draw_mask_res_t mask_res;
    lv_area_t fill_area;

    /*Apply some optimization if there is no other mask*/
    if(simple_mode) {
        /*Draw the upper corner area*/
        lv_coord_t upper_corner_end = coords->y1 - disp_area->y1 + corner_size;

        fill_area.x1 = coords->x1;
        fill_area.x2 = coords->x2;
        fill_area.y1 = disp_area->y1 + draw_area.y1;
        fill_area.y2 = fill_area.y1;
        for(h = draw_area.y1; h <= upper_corner_end; h++) {
            memset(mask_buf, LV_OPA_COVER, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

            lv_area_t fill_area2;
            fill_area2.y1 = fill_area.y1;
            fill_area2.y2 = fill_area.y2;

            fill_area2.x1 = coords->x1;
            fill_area2.x2 = coords->x1 + rout - 1;

            lv_blend_fill(clip, &fill_area2,
                    style->body.border.color, mask_buf, mask_res, opa, style->body.border.blend_mode);

            if(fill_area2.y2 < coords->y1 + style->body.border.width) {
                fill_area2.x1 = coords->x1 + rout;
                fill_area2.x2 = coords->x2 - rout;

                lv_blend_fill(clip, &fill_area2,
                        style->body.border.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, style->body.border.blend_mode);
            }

            fill_area2.x1 = coords->x2 - rout + 1;
            fill_area2.x2 = coords->x2;

            lv_coord_t mask_ofs = (coords->x2 - rout + 1) - (vdb->area.x1 + draw_area.x1);
            if(mask_ofs < 0) mask_ofs = 0;
            lv_blend_fill(clip, &fill_area2,
                    style->body.border.color, mask_buf + mask_ofs, mask_res, opa, style->body.border.blend_mode);

            fill_area.y1++;
            fill_area.y2++;
        }

        /*Draw the lower corner area corner area*/
        if(style->body.border.part & LV_BORDER_PART_BOTTOM) {
            lv_coord_t lower_corner_end = coords->y2 - disp_area->y1 - corner_size;
            if(lower_corner_end <= upper_corner_end) lower_corner_end = upper_corner_end + 1;
            fill_area.y1 = disp_area->y1 + lower_corner_end;
            fill_area.y2 = fill_area.y1;
            for(h = lower_corner_end; h <= draw_area.y2; h++) {
                memset(mask_buf, LV_OPA_COVER, draw_area_w);
                mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

                lv_area_t fill_area2;
                fill_area2.x1 = coords->x1;
                fill_area2.x2 = coords->x1 + rout - 1;
                fill_area2.y1 = fill_area.y1;
                fill_area2.y2 = fill_area.y2;

                lv_blend_fill(clip, &fill_area2,
                        style->body.border.color, mask_buf, mask_res, opa, style->body.border.blend_mode);


                if(fill_area2.y2 > coords->y2 - style->body.border.width ) {
                    fill_area2.x1 = coords->x1 + rout;
                    fill_area2.x2 = coords->x2 - rout;

                    lv_blend_fill(clip, &fill_area2,
                            style->body.border.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, style->body.border.blend_mode);
                }
                fill_area2.x1 = coords->x2 - rout + 1;
                fill_area2.x2 = coords->x2;

                lv_coord_t mask_ofs = (coords->x2 - rout + 1) - (vdb->area.x1 + draw_area.x1);
                if(mask_ofs < 0) mask_ofs = 0;
                lv_blend_fill(clip, &fill_area2,
                        style->body.border.color, mask_buf + mask_ofs, mask_res, opa, style->body.border.blend_mode);


                fill_area.y1++;
                fill_area.y2++;
            }
        }

        /*Draw the left vertical border part*/
        fill_area.y1 = coords->y1 + corner_size + 1;
        fill_area.y2 = coords->y2 - corner_size - 1;

        fill_area.x1 = coords->x1;
        fill_area.x2 = coords->x1 + border_width - 1;
        lv_blend_fill(clip, &fill_area,
                style->body.border.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, style->body.border.blend_mode);

        /*Draw the right vertical border*/
        fill_area.x1 = coords->x2 - border_width + 1;
        fill_area.x2 = coords->x2;

        lv_blend_fill(clip, &fill_area,
                style->body.border.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, style->body.border.blend_mode);
    }
    /*Process line by line if there is other mask too*/
    else {
        fill_area.x1 = coords->x1;
        fill_area.x2 = coords->x2;
        fill_area.y1 = disp_area->y1 + draw_area.y1;
        fill_area.y2 = fill_area.y1;
        for(h = draw_area.y1; h <= draw_area.y2; h++) {
            memset(mask_buf, LV_OPA_COVER, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

            lv_blend_fill( clip, &fill_area,
                    style->body.border.color, mask_buf, mask_res, opa, style->body.border.blend_mode);

            fill_area.y1++;
            fill_area.y2++;

        }
    }
    lv_draw_mask_remove_id(mask_rin_id);
    lv_draw_mask_remove_id(mask_rout_id);
    lv_draw_buf_release(mask_buf);
}

static lv_color_t grad_get(const lv_style_t * style, lv_coord_t s, lv_coord_t i)
{
    lv_coord_t min = (style->body.main_color_stop * s) >> 8;
    if(i <= min) return style->body.main_color;

    lv_coord_t max = (style->body.grad_color_stop * s) >> 8;
    if(i >= max) return style->body.grad_color;

    lv_coord_t d = style->body.grad_color_stop - style->body.main_color_stop;
    d = (s * d) >> 8;
    i -= min;
    lv_opa_t mix = (i * 255) / d;
    return lv_color_mix(style->body.grad_color, style->body.main_color, mix);
}

static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{
    /*Check whether the shadow is visible*/
    if(style->body.shadow.width == 0) return;

    if(style->body.shadow.width == 1 && style->body.shadow.offset.x == 0 &&
            style->body.shadow.offset.y == 0 && style->body.shadow.spread <= 0) {
        return;
    }

    lv_coord_t sw = style->body.shadow.width;

    lv_area_t sh_rect_area;
    sh_rect_area.x1 = coords->x1  + style->body.shadow.offset.x - style->body.shadow.spread;
    sh_rect_area.x2 = coords->x2  + style->body.shadow.offset.x + style->body.shadow.spread;
    sh_rect_area.y1 = coords->y1  + style->body.shadow.offset.y - style->body.shadow.spread;
    sh_rect_area.y2 = coords->y2  + style->body.shadow.offset.y + style->body.shadow.spread;

    lv_area_t sh_area;
    sh_area.x1 = sh_rect_area.x1 - sw / 2 - 1;
    sh_area.x2 = sh_rect_area.x2 + sw / 2 + 1;
    sh_area.y1 = sh_rect_area.y1 - sw / 2 - 1;
    sh_area.y2 = sh_rect_area.y2 + sw / 2 + 1;

    lv_opa_t opa = style->body.shadow.opa;

    if(opa_scale != LV_OPA_COVER) opa = (opa * opa_scale) >> 8;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, &sh_area, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Consider 1 px smaller bg to be sure the edge will be covered by the shadow*/
    lv_area_t bg_coords;
    lv_area_copy(&bg_coords, coords);
    bg_coords.x1 += 1;
    bg_coords.y1 += 1;
    bg_coords.x2 -= 1;
    bg_coords.y2 -= 1;

    /*Get the real radius*/
    lv_coord_t r_bg = style->body.radius;
    lv_coord_t short_side = LV_MATH_MIN(lv_area_get_width(&bg_coords), lv_area_get_height(&bg_coords));
    if(r_bg > short_side >> 1) r_bg = short_side >> 1;

    lv_coord_t r_sh = style->body.radius;
    short_side = LV_MATH_MIN(lv_area_get_width(&sh_rect_area), lv_area_get_height(&sh_rect_area));
    if(r_sh > short_side >> 1) r_sh = short_side >> 1;


    lv_coord_t corner_size = sw  + r_sh;

    lv_opa_t * sh_buf = lv_draw_buf_get(corner_size * corner_size);
    shadow_draw_corner_buf(&sh_rect_area, sh_buf, style->body.shadow.width, r_sh);

    bool simple_mode = true;
    if(lv_draw_mask_get_cnt() > 0) simple_mode = false;
    else if(style->body.shadow.offset.x != 0 || style->body.shadow.offset.y != 0) simple_mode = false;
    else if(style->body.shadow.spread != 0) simple_mode = false;

    lv_coord_t y_max;

    /*Create a mask*/
    lv_draw_mask_res_t mask_res;
    lv_opa_t * mask_buf = lv_draw_buf_get(lv_area_get_width(&sh_rect_area));

    lv_draw_mask_param_t mask_rout_param;
    lv_draw_mask_radius_init(&mask_rout_param, &bg_coords, r_bg, true);

    /*Draw a radius into the shadow buffer*/
    int16_t mask_rout_id = LV_MASK_ID_INV;

    mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);

    lv_area_t a;

    /*Draw the top right corner*/
    a.x2 = sh_area.x2;
    a.x1 = a.x2 - corner_size + 1;
    a.y1 = sh_area.y1;
    a.y2 = a.y1;

    lv_coord_t first_px;
    first_px = 0;
    if(disp_area->x1 > a.x1) {
        first_px = disp_area->x1 - a.x1;
    }

    lv_coord_t hor_mid_dist = (sh_area.x1 + lv_area_get_width(&sh_area) / 2) - (a.x1 + first_px);
    if(hor_mid_dist > 0) {
        first_px += hor_mid_dist;
    }
    a.x1 += first_px;


    lv_coord_t ver_mid_dist = (a.y1 + corner_size) - (sh_area.y1 + lv_area_get_height(&sh_area) / 2);
    lv_coord_t ver_mid_corr = 0;
    if(ver_mid_dist <= 0) ver_mid_dist = 0;
    else {
        if(lv_area_get_height(&sh_area) & 0x1) ver_mid_corr = 1;
    }
    lv_opa_t * sh_buf_tmp = sh_buf;

    lv_coord_t y;
    for(y = 0; y < corner_size - ver_mid_dist + ver_mid_corr; y++) {
        memcpy(mask_buf, sh_buf_tmp, corner_size);
        mask_res = lv_draw_mask_apply(mask_buf + first_px, a.x1, a.y1, lv_area_get_width(&a));
        if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

        lv_blend_fill(clip, &a,
                style->body.shadow.color, mask_buf + first_px, mask_res, opa, style->body.shadow.blend_mode);
        a.y1++;
        a.y2++;
        sh_buf_tmp += corner_size;
    }

    /*Draw the bottom right corner*/
    a.y1 = sh_area.y2;
    a.y2 = a.y1;

    sh_buf_tmp = sh_buf ;

    for(y = 0; y < corner_size - ver_mid_dist; y++) {
        memcpy(mask_buf, sh_buf_tmp, corner_size);
        mask_res = lv_draw_mask_apply(mask_buf + first_px, a.x1, a.y1, lv_area_get_width(&a));
        if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

        lv_blend_fill(clip, &a,
                style->body.shadow.color, mask_buf + first_px, mask_res, opa, style->body.shadow.blend_mode);
        a.y1--;
        a.y2--;
        sh_buf_tmp += corner_size;
    }

    /*Fill the right side*/
    a.y1 = sh_area.y1 + corner_size;
    a.y2 = a.y1;
    sh_buf_tmp = sh_buf + corner_size * (corner_size - 1);

    lv_coord_t x;

    if(simple_mode) {
        /*Draw vertical lines*/
        lv_area_t va;
        va.x1 = a.x1;
        va.x2 = a.x1;
        va.y1 = sh_area.y1 + corner_size;
        va.y2 = sh_area.y2 - corner_size;

        if(va.y1 <= va.y2) {
            for(x = a.x1; x < a.x2; x++) {
                if(x > coords->x2) {
                    lv_opa_t opa_tmp = sh_buf_tmp[x - a.x1 + first_px];
                    if(opa_tmp != LV_OPA_COVER || opa != LV_OPA_COVER) opa_tmp = (opa * opa_tmp) >> 8;
                    lv_blend_fill(clip, &va,
                            style->body.shadow.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa_tmp, style->body.shadow.blend_mode);
                }
                va.x1++;
                va.x2++;
            }
        }
    }
    else {
        for(y = corner_size; y < lv_area_get_height(&sh_area) - corner_size; y++) {
            memcpy(mask_buf, sh_buf_tmp, corner_size);
            mask_res = lv_draw_mask_apply(mask_buf + first_px, a.x1, a.y1, lv_area_get_width(&a));
            if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

            lv_blend_fill(clip, &a,
                    style->body.shadow.color, mask_buf+first_px, mask_res, opa, style->body.shadow.blend_mode);
            a.y1++;
            a.y2++;
        }
    }

    /*Invert the shadow corner buffer and draw the corners on the left*/
    sh_buf_tmp = sh_buf ;
    for(y = 0; y < corner_size; y++) {
        for(x = 0; x < corner_size / 2; x++) {
            lv_opa_t tmp = sh_buf_tmp[x];
            sh_buf_tmp[x] = sh_buf_tmp[corner_size - x - 1];
            sh_buf_tmp[corner_size - x - 1] = tmp;
        }
        sh_buf_tmp += corner_size;
    }

    /*Draw the top left corner*/
    a.x1 = sh_area.x1;
    a.x2 = a.x1 + corner_size - 1;
    a.y1 = sh_area.y1;
    a.y2 = a.y1;

    if(a.x2 > sh_area.x1 + lv_area_get_width(&sh_area)/2 - 1) {
        a.x2 = sh_area.x1 + lv_area_get_width(&sh_area)/2 -1 ;
    }

    first_px = 0;
    if(disp_area->x1 >= a.x1) {
        first_px = disp_area->x1 - a.x1;
        a.x1 += first_px;
    }

    sh_buf_tmp = sh_buf ;
    for(y = 0; y < corner_size - ver_mid_dist + ver_mid_corr; y++) {
        memcpy(mask_buf, sh_buf_tmp, corner_size);
        mask_res = lv_draw_mask_apply(mask_buf + first_px, a.x1, a.y1, lv_area_get_width(&a));
        if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

        lv_blend_fill(clip, &a,
                style->body.shadow.color, mask_buf + first_px, mask_res, opa, style->body.shadow.blend_mode);
        a.y1++;
        a.y2++;
        sh_buf_tmp += corner_size;
    }

    /*Draw the bottom left corner*/
    a.y1 = sh_area.y2;
    a.y2 = a.y1;

    sh_buf_tmp = sh_buf ;

    for(y = 0; y < corner_size - ver_mid_dist; y++) {
        memcpy(mask_buf, sh_buf_tmp, corner_size);
        mask_res = lv_draw_mask_apply(mask_buf + first_px, a.x1, a.y1, lv_area_get_width(&a));
        if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

        lv_blend_fill(clip, &a,
                style->body.shadow.color, mask_buf + first_px, mask_res, opa, style->body.shadow.blend_mode);
        a.y1--;
        a.y2--;
        sh_buf_tmp += corner_size;
    }

    /*Fill the left side*/
    a.y1 = sh_area.y1+corner_size;
    a.y2 = a.y1;

    sh_buf_tmp = sh_buf + corner_size * (corner_size - 1);

    if(simple_mode) {
        /*Draw vertical lines*/
        lv_area_t va;
        va.x1 = a.x1;
        va.x2 = a.x1;
        va.y1 = sh_area.y1 + corner_size;
        va.y2 = sh_area.y2 - corner_size;

        if(va.y1 <= va.y2) {
            for(x = a.x1; x < coords->x1; x++) {
                lv_opa_t opa_tmp = sh_buf_tmp[x - a.x1 + first_px];
                if(opa_tmp != LV_OPA_COVER || opa != LV_OPA_COVER) opa_tmp = (opa * opa_tmp) >> 8;
                lv_blend_fill(clip, &va,
                        style->body.shadow.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa_tmp, style->body.shadow.blend_mode);
                va.x1++;
                va.x2++;
            }
        }
    }
    else {
        for(y = corner_size; y < lv_area_get_height(&sh_area) - corner_size; y++) {
            memcpy(mask_buf, sh_buf_tmp, corner_size);
            mask_res = lv_draw_mask_apply(mask_buf + first_px, a.x1, a.y1, lv_area_get_width(&a));
            if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

            lv_blend_fill(clip, &a,
                    style->body.shadow.color, mask_buf + first_px, mask_res, opa, style->body.shadow.blend_mode);
            a.y1++;
            a.y2++;
        }
    }

    /*Fill the top side*/

    a.x1 = sh_area.x1 + corner_size;
    a.x2 = sh_area.x2 - corner_size;
    a.y1 = sh_area.y1;
    a.y2 = a.y1;


    first_px = 0;
    if(disp_area->x1 > a.x1) {
        first_px = disp_area->x1 - a.x1;
        a.x1 += first_px;
    }

    if(a.x1 <= a.x2) {

        sh_buf_tmp = sh_buf + corner_size - 1;

        y_max = corner_size - ver_mid_dist;
        if(simple_mode) {
            y_max = sw / 2 + 1;
            if(y_max > corner_size - ver_mid_dist) y_max = corner_size - ver_mid_dist;
        }

        for(y = 0; y < y_max; y++) {
            if(simple_mode == false) {
                memset(mask_buf, sh_buf_tmp[0], lv_area_get_width(&a));
                mask_res = lv_draw_mask_apply(mask_buf, a.x1, a.y1, lv_area_get_width(&a));
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

                lv_blend_fill(clip, &a,
                        style->body.shadow.color, mask_buf, mask_res, opa, style->body.shadow.blend_mode);
            } else {

                lv_opa_t opa_tmp = sh_buf_tmp[0];
                if(opa_tmp != LV_OPA_COVER || opa != LV_OPA_COVER) opa_tmp = (opa * opa_tmp) >> 8;
                lv_blend_fill(clip, &a,
                        style->body.shadow.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa_tmp, style->body.shadow.blend_mode);
            }

            a.y1++;
            a.y2++;
            sh_buf_tmp += corner_size;
        }

        /*Fill the bottom side*/
        lv_coord_t y_min = simple_mode ? (corner_size - (sh_area.y2 - coords->y2)) : ver_mid_dist;
        if(y_min < 0) y_min = 0;
        sh_buf_tmp = sh_buf + corner_size * (corner_size - y_min - 1 ) + corner_size - 1;

        a.y1 = sh_area.y2 - corner_size + 1 + y_min;
        a.y2 = a.y1;

        for(y = y_min; y < corner_size; y++) {
            if(simple_mode == false) {
                memset(mask_buf, sh_buf_tmp[0], lv_area_get_width(&a));
                mask_res = lv_draw_mask_apply(mask_buf, a.x1, a.y1, lv_area_get_width(&a));
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                lv_blend_fill(clip, &a,
                        style->body.shadow.color, mask_buf, mask_res, opa, LV_BLEND_MODE_NORMAL);
            } else {
                lv_opa_t opa_tmp = sh_buf_tmp[0];
                if(opa_tmp != LV_OPA_COVER || opa != LV_OPA_COVER) opa_tmp = (opa * opa_tmp) >> 8;
                lv_blend_fill(clip, &a,
                        style->body.shadow.color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa_tmp, style->body.shadow.blend_mode);
            }

            a.y1++;
            a.y2++;
            sh_buf_tmp -= corner_size;
        }
    }

    /*Finally fill the middle area*/
    if(simple_mode == false) {
        a.y1 = sh_area.y1 + corner_size;
        a.y2 = a.y1;
        if(a.x1 <= a.x2) {
            for(y = 0; y < lv_area_get_height(&sh_area) - corner_size * 2; y++) {
                memset(mask_buf, 0xFF, lv_area_get_width(&a));
                mask_res = lv_draw_mask_apply(mask_buf, a.x1, a.y1, lv_area_get_width(&a));
                lv_blend_fill(clip, &a,
                        style->body.shadow.color, mask_buf, mask_res, opa, style->body.shadow.blend_mode);

                a.y1++;
                a.y2++;
            }
        }
    }

    lv_draw_mask_remove_id(mask_rout_id);
    lv_draw_buf_release(mask_buf);
    lv_draw_buf_release(sh_buf);
}

static void shadow_draw_corner_buf(const lv_area_t * coords, lv_opa_t * sh_buf, lv_coord_t sw, lv_coord_t r)
{
    lv_coord_t sw_ori = sw;
    lv_coord_t size = sw_ori  + r;

    lv_area_t sh_area;
    lv_area_copy(&sh_area, coords);
    sh_area.x2 = sw / 2 + r -1  - (sw & 1 ? 0 : 1);
    sh_area.y1 = sw / 2 + 1;

    sh_area.x1 = sh_area.x2 - lv_area_get_width(coords);
    sh_area.y2 = sh_area.y1 + lv_area_get_height(coords);

    lv_draw_mask_param_t mask_param;
    lv_draw_mask_radius_init(&mask_param, &sh_area, r, false);

#if SHADOW_ENHANCE
    /*Set half shadow width width because blur will be repeated*/
    if(sw_ori == 1) sw = 1;
    else if(sw_ori == 2) sw = 2;
    else if(sw_ori == 3) sw = 2;
    else sw = sw_ori >> 1;
#endif

    lv_draw_mask_res_t mask_res;
    lv_coord_t y;
    lv_opa_t * mask_line = lv_draw_buf_get(size);
    uint16_t * sh_ups_buf = lv_draw_buf_get(size * size * sizeof(uint16_t));
    uint16_t * sh_ups_tmp_buf = sh_ups_buf;
    for(y = 0; y < size; y++) {
        memset(mask_line, 0xFF, size);
        mask_res = mask_param.radius.cb(mask_line, 0, y, size, &mask_param);
        if(mask_res == LV_DRAW_MASK_RES_FULL_TRANSP) {
            memset(sh_ups_tmp_buf, 0x00, size * sizeof(sh_ups_buf[0]));
        } else {
            lv_coord_t i;
            sh_ups_tmp_buf[0] = (mask_line[0] << SHADOW_UPSACALE_SHIFT) / sw;
            for(i = 1; i < size; i++) {
                if(mask_line[i] == mask_line[i-1]) sh_ups_tmp_buf[i] = sh_ups_tmp_buf[i-1];
                else  sh_ups_tmp_buf[i] = (mask_line[i] << SHADOW_UPSACALE_SHIFT) / sw;
            }
        }

        sh_ups_tmp_buf += size;
    }
    lv_draw_buf_release(mask_line);

    //        uint32_t k;
    //        for(k = 0; k < size * size; k++) {
    //            sh_buf[k] = (sh_ups_buf[k] * sw)  >> SHADOW_UPSACALE_SHIFT ;
    //        }
    //        return;

    if(sw == 1) {
        lv_coord_t i;
        for(i = 0; i < size * size; i++) {
            sh_buf[i] = (sh_ups_buf[i] >> SHADOW_UPSACALE_SHIFT);
        }
        lv_draw_buf_release(sh_ups_buf);
        return;
    }

    shadow_blur_corner(size, sw, sh_buf, sh_ups_buf);

#if SHADOW_ENHANCE
    sw = sw_ori - sw;
    if(sw <= 1) {
        lv_draw_buf_release(sh_ups_buf);
        return;
    }

    uint32_t i;
    sh_ups_buf[0] = (sh_buf[0] << SHADOW_UPSACALE_SHIFT) / sw;
    for(i = 1; i < (uint32_t) size * size; i++) {
        if(sh_buf[i] == sh_buf[i-1]) sh_ups_buf[i] = sh_ups_buf[i-1];
        else  sh_ups_buf[i] = (sh_buf[i] << SHADOW_UPSACALE_SHIFT) / sw;
    }

    shadow_blur_corner(size, sw, sh_buf, sh_ups_buf);
#endif

    lv_draw_buf_release(sh_ups_buf);

}

static void shadow_blur_corner(lv_coord_t size, lv_coord_t sw, lv_opa_t * res_buf, uint16_t * sh_ups_buf)
{
    lv_coord_t s_left = sw >> 1;
    lv_coord_t s_right = (sw >> 1);
    if((sw & 1) == 0) s_left--;

    /*Horizontal blur*/
    uint16_t * sh_ups_hor_buf = lv_draw_buf_get(size * size * sizeof(uint16_t));
    uint16_t * sh_ups_hor_buf_tmp;

    lv_coord_t x;
    lv_coord_t y;

    uint16_t * sh_ups_tmp_buf = sh_ups_buf;
    sh_ups_hor_buf_tmp = sh_ups_hor_buf;

    for(y = 0; y < size; y++) {
        int32_t v = sh_ups_tmp_buf[size-1] * sw;
        for(x = size - 1; x >=0; x--) {
            sh_ups_hor_buf_tmp[x] = v;

            /*Forget the right pixel*/
            uint32_t right_val = 0;
            if(x + s_right < size) right_val = sh_ups_tmp_buf[x + s_right];
            v -= right_val;

            /*Add the left pixel*/
            uint32_t left_val;
            if(x - s_left - 1 < 0) left_val = sh_ups_tmp_buf[0];
            else left_val = sh_ups_tmp_buf[x - s_left - 1];
            v += left_val;

        }
        sh_ups_tmp_buf += size;
        sh_ups_hor_buf_tmp += size;
    }

    /*Vertical blur*/
    uint32_t i;
    sh_ups_hor_buf[0] = sh_ups_hor_buf[0] / sw;
    for(i = 1; i < (uint32_t)size * size; i++) {
        if(sh_ups_hor_buf[i] == sh_ups_hor_buf[i-1]) sh_ups_hor_buf[i] = sh_ups_hor_buf[i-1];
        else  sh_ups_hor_buf[i] = sh_ups_hor_buf[i] / sw;
    }



    for(x = 0; x < size; x++) {
        sh_ups_hor_buf_tmp = &sh_ups_hor_buf[x];
        lv_opa_t * sh_buf_tmp = &res_buf[x];
        int32_t v = sh_ups_hor_buf_tmp[0] * sw;
        for(y = 0; y < size ; y++, sh_ups_hor_buf_tmp += size, sh_buf_tmp += size) {
            sh_buf_tmp[0] = v < 0 ? 0 : (v >> SHADOW_UPSACALE_SHIFT);

            /*Forget the top pixel*/
            uint32_t top_val;
            if(y - s_right <= 0) top_val = sh_ups_hor_buf_tmp[0];
            else top_val = sh_ups_hor_buf[(y - s_right) * size + x];
            v -= top_val;

            /*Add the bottom pixel*/
            uint32_t bottom_val;
            if(y + s_left + 1 < size) bottom_val = sh_ups_hor_buf[(y + s_left + 1) * size + x];
            else bottom_val = sh_ups_hor_buf[(size - 1) * size + x];
            v += bottom_val;
        }
    }

    lv_draw_buf_release(sh_ups_hor_buf);
}

