/**
 * @file font.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_FONT != 0

#include <stddef.h>
#include "lv_font.h"

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
void font_init(void)
{

    /*DEJAVU 10*/
#if USE_FONT_DEJAVU_10 != 0
    font_add(&font_dejavu_10, NULL);
#endif

#if USE_FONT_DEJAVU_10_SUP != 0
#if USE_FONT_DEJAVU_10 != 0
    font_add(&font_dejavu_10_sup, &font_dejavu_10);
#else
    font_add(&font_dejavu_10_sup, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_10_LATIN_EXT_A != 0
#if USE_FONT_DEJAVU_10 != 0
    font_add(&font_dejavu_10_latin_ext_a, &font_dejavu_10);
#else
    font_add(&font_dejavu_10_latine_ext_a, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_10_LATIN_EXT_B != 0
#if USE_FONT_DEJAVU_10 != 0
    font_add(&font_dejavu_10_latin_ext_b, &font_dejavu_10);
#else
    font_add(&font_dejavu_10_latin_ext_b, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_10_CYRILLIC != 0
#if USE_FONT_DEJAVU_10 != 0
    font_add(&font_dejavu_10_cyrillic, &font_dejavu_10);
#else
    font_add(&font_dejavu_10_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 10*/
#if USE_FONT_SYMBOL_10_BASIC != 0
#if USE_FONT_DEJAVU_10 != 0
    font_add(&font_symbol_10_basic, &font_dejavu_10);
#else
    font_add(&font_symbol_10_basic, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_10_FILE != 0
#if USE_FONT_SYMBOL_10_BASIC != 0
    font_add(&font_symbol_10_file, &font_symbol_10_basic);
#else
    font_add(&font_symbol_10_file, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_10_FEEDBACK != 0
#if USE_FONT_SYMBOL_10_BASIC != 0
    font_add(&font_symbol_10_feedback, &font_symbol_10_basic);
#else
    font_add(&font_symbol_10_feedback, NULL);
#endif
#endif

    /*DEJAVU 20*/
#if USE_FONT_DEJAVU_20 != 0
    font_add(&font_dejavu_20, NULL);
#endif

#if USE_FONT_DEJAVU_20_SUP != 0
#if USE_FONT_DEJAVU_20 != 0
    font_add(&font_dejavu_20_sup, &font_dejavu_20);
#else
    font_add(&font_symbol_20_sup, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_20_LATIN_EXT_A != 0
#if USE_FONT_DEJAVU_20 != 0
    font_add(&font_dejavu_20_latin_ext_a, &font_dejavu_20);
#else
    font_add(&font_dejavu_20_latin_ext_a, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_20_LATIN_EXT_B != 0
#if USE_FONT_DEJAVU_20 != 0
    font_add(&font_dejavu_20_latin_ext_b, &font_dejavu_20);
#else
    font_add(&font_dejavu_20_latin_ext_b, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_20_CYRILLIC != 0
#if USE_FONT_DEJAVU_20 != 0
    font_add(&font_dejavu_20_cyrillic, &font_dejavu_20);
#else
    font_add(&font_dejavu_20_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 20*/
#if USE_FONT_SYMBOL_20_BASIC != 0
#if USE_FONT_DEJAVU_20 != 0
    font_add(&font_symbol_20_basic, &font_dejavu_20);
#else
    font_add(&font_symbol_20_basic, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_20_FILE != 0
#if USE_FONT_SYMBOL_20_BASIC != 0
    font_add(&font_symbol_20_file, &font_symbol_20_basic);
#else
    font_add(&font_symbol_20_file, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_20_FEEDBACK != 0
#if USE_FONT_SYMBOL_20_BASIC != 0
    font_add(&font_symbol_20_feedback, &font_symbol_20_basic);
#else
    font_add(&font_symbol_20_feedback, NULL);
#endif
#endif

    /*DEJAVU 30*/
#if USE_FONT_DEJAVU_30 != 0
    font_add(&font_dejavu_30, NULL);
#endif

#if USE_FONT_DEJAVU_30_SUP != 0
#if USE_FONT_DEJAVU_30 != 0
    font_add(&font_dejavu_30_sup, &font_dejavu_30);
#else
    font_add(&font_dejavu_30_sup, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_30_LATIN_EXT_A != 0
#if USE_FONT_DEJAVU_30 != 0
    font_add(&font_dejavu_30_latin_ext_a, &font_dejavu_30);
#else
    font_add(&font_dejavu_30_latin_ext_a, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_30_LATIN_EXT_B != 0
#if USE_FONT_DEJAVU_30 != 0
    font_add(&font_dejavu_30_latin_ext_b, &font_dejavu_30);
#else
    font_add(&font_dejavu_30_latin_ext_b, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_30_CYRILLIC != 0
#if USE_FONT_DEJAVU_30 != 0
    font_add(&font_dejavu_30_cyrillic, &font_dejavu_30);
#else
    font_add(&font_dejavu_30_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 30*/
#if USE_FONT_SYMBOL_30_BASIC != 0
#if USE_FONT_DEJAVU_30 != 0
    font_add(&font_symbol_30_basic, &font_dejavu_30);
#else
    font_add(&font_symbol_30_basic, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_30_FILE != 0
#if USE_FONT_SYMBOL_30_BASIC != 0
    font_add(&font_symbol_30_file, &font_symbol_30_basic);
#else
    font_add(&font_symbol_30_file, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_30_FEEDBACK != 0
#if USE_FONT_SYMBOL_30_BASIC != 0
    font_add(&font_symbol_30_feedback, &font_symbol_30_basic);
#else
    font_add(&font_symbol_30_feedback, NULL);
#endif
#endif

    /*DEJAVU 40*/
#if USE_FONT_DEJAVU_40 != 0
    font_add(&font_dejavu_40, NULL);
#endif

#if USE_FONT_DEJAVU_40_SUP != 0
#if USE_FONT_DEJAVU_40 != 0
    font_add(&font_dejavu_40_sup, &font_dejavu_40);
#else
    font_add(&font_dejavu_40_sup, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_40_LATIN_EXT_A != 0
#if USE_FONT_DEJAVU_40 != 0
    font_add(&font_dejavu_40_latin_ext_a, &font_dejavu_40);
#else
    font_add(&font_dejavu_40_latin_ext_a, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_40_LATIN_EXT_B != 0
#if USE_FONT_DEJAVU_40 != 0
    font_add(&font_dejavu_40_latin_ext_b, &font_dejavu_40);
#else
    font_add(&font_dejavu_40_latin_ext_b, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_40_CYRILLIC != 0
#if USE_FONT_DEJAVU_40 != 0
    font_add(&font_dejavu_40_cyrillic, &font_dejavu_40);
#else
    font_add(&font_dejavu_40_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 40*/
#if USE_FONT_SYMBOL_40_BASIC != 0
#if USE_FONT_DEJAVU_40 != 0
    font_add(&font_symbol_40_basic, &font_dejavu_40);
#else
    font_add(&font_symbol_40_basic, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_40_FILE != 0
#if USE_FONT_SYMBOL_40_BASIC != 0
    font_add(&font_symbol_40_file, &font_symbol_40_basic);
#else
    font_add(&font_symbol_40_file, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_40_FEEDBACK != 0
#if USE_FONT_SYMBOL_40_BASIC != 0
    font_add(&font_symbol_40_feedback, &font_symbol_40_basic);
#else
    font_add(&font_symbol_40_feedback, NULL);
#endif
#endif

    /*DEJAVU 60*/
#if USE_FONT_DEJAVU_60 != 0
    font_add(&font_dejavu_60, NULL);
#endif

#if USE_FONT_DEJAVU_60_SUP != 0
#if USE_FONT_DEJAVU_60 != 0
    font_add(&font_dejavu_60_sup, &font_dejavu_60);
#else
    font_add(&font_dejavu_60_sup, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_60_LATIN_EXT_A != 0
#if USE_FONT_DEJAVU_60 != 0
    font_add(&font_dejavu_60_latin_ext_a, &font_dejavu_60);
#else
    font_add(&font_dejavu_60_latin_ext_a, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_60_LATIN_EXT_B != 0
#if USE_FONT_DEJAVU_60 != 0
    font_add(&font_dejavu_60_latin_ext_b, &font_dejavu_60);
#else
    font_add(&font_dejavu_60_latin_ext_b, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_60_CYRILLIC != 0
#if USE_FONT_DEJAVU_60 != 0
    font_add(&font_dejavu_60_cyrillic, &font_dejavu_60);
#else
    font_add(&font_dejavu_60_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 60*/
#if USE_FONT_SYMBOL_60_BASIC != 0
#if USE_FONT_DEJAVU_60 != 0
    font_add(&font_symbol_60_basic, &font_dejavu_60);
#else
    font_add(&font_symbol_60_basic, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_60_FILE != 0
#if USE_FONT_SYMBOL_60_BASIC != 0
    font_add(&font_symbol_60_file, &font_symbol_60_basic);
#else
    font_add(&font_symbol_60_file, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_60_FEEDBACK != 0
#if USE_FONT_SYMBOL_60_BASIC != 0
    font_add(&font_symbol_60_feedback, &font_symbol_60_basic);
#else
    font_add(&font_symbol_60_feedback, NULL);
#endif
#endif

    /*DEJAVU 80*/
#if USE_FONT_DEJAVU_80 != 0
    font_add(&font_dejavu_80, NULL);
#endif

#if USE_FONT_DEJAVU_80_SUP != 0
#if USE_FONT_DEJAVU_80 != 0
    font_add(&font_dejavu_80_sup, &font_dejavu_80);
#else
    font_add(&font_dejavu_80_sup, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_80_LATIN_EXT_A != 0
#if USE_FONT_DEJAVU_80 != 0
    font_add(&font_dejavu_80_latin_ext_a, &font_dejavu_80);
#else
    font_add(&font_dejavu_80_latin_ext_a, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_80_LATIN_EXT_B != 0
#if USE_FONT_DEJAVU_80 != 0
    font_add(&font_dejavu_80_latin_ext_b, &font_dejavu_80);
#else
    font_add(&font_dejavu_80_latin_ext_b, NULL);
#endif
#endif

#if USE_FONT_DEJAVU_80_CYRILLIC != 0
#if USE_FONT_DEJAVU_80 != 0
    font_add(&font_dejavu_80_cyrillic, &font_dejavu_80);
#else
    font_add(&font_dejavu_80_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 80*/
#if USE_FONT_SYMBOL_80_BASIC != 0
#if USE_FONT_DEJAVU_80 != 0
    font_add(&font_symbol_80_basic, &font_dejavu_80);
#else
    font_add(&font_symbol_80_basic, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_80_FILE != 0
#if USE_FONT_SYMBOL_80_BASIC != 0
    font_add(&font_symbol_80_file, &font_symbol_80_basic);
#else
    font_add(&font_symbol_80_file, NULL);
#endif
#endif

#if USE_FONT_SYMBOL_80_FEEDBACK != 0
#if USE_FONT_SYMBOL_80_BASIC != 0
    font_add(&font_symbol_80_feedback, &font_symbol_80_basic);
#else
    font_add(&font_symbol_80_feedback, NULL);
#endif
#endif
}

/**
 * Create a pair from font name and font dsc. get function. After it 'font_get' can be used for this font
 * @param name name of the font
 * @param dsc_get_fp the font descriptor get function
 * @param parent add this font as charter set extension of 'parent'
 */
void font_add(font_t *child, font_t *parent)
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
const uint8_t * font_get_bitmap(const font_t * font_p, uint32_t letter)
{
    const font_t * font_i = font_p;
    while(font_i != NULL) {
        if(letter >= font_i->first_ascii && letter <= font_i->last_ascii) {
            uint32_t index = (letter - font_i->first_ascii);
            return &font_i->bitmap[font_i->map[index]];
        }

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
uint8_t font_get_width(const font_t * font_p, uint32_t letter)
{
    const font_t * font_i = font_p;
    while(font_i != NULL) {
        if(letter >= font_i->first_ascii && letter <= font_i->last_ascii) {
            uint32_t index = (letter - font_i->first_ascii);
            return font_i->width[index];
        }
        font_i = font_i->next_page;
    }

    return 0;
}

/**
 * Get the width of a letter in a font )Give the real size on the screen (half size if FONT_ANTIALIAS is enabled)
 * @param font_p pointer to a font
 * @param letter a letter
 * @return the width of a letter
 */
uint8_t font_get_width_scale(const font_t * font_p, uint32_t letter)
{
    return font_get_width(font_p, letter) >> FONT_ANTIALIAS;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
         
#endif /*USE_FONT*/
