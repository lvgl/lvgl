/**
 * @file lv_draw_rbasic.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rbasic.h"
#include "lv_conf.h"
#include "../hal/disp/hal_disp.h"
#include "../misc/gfx/font.h"

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
void lv_rpx(cord_t x, cord_t y, const area_t * mask_p, color_t color, opa_t opa)
{
    area_t area;
    area.x1 = x;
    area.y1 = y;
    area.x2 = x;
    area.y2 = y;

    lv_rfill(&area, mask_p, color, OPA_COVER);
}

/**
 * Fill an area on the display
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask
 * @param color fill color
 * @param opa opacity (ignored, only for compatibility with lv_vfill)
 */
void lv_rfill(const area_t * cords_p, const area_t * mask_p, 
              color_t color, opa_t opa)
{   
    area_t masked_area;
    bool union_ok = true;
    
    if(mask_p != NULL) {
        union_ok = area_union(&masked_area, cords_p, mask_p);
    } else {
        area_t scr_area;
        area_set(&scr_area, 0, 0, LV_HOR_RES - 1, LV_VER_RES - 1);
        union_ok = area_union(&masked_area, cords_p, &scr_area);
    }
        
    
    if(union_ok != false){
    	//TODO disp_fill(masked_area.x1, masked_area.y1, masked_area.x2, masked_area.y2, color);
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
void lv_rletter(const point_t * pos_p, const area_t * mask_p,
                     const font_t * font_p, uint8_t letter,
                     color_t color, opa_t opa)
{
    uint8_t w = font_get_width(font_p, letter);
    const uint8_t * bitmap_p = font_get_bitmap(font_p, letter);

    uint8_t col, col_sub, row;
#if FONT_ANTIALIAS == 0
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
        
        /*Correction if the letter is short*/
        bitmap_p += font_p->width_byte - ((w >> 3) + 1);  
        /*Go to the next row*/
        bitmap_p ++;
    }
#else
       const uint8_t * map1_p = bitmap_p;
       const uint8_t * map2_p = bitmap_p + font_p->width_byte;
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
                   lv_rpx(pos_p->x + col, pos_p->y + row, mask_p, color_mix(color, COLOR_SILVER, 63 * px_cnt), OPA_COVER);
               }
           }

           map1_p += font_p->width_byte;
           map2_p += font_p->width_byte;
           map1_p += font_p->width_byte - col_byte_cnt;
           map2_p += font_p->width_byte - col_byte_cnt;
       }
#endif
}

/**
 * Draw a color map to the display
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area
 * @param map_p pointer to a color_t array
 * @param opa opacity of the map (ignored, only for compatibility with lv_vmap)
 * @param transp true: enable transparency of LV_IMG_COLOR_TRANSP color pixels
 * @param upscale true: upscale to double size (not supported)
 * @param recolor mix the pixels with this color (not supported)
 * @param recolor_opa the intense of recoloring (not supported)
 */
void lv_rmap(const area_t * cords_p, const area_t * mask_p,
             const color_t * map_p, opa_t opa, bool transp, bool upscale,
			 color_t recolor, opa_t recolor_opa)
{
    area_t masked_a;
    bool union_ok;

    union_ok = area_union(&masked_a, cords_p, mask_p);

    /*If there are common part of the mask and map then draw the map*/
    if(union_ok == false) return;

    /*Go to the first pixel*/
    cord_t map_width = area_get_width(cords_p);
    map_p+= (masked_a.y1 - cords_p->y1) * map_width;
    map_p += masked_a.x1 - cords_p->x1;

    if(transp == false) {
        cord_t row;
        cord_t mask_w = area_get_width(&masked_a) - 1;
        for(row = 0; row < area_get_height(&masked_a); row++) {
            //TODO disp_map(masked_a.x1, masked_a.y1 + row, masked_a.x1 + mask_w, masked_a.y1 + row, map_p);

            map_p += map_width;
        }
    }else {
        color_t transp_color = LV_COLOR_TRANSP;
        cord_t row;
        for(row = 0; row < area_get_height(&masked_a); row++) {
            cord_t col;
            for(col = 0; col < area_get_width(&masked_a); col ++) {
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
