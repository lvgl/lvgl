/**
 * @file lv_draw_rbasic.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rbasic.h"
#if USE_LV_REAL_DRAW != 0

#include "../lv_hal/lv_hal_disp.h"
#include "../lv_misc/lv_font.h"
#include "lv_draw.h"

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
static lv_color_t letter_bg_color;

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
        lv_area_set(&scr_area, 0, 0, LV_HOR_RES - 1, LV_VER_RES - 1);
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

    static uint8_t bpp1_opa_table[2] =  {0, 255};                   /*Opacity mapping with bpp = 1 (Just for compatibility)*/
    static uint8_t bpp2_opa_table[4] =  {0, 85, 170, 255};          /*Opacity mapping with bpp = 2*/
    static uint8_t bpp4_opa_table[16] = {0,   17,  34,  51,         /*Opacity mapping with bpp = 4*/
                                        68,  85,  102, 119,
                                        136, 153, 170, 187,
                                        204, 221, 238, 255};

    if(font_p == NULL) return;

    uint8_t letter_w = lv_font_get_width(font_p, letter);
    uint8_t letter_h = lv_font_get_height(font_p);
    uint8_t bpp = lv_font_get_bpp(font_p, letter);  /*Bit per pixel (1,2, 4 or 8)*/
    uint8_t *bpp_opa_table;
    uint8_t mask_init;
    uint8_t mask;

    switch(bpp) {
        case 1: bpp_opa_table = bpp1_opa_table;  mask_init = 0x80; break;
        case 2: bpp_opa_table = bpp2_opa_table;  mask_init = 0xC0; break;
        case 4: bpp_opa_table = bpp4_opa_table;  mask_init = 0xF0; break;
        case 8: bpp_opa_table = NULL;  mask_init = 0xFF; break;             /*No opa table, pixel value will be used directly*/
        default: return;        /*Invalid bpp. Can't render the letter*/
    }

    const uint8_t * map_p = lv_font_get_bitmap(font_p, letter);

    if(map_p == NULL) return;

    /*If the letter is completely out of mask don't draw it */
    if(pos_p->x + letter_w < mask_p->x1 || pos_p->x > mask_p->x2 ||
       pos_p->y + letter_h < mask_p->y1 || pos_p->y > mask_p->y2) return;

    lv_coord_t col, row;
    uint8_t col_bit;
    uint8_t col_byte_cnt;
    uint8_t width_byte_scr = letter_w >> 3;      /*Width in bytes (on the screen finally) (e.g. w = 11 -> 2 bytes wide)*/
    if(letter_w & 0x7) width_byte_scr++;
    uint8_t width_byte_bpp = (letter_w * bpp) >> 3;    /*Letter width in byte. Real width in the font*/
    if((letter_w * bpp) & 0x7) width_byte_bpp++;

    /* Calculate the col/row start/end on the map*/
    lv_coord_t col_start = pos_p->x >= mask_p->x1 ? 0 : mask_p->x1 - pos_p->x;
    lv_coord_t col_end = pos_p->x + letter_w <= mask_p->x2 ? letter_w : mask_p->x2 - pos_p->x + 1;
    lv_coord_t row_start = pos_p->y >= mask_p->y1 ? 0 : mask_p->y1 - pos_p->y;
    lv_coord_t row_end  = pos_p->y + letter_h <= mask_p->y2 ? letter_h : mask_p->y2 - pos_p->y + 1;

    /*Move on the map too*/
    map_p += (row_start * width_byte_bpp) + ((col_start * bpp) >> 3);

    uint8_t letter_px;
    for(row = row_start; row < row_end; row ++) {
        col_byte_cnt = 0;
        col_bit = (col_start * bpp) % 8;
        mask = mask_init >> col_bit;
        for(col = col_start; col < col_end; col ++) {
            letter_px = (*map_p & mask) >> (8 - col_bit - bpp);
            if(letter_px != 0) {
                lv_rpx(pos_p->x + col, pos_p->y + row, mask_p, lv_color_mix(color, letter_bg_color, bpp == 8 ? letter_px : bpp_opa_table[letter_px]), LV_OPA_COVER);
            }

            if(col_bit < 8 - bpp) {
                col_bit += bpp;
                mask = mask >> bpp;
            }
            else {
                col_bit = 0;
                col_byte_cnt ++;
                mask = mask_init;
                map_p ++;
            }
        }

        map_p += (width_byte_bpp) - col_byte_cnt;
    }
}

/**
 * When the letter is ant-aliased it needs to know the background color
 * @param bg_color the background color of the currently drawn letter
 */
void lv_rletter_set_background(lv_color_t color)
{
    letter_bg_color = color;
}

/**
 * Draw a color map to the display (image)
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area
 * @param map_p pointer to a lv_color_t array
 * @param opa opacity of the map (ignored, only for compatibility with 'lv_vmap')
 * @param chroma_keyed true: enable transparency of LV_IMG_LV_COLOR_TRANSP color pixels
 * @param alpha_byte true: extra alpha byte is inserted for every pixel (not supported, only l'v_vmap' can draw it)
 * @param recolor mix the pixels with this color
 * @param recolor_opa the intense of recoloring
 */
void lv_rmap(const lv_area_t * cords_p, const lv_area_t * mask_p,
            const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
            lv_color_t recolor, lv_opa_t recolor_opa)
{
    if(alpha_byte) return;      /*Pixel level opacity i not supported in real map drawing*/

    (void)opa;              /*opa is used only for compatibility with lv_vmap*/
    lv_area_t masked_a;
    bool union_ok;

    union_ok = lv_area_union(&masked_a, cords_p, mask_p);

    /*If there are common part of the mask and map then draw the map*/
    if(union_ok == false) return;

    /*Go to the first pixel*/
    lv_coord_t map_width = lv_area_get_width(cords_p);
    map_p += (masked_a.y1 - cords_p->y1) * map_width * sizeof(lv_color_t);
    map_p += (masked_a.x1 - cords_p->x1) * sizeof(lv_color_t);

    lv_coord_t row;
    if(recolor_opa == LV_OPA_TRANSP && chroma_key == false) {
        lv_coord_t mask_w = lv_area_get_width(&masked_a) - 1;
        for(row = masked_a.y1; row <= masked_a.y2; row++) {
            lv_disp_map(masked_a.x1, row, masked_a.x1 + mask_w, row, (lv_color_t*)map_p);
            map_p += map_width * sizeof(lv_color_t);               /*Next row on the map*/
        }
    } else {
        lv_color_t chroma_key_color = LV_COLOR_TRANSP;
        lv_coord_t col;
        for(row = masked_a.y1; row <= masked_a.y2; row++) {
            for(col = masked_a.x1; col <= masked_a.x2; col++) {
                lv_color_t * px_color = (lv_color_t *) &map_p[(uint32_t)(col - masked_a.x1) * sizeof(lv_color_t)];

                if(chroma_key && chroma_key_color.full == px_color->full) continue;

                if(recolor_opa != LV_OPA_TRANSP) {
                    lv_color_t recolored_px = lv_color_mix(recolor, *px_color, recolor_opa);

                    lv_rpx(col, row, mask_p, recolored_px, LV_OPA_COVER);
                } else {
                    lv_rpx(col, row, mask_p, *px_color, LV_OPA_COVER);
                }

            }
            map_p += map_width * sizeof(lv_color_t);               /*Next row on the map*/
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif  /*USE_LV_REAL_DRAW*/
