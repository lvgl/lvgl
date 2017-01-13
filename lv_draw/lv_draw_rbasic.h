/**
 * @file lv_draw_rbasic..h
 * 
 */

#ifndef LV_DRAW_RBASIC_H
#define LV_DRAW_RBASIC_H

/*********************
 *      INCLUDES
 *********************/
#include "misc/others/color.h"
#include "../lv_misc/area.h"
#include "../lv_misc/font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Fill an area on the display
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask
 * @param color fill color
 * @param opa opacity (ignored, only for compatibility with lv_vfill)
 */
void lv_rfill(const area_t * cords_p, const area_t * mask_p,
                color_t color, opa_t opa);

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
                color_t color, opa_t opa);

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
            color_t recolor, opa_t recolor_opa);

/**********************
 *      MACROS
 **********************/

#endif
