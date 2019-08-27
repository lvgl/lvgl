/**
 * @file lv_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_blend.h"
#include "../lv_misc/lv_math.h"
#include "../lv_hal/lv_hal_disp.h"
#include "../lv_core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/
#define FILL_DIRECT_LEN     32
#define FILL_DIRECT_MASK    0x1F

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


/**
 *
 * @param disp_area
 * @param clip_area already truncated to disp_arae
 * @param fill_area
 * @param disp_buf
 * @param cf
 * @param color
 * @param mask
 * @param mask_res
 * @param opa
 * @param mode
 */
void lv_blend_fill(const lv_area_t * disp_area, const lv_area_t * clip_area, const lv_area_t * fill_area,
        lv_color_t * disp_buf, lv_img_cf_t cf, lv_color_t color,
        lv_opa_t * mask, lv_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_MASK_RES_FULL_TRANSP) return;

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, clip_area, fill_area);
    if(!is_common) return;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    lv_coord_t disp_w = lv_area_get_width(disp_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area.y1;

    lv_coord_t x;
    lv_coord_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_MASK_RES_FULL_COVER) {
        if(opa > LV_OPA_MAX) {
            lv_coord_t draw_area_w  = lv_area_get_width(&draw_area);
            lv_color_t * disp_buf_tmp_ori =  disp_buf_tmp;

            /*Fill the first line. Use `memcpy` because it's faster then simple value assignment*/
            /*Set the first pixels manually*/
            lv_coord_t direct_fill_end = LV_MATH_MIN(draw_area.x2, draw_area.x1 + FILL_DIRECT_LEN + (draw_area_w & FILL_DIRECT_MASK) - 1);
            for(x = draw_area.x1; x <= direct_fill_end ; x++) {
                disp_buf_tmp[x].full = color.full;
            }

            for(; x <= draw_area.x2; x += FILL_DIRECT_LEN) {
                memcpy(&disp_buf_tmp[x], &disp_buf_tmp[draw_area.x1], FILL_DIRECT_LEN * sizeof(lv_color_t));
            }

            disp_buf_tmp += disp_w;

            for(y = draw_area.y1 + 1; y <= draw_area.y2; y++) {
                memcpy(&disp_buf_tmp[draw_area.x1], &disp_buf_tmp_ori[draw_area.x1], draw_area_w * sizeof(lv_color_t));
                disp_buf_tmp += disp_w;
            }
        }
        else {
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
               for(x = draw_area.x1; x <= draw_area.x2; x++) {
                   disp_buf_tmp[x] = lv_color_mix(color, disp_buf_tmp[x], opa);
               }
               disp_buf_tmp += disp_w;
           }
        }
    }
    /*Masked*/
    else {
        /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

        /* The mask is relative to the clipped area.
         * In the cycles below mask will be indexed from `draw_area.x1`
         * but it corresponds to zero index. So prepare `mask_tmp` accordingly. */
        lv_opa_t * mask_tmp = mask - draw_area.x1;

        /*Buffer the result color to avoid recalculating the same color*/
        lv_color_t last_dest_color;
        lv_color_t last_res_color;
        lv_opa_t last_mask = LV_OPA_TRANSP;
        last_dest_color.full = disp_buf_tmp[0].full;
        last_res_color.full = disp_buf_tmp[0].full;

        /*Only the mask matters*/
        if(opa > LV_OPA_MAX) {
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
               for(x = draw_area.x1; x <= draw_area.x2; x++) {
                   if(mask_tmp[x] == 0) continue;
                    if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
                        if(mask_tmp[x] > LV_OPA_MAX) last_res_color = color;
                        else if(mask_tmp[x] < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
                        else last_res_color = lv_color_mix(color, disp_buf_tmp[x], mask_tmp[x]);
                        last_mask = mask_tmp[x];
                        last_dest_color.full = disp_buf_tmp[x].full;
                    }
                    disp_buf_tmp[x] = last_res_color;
               }
               disp_buf_tmp += disp_w;
               mask_tmp += draw_area_w;
            }
        }
        /*Handle opa and mask values too*/
        else {
            for(y = draw_area.y1; y <= draw_area.y2; y++) {
               for(x = draw_area.x1; x <= draw_area.x2; x++) {
                   if(mask_tmp[x] == 0) continue;
                    if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
                        lv_opa_t opa_tmp = (uint16_t)((uint16_t)mask_tmp[x] * opa) >> 8;

                        if(opa_tmp > LV_OPA_MAX) last_res_color = color;
                        else if(opa_tmp < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
                        else last_res_color = lv_color_mix(color, disp_buf_tmp[x],opa_tmp);
                        last_mask = mask_tmp[x];
                        last_dest_color.full = disp_buf_tmp[x].full;
                    }
                    disp_buf_tmp[x] = last_res_color;
               }
               disp_buf_tmp += disp_w;
               mask_tmp += draw_area_w;
            }
        }
    }
}




void lv_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area, const lv_color_t * map_buf,
                  const lv_opa_t * mask, lv_mask_res_t mask_res,
                  lv_opa_t opa, lv_blend_mode_t mode)
{
    /*Do not draw transparent things*/
    if(opa < LV_OPA_MIN) return;
    if(mask_res == LV_MASK_RES_FULL_TRANSP) return;

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = lv_area_intersect(&draw_area, clip_area, map_area);
    if(!is_common) return;

    lv_disp_t * disp = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);
    const lv_area_t * disp_area = &vdb->area;
    lv_color_t * disp_buf = vdb->buf_act;
    lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Get the width of the `disp_area` it will be used to go to the next line*/
    lv_coord_t disp_w = lv_area_get_width(disp_area);

    /*Get the width of the `mask_area` it will be used to go to the next line*/
    lv_coord_t map_w = lv_area_get_width(map_area);

    /*Create a temp. disp_buf which always point to current line to draw*/
    lv_color_t * disp_buf_tmp = disp_buf + disp_w * draw_area.y1;

    /*Create a temp. map_buf which always point to current line to draw*/
    const lv_color_t * map_buf_tmp = map_buf + map_w * (draw_area.y1 - (map_area->y1 - disp_area->y1));

    lv_coord_t x;
    lv_coord_t y;

    /*Simple fill (maybe with opacity), no masking*/
    if(mask_res == LV_MASK_RES_FULL_COVER) {
        /*Go to the first px of the map*/
        map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1));
        if(opa > LV_OPA_MAX) {
            lv_coord_t draw_area_w  = lv_area_get_width(&draw_area);

            for(y = draw_area.y1; y <= draw_area.y2; y++) {
                memcpy(&disp_buf_tmp[draw_area.x1], map_buf_tmp, draw_area_w * sizeof(lv_color_t));
                disp_buf_tmp += disp_w;
                map_buf_tmp += map_w;
            }
        }
        else {

        }
    }
    /*Masked*/
      else {
          /*Get the width of the `draw_area` it will be used to go to the next line of the mask*/
          lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

          /* The mask is relative to the clipped area.
           * In the cycles below mask will be indexed from `draw_area.x1`
           * but it corresponds to zero index. So prepare `mask_tmp` accordingly. */
          lv_opa_t * mask_tmp = mask - draw_area.x1;

          /*Buffer the result color to avoid recalculating the same color*/
          lv_color_t last_dest_color;
          lv_color_t last_res_color;
          lv_opa_t last_mask = LV_OPA_TRANSP;
          last_dest_color.full = disp_buf_tmp[0].full;
          last_res_color.full = disp_buf_tmp[0].full;

          /*Only the mask matters*/
          if(opa > LV_OPA_MAX) {
              map_buf_tmp += (draw_area.x1 - (map_area->x1 - disp_area->x1)) - draw_area.x1;
              for(y = draw_area.y1; y <= draw_area.y2; y++) {
                 for(x = draw_area.x1; x <= draw_area.x2; x++) {
//                     if(mask_tmp[x] == 0) continue;
//                      if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
//                          if(mask_tmp[x] > LV_OPA_MAX) last_res_color = map_buf_tmp[x];
//                          else if(mask_tmp[x] < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
//                          else last_res_color = lv_color_mix(map_buf_tmp[x], disp_buf_tmp[x], mask_tmp[x]);
//                          last_mask = mask_tmp[x];
//                          last_dest_color.full = disp_buf_tmp[x].full;
//                      }
                      disp_buf_tmp[x] =  lv_color_mix(map_buf_tmp[x], disp_buf_tmp[x], mask_tmp[x]); //map_buf_tmp[x]; //last_res_color;
                 }
                 disp_buf_tmp += disp_w;
                 mask_tmp += draw_area_w;
                 map_buf_tmp += map_w;
              }
          }
          /*Handle opa and mask values too*/
          else {
//              for(y = draw_area.y1; y <= draw_area.y2; y++) {
//                 for(x = draw_area.x1; x <= draw_area.x2; x++) {
//                     if(mask_tmp[x] == 0) continue;
//                      if(mask_tmp[x] != last_mask || last_dest_color.full != disp_buf_tmp[x].full) {
//                          lv_opa_t opa_tmp = (uint16_t)((uint16_t)mask_tmp[x] * opa) >> 8;
//
//                          if(opa_tmp > LV_OPA_MAX) last_res_color = color;
//                          else if(opa_tmp < LV_OPA_MIN) last_res_color = disp_buf_tmp[x];
//                          else last_res_color = lv_color_mix(color, disp_buf_tmp[x],opa_tmp);
//                          last_mask = mask_tmp[x];
//                          last_dest_color.full = disp_buf_tmp[x].full;
//                      }
//                      disp_buf_tmp[x] = last_res_color;
//                 }
//                 disp_buf_tmp += disp_w;
//                 mask_tmp += draw_area_w;
//              }
          }
      }


}


/**********************
 *   STATIC FUNCTIONS
 **********************/
