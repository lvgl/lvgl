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

#if USE_LV_FONT_DEJAVU_10_SUP != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10_sup, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_dejavu_10_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_10_LATIN_EXT_A != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10_latin_ext_a, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_dejavu_10_latine_ext_a, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_10_LATIN_EXT_B != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_dejavu_10_latin_ext_b, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_dejavu_10_latin_ext_b, NULL);
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
#if USE_LV_FONT_SYMBOL_10_BASIC != 0
#if USE_LV_FONT_DEJAVU_10 != 0
    lv_font_add(&lv_font_symbol_10_basic, &lv_font_dejavu_10);
#else
    lv_font_add(&lv_font_symbol_10_basic, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_10_FILE != 0
#if USE_LV_FONT_SYMBOL_10_BASIC != 0
    lv_font_add(&lv_font_symbol_10_file, &lv_font_symbol_10_basic);
#else
    lv_font_add(&lv_font_symbol_10_file, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_10_FEEDBACK != 0
#if USE_LV_FONT_SYMBOL_10_BASIC != 0
    lv_font_add(&lv_font_symbol_10_feedback, &lv_font_symbol_10_basic);
#else
    lv_font_add(&lv_font_symbol_10_feedback, NULL);
#endif
#endif

    /*DEJAVU 20*/
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20, NULL);
#endif

#if USE_LV_FONT_DEJAVU_20_SUP != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20_sup, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_symbol_20_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_20_LATIN_EXT_A != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20_latin_ext_a, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_dejavu_20_latin_ext_a, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_20_LATIN_EXT_B != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_dejavu_20_latin_ext_b, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_dejavu_20_latin_ext_b, NULL);
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
#if USE_LV_FONT_SYMBOL_20_BASIC != 0
#if USE_LV_FONT_DEJAVU_20 != 0
    lv_font_add(&lv_font_symbol_20_basic, &lv_font_dejavu_20);
#else
    lv_font_add(&lv_font_symbol_20_basic, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_20_FILE != 0
#if USE_LV_FONT_SYMBOL_20_BASIC != 0
    lv_font_add(&lv_font_symbol_20_file, &lv_font_symbol_20_basic);
#else
    lv_font_add(&lv_font_symbol_20_file, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_20_FEEDBACK != 0
#if USE_LV_FONT_SYMBOL_20_BASIC != 0
    lv_font_add(&lv_font_symbol_20_feedback, &lv_font_symbol_20_basic);
#else
    lv_font_add(&lv_font_symbol_20_feedback, NULL);
#endif
#endif

    /*DEJAVU 30*/
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30, NULL);
#endif

#if USE_LV_FONT_DEJAVU_30_SUP != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30_sup, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_dejavu_30_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_30_LATIN_EXT_A != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30_latin_ext_a, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_dejavu_30_latin_ext_a, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_30_LATIN_EXT_B != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_dejavu_30_latin_ext_b, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_dejavu_30_latin_ext_b, NULL);
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
#if USE_LV_FONT_SYMBOL_30_BASIC != 0
#if USE_LV_FONT_DEJAVU_30 != 0
    lv_font_add(&lv_font_symbol_30_basic, &lv_font_dejavu_30);
#else
    lv_font_add(&lv_font_symbol_30_basic, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_30_FILE != 0
#if USE_LV_FONT_SYMBOL_30_BASIC != 0
    lv_font_add(&lv_font_symbol_30_file, &lv_font_symbol_30_basic);
#else
    lv_font_add(&lv_font_symbol_30_file, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_30_FEEDBACK != 0
#if USE_LV_FONT_SYMBOL_30_BASIC != 0
    lv_font_add(&lv_font_symbol_30_feedback, &lv_font_symbol_30_basic);
#else
    lv_font_add(&lv_font_symbol_30_feedback, NULL);
#endif
#endif

    /*DEJAVU 40*/
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40, NULL);
#endif

#if USE_LV_FONT_DEJAVU_40_SUP != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40_sup, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_dejavu_40_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_40_LATIN_EXT_A != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40_latin_ext_a, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_dejavu_40_latin_ext_a, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_40_LATIN_EXT_B != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_dejavu_40_latin_ext_b, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_dejavu_40_latin_ext_b, NULL);
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
#if USE_LV_FONT_SYMBOL_40_BASIC != 0
#if USE_LV_FONT_DEJAVU_40 != 0
    lv_font_add(&lv_font_symbol_40_basic, &lv_font_dejavu_40);
#else
    lv_font_add(&lv_font_symbol_40_basic, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_40_FILE != 0
#if USE_LV_FONT_SYMBOL_40_BASIC != 0
    lv_font_add(&lv_font_symbol_40_file, &lv_font_symbol_40_basic);
#else
    lv_font_add(&lv_font_symbol_40_file, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_40_FEEDBACK != 0
#if USE_LV_FONT_SYMBOL_40_BASIC != 0
    lv_font_add(&lv_font_symbol_40_feedback, &lv_font_symbol_40_basic);
#else
    lv_font_add(&lv_font_symbol_40_feedback, NULL);
#endif
#endif

    /*DEJAVU 60*/
#if USE_LV_FONT_DEJAVU_60 != 0
    lv_font_add(&lv_font_dejavu_60, NULL);
#endif

#if USE_LV_FONT_DEJAVU_60_SUP != 0
#if USE_LV_FONT_DEJAVU_60 != 0
    lv_font_add(&lv_font_dejavu_60_sup, &lv_font_dejavu_60);
#else
    lv_font_add(&lv_font_dejavu_60_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_60_LATIN_EXT_A != 0
#if USE_LV_FONT_DEJAVU_60 != 0
    lv_font_add(&lv_font_dejavu_60_latin_ext_a, &lv_font_dejavu_60);
#else
    lv_font_add(&lv_font_dejavu_60_latin_ext_a, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_60_LATIN_EXT_B != 0
#if USE_LV_FONT_DEJAVU_60 != 0
    lv_font_add(&lv_font_dejavu_60_latin_ext_b, &lv_font_dejavu_60);
#else
    lv_font_add(&lv_font_dejavu_60_latin_ext_b, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_60_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_60 != 0
    lv_font_add(&lv_font_dejavu_60_cyrillic, &lv_font_dejavu_60);
#else
    lv_font_add(&lv_font_dejavu_60_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 60*/
#if USE_LV_FONT_SYMBOL_60_BASIC != 0
#if USE_LV_FONT_DEJAVU_60 != 0
    lv_font_add(&lv_font_symbol_60_basic, &lv_font_dejavu_60);
#else
    lv_font_add(&lv_font_symbol_60_basic, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_60_FILE != 0
#if USE_LV_FONT_SYMBOL_60_BASIC != 0
    lv_font_add(&lv_font_symbol_60_file, &lv_font_symbol_60_basic);
#else
    lv_font_add(&lv_font_symbol_60_file, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_60_FEEDBACK != 0
#if USE_LV_FONT_SYMBOL_60_BASIC != 0
    lv_font_add(&lv_font_symbol_60_feedback, &lv_font_symbol_60_basic);
#else
    lv_font_add(&lv_font_symbol_60_feedback, NULL);
#endif
#endif

    /*DEJAVU 80*/
#if USE_LV_FONT_DEJAVU_80 != 0
    lv_font_add(&lv_font_dejavu_80, NULL);
#endif

#if USE_LV_FONT_DEJAVU_80_SUP != 0
#if USE_LV_FONT_DEJAVU_80 != 0
    lv_font_add(&lv_font_dejavu_80_sup, &lv_font_dejavu_80);
#else
    lv_font_add(&lv_font_dejavu_80_sup, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_80_LATIN_EXT_A != 0
#if USE_LV_FONT_DEJAVU_80 != 0
    lv_font_add(&lv_font_dejavu_80_latin_ext_a, &lv_font_dejavu_80);
#else
    lv_font_add(&lv_font_dejavu_80_latin_ext_a, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_80_LATIN_EXT_B != 0
#if USE_LV_FONT_DEJAVU_80 != 0
    lv_font_add(&lv_font_dejavu_80_latin_ext_b, &lv_font_dejavu_80);
#else
    lv_font_add(&lv_font_dejavu_80_latin_ext_b, NULL);
#endif
#endif

#if USE_LV_FONT_DEJAVU_80_CYRILLIC != 0
#if USE_LV_FONT_DEJAVU_80 != 0
    lv_font_add(&lv_font_dejavu_80_cyrillic, &lv_font_dejavu_80);
#else
    lv_font_add(&lv_font_dejavu_80_cyrillic, NULL);
#endif
#endif

    /*SYMBOL 80*/
#if USE_LV_FONT_SYMBOL_80_BASIC != 0
#if USE_LV_FONT_DEJAVU_80 != 0
    lv_font_add(&lv_font_symbol_80_basic, &lv_font_dejavu_80);
#else
    lv_font_add(&lv_font_symbol_80_basic, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_80_FILE != 0
#if USE_LV_FONT_SYMBOL_80_BASIC != 0
    lv_font_add(&lv_font_symbol_80_file, &lv_font_symbol_80_basic);
#else
    lv_font_add(&lv_font_symbol_80_file, NULL);
#endif
#endif

#if USE_LV_FONT_SYMBOL_80_FEEDBACK != 0
#if USE_LV_FONT_SYMBOL_80_BASIC != 0
    lv_font_add(&lv_font_symbol_80_feedback, &lv_font_symbol_80_basic);
#else
    lv_font_add(&lv_font_symbol_80_feedback, NULL);
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
uint8_t lv_font_get_width(const lv_font_t * font_p, uint32_t letter)
{
    const lv_font_t * font_i = font_p;
    while(font_i != NULL) {
        if(letter >= font_i->first_ascii && letter <= font_i->last_ascii) {
            uint32_t index = (letter - font_i->first_ascii);
            return font_i->width[index];
        }
        font_i = font_i->next_page;
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
