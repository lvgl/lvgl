/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "lv_draw_rbasic.h"
#include "lv_draw_vbasic.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_ufs.h"
#include "../lv_objx/lv_img.h"

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

#if LV_VDB_SIZE != 0
void (*const px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_vpx;
void (*const fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_vfill;
void (*const letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_vletter;
void (*const map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
                     const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
                     lv_color_t recolor, lv_opa_t recolor_opa) = lv_vmap;
#else
void (*const px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_rpx;
void (*const fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_rfill;
void (*const letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_rletter;
void (*const map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
                     const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
                     lv_color_t recolor, lv_opa_t recolor_opa) = lv_rmap;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_ANTIALIAS != 0

/**
 * Get the opacity of a pixel based it's position in a line segment
 * @param seg segment length
 * @param px_id position of  of a pixel which opacity should be get [0..seg-1]
 * @param base_opa the base opacity
 * @return the opacity of the given pixel
 */
lv_opa_t lv_draw_aa_get_opa(lv_coord_t seg, lv_coord_t px_id, lv_opa_t base_opa)
{
    /* How to calculate the opacity of pixels on the edges which makes the anti-aliasing?
     * For example we have a line like this (y = -0.5 * x):
     *
     *  | _ _
     *    * * |
     *
     * Anti-aliased pixels come to the '*' characters
     * Calculate what percentage of the pixels should be covered if real line (not rasterized) would be drawn:
     * 1. A real line should start on (0;0) and end on (2;1)
     * 2. So the line intersection coordinates on the first pixel: (0;0) (1;0.5) -> 25% covered pixel in average
     * 3. For the second pixel: (1;0.5) (2;1) -> 75% covered pixel in average
     * 4. The equation: (px_id * 2 + 1) / (segment_width * 2)
     *                   segment_width: the line segment which is being anti-aliased (was 2 in the example)
     *                   px_id: pixel ID from 0 to  (segment_width - 1)
     *                   result: [0..1] coverage of the pixel
     */

    /*Accelerate the common segment sizes to avoid division*/
    static const  lv_opa_t seg1[1] = {128};
    static const  lv_opa_t seg2[2] = {64, 192};
    static const  lv_opa_t seg3[3] = {42, 128, 212};
    static const  lv_opa_t seg4[4] = {32, 96, 159, 223};
    static const  lv_opa_t seg5[5] = {26,  76, 128, 178, 230};
    static const  lv_opa_t seg6[6] = {21,  64, 106, 148, 191, 234};
    static const  lv_opa_t seg7[7] = {18,  55, 91, 128, 164, 200, 237};
    static const  lv_opa_t seg8[8] = {16,  48, 80, 112, 143, 175, 207, 239};

    static const lv_opa_t * seg_map[] = {seg1, seg2, seg3, seg4,
                                         seg5, seg6, seg7, seg8
                                        };

    if(seg == 0) return LV_OPA_TRANSP;
    else if(seg < 8) return (uint32_t)((uint32_t)seg_map[seg - 1][px_id] * base_opa) >> 8;
    else {
        return ((px_id * 2 + 1) * base_opa) / (2 * seg);
    }

}

/**
 * Add a vertical  anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void lv_draw_aa_ver_seg(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa)
{
    bool aa_inv = false;
    if(length < 0) {
        aa_inv = true;
        length = -length;
    }

    lv_coord_t i;
    for(i = 0; i < length; i++) {
        lv_opa_t px_opa = lv_draw_aa_get_opa(length, i, opa);
        if(aa_inv) px_opa = opa - px_opa;
        px_fp(x, y + i, mask, color, px_opa);
    }
}

/**
 * Add a horizontal anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void lv_draw_aa_hor_seg(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa)
{
    bool aa_inv = false;
    if(length < 0) {
        aa_inv = true;
        length = -length;
    }

    lv_coord_t i;
    for(i = 0; i < length; i++) {
        lv_opa_t px_opa = lv_draw_aa_get_opa(length, i, opa);
        if(aa_inv) px_opa = opa - px_opa;
        px_fp(x + i, y, mask, color, px_opa);
    }
}

#endif
