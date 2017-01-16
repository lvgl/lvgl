/**
 * @file lv_draw_vbasic.h
 * 
 */

#ifndef LV_DRAW_VBASIC_H
#define LV_DRAW_VBASIC_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"

#if LV_VDB_SIZE != 0

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
 * Fill an area in the Virtual Display Buffer
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask
 * @param color fill color
 * @param opa opacity of the area (0..255)
 */
void lv_vfill(const area_t * cords_p, const area_t * mask_p,
                color_t color, opa_t opa);

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
                color_t color, opa_t opa);

/**
 * Draw a color map to the display
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area
 * @param map_p pointer to a color_t array
 * @param opa opacity of the map (ignored, only for compatibility with lv_vmap)
 * @param transp true: enable transparency of LV_IMG_COLOR_TRANSP color pixels
 * @param upscale true: upscale to double size
 * @param recolor mix the pixels with this color
 * @param recolor_opa the intense of recoloring
 */
void lv_vmap(const area_t * cords_p, const area_t * mask_p,
            const color_t * map_p, opa_t opa, bool transp, bool upscale,
            color_t recolor, opa_t recolor_opa);



/**********************
 *      MACROS
 **********************/

#endif

#endif
