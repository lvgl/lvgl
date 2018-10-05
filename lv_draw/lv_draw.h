/**
 * @file lv_draw.h
 *
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#include "../lv_core/lv_style.h"
#include "../lv_misc/lv_txt.h"

/*********************
 *      DEFINES
 *********************/
/*If image pixels contains alpha we need to know how much byte is a pixel*/
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
# define LV_IMG_PX_SIZE_ALPHA_BYTE   2
#elif LV_COLOR_DEPTH == 16
# define LV_IMG_PX_SIZE_ALPHA_BYTE   3
#elif LV_COLOR_DEPTH == 32
# define LV_IMG_PX_SIZE_ALPHA_BYTE   4
#endif

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_IMG_SRC_VARIABLE,
    LV_IMG_SRC_FILE,
    LV_IMG_SRC_SYMBOL,
    LV_IMG_SRC_UNKNOWN,
};
typedef uint8_t lv_img_src_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_ANTIALIAS != 0

/**
 * Get the opacity of a pixel based it's position in a line segment
 * @param seg segment length
 * @param px_id position of  of a pixel which opacity should be get [0..seg-1]
 * @param base_opa the base opacity
 * @return the opacity of the given pixel
 */
lv_opa_t lv_draw_aa_get_opa(lv_coord_t seg, lv_coord_t px_id, lv_opa_t base_opa);

/**
 * Add a vertical  anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void lv_draw_aa_ver_seg(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa);

/**
 * Add a horizontal anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void lv_draw_aa_hor_seg(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa);
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern void (*const px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa);
extern void (*const fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa);
extern void (*const letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa);
extern void (*const map_fp)(const lv_area_t * cords_p, const lv_area_t * mask_p,
                            const uint8_t * map_p, lv_opa_t opa, bool chroma_key, bool alpha_byte,
                            lv_color_t recolor, lv_opa_t recolor_opa);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/
#include "lv_draw_rect.h"
#include "lv_draw_label.h"
#include "lv_draw_img.h"
#include "lv_draw_line.h"
#include "lv_draw_triangle.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_DRAW_H*/
