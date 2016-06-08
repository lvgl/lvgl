/**
 * @file lv_vdraw.c
 * 
 */
#include "../../lv_conf.h"
#if LV_VDB_SIZE != 0

#include <stddef.h>
#include "lvgl/lv_obj/lv_vdb.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_put_vpx(point_t * point_p, const area_t * mask_p,
                       color_t color, opa_t opa);

static bool lv_vletter_get_px(const font_t * font_p, uint8_t letter, cord_t x, cord_t y);

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
 * Fill an area in the Virtual Display Buffer
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask
 * @param color fill color
 * @param opa opacity of the area (0..255)
 */
void lv_vfill(const area_t * cords_p, const area_t * mask_p, 
                          color_t color, opa_t opa)
{
    area_t res_a;
    bool union_ok;
    lv_vdb_t * vdb_p = lv_vdb_get();
    
    /*Get the union of cord and mask*/
    /* The mask is already truncated to the vdb size
     * in 'lv_refr_area_with_vdb' function */
    union_ok = area_union(&res_a, cords_p, mask_p);
    
    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == true) {
        area_t vdb_rel_a;   /*Stores relative coordinates on vdb*/
        vdb_rel_a.x1 = res_a.x1 - vdb_p->vdb_area.x1;
        vdb_rel_a.y1 = res_a.y1 - vdb_p->vdb_area.y1;
        vdb_rel_a.x2 = res_a.x2 - vdb_p->vdb_area.x1;
        vdb_rel_a.y2 = res_a.y2 - vdb_p->vdb_area.y1;
        
        color_t * vdb_buf_tmp = vdb_p->buf;
        uint32_t vdb_width = area_get_width(&vdb_p->vdb_area);
        /*Move the vdb_tmp to the first row*/
        vdb_buf_tmp += vdb_width * vdb_rel_a.y1;
        
        /*Set all row in vdb to the given color*/
        cord_t row;
        uint32_t col;
        
        /*Run simpler function without opacity*/
        if(opa == OPA_COVER) {
            for(row = vdb_rel_a.y1; row <= vdb_rel_a.y2; row++) {
                for(col = vdb_rel_a.x1; col <= vdb_rel_a.x2; col++) {
                    vdb_buf_tmp[col] = color;
                }

                vdb_buf_tmp += vdb_width;
            }            
        }
        /*Calculate the alpha too*/
        else {
            for(row = vdb_rel_a.y1; row <= vdb_rel_a.y2; row++) {
                for(col = vdb_rel_a.x1; col <= vdb_rel_a.x2; col++) {
                    color_t c = color_mix(color, vdb_buf_tmp[col], opa);
                    vdb_buf_tmp[col] = c;
                }
                vdb_buf_tmp += vdb_width;
            }
        }
    }    
}

/**
 * Draw a letter in the Virtual Display Buffer
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area
 * @param font_p pointer to font 
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (0..255)
 */
void lv_vletter(const point_t * pos_p, const area_t * mask_p, 
                     const font_t * font_p, uint8_t letter,
                     color_t color, opa_t opa)
{      
    if(font_p == NULL) return;
    
    uint8_t w = font_get_width(font_p, letter);
    uint8_t h = font_get_height(font_p);

    /*If the letter is completely out of mask don't draw it */
    if(pos_p->x + w < mask_p->x1 || pos_p->x > mask_p->x2 ||
       pos_p->y + h < mask_p->y1 || pos_p->y > mask_p->y2) return;

    cord_t col, row;
    point_t act_point;

    for(row = 0; row < h; row ++) {
        for(col = 0; col < w; col ++) {
            act_point.x = pos_p->x + col;
            act_point.y = pos_p->y + row;

            if(lv_vletter_get_px(font_p, letter, col, row)) {
                lv_put_vpx(&act_point, mask_p, color,  opa);
            }

        }
    }
}

/**
 * Draw a color map to the display
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area
 * @param map_p pointer to a color_t array
 * @param opa opacity of the map (ignored, only for compatibility with lv_vmap)
 * @param transp true: enable transparency of LV_IMG_COLOR_TRANSP color pixels
 */
void lv_vmap(const area_t * cords_p, const area_t * mask_p, 
             const color_t * map_p, opa_t opa, bool transp)
{
    area_t masked_a;
    bool union_ok;
    lv_vdb_t * vdb_p = lv_vdb_get();
    
    /*Get the union of map size and mask*/
    /* The mask is already truncated to the vdb size
     * in 'lv_refr_area_with_vdb' function */
    union_ok = area_union(&masked_a, cords_p, mask_p);
    
    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == false)  return; 
    
    uint8_t ds_shift = 0;
#if LV_DOWNSCALE <= 1 || LV_UPSCALE_MAP == 0
    ds_shift = 0;
#elif LV_DOWNSCALE == 2
    ds_shift = 1;
#elif LV_DOWNSCALE == 4
    ds_shift = 2;
#else
#error "LV: not supported LV_DOWNSCALE value"
#endif

    /*If the map starts OUT of the masked area then calc. the first pixel*/
    cord_t map_width = area_get_width(cords_p) >> ds_shift;
    if(cords_p->y1 < masked_a.y1) {
        map_p += (uint32_t) map_width * ((masked_a.y1 - cords_p->y1) >> ds_shift);
    }
    if(cords_p->x1 < masked_a.x1) {
        map_p += (masked_a.x1 - cords_p->x1) >> ds_shift;
    }

    /*Stores coordinates relative to the act vdb*/
    masked_a.x1 = masked_a.x1 - vdb_p->vdb_area.x1;
    masked_a.y1 = masked_a.y1 - vdb_p->vdb_area.y1;
    masked_a.x2 = masked_a.x2 - vdb_p->vdb_area.x1;
    masked_a.y2 = masked_a.y2 - vdb_p->vdb_area.y1;

    cord_t vdb_width = area_get_width(&vdb_p->vdb_area);
    color_t * vdb_buf_tmp = vdb_p->buf; 
    vdb_buf_tmp += (uint32_t) vdb_width * masked_a.y1; /*Move to the first row*/

    map_p -= masked_a.x1 >> ds_shift;

#if LV_DOWNSCALE > 1 && LV_UPSCALE_MAP != 0
    cord_t row;
    cord_t col;
    color_t transp_color = LV_IMG_COLOR_TRANSP;
    cord_t map_i;
    map_p -= map_width; /*Compensate the first row % LV_DOWNSCALE*/
    for(row = masked_a.y1; row <= masked_a.y2; row++) {
        if(row % LV_DOWNSCALE == 0) map_p += map_width;              /*Next row on the map*/
        map_i = masked_a.x1 >> ds_shift;
        map_i--; /*Compensate the first col % LV_DOWNSCALE*/
        for(col = masked_a.x1; col <= masked_a.x2; col++) {
           if(col % LV_DOWNSCALE == 0) map_i++;
           if(transp == false || map_p[map_i].full != transp_color.full) {
               vdb_buf_tmp[col] = color_mix(map_p[map_i], vdb_buf_tmp[col], opa);
           }
        }
        vdb_buf_tmp += vdb_width;        /*Next row on the VDB*/
     }
#else
   if(transp == false) { /*Simply copy the pixels to the VDB*/
        cord_t row; 

        if(opa == OPA_COVER)  { /*no opa */
            for(row = masked_a.y1; row <= masked_a.y2; row++) {
                memcpy(&vdb_buf_tmp[masked_a.x1], 
                       &map_p[masked_a.x1], 
                       area_get_width(&masked_a) * sizeof(color_t));
                map_p += map_width;               /*Next row on the map*/
                vdb_buf_tmp += vdb_width;         /*Next row on the VDB*/
            }
        } else {
            cord_t col;
            for(row = masked_a.y1; row <= masked_a.y2; row++) {
                for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                   vdb_buf_tmp[col] = color_mix( map_p[col], vdb_buf_tmp[col], opa);
                }
                map_p += map_width;              /*Next row on the map*/
                vdb_buf_tmp += vdb_width;        /*Next row on the VDB*/
            }
        }
    } else { /*transp == true: Check all pixels */
        cord_t row;
        cord_t col;
        color_t transp_color = LV_IMG_COLOR_TRANSP;
        if(opa == OPA_COVER)  { /*no opa */
            for(row = masked_a.y1; row <= masked_a.y2; row++) {
               for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                   if(map_p[col].full != transp_color.full) {
                       vdb_buf_tmp[col] = map_p[col];
                   }
               }

               map_p += map_width; /*Next row on the map*/
               vdb_buf_tmp += vdb_width;         /*Next row on the VDB*/   
           }           
        } else {
            for(row = masked_a.y1; row <= masked_a.y2; row++) {
                for(col = masked_a.x1; col <= masked_a.x2; col ++) {
                    if(map_p[col].full != transp_color.full) {
                        vdb_buf_tmp[col] = color_mix( map_p[col], vdb_buf_tmp[col], opa);
                    }
                }

                map_p += map_width; /*Next row on the map*/
                vdb_buf_tmp += vdb_width;         /*Next row on the VDB*/   
            }
        }
    }
#endif /*No upscale and no downscale*/
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Put a pixel into the Virtual Dispaly Buffer
 * @param x x coordinate of the pixel
 * @param y y coordinate of the pixel
 * @param mask_p the pixel will be drawn on this area
 * @param color color of the pixel
 * @param opa opacity of the pixel
 */
static void lv_put_vpx(point_t * point_p, const area_t * mask_p,
                            color_t color, opa_t opa)
{    
	if(opa == OPA_TRANSP) return;

    bool point_ok;
    lv_vdb_t * vdb_p = lv_vdb_get();
    
    /*The point is on vdb?*/
    point_ok = area_is_point_on(mask_p, point_p);
    
    /*If there are common part of the three area then draw to the vdb*/
    if(point_ok == false) return;   
    point_t vdb_rel_point;   /*Stores relative coordinates on vdb*/
    vdb_rel_point.x = point_p->x - vdb_p->vdb_area.x1;
    vdb_rel_point.y = point_p->y - vdb_p->vdb_area.y1;

    color_t * vdb_buf_tmp = vdb_p->buf;
    uint32_t vdb_width = vdb_p->vdb_area.x2 - vdb_p->vdb_area.x1 + 1;

    /*Move the vdb_tmp to the point*/
    vdb_buf_tmp += vdb_width * vdb_rel_point.y + vdb_rel_point.x;

    if(opa == OPA_COVER) *vdb_buf_tmp = color;
    else *vdb_buf_tmp = color_mix(color, *vdb_buf_tmp, opa); 
}

/**
 * Get a pixel from a letter
 * @param font_p pointer to a font
 * @param letter a letter 
 * @param x x coordinate of the pixel to get
 * @param y y coordinate of the pixel to get
 * @return true: pixel is set, false: pixel is clear
 */
static bool lv_vletter_get_px(const font_t * font_p, uint8_t letter, cord_t x, cord_t y)
{
	uint8_t w = font_get_width(font_p, letter);
	uint8_t h = font_get_height(font_p);
	const uint8_t * map_p = font_get_bitmap(font_p, letter);

	if(map_p == NULL) return NULL;

	if(x < 0) x = 0;
	if(y < 0) x = 0;
	if(x >= w) x = w - 1;
	if(y >= h) y = h - 1;

#if LV_UPSCALE_FONT != 0
#if LV_DOWNSCALE == 1
/*Do nothing*/
#elif LV_DOWNSCALE == 2
	x = x >> 1;
	y = y >> 1;
#elif LV_DOWNSCALE == 4
	x = x >> 2;
	y = y >> 2;
#else
#error "LV: not supported LV_DOWNSCALE value"
#endif
#endif /*LV_UPSCALE_FONT == 0*/

	map_p += (uint32_t)y * font_p->width_byte; /*Go to the corresponding row of the map*/
	map_p += (x >> 3); /*Go to he corresponding col of the map*/ 

	/*Get the corresponding col within a byte*/
	uint8_t map_byte = *map_p;
	uint8_t col_sub = 7 - (x % 8);
	if((map_byte & (1 << col_sub)) == 0) return false;
	else return true;
}
#endif
