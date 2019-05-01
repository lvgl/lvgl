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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "lv_symbol_def.h"

/*********************
 *      DEFINES
 *********************/
/*Number of fractional digits in the advanced width (`adv_w`) field of `lv_font_glyph_dsc_t`*/
#define LV_FONT_WIDTH_FRACT_DIGIT       4

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    int32_t next_unicode    :23;
    int32_t space           :9;       /*5 integer,  4 fractional*/
}lv_font_kern_t;

typedef struct
{
    uint16_t adv_w; /*The glyph needs this space. Draw the next glyph after this width. 8 bit integer, 4 bit fractional */
    uint8_t box_w;  /*Width of the glyph's bounding box*/
    uint8_t box_h;  /*Height of the glyph's bounding box*/
    int8_t ofs_x;   /*x offset of the bounding box*/
    int8_t ofs_y;  /*y offset of the bounding box*/
    uint8_t bpp;   /*Bit-per-pixel: 1, 2, 4, 8*/
    const lv_font_kern_t * kern_table;
}lv_font_glyph_dsc_t;

typedef struct _lv_font_struct
{
    uint32_t unicode_first;
    uint32_t unicode_last;

    /*Get a glyph's  descriptor from a font*/
    bool (*get_glyph_dsc)(const struct _lv_font_struct *, lv_font_glyph_dsc_t *, uint32_t letter);

    /*Get a glyph's bitmap from a font*/
    const uint8_t * (*get_glyph_bitmap)(const struct _lv_font_struct *, uint32_t);

    /*Pointer to the font in a font pack (must have the same line height)*/
    struct _lv_font_struct * next_page;
    uint8_t size;             /*The original size (height)*/
    uint8_t line_height;      /*The real line height where any text fits*/
    uint8_t base_line;        /*Base line measured from the top of the line_height*/
    void * dsc;               /*Store implementation specific data here*/
} lv_font_t;

typedef struct
{
    uint32_t bitmap_index : 20;     /* Start index of the bitmap. A font can be max 1 MB. */
    uint32_t adv_w :12;             /*The glyph needs this space. Draw the next glyph after this width. 8 bit integer, 4 bit fractional */

    uint8_t box_w;                  /*Width of the glyph's bounding box*/
    uint8_t box_h;                  /*Height of the glyph's bounding box*/
    int8_t ofs_x;                   /*x offset of the bounding box*/
    int8_t ofs_y;                   /*y offset of the bounding box*/

    const lv_font_kern_t * kern_table;
}lv_font_glyph_dsc_built_in_t;


typedef struct {
    const uint8_t * glyph_bitmap;
    const lv_font_glyph_dsc_built_in_t * glyph_dsc;
    const uint16_t * unicode_list;
    uint16_t glyph_cnt;        /*Number of glyphs in the font. */
    uint8_t bpp;               /*Bit per pixel: 1, 2, 4 or 8*/
}lv_font_dsc_built_in_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the fonts
 */
void lv_font_init(void);

/**
 * Add a font to an other to extend the character set.
 * @param child the font to add
 * @param parent this font will be extended. Using it later will contain the characters from `child`
 */
void lv_font_add(lv_font_t * child, lv_font_t * parent);

/**
 * Remove a font from a character set.
 * @param child the font to remove
 * @param parent remove `child` from here
 */
void lv_font_remove(lv_font_t * child, lv_font_t * parent);

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return  pointer to the bitmap of the letter
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
bool lv_font_get_glyph_dsc(const lv_font_t * font_p, lv_font_glyph_dsc_t * dsc_out, uint32_t letter);

/**
 * Get the width of a glyph with kerning
 * @param font pointer to a font
 * @param letter an UNICODE letter
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return the width of the glyph
 */
uint8_t lv_font_get_glyph_width(const lv_font_t * font, uint32_t letter, uint32_t letter_next);

/**
 * Get the line height of a font. All characters fit into this height
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline uint8_t lv_font_get_line_height(const lv_font_t * font_p)
{
    return font_p->line_height;
}

/**
 * Used as `get_glyph_bitmap` callback in LittelvGL's native font format if the font is uncompressed.
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_glyph_bitmap_plain(const lv_font_t * font, uint32_t letter);

/**
 * Used as `get_glyph_dsc` callback in LittelvGL's native font format if the font is uncompressed.
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc_plain(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter);

/**********************
 *      MACROS
 **********************/

#define LV_FONT_DECLARE(font_name) extern lv_font_t font_name;

#define LV_FONT_SET_WIDTH(_integer, _fract) ((_integer << LV_FONT_WIDTH_FRACT_DIGIT) + _fract)
#define LV_FONT_GET_WIDTH_INT(_w)       (_w >> LV_FONT_WIDTH_FRACT_DIGIT)
#define LV_FONT_GET_WIDTH_FRACT(_w)     (_w & ((1 << LV_FONT_WIDTH_FRACT_DIGIT) -1))


/**********************
 * ADD BUILT IN FONTS
 **********************/
#include "../lv_fonts/lv_font_builtin.h"

/*Declare the custom (user defined) fonts*/
#ifdef LV_FONT_CUSTOM_DECLARE
LV_FONT_CUSTOM_DECLARE
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_FONT*/
