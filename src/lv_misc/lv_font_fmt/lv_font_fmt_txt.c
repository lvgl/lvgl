/**
 * @file lv_font.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_font.h"
#include "../lv_log.h"
#include "../lv_utils.h"
#include "lv_font_fmt_txt.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter);
static int8_t get_kern_value(const lv_font_t * font, uint32_t gid_left, uint32_t gid_right);
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
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *) font->dsc;
    uint32_t gid = get_glyph_dsc_id(font, unicode_letter);
    if(!gid) return false;

    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];

    if(gdsc) return &fdsc->glyph_bitmap[gdsc->bitmap_index];

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
bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *) font->dsc;
    uint32_t gid = get_glyph_dsc_id(font, unicode_letter);
    if(!gid) return false;

    int8_t kvalue = 0;
    if(fdsc->kern_dsc) {
        uint32_t gid_next = get_glyph_dsc_id(font, unicode_letter_next);
        if(gid_next) {
            kvalue = get_kern_value(font, gid, gid_next);
        }
    }

    /*Put together a glyph dsc*/
    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];

    uint32_t adw_w = gdsc->adv_w + ((int32_t)((int32_t)kvalue * fdsc->kern_scale) >> 4);
    adw_w  = (adw_w + (1 << 3)) >> 4;

    dsc_out->adv_w = adw_w;
    dsc_out->box_h = gdsc->box_h;
    dsc_out->box_w = gdsc->box_w;
    dsc_out->ofs_x = gdsc->ofs_x;
    dsc_out->ofs_y = gdsc->ofs_y;
    dsc_out->bpp   = fdsc->bpp;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter)
{
    if(letter == '\0') return 0;

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *) font->dsc;

    uint16_t i;
    for(i = 0; i < fdsc->cmap_num; i++) {

        /*Relative code point*/
        uint32_t rcp = letter - fdsc->cmaps[i].range_start;
        if(rcp > fdsc->cmaps[i].range_length) continue;
        uint32_t glyph_id = 0;
        if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY) {
            glyph_id = fdsc->cmaps[i].glyph_id_start + rcp;
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL) {
            const uint8_t * gid_ofs_8 = fdsc->cmaps[i].glyph_id_ofs_list;
            glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_8[rcp];
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY) {
            uint32_t u;
            for(u = 0; u < fdsc->cmaps[i].list_length; u++) {
                if(fdsc->cmaps[i].unicode_list[u] == rcp) {
                    glyph_id = fdsc->cmaps[i].glyph_id_start + u;
                }
            }
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY) {
            const uint8_t * gid_ofs_16 = fdsc->cmaps[i].glyph_id_ofs_list;
            uint32_t u;
            for(u = 0; u < 50 /*fdsc->cmaps[i].list_length*/; u++) {
                if(fdsc->cmaps[i].unicode_list[u] == rcp) {
                    glyph_id = fdsc->cmaps[i].glyph_id_start + u;
                }
                glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_16[u];
            }
        }

        return glyph_id;
    }

    return 0;

}

static int8_t get_kern_value(const lv_font_t * font, uint32_t gid_left, uint32_t gid_right)
{
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *) font->dsc;

    int8_t value = 0;
    uint32_t k;
    if(fdsc->kern_classes == 0) {
        /*Kern pairs*/
        const lv_font_fmt_txt_kern_pair_t * kdsc = fdsc->kern_dsc;
        if(kdsc->glyph_ids_size == 1) {
            const uint8_t * g_ids = kdsc->glyph_ids;
            for(k = 0; k < (uint32_t)kdsc->pair_cnt * 2; k += 2) {
                if(g_ids[k] == gid_left &&
                        g_ids[k+1] == gid_right) {
                    value = kdsc->values[k >> 1];
                    break;
                }
            }
        } else {
            const uint16_t * g_ids = kdsc->glyph_ids;
            for(k = 0; k < (uint32_t)kdsc->pair_cnt * 2; k += 2) {
                if(g_ids[k] == gid_left &&
                        g_ids[k+1] == gid_right) {
                    value = kdsc->values[k >> 1];
                    break;
                }
            }
        }
    } else {
        /*Kern classes*/
        const lv_font_fmt_txt_kern_classes_t * kdsc = fdsc->kern_dsc;
        uint8_t left_class = kdsc->left_class_mapping[gid_left];
        uint8_t right_class = kdsc->left_class_mapping[gid_right];

        /* If class = 0, kerning not exist for that glyph
         * else got the value form `class_pair_values` 2D array*/
        if(left_class > 0 && right_class > 0) {
            value = kdsc->class_pair_values[(left_class-1)* kdsc->right_class_cnt + (right_class-1)];
        }

    }
    return value;
}


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
