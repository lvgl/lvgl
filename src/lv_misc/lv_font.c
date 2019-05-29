/**
 * @file lv_font.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font.h"
#include "lv_log.h"
#include "lv_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t lv_font_codeCompare(const void * pRef, const void * pElement);

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
 * Initialize the font module
 */
void lv_font_init(void)
{
    lv_font_builtin_init();
}

/**
 * Add a font to an other to extend the character set.
 * @param child the font to add
 * @param parent this font will be extended. Using it later will contain the characters from `child`
 */
void lv_font_add(lv_font_t * child, lv_font_t * parent)
{
    if(parent == NULL) return;

    while(parent->next_page != NULL) {
        parent = parent->next_page; /*Got to the last page and add the new font there*/
    }

    parent->next_page = child;
}

/**
 * Remove a font from a character set.
 * @param child the font to remove
 * @param parent remove `child` from here
 */
void lv_font_remove(lv_font_t * child, lv_font_t * parent)
{
    if(parent == NULL) return;
    if(child == NULL) return;

    while(parent->next_page != child) {
        parent = parent->next_page; /*Got to the last page and add the new font there*/
    }

    parent->next_page = child->next_page;
}

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * lv_font_get_glyph_bitmap(const lv_font_t * font_p, uint32_t letter)
{
    const lv_font_t * font_i = font_p;
    while(font_i != NULL) {
        const uint8_t * bitmap = font_i->get_glyph_bitmap(font_i, letter);
        if(bitmap) return bitmap;

        font_i = font_i->next_page;
    }

    return NULL;
}

/**
 * Get the descriptor of a glyph
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc(const lv_font_t * font_p, lv_font_glyph_dsc_t * dsc_out, uint32_t letter)
{
    const lv_font_t * font_i = font_p;
    bool ret;
    while(font_i != NULL) {
        ret = font_i->get_glyph_dsc(font_i, dsc_out, letter);
        if(ret) return ret;

        font_i = font_i->next_page;
    }

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
