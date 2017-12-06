/**
 * @file lv_draw_rbasic.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_hal/lv_hal_disp.h"
#include "lv_draw_rbasic.h"
#include "../../lv_conf.h"
#include "../lv_misc/lv_font.h"

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

/**
 * Put a pixel to the display
 * @param x x coordinate of the pixel
 * @param y y coordinate of the pixel
 * @param mask_p the pixel will be drawn on this area
 * @param color color of the pixel
 * @param opa opacity (ignored, only for compatibility with lv_vpx)
 */
void lv_rpx(lv_coord_t x, lv_coord_t y, const lv_area_t * mask_p, lv_color_t color, lv_opa_t opa)
{
    (void)opa;      /*Opa is used only for compatibility with lv_vpx*/

    lv_area_t area;
    area.x1 = x;
    area.y1 = y;
    area.x2 = x;
    area.y2 = y;

    lv_rfill(&area, mask_p, color, LV_OPA_COVER);
}

/**
 * Fill an area on the display
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask
 * @param color fill color
 * @param opa opacity (ignored, only for compatibility with lv_vfill)
 */
void lv_rfill(const lv_area_t * cords_p, const lv_area_t * mask_p, 
              lv_color_t color, lv_opa_t opa)
{   

    (void)opa;      /*Opa is used only for compatibility with lv_vfill*/

    lv_area_t masked_area;
    bool union_ok = true;
    
    if(mask_p != NULL) {
        union_ok = lv_area_union(&masked_area, cords_p, mask_p);
    } else {
        lv_area_t scr_area;
        lv_area_set(&scr_area, 0, 0, LV_HOR_RES - 1, LV_HOR_RES - 1);
        union_ok = lv_area_union(&masked_area, cords_p, &scr_area);
    }
    
    if(union_ok != false){
    	lv_disp_fill(masked_area.x1, masked_area.y1, masked_area.x2, masked_area.y2, color);
    }
}

/**
 * Draw a letter to the display
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area
 * @param font_p pointer to font 
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (ignored, only for compatibility with lv_vletter)
 */
void lv_rletter(const lv_point_t * pos_p, const lv_area_t * mask_p,
                     const lv_font_t * font_p, uint32_t letter,
                     lv_color_t color, lv_opa_t opa)
{
    (void)opa;      /*Opa is used only for compatibility with lv_vletter*/

    uint8_t w = lv_font_get_width(font_p, letter);

    if(letter == 'C') {
        letter = 'C';
    }

    const uint8_t * bitmap_p = lv_font_get_bitmap(font_p, letter);

    uint8_t col, col_sub, row;
#if LV_FONT_ANTIALIAS == 0
    for(row = 0; row < font_p->height_row; row ++) {
        for(col = 0, col_sub = 7; col < w; col ++, col_sub--) {
            if(*bitmap_p & (1 << col_sub)) {
                lv_rpx(pos_p->x + col, pos_p->y + row, mask_p, color, opa);
            }

            if(col_sub == 0) {
                bitmap_p++;
                col_sub = 8;
            }
        }
        /*Go to the next row*/
        if(col_sub != 7) bitmap_p ++;   /*Go to the next byte if it not done in the last step*/
    }
#else
       uint8_t width_byte = w >> 3;    /*Width in bytes (e.g. w = 11 -> 2 bytes wide)*/
       if(w & 0x7) width_byte++;
       const uint8_t * map1_p = bitmap_p;
       const uint8_t * map2_p = bitmap_p + width_byte;
       uint8_t px_cnt;
       uint8_t col_byte_cnt;
       for(row = 0; row < (font_p->height_row >> 1); row ++) {
           col_byte_cnt = 0;
           col_sub = 7;
           for(col = 0; col < (w >> 1); col ++) {

               px_cnt = 0;
               if((*map1_p & (1 << col_sub)) != 0) px_cnt++;
               if((*map2_p & (1 << col_sub)) != 0) px_cnt++;
               if(col_sub != 0) col_sub --;
               else {
                   col_sub = 7;
                   col_byte_cnt ++;
                   map1_p ++;
                   map2_p ++;
               }
               if((*map1_p & (1 << col_sub)) != 0) px_cnt++;
               if((*map2_p & (1 << col_sub)) != 0) px_cnt++;
               if(col_sub != 0) col_sub --;
               else {
                   col_sub = 7;
                   col_byte_cnt ++;
                   map1_p ++;
                   map2_p ++;
               }


               if(px_cnt != 0) {
                   lv_rpx(pos_p->x + col, pos_p->y + row, mask_p, lv_color_mix(color, LV_COLOR_SILVER, 63 * px_cnt), LV_OPA_COVER);
               }
           }

           map1_p += width_byte;
           map2_p += width_byte;
           map1_p += width_byte - col_byte_cnt;
           map2_p += width_byte - col_byte_cnt;
       }
#endif
}

/**
 * Draw a color map to the display
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area
 * @param map_p pointer to a lv_color_t array
 * @param opa opacity of the map (ignored, only for compatibility with lv_vmap)
 * @param transp true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param upscale true: upscale to double size (not supported)
 * @param recolor mix the pixels with this color (not supported)
 * @param recolor_opa the intense of recoloring (not supported)
 */
void lv_rmap(const lv_area_t * cords_p, const lv_area_t * mask_p,
             const lv_color_t * map_p, lv_opa_t opa, bool transp, bool upscale,
			 lv_color_t recolor, lv_opa_t recolor_opa)
{
    (void)opa;              /*opa is used only for compatibility with lv_vmap*/
    (void)recolor_opa;      /*recolor_opa is used only for compatibility with lv_vmap*/
    (void)recolor;          /*recolor is used only for compatibility with lv_vmap*/
    (void)upscale;          /*upscale is used only for compatibility with lv_vmap*/

    lv_area_t masked_a;
    bool union_ok;

    union_ok = lv_area_union(&masked_a, cords_p, mask_p);

    /*If there are common part of the mask and map then draw the map*/
    if(union_ok == false) return;

    /*Go to the first pixel*/
    lv_coord_t map_width = lv_area_get_width(cords_p);
    map_p+= (masked_a.y1 - cords_p->y1) * map_width;
    map_p += masked_a.x1 - cords_p->x1;

    if(transp == false) {
        lv_coord_t row;
        lv_coord_t mask_w = lv_area_get_width(&masked_a) - 1;
        for(row = 0; row < lv_area_get_height(&masked_a); row++) {
            lv_disp_map(masked_a.x1, masked_a.y1 + row, masked_a.x1 + mask_w, masked_a.y1 + row, map_p);

            map_p += map_width;
        }
    }else {
        lv_color_t transp_color = LV_COLOR_TRANSP;
        lv_coord_t row;
        for(row = 0; row < lv_area_get_height(&masked_a); row++) {
            lv_coord_t col;
            for(col = 0; col < lv_area_get_width(&masked_a); col ++) {
                if(map_p[col].full != transp_color.full) {
                    lv_rpx(masked_a.x1 + col, masked_a.y1 + row, mask_p, map_p[col], opa);
                }
            }
            map_p += map_width;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
