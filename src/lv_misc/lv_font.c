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
 * Initialize the fonts
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
 * Get the description of a glyph in a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return pointer to a glyph descriptor
 */
const lv_font_glyph_dsc_t * lv_font_get_glyph_dsc(const lv_font_t * font_p, uint32_t letter)
{
    const lv_font_t * font_i = font_p;
    const lv_font_glyph_dsc_t * dsc;
    while(font_i != NULL) {
        dsc = font_i->get_glyph_dsc(font_i, letter);
        if(dsc) {
            /*Glyph found*/
            return dsc;
        }

        font_i = font_i->next_page;
    }

    return NULL;
}

uint8_t lv_font_get_width_int(const lv_font_t * font, uint32_t letter)
{
    const lv_font_glyph_dsc_t * dsc = lv_font_get_glyph_dsc(font, letter);
    return dsc ? LV_FONT_GET_ADV_W_INT(dsc->adv_w) : 0;
}

uint8_t lv_font_get_width_fract(const lv_font_t * font, uint32_t letter)
{
    const lv_font_glyph_dsc_t * dsc = lv_font_get_glyph_dsc(font, letter);
    return dsc ? LV_FONT_GET_ADV_W_FRACT(dsc->adv_w) : 0;
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
 * Generic bitmap get function used in 'font->get_bitmap' when the font contains all characters in
 * the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_glyph_bitmap_plain(const lv_font_t * font, uint32_t unicode_letter)
{
    /*Check the range*/
    if(unicode_letter < font->unicode_first || unicode_letter > font->unicode_last) return NULL;

    lv_font_dsc_built_in_t * font_dsc = (lv_font_dsc_built_in_t *) font->dsc;

    /*No Unicode list -> Continuous font*/
    if(font_dsc->unicode_list == NULL) {
        uint32_t index = (unicode_letter - font->unicode_first);
        return &font_dsc->glyph_bitmap[font_dsc->glyph_dsc[index].bitmap_index];
    }
    /*Has Unicode list -> Sparse font */
    else {
        uint16_t * pUnicode;
        pUnicode = lv_utils_bsearch(&unicode_letter, font_dsc->unicode_list, font_dsc->glyph_cnt,
                                    sizeof(font_dsc->unicode_list[0]), lv_font_codeCompare);
        if(pUnicode != NULL) {
            uint32_t idx = (uint32_t)(pUnicode - font_dsc->unicode_list);
            return &font_dsc->glyph_bitmap[font_dsc->glyph_dsc[idx].bitmap_index];
        }
    }

    /*If not returned earlier then the letter is not found in this font*/
    return NULL;
}

/**
 * Generic glyph width get function used in 'font->get_width' when the font contains all characters
 * in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the gylph or -1 if not found
 */
const lv_font_glyph_dsc_t * lv_font_get_glyph_dsc_plain(const lv_font_t * font, uint32_t unicode_letter)
{
    /*Check the range*/
    if(unicode_letter < font->unicode_first || unicode_letter > font->unicode_last) return NULL;

    lv_font_dsc_built_in_t * font_dsc = (lv_font_dsc_built_in_t *) font->dsc;

    /*No Unicode list -> Continuous font*/
    if(font_dsc->unicode_list == NULL) {
        uint32_t index = (unicode_letter - font->unicode_first);
        return &font_dsc->glyph_dsc[index];
    }
    /*Has Unicode list -> Sparse font */
    else {
        uint16_t * pUnicode;
        pUnicode = lv_utils_bsearch(&unicode_letter, font_dsc->unicode_list, font_dsc->glyph_cnt,
                                    sizeof(font_dsc->unicode_list[0]), lv_font_codeCompare);

        if(pUnicode != NULL) {
            uint32_t idx = (uint16_t)(pUnicode - font_dsc->unicode_list);
            return &font_dsc->glyph_dsc[idx];
        }
    }

    /*If not returned earlier then the letter is not found in this font*/
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/** Code Comparator.
 *
 *  Compares the value of both input arguments.
 *
 *  @param[in]  pRef        Pointer to the reference.
 *  @param[in]  pElement    Pointer to the element to compare.
 *
 *  @return Result of comparison.
 *  @retval < 0   Reference is greater than element.
 *  @retval = 0   Reference is equal to element.
 *  @retval > 0   Reference is less than element.
 *
 */
static int32_t lv_font_codeCompare(const void * pRef, const void * pElement)
{
    return (*(uint16_t *)pRef) - (*(uint16_t *)pElement);
}
