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
#include "../draw/lv_draw_buf.h"
#include "../misc/lv_area.h"
#include "../misc/lv_types.h"
#include "../misc/cache/lv_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*------------------
 * General types
 *-----------------*/

/** The font format.*/
enum _lv_font_glyph_format_t {
    LV_FONT_GLYPH_FORMAT_NONE   = 0, /**< Maybe not visible*/

    /**< Legacy simple formats*/
    LV_FONT_GLYPH_FORMAT_A1     = 0x01, /**< 1 bit per pixel*/
    LV_FONT_GLYPH_FORMAT_A2     = 0x02, /**< 2 bit per pixel*/
    LV_FONT_GLYPH_FORMAT_A4     = 0x04, /**< 4 bit per pixel*/
    LV_FONT_GLYPH_FORMAT_A8     = 0x08, /**< 8 bit per pixel*/

    LV_FONT_GLYPH_FORMAT_IMAGE  = 0x09, /**< Image format*/

    /**< Advanced formats*/
    LV_FONT_GLYPH_FORMAT_VECTOR = 0x0A, /**< Vectorial format*/
    LV_FONT_GLYPH_FORMAT_SVG    = 0x0B, /**< SVG format*/
    LV_FONT_GLYPH_FORMAT_CUSTOM = 0xFF, /**< Custom format*/
};

#ifdef DOXYGEN
typedef _lv_font_glyph_format_t lv_font_glyph_format_t;
#else
typedef uint8_t lv_font_glyph_format_t;
#endif /*DOXYGEN*/

/** Describes the properties of a glyph.*/
typedef struct {
    const lv_font_t *
    resolved_font; /**< Pointer to a font where the glyph was actually found after handling fallbacks*/
    uint16_t adv_w; /**< The glyph needs this space. Draw the next glyph after this width.*/
    uint16_t box_w; /**< Width of the glyph's bounding box*/
    uint16_t box_h; /**< Height of the glyph's bounding box*/
    int16_t ofs_x;  /**< x offset of the bounding box*/
    int16_t ofs_y;  /**< y offset of the bounding box*/
    lv_font_glyph_format_t format;  /**< Font format of the glyph see @lv_font_glyph_format_t*/
    uint8_t is_placeholder: 1; /**< Glyph is missing. But placeholder will still be displayed*/

    uint32_t glyph_index; /**< The index of the glyph in the font file. Used by the font cache*/
    lv_cache_entry_t * entry; /**< The cache entry of the glyph draw data. Used by the font cache*/
} lv_font_glyph_dsc_t;

/** The bitmaps might be upscaled by 3 to achieve subpixel rendering.*/
enum _lv_font_subpx_t {
    LV_FONT_SUBPX_NONE,
    LV_FONT_SUBPX_HOR,
    LV_FONT_SUBPX_VER,
    LV_FONT_SUBPX_BOTH,
};

#ifdef DOXYGEN
typedef _lv_font_subpx_t lv_font_subpx_t;
#else
typedef uint8_t lv_font_subpx_t;
#endif /*DOXYGEN*/

/** Adjust letter spacing for specific character pairs.*/
enum _lv_font_kerning_t {
    LV_FONT_KERNING_NORMAL,
    LV_FONT_KERNING_NONE,
};

#ifdef DOXYGEN
typedef _lv_font_kerning_t lv_font_kerning_t;
#else
typedef uint8_t lv_font_kerning_t;
#endif /*DOXYGEN*/

/** Describe the properties of a font*/
struct _lv_font_t {
    /** Get a glyph's descriptor from a font*/
    bool (*get_glyph_dsc)(const lv_font_t *, lv_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    /** Get a glyph's bitmap from a font*/
    const void * (*get_glyph_bitmap)(lv_font_glyph_dsc_t *, uint32_t, lv_draw_buf_t *);

    /** Release a glyph*/
    void (*release_glyph)(const lv_font_t *, lv_font_glyph_dsc_t *);

    /*Pointer to the font in a font pack (must have the same line height)*/
    int32_t line_height;         /**< The real line height where any text fits*/
    int32_t base_line;           /**< Base line measured from the top of the line_height*/
    uint8_t subpx   : 2;            /**< An element of `lv_font_subpx_t`*/
    uint8_t kerning : 1;            /**< An element of `lv_font_kerning_t`*/

    int8_t underline_position;      /**< Distance between the top of the underline and base line (< 0 means below the base line)*/
    int8_t underline_thickness;     /**< Thickness of the underline*/

    const void * dsc;               /**< Store implementation specific or run_time data or caching here*/
    const lv_font_t * fallback;   /**< Fallback font for missing glyph. Resolved recursively */
    void * user_data;               /**< Custom user data for font.*/
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Return with the bitmap of a font.
 * @param g_dsc         the glyph descriptor including which font to use etc.
 * @param letter        a UNICODE character code
 * @param draw_buf      a draw buffer that can be used to store the bitmap of the glyph, it's OK not to use it.
 * @return pointer to the glyph's data. It can be a draw buffer for bitmap fonts or an image source for imgfonts.
 */
const void * lv_font_get_glyph_bitmap(lv_font_glyph_dsc_t * g_dsc, uint32_t letter,
                                      lv_draw_buf_t * draw_buf);

/**
 * Get the descriptor of a glyph
 * @param font          pointer to font
 * @param dsc_out       store the result descriptor here
 * @param letter        a UNICODE letter code
 * @param letter_next   the next letter after `letter`. Used for kerning
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t letter,
                           uint32_t letter_next);

/**
 * Get the width of a glyph with kerning
 * @param font          pointer to a font
 * @param letter        a UNICODE letter
 * @param letter_next   the next letter after `letter`. Used for kerning
 * @return the width of the glyph
 */
uint16_t lv_font_get_glyph_width(const lv_font_t * font, uint32_t letter, uint32_t letter_next);

/**
 * Get the line height of a font. All characters fit into this height
 * @param font      pointer to a font
 * @return the height of a font
 */
static inline int32_t lv_font_get_line_height(const lv_font_t * font)
{
    return font->line_height;
}

/**
 * Configure the use of kerning information stored in a font
 * @param font    pointer to a font
 * @param kerning `LV_FONT_KERNING_NORMAL` (default) or `LV_FONT_KERNING_NONE`
 */
void lv_font_set_kerning(lv_font_t * font, lv_font_kerning_t kerning);

/**********************
 *      MACROS
 **********************/

#define LV_FONT_DECLARE(font_name) LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_t font_name;

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

#if LV_FONT_DEJAVU_16_PERSIAN_HEBREW
LV_FONT_DECLARE(lv_font_dejavu_16_persian_hebrew)
#endif

#if LV_FONT_SIMSUN_16_CJK
LV_FONT_DECLARE(lv_font_simsun_16_cjk)
#endif

#if LV_FONT_UNSCII_8
LV_FONT_DECLARE(lv_font_unscii_8)
#endif

#if LV_FONT_UNSCII_16
LV_FONT_DECLARE(lv_font_unscii_16)
#endif

/*Declare the custom (user defined) fonts*/
#ifdef LV_FONT_CUSTOM_DECLARE
LV_FONT_CUSTOM_DECLARE
#endif

/**
 * Just a wrapper around LV_FONT_DEFAULT because it might be more convenient to use a function in some cases
 * @return  pointer to LV_FONT_DEFAULT
 */
static inline const lv_font_t * lv_font_default(void)
{
    return LV_FONT_DEFAULT;
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*USE_FONT*/
