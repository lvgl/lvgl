/**
 * @file lv_font.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"

#include <stddef.h>
#include "lv_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t glyph_index;
    uint32_t unicode;
    uint8_t w_px;
}asd_glyph_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the built-in fonts
 */
void lv_font_init(void)
{
    /*DEJAVU 10*/
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10, NULL);
#endif

#if USE_LV_FONT_DEJAVU_10_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10_latin_sup, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_dejavu_10_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_10_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10_cyrillic, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_dejavu_10_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 10*/
#if USE_LV_FONT_SYMBOL_10 != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_symbol_10, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_symbol_10, NULL);
#endif
#endif


    /*DEJAVU 20*/
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20, NULL);
#endif

#if USE_LV_FONT_DEJAVU_20_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20_latin_sup, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_symbol_20_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_20_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20_cyrillic, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_dejavu_20_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 20*/
#if USE_LV_FONT_SYMBOL_20 != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_symbol_20, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_symbol_20, NULL);
#endif
#endif


    /*DEJAVU 30*/
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30, NULL);
#endif

#if USE_LV_FONT_DEJAVU_30_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30_latin_sup, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_dejavu_30_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_30_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30_cyrillic, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_dejavu_30_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 30*/
#if USE_LV_FONT_SYMBOL_30 != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_symbol_30, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_symbol_30_basic, NULL);
#endif
#endif

    /*DEJAVU 40*/
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40, NULL);
#endif

#if USE_LV_FONT_DEJAVU_40_LATIN_SUP != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40_latin_sup, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_dejavu_40_latin_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_40_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40_cyrillic, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_dejavu_40_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 40*/
#if USE_LV_FONT_SYMBOL_40 != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_symbol_40, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_symbol_40, NULL);
#endif
#endif

}

/**
 * Create a pair from font name and font dsc. get function. After it 'font_get' can be used for this font
 * @param name name of the font
 * @param dsc_get_fp the font descriptor get function
 * @param parent add this font as charter set extension of 'parent'
 */
void lv_font_add(lv_font_t *child, lv_font_t *parent)
{
    if(parent == NULL) return;

    while(parent->next_page != NULL) {
        parent = parent->next_page; /*Got to the last page and add the new font there*/
    }

    parent->next_page = child;

}

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter a letter
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * lv_font_get_bitmap(const lv_font_t * font_p, uint32_t letter)
{
    const lv_font_t * font_i = font_p;
    while(font_i != NULL) {
        const uint8_t * bitmap = font_i->get_bitmap(font_i, letter);
        if(bitmap) return bitmap;

        font_i = font_i->next_page;
    }

    return NULL;
}

/**
 * Get the width of a letter in a font
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
uint8_t lv_font_get_width(const lv_font_t * font_p, uint32_t letter)
{
    const lv_font_t * font_i = font_p;
    int16_t w;
    while(font_i != NULL) {
        w = font_i->get_width(font_i, letter);
        if(w >= 0) return w;

        font_i = font_i->next_page;
    }

    return 0;
}

/**
 * Get the bit-per-pixel of font
 * @param font pointer to font
 * @param letter a letter from font (font extensions can have different bpp)
 * @return bpp of the font (or font extension)
 */
uint8_t lv_font_get_bpp(const lv_font_t * font, uint32_t letter)
{
    const lv_font_t * font_i = font;
    while(font_i != NULL) {
    if(letter >= font_i->unicode_first && letter <= font_i->unicode_last) {
        return font_i->bpp;
    }
    font_i = font_i->next_page;
}

return 0;

}

/**
 * Generic bitmap get function used in 'font->get_bitmap' when the font contains all characters in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_bitmap_continuous(const lv_font_t * font, uint32_t unicode_letter)
{
    /*Check the range*/
    if(unicode_letter < font->unicode_first || unicode_letter > font->unicode_last) return NULL;

    uint32_t index = (unicode_letter - font->unicode_first);
    return &font->glyph_bitmap[font->glyph_dsc[index].glyph_index];
}

/**
 * Generic bitmap get function used in 'font->get_bitmap' when the font NOT contains all characters in the range (sparse)
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_bitmap_sparse(const lv_font_t * font, uint32_t unicode_letter)
{
    /*Check the range*/
    if(unicode_letter < font->unicode_first || unicode_letter > font->unicode_last) return NULL;

    uint32_t i;
    for(i = 0; font->unicode_list[i] != 0; i++) {
        if(font->unicode_list[i] == unicode_letter) {
            return &font->glyph_bitmap[font->glyph_dsc[i].glyph_index];
        }
    }

    return NULL;
}

/**
 * Generic glyph width get function used in 'font->get_width' when the font contains all characters in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the gylph or -1 if not found
 */
int16_t lv_font_get_width_continuous(const lv_font_t * font, uint32_t unicode_letter)
{
    /*Check the range*/
    if(unicode_letter < font->unicode_first || unicode_letter > font->unicode_last) {
        return -1;
    }

    uint32_t index = (unicode_letter - font->unicode_first);
    return font->glyph_dsc[index].w_px;
}

/**
 * Generic glyph width get function used in 'font->get_bitmap' when the font NOT contains all characters in the range (sparse)
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the glyph or -1 if not found
 */
int16_t lv_font_get_width_sparse(const lv_font_t * font, uint32_t unicode_letter)
{
    /*Check the range*/
    if(unicode_letter < font->unicode_first || unicode_letter > font->unicode_last) return -1;

    uint32_t i;
    for(i = 0; font->unicode_list[i] != 0; i++) {
        if(font->unicode_list[i] == unicode_letter) {
            return font->glyph_dsc[i].w_px;
        }
    }

    return -1;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
