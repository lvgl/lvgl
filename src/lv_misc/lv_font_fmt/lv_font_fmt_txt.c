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
 * Used as `get_glyph_bitmap` callback in LittelvGL's native font format if the font is uncompressed.
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_bitmap_fmt_txt(const lv_font_t * font, uint32_t unicode_letter)
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
bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter)
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
