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

/**
 * Get the width of a glyph with kerning
 * @param font pointer to a font
 * @param letter an UNICODE letter
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return the width of the glyph
 */
uint8_t lv_font_get_glyph_width(const lv_font_t * font, uint32_t letter, uint32_t letter_next)
{
    lv_font_glyph_dsc_t dsc;
    bool ret = lv_font_get_glyph_dsc(font, &dsc, letter);
    if(ret == false) return 0;

    int32_t w = dsc.adv_w;

    /*Apply kerning is required*/
    if(dsc.kern_table && letter_next != 0) {
        uint32_t i;
        for(i = 0; dsc.kern_table[i].next_unicode != 0; i++) {
            if((uint32_t)dsc.kern_table[i].next_unicode == letter_next) {
                w += dsc.kern_table[i].space;
                break;
            }
        }
    }

    if(w < 0) w = 0;

    return w <= 0 ? 0 : LV_FONT_GET_WIDTH_INT(w);
}

/**
 * Used as `get_glyph_bitmap` callback in LittelvGL's native font format if the font is uncompressed.
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
 * Used as `get_glyph_dsc` callback in LittelvGL's native font format if the font is uncompressed.
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc_plain(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter)
{
    /*Check the range*/
    if(unicode_letter < font->unicode_first || unicode_letter > font->unicode_last) return NULL;

    lv_font_dsc_built_in_t * font_dsc = (lv_font_dsc_built_in_t *) font->dsc;
    int32_t index = -1;
    /*No Unicode list -> Continuous font*/
    if(font_dsc->unicode_list == NULL) {
        index = (unicode_letter - font->unicode_first);
    }
    /*Has Unicode list -> Sparse font */
    else {
        uint16_t * pUnicode;
        pUnicode = lv_utils_bsearch(&unicode_letter, font_dsc->unicode_list, font_dsc->glyph_cnt,
                                    sizeof(font_dsc->unicode_list[0]), lv_font_codeCompare);

        if(pUnicode != NULL) {
            index = (uint16_t)(pUnicode - font_dsc->unicode_list);
        }
    }

    if(index > 0) {
        dsc_out->adv_w = font_dsc->glyph_dsc[index].adv_w;
        dsc_out->box_h = font_dsc->glyph_dsc[index].box_h;
        dsc_out->box_w = font_dsc->glyph_dsc[index].box_w;
        dsc_out->ofs_x = font_dsc->glyph_dsc[index].ofs_x;
        dsc_out->ofs_y = font_dsc->glyph_dsc[index].ofs_y;
        dsc_out->kern_table = font_dsc->glyph_dsc[index].kern_table;
        dsc_out->bpp   = font_dsc->bpp;
        return true;
    } else {
        return false;
    }
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
