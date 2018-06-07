/**
 * @file lv_draw.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"

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
void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_vpx;
void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_vfill;
void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_vletter;
#  if USE_LV_IMG
void (*map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
        const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
        lv_color_t recolor, lv_opa_t recolor_opa) = lv_vmap;
#  endif /*USE_LV_IMG*/
#elif USE_LV_REAL_DRAW != 0
/* px_fp used only by shadow drawing and anti aliasing
 * static void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_rpx;
 */
static void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_rfill;
static void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_rletter;
#  if USE_LV_IMG
static void (*map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
        const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
        lv_color_t recolor, lv_opa_t recolor_opa) = lv_rmap;
#  endif /*USE_LV_IMG*/
#else
/*Invalid settings. Compiler error will be thrown*/
static void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = NULL;
static void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  NULL;
static void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = NULL;
static void (*map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
        const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
        lv_color_t recolor, lv_opa_t recolor_opa) = NULL;
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
 lv_opa_t antialias_get_opa(lv_coord_t seg, lv_coord_t px_id, lv_opa_t line_opa)
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
                                          seg5, seg6, seg7, seg8};

     if(seg == 0) return LV_OPA_TRANSP;
     else if(seg < 8) return (uint32_t)((uint32_t)seg_map[seg - 1][px_id] * line_opa) >> 8;
     else {
         return ((px_id * 2 + 1) * line_opa) / (2 * seg);
     }

 }



#endif

