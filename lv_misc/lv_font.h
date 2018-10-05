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
#include "../../lv_conf.h"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "lv_symbol_def.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    uint32_t w_px         :8;
    uint32_t glyph_index  :24;
} lv_font_glyph_dsc_t;

typedef struct
{
    uint32_t unicode         :21;
    uint32_t glyph_dsc_index :11;
} lv_font_unicode_map_t;

typedef struct _lv_font_struct
{
    uint32_t unicode_first;
    uint32_t unicode_last;
    const uint8_t * glyph_bitmap;
    const lv_font_glyph_dsc_t * glyph_dsc;
    const uint32_t * unicode_list;
    const uint8_t * (*get_bitmap)(const struct _lv_font_struct *,uint32_t);     /*Get a glyph's  bitmap from a font*/
    int16_t (*get_width)(const struct _lv_font_struct *,uint32_t);        /*Get a glyph's with with a given font*/
    struct _lv_font_struct * next_page;    /*Pointer to a font extension*/
    uint32_t h_px       :8;
    uint32_t bpp        :4;                /*Bit per pixel: 1, 2 or 4*/
    uint32_t monospace  :8;                /*Fix width (0: normal width)*/
    uint16_t glyph_cnt;                    /*Number of glyphs (letters) in the font*/
} lv_font_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the fonts
 */
void lv_font_init(void);

/**
 * Create a pair from font name and font dsc. get function. After it 'font_get' can be used for this font
 * @param child pointer to a font to join to the 'parent'
 * @param parent pointer to a font. 'child' will be joined here
 */
void lv_font_add(lv_font_t *child, lv_font_t *parent);

/**
 * Tells if font which contains `letter` is monospace or not
 * @param font_p point to font
 * @param letter an UNICODE character code
 * @return true: the letter is monospace; false not monospace
 */
bool lv_font_is_monospace(const lv_font_t * font_p, uint32_t letter);

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * lv_font_get_bitmap(const lv_font_t * font_p, uint32_t letter);

/**
 * Get the width of a letter in a font. If `monospace` is set then return with it.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return the width of a letter
 */
uint8_t lv_font_get_width(const lv_font_t * font_p, uint32_t letter);


/**
 * Get the width of the letter without overwriting it with the `monospace` attribute
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return the width of a letter
 */
uint8_t lv_font_get_real_width(const lv_font_t * font_p, uint32_t letter);

/**
 * Get the height of a font
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline uint8_t lv_font_get_height(const lv_font_t * font_p)
{
    return font_p->h_px;
}

/**
 * Get the bit-per-pixel of font
 * @param font pointer to font
 * @param letter a letter from font (font extensions can have different bpp)
 * @return bpp of the font (or font extension)
 */
uint8_t lv_font_get_bpp(const lv_font_t * font, uint32_t letter);

/**
 * Generic bitmap get function used in 'font->get_bitmap' when the font contains all characters in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_bitmap_continuous(const lv_font_t * font, uint32_t unicode_letter);

/**
 * Generic bitmap get function used in 'font->get_bitmap' when the font NOT contains all characters in the range (sparse)
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_bitmap_sparse(const lv_font_t * font, uint32_t unicode_letter);
/**
 * Generic glyph width get function used in 'font->get_width' when the font contains all characters in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the gylph or -1 if not found
 */
int16_t lv_font_get_width_continuous(const lv_font_t * font, uint32_t unicode_letter);

/**
 * Generic glyph width get function used in 'font->get_bitmap' when the font NOT contains all characters in the range (sparse)
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the glyph or -1 if not found
 */
int16_t lv_font_get_width_sparse(const lv_font_t * font, uint32_t unicode_letter);

/**********************
 *      MACROS
 **********************/

#define LV_FONT_DECLARE(font_name) extern lv_font_t font_name;


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

