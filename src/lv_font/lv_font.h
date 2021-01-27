/**
 * @file lv_font.h
 *
 */

#ifndef LV_FONT_H
#define LV_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "lv_symbol_def.h"
#include "../lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*------------------
 * General types
 *-----------------*/

/** Describes the properties of a glyph. */
typedef struct {
    uint16_t adv_w; /**< The glyph needs this space. Draw the next glyph after this width. */
    uint16_t box_w;  /**< Width of the glyph's bounding box*/
    uint16_t box_h;  /**< Height of the glyph's bounding box*/
    int16_t ofs_x;   /**< x offset of the bounding box*/
    int16_t ofs_y;  /**< y offset of the bounding box*/
    uint8_t bpp;   /**< Bit-per-pixel: 1, 2, 4, 8*/
} lv_font_glyph_dsc_t;

/** The bitmaps might be upscaled by 3 to achieve subpixel rendering. */
enum {
    LV_FONT_SUBPX_NONE,
    LV_FONT_SUBPX_HOR,
    LV_FONT_SUBPX_VER,
    LV_FONT_SUBPX_BOTH,
};

typedef uint8_t lv_font_subpx_t;

/** Describe the properties of a font*/
typedef struct _lv_font_struct {
    /** Get a glyph's descriptor from a font*/
    bool (*get_glyph_dsc)(const struct _lv_font_struct *, lv_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    /** Get a glyph's bitmap from a font*/
    const uint8_t * (*get_glyph_bitmap)(const struct _lv_font_struct *, uint32_t);

    /*Pointer to the font in a font pack (must have the same line height)*/
    lv_coord_t line_height;         /**< The real line height where any text fits*/
    lv_coord_t base_line;           /**< Base line measured from the top of the line_height*/
    uint8_t subpx  : 2;             /**< An element of `lv_font_subpx_t`*/

    int8_t underline_position;      /**< Distance between the top of the underline and base line (< 0 means below the base line)*/
    int8_t underline_thickness;     /**< Thickness of the underline*/

    void * dsc;                     /**< Store implementation specific or run_time data or caching here*/
#if LV_USE_USER_DATA
    lv_font_user_data_t user_data;  /**< Custom user data for font. */
#endif

} lv_font_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return pointer to the bitmap of the letter
 */
const uint8_t * lv_font_get_glyph_bitmap(const lv_font_t * font_p, uint32_t letter);

/**
 * Get the descriptor of a glyph
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc(const lv_font_t * font_p, lv_font_glyph_dsc_t * dsc_out, uint32_t letter,
                           uint32_t letter_next);

/**
 * Get the width of a glyph with kerning
 * @param font pointer to a font
 * @param letter an UNICODE letter
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return the width of the glyph
 */
uint16_t lv_font_get_glyph_width(const lv_font_t * font, uint32_t letter, uint32_t letter_next);

/**
 * Get the line height of a font. All characters fit into this height
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline lv_coord_t lv_font_get_line_height(const lv_font_t * font_p)
{
    return font_p->line_height;
}

/**********************
 *      MACROS
 **********************/

#define LV_FONT_DECLARE(font_name) extern lv_font_t font_name;

#if LV_FONT_MONTSERRAT_8
LV_FONT_DECLARE(lv_font_montserrat_8)
#endif

#if LV_FONT_MONTSERRAT_10
LV_FONT_DECLARE(lv_font_montserrat_10)
#endif

#if LV_FONT_MONTSERRAT_12
LV_FONT_DECLARE(lv_font_montserrat_12)
#endif

#if LV_FONT_MONTSERRAT_14
LV_FONT_DECLARE(lv_font_montserrat_14)
#endif

#if LV_FONT_MONTSERRAT_16
LV_FONT_DECLARE(lv_font_montserrat_16)
#endif

#if LV_FONT_MONTSERRAT_18
LV_FONT_DECLARE(lv_font_montserrat_18)
#endif

#if LV_FONT_MONTSERRAT_20
LV_FONT_DECLARE(lv_font_montserrat_20)
#endif

#if LV_FONT_MONTSERRAT_22
LV_FONT_DECLARE(lv_font_montserrat_22)
#endif

#if LV_FONT_MONTSERRAT_24
LV_FONT_DECLARE(lv_font_montserrat_24)
#endif

#if LV_FONT_MONTSERRAT_26
LV_FONT_DECLARE(lv_font_montserrat_26)
#endif

#if LV_FONT_MONTSERRAT_28
LV_FONT_DECLARE(lv_font_montserrat_28)
#endif

#if LV_FONT_MONTSERRAT_30
LV_FONT_DECLARE(lv_font_montserrat_30)
#endif

#if LV_FONT_MONTSERRAT_32
LV_FONT_DECLARE(lv_font_montserrat_32)
#endif

#if LV_FONT_MONTSERRAT_34
LV_FONT_DECLARE(lv_font_montserrat_34)
#endif

#if LV_FONT_MONTSERRAT_36
LV_FONT_DECLARE(lv_font_montserrat_36)
#endif

#if LV_FONT_MONTSERRAT_38
LV_FONT_DECLARE(lv_font_montserrat_38)
#endif

#if LV_FONT_MONTSERRAT_40
LV_FONT_DECLARE(lv_font_montserrat_40)
#endif

#if LV_FONT_MONTSERRAT_42
LV_FONT_DECLARE(lv_font_montserrat_42)
#endif

#if LV_FONT_MONTSERRAT_44
LV_FONT_DECLARE(lv_font_montserrat_44)
#endif

#if LV_FONT_MONTSERRAT_46
LV_FONT_DECLARE(lv_font_montserrat_46)
#endif

#if LV_FONT_MONTSERRAT_48
LV_FONT_DECLARE(lv_font_montserrat_48)
#endif

#if LV_FONT_MONTSERRAT_28_COMPRESSED
LV_FONT_DECLARE(lv_font_montserrat_28_compressed)
#endif

#if LV_FONT_MONTSERRAT_12_SUBPX
LV_FONT_DECLARE(lv_font_montserrat_12_subpx)
#endif

#if LV_FONT_UNSCII_8
LV_FONT_DECLARE(lv_font_unscii_8)
#endif

#if LV_FONT_UNSCII_16
LV_FONT_DECLARE(lv_font_unscii_16)
#endif

#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
LV_FONT_DECLARE(lv_font_dejavu_16_persian_hebrew)
#endif

#if LV_FONT_SIMSUN_16_CJK
LV_FONT_DECLARE(lv_font_simsun_16_cjk)
#endif

/*Declare the custom (user defined) fonts*/
#ifdef LV_FONT_CUSTOM_DECLARE
LV_FONT_CUSTOM_DECLARE
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_FONT*/
