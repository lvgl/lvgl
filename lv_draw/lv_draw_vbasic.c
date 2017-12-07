/**
 * @file lv_vdraw.c
 * 
 */

#include "../../lv_conf.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../lv_hal/lv_hal_disp.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_font.h"
#include "../lv_misc/lv_color.h"

#if LV_VDB_SIZE != 0

#include <stddef.h>
#include "../lv_core/lv_vdb.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
#define VFILL_HW_ACC_SIZE_LIMIT    50      /*Always fill < 50 px with 'sw_color_fill' because of the hw. init overhead*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void sw_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void sw_color_fill(lv_area_t * mem_area, lv_color_t * mem, const lv_area_t * fill_area, lv_color_t color, lv_opa_t opa);

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
 * Put a pixel in the Virtual Display Buffer
 * @param x pixel x coordinate
 * @param y pixel y coordinate
 * @param mask_p fill only on this mask (truncated to VDB area)
 * @param color pixel color
 * @param opa opacity of the area (0..255)
 */
void lv_vpx(lv_coord_t x, lv_coord_t y, const lv_area_t * mask_p, lv_color_t color, lv_opa_t opa)
{
    lv_vdb_t * vdb_p = lv_vdb_get();

    /*Pixel out of the mask*/
    if(x < mask_p->x1 || x > mask_p->x2 ||
       y < mask_p->y1 || y > mask_p->y2) {
        return;
    }

    uint32_t vdb_width = lv_area_get_width(&vdb_p->area);

    /*Make the coordinates relative to VDB*/
    x-=vdb_p->area.x1;
    y-=vdb_p->area.y1;
    lv_color_t * vdb_px_p = vdb_p->buf + y * vdb_width + x;
    if(opa == LV_OPA_COVER) {
        *vdb_px_p = color;
    }
    else {
        *vdb_px_p = lv_color_mix(color,*vdb_px_p, opa);
    }

}


/**
 * Fill an area in the Virtual Display Buffer
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask  (truncated to VDB area)
 * @param color fill color
 * @param opa opacity of the area (0..255)
 */
void lv_vfill(const lv_area_t * cords_p, const lv_area_t * mask_p, 
                          lv_color_t color, lv_opa_t opa)
{
    lv_area_t res_a;
    bool union_ok;
    lv_vdb_t * vdb_p = lv_vdb_get();
    
    /*Get the union of cord and mask*/
    /* The mask is already truncated to the vdb size
     * in 'lv_refr_area_with_vdb' function */
    union_ok = lv_area_union(&res_a, cords_p, mask_p);
    
    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == false) return;

    lv_area_t vdb_rel_a;   /*Stores relative coordinates on vdb*/
    vdb_rel_a.x1 = res_a.x1 - vdb_p->area.x1;
    vdb_rel_a.y1 = res_a.y1 - vdb_p->area.y1;
    vdb_rel_a.x2 = res_a.x2 - vdb_p->area.x1;
    vdb_rel_a.y2 = res_a.y2 - vdb_p->area.y1;

    lv_color_t * vdb_buf_tmp = vdb_p->buf;
    uint32_t vdb_width = lv_area_get_width(&vdb_p->area);
    /*Move the vdb_tmp to the first row*/
    vdb_buf_tmp += vdb_width * vdb_rel_a.y1;


#if USE_LV_GPU
    static lv_color_t color_array_tmp[LV_HOR_RES << LV_ANTIALIAS];       /*Used by 'sw_color_fill'*/
    static lv_coord_t last_width = -1;

    lv_coord_t w = lv_area_get_width(&vdb_rel_a);
    /*Don't use hw. acc. for every small fill (because of the init overhead)*/
    if(w < VFILL_HW_ACC_SIZE_LIMIT) {
        sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
    }
    /*Not opaque fill*/
    else if(opa == LV_OPA_COVER) {
        /*Use hw fill if present*/
        if(lv_disp_is_mem_fill_supported()) {
            lv_coord_t row;
            for(row = vdb_rel_a.y1;row <= vdb_rel_a.y2; row++) {
                lv_disp_mem_fill(&vdb_buf_tmp[vdb_rel_a.x1], w, color);
                vdb_buf_tmp += vdb_width;
            }
        }
        /*Use hw blend if present and the area is not too small*/
        else if(lv_area_get_height(&vdb_rel_a) > VFILL_HW_ACC_SIZE_LIMIT &&
                lv_disp_is_mem_blend_supported())
        {
            if(color_array_tmp[0].full != color.full || last_width != w) {
                uint16_t i;
                for(i = 0; i < w; i++) {
                    color_array_tmp[i].full = color.full;
                }
                last_width = w;
            }
            lv_coord_t row;
            for(row = vdb_rel_a.y1;row <= vdb_rel_a.y2; row++) {
                lv_disp_mem_blend(&vdb_buf_tmp[vdb_rel_a.x1], color_array_tmp, w, opa);
                vdb_buf_tmp += vdb_width;
            }

        }
        /*Else use sw fill if no better option*/
        else {
            sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
        }

    }
    /*Fill with opacity*/
    else {
    	/*Use hw blend if present*/
        if(lv_disp_is_mem_blend_supported()) {
            if(color_array_tmp[0].full != color.full || last_width != w) {
                uint16_t i;
                for(i = 0; i < w; i++) {
                    color_array_tmp[i].full = color.full;
                }

                last_width = w;
            }
            lv_coord_t row;
            for(row = vdb_rel_a.y1;row <= vdb_rel_a.y2; row++) {
                lv_disp_mem_blend(&vdb_buf_tmp[vdb_rel_a.x1], color_array_tmp, w, opa);
                vdb_buf_tmp += vdb_width;
            }

        }
        /*Use sw fill with opa if no better option*/
        else {
            sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
        }

    }
#else
    sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
#endif
}

/**
 * Draw a letter in the Virtual Display Buffer
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area  (truncated to VDB area)
 * @param font_p pointer to font 
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (0..255)
 */
void lv_vletter(const lv_point_t * pos_p, const lv_area_t * mask_p, 
                     const lv_font_t * font_p, uint32_t letter,
                     lv_color_t color, lv_opa_t opa)
{      
    if(font_p == NULL) return;

    uint8_t letter_w = lv_font_get_width(font_p, letter);
    uint8_t letter_h = lv_font_get_height(font_p);

    const uint8_t * map_p = lv_font_get_bitmap(font_p, letter);

    if(map_p == NULL) return;

    /*If the letter is completely out of mask don't draw it */
    if(pos_p->x + letter_w < mask_p->x1 || pos_p->x > mask_p->x2 ||
       pos_p->y + letter_h < mask_p->y1 || pos_p->y > mask_p->y2) return;

    lv_vdb_t * vdb_p = lv_vdb_get();
    lv_coord_t vdb_width = lv_area_get_width(&vdb_p->area);
    lv_color_t * vdb_buf_tmp = vdb_p->buf;
    lv_coord_t col, row;
    uint8_t col_bit;
    uint8_t col_byte_cnt;
    uint8_t width_byte = letter_w >> 3;    /*Width in bytes (e.g. w = 11 -> 2 bytes wide)*/
    if(letter_w & 0x7) width_byte++;

    /* Calculate the col/row start/end on the map
     * If font anti alaiassing is enabled use the reduced letter sizes*/
    lv_coord_t col_start = pos_p->x > mask_p->x1 ? 0 : mask_p->x1 - pos_p->x;
    lv_coord_t col_end = pos_p->x + (letter_w >> LV_FONT_ANTIALIAS) < mask_p->x2 ? (letter_w >> LV_FONT_ANTIALIAS) : mask_p->x2 - pos_p->x + 1;
    lv_coord_t row_start = pos_p->y > mask_p->y1 ? 0 : mask_p->y1 - pos_p->y;
    lv_coord_t row_end  = pos_p->y + (letter_h >> LV_FONT_ANTIALIAS) < mask_p->y2 ? (letter_h >> LV_FONT_ANTIALIAS) : mask_p->y2 - pos_p->y + 1;

    /*Set a pointer on VDB to the first pixel of the letter*/
    vdb_buf_tmp += ((pos_p->y - vdb_p->area.y1) * vdb_width)
                    + pos_p->x - vdb_p->area.x1;

    /*If the letter is partially out of mask the move there on VDB*/
    vdb_buf_tmp += (row_start * vdb_width) + col_start;

    /*Move on the map too*/
    map_p += ((row_start << LV_FONT_ANTIALIAS) * width_byte) + ((col_start << LV_FONT_ANTIALIAS) >> 3);

#if LV_FONT_ANTIALIAS != 0
    lv_opa_t opa_tmp = opa;
    if(opa_tmp != LV_OPA_COVER) opa_tmp = opa_tmp >> 2;   /*Opacity per pixel (used when sum the pixels)*/
    const uint8_t * map1_p = map_p;
    const uint8_t * map2_p = map_p + width_byte;
    uint8_t px_cnt;
    for(row = row_start; row < row_end; row ++) {
        col_byte_cnt = 0;
        col_bit = 7 - ((col_start << LV_FONT_ANTIALIAS) % 8);
        for(col = col_start; col < col_end; col ++) {

            px_cnt = 0;
            if((*map1_p & (1 << col_bit)) != 0) px_cnt++;
            if((*map2_p & (1 << col_bit)) != 0) px_cnt++;
            if(col_bit != 0) col_bit --;
            else {
                col_bit = 7;
                col_byte_cnt ++;
                map1_p ++;
                map2_p ++;
            }
            if((*map1_p & (1 << col_bit)) != 0) px_cnt++;
            if((*map2_p & (1 << col_bit)) != 0) px_cnt++;
            if(col_bit != 0) col_bit --;
            else {
                col_bit = 7;
                col_byte_cnt ++;
                map1_p ++;
                map2_p ++;
            }

            if(px_cnt != 0) {
                if(opa == LV_OPA_COVER) *vdb_buf_tmp = lv_color_mix(color, *vdb_buf_tmp, 63*px_cnt);
                else *vdb_buf_tmp = lv_color_mix(color, *vdb_buf_tmp, opa_tmp * px_cnt);
            }

           vdb_buf_tmp++;
        }

        map1_p += width_byte;
        map2_p += width_byte;
        map1_p += width_byte - col_byte_cnt;
        map2_p += width_byte - col_byte_cnt;
        vdb_buf_tmp += vdb_width  - ((col_end) - (col_start)); /*Next row in VDB*/
    }
#else
    for(row = row_start; row < row_end; row ++) {
        col_byte_cnt = 0;
        col_bit = 7 - (col_start % 8);
        for(col = col_start; col < col_end; col ++) {

            if((*map_p & (1 << col_bit)) != 0) {
                if(opa == LV_OPA_COVER) *vdb_buf_tmp = color;
                else *vdb_buf_tmp = lv_color_mix(color, *vdb_buf_tmp, opa);
            }

           vdb_buf_tmp++;

           if(col_bit != 0) col_bit --;
           else {
               col_bit = 7;
               col_byte_cnt ++;
               map_p ++;
            }
        }

        map_p += width_byte - col_byte_cnt;
        vdb_buf_tmp += vdb_width  - (col_end - col_start); /*Next row in VDB*/
    }
#endif
}

/**
 * Draw a color map to the display
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area  (truncated to VDB area)
 * @param map_p pointer to a lv_color_t array
 * @param opa opacity of the map (ignored, only for compatibility with lv_vmap)
 * @param transp true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param upscale true: upscale to double size
 * @param recolor mix the pixels with this color
 * @param recolor_opa the intense of recoloring
 */
void lv_vmap(const lv_area_t * cords_p, const lv_area_t * mask_p, 
             const lv_color_t * map_p, lv_opa_t opa, bool transp, bool upscale,
			 lv_color_t recolor, lv_opa_t recolor_opa)
{
    lv_area_t masked_a;
    bool union_ok;
    lv_vdb_t * vdb_p = lv_vdb_get();

    /*Get the union of map size and mask*/
    /* The mask is already truncated to the vdb size
    * in 'lv_refr_area_with_vdb' function */
    union_ok = lv_area_union(&masked_a, cords_p, mask_p);

    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == false)  return;

    uint8_t ds_shift = 0;
    if(upscale != false) ds_shift = 1;

    /*If the map starts OUT of the masked area then calc. the first pixel*/
    lv_coord_t map_width = lv_area_get_width(cords_p) >> ds_shift;
    if(cords_p->y1 < masked_a.y1) {
        map_p += (uint32_t) map_width * ((masked_a.y1 - cords_p->y1) >> ds_shift);
    }
    if(cords_p->x1 < masked_a.x1) {
        map_p += (masked_a.x1 - cords_p->x1) >> ds_shift;
    }

    /*Stores coordinates relative to the act vdb*/
    masked_a.x1 = masked_a.x1 - vdb_p->area.x1;
    masked_a.y1 = masked_a.y1 - vdb_p->area.y1;
    masked_a.x2 = masked_a.x2 - vdb_p->area.x1;
    masked_a.y2 = masked_a.y2 - vdb_p->area.y1;

    lv_coord_t vdb_width = lv_area_get_width(&vdb_p->area);
    lv_color_t * vdb_buf_tmp = vdb_p->buf;
    vdb_buf_tmp += (uint32_t) vdb_width * masked_a.y1; /*Move to the first row*/

    map_p -= (masked_a.x1 >> ds_shift); /*Move back. It will be easier to index 'map_p' later*/

    /*No upscalse*/
    if(upscale == false) {
        if(transp == false) { /*Simply copy the pixels to the VDB*/
            lv_coord_t row;
            lv_coord_t map_useful_w = lv_area_get_width(&masked_a);

            for(row = masked_a.y1; row <= masked_a.y2; row++) {
#if USE_LV_GPU
                if(lv_disp_is_mem_blend_supported() == false) {
                    sw_mem_blend(&vdb_buf_tmp[masked_a.x1], &map_p[masked_a.x1], map_useful_w, opa);
                } else {
                    lv_disp_mem_blend(&vdb_buf_tmp[masked_a.x1], &map_p[masked_a.x1], map_useful_w, opa);
                }
#else
                sw_mem_blend(&vdb_buf_tmp[masked_a.x1], &map_p[masked_a.x1], map_useful_w, opa);
#endif
                map_p += map_width;               /*Next row on the map*/
                vdb_buf_tmp += vdb_width;         /*Next row on the VDB*/
            }
            /*To recolor draw simply a rectangle above the image*/
            if(recolor_opa != LV_OPA_TRANSP) {
                lv_vfill(cords_p, mask_p, recolor, recolor_opa);
            }
        } else { /*transp == true: Check all pixels */
            lv_coord_t row;
            lv_coord_t col;
            lv_color_t transp_color = LV_COLOR_TRANSP;

            if(recolor_opa == LV_OPA_TRANSP) {/*No recolor*/
                if(opa == LV_OPA_COVER)  { /*no opa */
                    for(row = masked_a.y1; row <= masked_a.y2; row++) {
                        for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                            if(map_p[col].full != transp_color.full) {
                                vdb_buf_tmp[col] = map_p[col];
                            }
                        }

                        map_p += map_width;         /*Next row on the map*/
                        vdb_buf_tmp += vdb_width;   /*Next row on the VDB*/
                    }
                } else {
                    for(row = masked_a.y1; row <= masked_a.y2; row++) {
                        for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                            if(map_p[col].full != transp_color.full) {
                                vdb_buf_tmp[col] = lv_color_mix( map_p[col], vdb_buf_tmp[col], opa);
                            }
                        }

                        map_p += map_width;          /*Next row on the map*/
                        vdb_buf_tmp += vdb_width;   /*Next row on the VDB*/
                    }
                }
            } else { /*Recolor needed*/
                lv_color_t lv_color_tmp;
                if(opa == LV_OPA_COVER)  { /*no opa */
                    for(row = masked_a.y1; row <= masked_a.y2; row++) {
                        for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                            if(map_p[col].full != transp_color.full) {
                                lv_color_tmp = lv_color_mix(recolor, map_p[col], recolor_opa);
                                vdb_buf_tmp[col] = lv_color_tmp;
                            }
                        }

                        map_p += map_width; /*Next row on the map*/
                        vdb_buf_tmp += vdb_width;         /*Next row on the VDB*/
                    }
                } else {
                    for(row = masked_a.y1; row <= masked_a.y2; row++) {
                        for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                            if(map_p[col].full != transp_color.full) {
                                lv_color_tmp = lv_color_mix(recolor, map_p[col], recolor_opa);
                                vdb_buf_tmp[col] = lv_color_mix(lv_color_tmp, vdb_buf_tmp[col], opa);
                            }
                        }

                        map_p += map_width; /*Next row on the map*/
                        vdb_buf_tmp += vdb_width;         /*Next row on the VDB*/
                    }
                }
            }
        }
    }
    /*Upscalse*/
    else {
        lv_coord_t row;
        lv_coord_t col;
        lv_color_t transp_color = LV_COLOR_TRANSP;
        lv_color_t lv_color_tmp;
        lv_color_t prev_color = LV_COLOR_BLACK;
        lv_coord_t map_col;

        /*The most simple case (but upscale): 0 opacity, no recolor, no transp. pixels*/
        if(transp == false && opa == LV_OPA_COVER && recolor_opa == LV_OPA_TRANSP) { 
            lv_coord_t map_col_start = masked_a.x1 >> 1;
            lv_coord_t map_col_end = masked_a.x2 >> 1;
            lv_coord_t vdb_col;         /*Col. in this row*/
            lv_coord_t vdb_col2;        /*Col. in next row*/

            for(row = masked_a.y1; row <= masked_a.y2; row += 2) {
                map_col_start = masked_a.x1 >> 1;
                map_col_end = masked_a.x2 >> 1;
                vdb_col = masked_a.x1;
                vdb_col2 = masked_a.x1 + vdb_width;
               for(map_col = map_col_start; map_col <= map_col_end; map_col ++, vdb_col += 2, vdb_col2 += 2) {

                   vdb_buf_tmp[vdb_col].full = map_p[map_col].full;
                   vdb_buf_tmp[vdb_col + 1].full = map_p[map_col].full;
                   vdb_buf_tmp[vdb_col2].full = map_p[map_col].full;
                   vdb_buf_tmp[vdb_col2 + 1].full = map_p[map_col].full;
               }


               map_p += map_width;
               vdb_buf_tmp += 2 * vdb_width ; /*+ 2 row on the VDB (2 rows are filled because of the upscale)*/

            }
        }
        /*Handle other cases*/
        else {
           lv_color_tmp = lv_color_mix(recolor, prev_color, recolor_opa);
           for(row = masked_a.y1; row <= masked_a.y2; row++) {
               for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                   map_col = col >> 1;

                   /*Handle recoloring*/
                   if(recolor_opa == LV_OPA_TRANSP) {
                       lv_color_tmp.full = map_p[map_col].full;
                   } else {
                       if(map_p[map_col].full != prev_color.full) {
                           prev_color.full = map_p[map_col].full;
                           lv_color_tmp = lv_color_mix(recolor, prev_color, recolor_opa);
                       }
                   }
                   /*Put the NOT transparent pixels*/
                   if(transp == false || map_p[map_col].full != transp_color.full) {
                       /*Handle opacity*/
                       if(opa == LV_OPA_COVER) {
                           vdb_buf_tmp[col] = lv_color_tmp;
                       } else {
                           vdb_buf_tmp[col] = lv_color_mix( lv_color_tmp, vdb_buf_tmp[col], opa);
                       }
                   }
               }
               if((row & 0x1) != 0) map_p += map_width; /*Next row on the map*/
               vdb_buf_tmp += vdb_width ;        /*Next row on the VDB*/
           }
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Blend pixels to destination memory using opacity
 * @param dest a memory address. Copy 'src' here.
 * @param src pointer to pixel map. Copy it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
static void sw_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    if(opa == LV_OPA_COVER) {
        memcpy(dest, src, length * sizeof(lv_color_t));
    } else {
        uint32_t col;
        for(col = 0; col < length; col++) {
        	dest[col] = lv_color_mix(src[col], dest[col], opa);
		}
    }
}

/**
 *
 * @param mem_area coordinates of 'mem' memory area
 * @param mem a memory address. Considered to a rectangular window according to 'mem_area'
 * @param fill_area coordinates of an area to fill. Relative to 'mem_area'.
 * @param color fill color
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
static void sw_color_fill(lv_area_t * mem_area, lv_color_t * mem, const lv_area_t * fill_area, lv_color_t color, lv_opa_t opa)
{

    /*Set all row in vdb to the given color*/
    lv_coord_t row;
    lv_coord_t col;
    lv_coord_t mem_width = lv_area_get_width(mem_area);

    /*Run simpler function without opacity*/
    if(opa == LV_OPA_COVER) {
        /*Fill the first row with 'color'*/
        for(col = fill_area->x1; col <= fill_area->x2; col++) {
            mem[col] = color;
        }

        /*Copy the first row to all other rows*/
        lv_color_t * mem_first = &mem[fill_area->x1];
        lv_coord_t copy_size =  (fill_area->x2 - fill_area->x1 + 1) * sizeof(lv_color_t);
        mem += mem_width;

        for(row = fill_area->y1 + 1; row <= fill_area->y2; row++) {
            memcpy(&mem[fill_area->x1], mem_first, copy_size);
            mem += mem_width;
        }
    }
    /*Calculate with alpha too*/
    else {
        lv_color_t bg_tmp = LV_COLOR_BLACK;
        lv_color_t opa_tmp = lv_color_mix(color, bg_tmp, opa);
        for(row = fill_area->y1; row <= fill_area->y2; row++) {
            for(col = fill_area->x1; col <= fill_area->x2; col++) {
                /*If the bg color changed recalculate the result color*/
                if(mem[col].full != bg_tmp.full) {
                    bg_tmp = mem[col];
                    opa_tmp = lv_color_mix(color, bg_tmp, opa);
                }
                mem[col] = opa_tmp;
            }
            mem += mem_width;
        }
    }
}

#endif
