/**
 * @file lv_font.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_font.h"
#include "lv_font_fmt_txt.h"
#include "../lv_core/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_types.h"
#include "../lv_misc/lv_log.h"
#include "../lv_misc/lv_utils.h"
#include "../lv_misc/lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    RLE_STATE_SINGLE = 0,
    RLE_STATE_REPEATE,
    RLE_STATE_COUNTER,
}rle_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter);
static int8_t get_kern_value(const lv_font_t * font, uint32_t gid_left, uint32_t gid_right);
static int32_t unicode_list_compare(const void * ref, const void * element);
static int32_t kern_pair_8_compare(const void * ref, const void * element);
static int32_t kern_pair_16_compare(const void * ref, const void * element);

static void decompress(const uint8_t * in, uint8_t * out, lv_coord_t w, lv_coord_t h, uint8_t bpp);
static void decompress_line(uint8_t * out, lv_coord_t w);
static uint8_t get_bits(const uint8_t * in, uint32_t bit_pos, uint8_t len);
static void bits_write(uint8_t * out, uint32_t bit_pos, uint8_t val, uint8_t len);
static void rle_init(const uint8_t * in,  uint8_t bpp);
static uint8_t rle_next(void);


/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t rle_rdp;
static const uint8_t * rle_in;
static uint8_t rle_bpp;
static uint8_t rle_prev_v;
static uint8_t rle_cnt;
static rle_state_t rle_state;

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
    if(!gid) return NULL;

    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];

    if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
        if(gdsc) return &fdsc->glyph_bitmap[gdsc->bitmap_index];
    }
    /*Handle compressed bitmap*/
    else
    {
        static uint8_t * buf = NULL;

        uint32_t gsize = gdsc->box_w * gdsc->box_h;
        if(gsize == 0) return NULL;

        uint32_t buf_size = gsize;
        /*Compute memory size needed to hold decompressed glyph, rounding up*/
        switch(fdsc->bpp) {
        case 1: buf_size = (gsize + 7) >> 3;  break;
        case 2: buf_size = (gsize + 3) >> 2;  break;
        case 3: buf_size = (gsize + 1) >> 1;  break;
        case 4: buf_size = (gsize + 1) >> 1;  break;
        }

        if(lv_mem_get_size(buf) < buf_size) {
            buf = lv_mem_realloc(buf, buf_size);
            LV_ASSERT_MEM(buf);
            if(buf == NULL) return NULL;
        }

        decompress(&fdsc->glyph_bitmap[gdsc->bitmap_index], buf, gdsc->box_w , gdsc->box_h, (uint8_t)fdsc->bpp);
        return buf;
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

    int32_t kv = ((int32_t)((int32_t)kvalue * fdsc->kern_scale) >> 4);

    uint32_t adv_w = gdsc->adv_w + kv;
    adv_w  = (adv_w + (1 << 3)) >> 4;

    dsc_out->adv_w = adv_w;
    dsc_out->box_h = gdsc->box_h;
    dsc_out->box_w = gdsc->box_w;
    dsc_out->ofs_x = gdsc->ofs_x;
    dsc_out->ofs_y = gdsc->ofs_y;
    dsc_out->bpp   = (uint8_t)fdsc->bpp;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter)
{
    if(letter == '\0') return 0;

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *) font->dsc;

    /*Check the cache first*/
    if(letter == fdsc->last_letter) return fdsc->last_glyph_id;

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
            uint8_t * p = lv_utils_bsearch(&rcp, fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length, sizeof(fdsc->cmaps[i].unicode_list[0]), unicode_list_compare);

            if(p) {
                lv_uintptr_t ofs = (lv_uintptr_t)(p - (uint8_t *) fdsc->cmaps[i].unicode_list);
                ofs = ofs >> 1;     /*The list stores `uint16_t` so the get the index divide by 2*/
                glyph_id = fdsc->cmaps[i].glyph_id_start + ofs;
            }
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
            uint8_t * p = lv_utils_bsearch(&rcp, fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length, sizeof(fdsc->cmaps[i].unicode_list[0]), unicode_list_compare);

            if(p) {
                lv_uintptr_t ofs = (lv_uintptr_t)(p - (uint8_t*) fdsc->cmaps[i].unicode_list);
                ofs = ofs >> 1;     /*The list stores `uint16_t` so the get the index divide by 2*/
                const uint8_t * gid_ofs_16 = fdsc->cmaps[i].glyph_id_ofs_list;
                glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_16[ofs];
            }
        }

        /*Update the cache*/
        fdsc->last_letter = letter;
        fdsc->last_glyph_id = glyph_id;
        return glyph_id;
    }

    fdsc->last_letter = letter;
    fdsc->last_glyph_id = 0;
    return 0;

}

static int8_t get_kern_value(const lv_font_t * font, uint32_t gid_left, uint32_t gid_right)
{
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *) font->dsc;

    int8_t value = 0;

    if(fdsc->kern_classes == 0) {
        /*Kern pairs*/
        const lv_font_fmt_txt_kern_pair_t * kdsc = fdsc->kern_dsc;
        if(kdsc->glyph_ids_size == 0) {
            /* Use binary search to find the kern value.
             * The pairs are ordered left_id first, then right_id secondly. */
            const uint8_t * g_ids = kdsc->glyph_ids;
            uint16_t g_id_both = (gid_right << 8) + gid_left; /*Create one number from the ids*/
            uint8_t * kid_p = lv_utils_bsearch(&g_id_both, g_ids, kdsc->pair_cnt, 2, kern_pair_8_compare);

            /*If the `g_id_both` were found get its index from the pointer*/
            if(kid_p) {
                lv_uintptr_t ofs = (lv_uintptr_t)(kid_p - g_ids);
                ofs = ofs >> 1;     /*ofs is for pair, divide by 2 to refer as a single value*/
                value = kdsc->values[ofs];
            }
        } else if(kdsc->glyph_ids_size == 1) {
            /* Use binary search to find the kern value.
             * The pairs are ordered left_id first, then right_id secondly. */
            const uint16_t * g_ids = kdsc->glyph_ids;
            lv_uintptr_t g_id_both = (uint32_t)((uint32_t)gid_right << 8) + gid_left; /*Create one number from the ids*/
            uint8_t * kid_p = lv_utils_bsearch(&g_id_both, g_ids, kdsc->pair_cnt, 4, kern_pair_16_compare);

            /*If the `g_id_both` were found get its index from the pointer*/
            if(kid_p) {
                lv_uintptr_t ofs = (lv_uintptr_t) (kid_p - (const uint8_t *)g_ids);
                ofs = ofs >> 4;     /*ofs is 4 byte pairs, divide by 4 to refer as a single value*/
                value = kdsc->values[ofs];
            }

        } else {
            /*Invalid value*/
        }
    } else {
        /*Kern classes*/
        const lv_font_fmt_txt_kern_classes_t * kdsc = fdsc->kern_dsc;
        uint8_t left_class = kdsc->left_class_mapping[gid_left];
        uint8_t right_class = kdsc->right_class_mapping[gid_right];

        /* If class = 0, kerning not exist for that glyph
         * else got the value form `class_pair_values` 2D array*/
        if(left_class > 0 && right_class > 0) {
            value = kdsc->class_pair_values[(left_class-1)* kdsc->right_class_cnt + (right_class-1)];
        }

    }
    return value;
}

static int32_t kern_pair_8_compare(const void * ref, const void * element)
{
    const uint8_t * ref8_p = ref;
    const uint8_t * element8_p = element;

    /*If the MSB is different it will matter. If not return the diff. of the LSB*/
    if(ref8_p[0] != element8_p[0]) return (int32_t)ref8_p[0] - element8_p[0];
    else return (int32_t) ref8_p[1] - element8_p[1];

}

static int32_t kern_pair_16_compare(const void * ref, const void * element)
{
    const uint16_t * ref16_p = ref;
    const uint16_t * element16_p = element;

    /*If the MSB is different it will matter. If not return the diff. of the LSB*/
    if(ref16_p[0] != element16_p[0]) return (int32_t)ref16_p[0] - element16_p[0];
    else return (int32_t) ref16_p[1] - element16_p[1];
}

/**
 * The compress a glyph's bitmap
 * @param in the compressed bitmap
 * @param out buffer to store the result
 * @param px_num number of pixels in the glyph (width * height)
 * @param bpp bit per pixel (bpp = 3 will be converted to bpp = 4)
 */
static void decompress(const uint8_t * in, uint8_t * out, lv_coord_t w, lv_coord_t h, uint8_t bpp)
{
    uint32_t wrp = 0;
    uint8_t wr_size = bpp;
    if(bpp == 3) wr_size = 4;

    rle_init(in, bpp);

    uint8_t * line_buf = lv_draw_get_buf(w * 2);
    uint8_t * line_buf1 = line_buf;
    uint8_t * line_buf2 = line_buf + w;

    decompress_line(line_buf1, w);

    lv_coord_t y;
    lv_coord_t x;
    for(x = 0; x < w; x++) {
        bits_write(out,wrp, line_buf1[x], bpp);
        wrp += wr_size;
    }

    for(y = 1; y < h; y++) {
        decompress_line(line_buf2, w);

        for(x = 0; x < w; x++) {
            line_buf1[x] = line_buf2[x] ^ line_buf1[x];
            bits_write(out,wrp, line_buf1[x], bpp);
            wrp += wr_size;
        }
    }
}

/**
 * Decompress one line. Store one pixel per byte
 * @param out output buffer
 * @param w width of the line in pixel count
 */
static void decompress_line(uint8_t * out, lv_coord_t w)
{
    lv_coord_t i;
    for(i = 0; i < w; i++) {
        out[i] = rle_next();
    }
}

/**
 * Read bits from an input buffer. The read can cross byte boundary.
 * @param in the input buffer to read from.
 * @param bit_pos index of teh first bit to read.
 * @param len number of bits to read (must be <= 8).
 * @return the read bits
 */
static uint8_t get_bits(const uint8_t * in, uint32_t bit_pos, uint8_t len)
{
    uint8_t res = 0;
    uint32_t byte_pos = bit_pos >> 3;
    bit_pos = bit_pos & 0x7;
    uint8_t bit_mask = (uint16_t)((uint16_t) 1 << len) - 1;
    uint16_t in16 = (in[byte_pos] << 8) + in[byte_pos + 1];

    res = (in16 >> (16 - bit_pos - len)) & bit_mask;
    return res;
}

/**
 * Write `val` data to `bit_pos` position of `out`. The write can NOT cross byte boundary.
 * @param out buffer where to write
 * @param bit_pos bit index to write
 * @param val value to write
 * @param len length of bits to write from `val`. (Counted from the LSB).
 * @note `len == 3` will be converted to `len = 4` and `val` will be upscaled too
 */
static void bits_write(uint8_t * out, uint32_t bit_pos, uint8_t val, uint8_t len)
{
    if(len == 3) {
        len = 4;
        switch(val) {
        case 0: val = 0; break;
        case 1: val = 2; break;
        case 2: val = 4; break;
        case 3: val = 6; break;
        case 4: val = 9; break;
        case 5: val = 11; break;
        case 6: val = 13; break;
        case 7: val = 15; break;
        }
    }

    uint16_t byte_pos = bit_pos >> 3;
    bit_pos = bit_pos & 0x7;
    bit_pos = 8 - bit_pos - len;

    uint8_t bit_mask = (uint16_t)((uint16_t) 1 << len) - 1;
    out[byte_pos] &= ((~bit_mask) << bit_pos);
    out[byte_pos] |= (val << bit_pos);
}

static void rle_init(const uint8_t * in,  uint8_t bpp)
{
    rle_in = in;
    rle_bpp = bpp;
    rle_state = RLE_STATE_SINGLE;
    rle_rdp = 0;
    rle_prev_v = 0;
    rle_cnt = 0;
}

static uint8_t rle_next(void)
{
    uint8_t v = 0;
    uint8_t ret = 0;

    if(rle_state == RLE_STATE_SINGLE) {
        ret = get_bits(rle_in, rle_rdp, rle_bpp);
        if(rle_rdp != 0 && rle_prev_v == ret) {
            rle_cnt = 0;
            rle_state = RLE_STATE_REPEATE;
        }

        rle_prev_v = ret;
        rle_rdp += rle_bpp;
    }
    else if(rle_state == RLE_STATE_REPEATE) {
        v = get_bits(rle_in, rle_rdp, 1);
        rle_cnt++;
        rle_rdp += 1;
        if(v == 1) {
            ret = rle_prev_v;
            if(rle_cnt == 11) {
                rle_cnt = get_bits(rle_in, rle_rdp, 6);
                rle_rdp += 6;
                if(rle_cnt != 0) {
                    rle_state = RLE_STATE_COUNTER;
                } else {
                    ret = get_bits(rle_in, rle_rdp, rle_bpp);
                    rle_prev_v = ret;
                    rle_rdp += rle_bpp;
                    rle_state = RLE_STATE_SINGLE;
                }
            }
        } else {
            ret = get_bits(rle_in, rle_rdp, rle_bpp);
            rle_prev_v = ret;
            rle_rdp += rle_bpp;
            rle_state = RLE_STATE_SINGLE;
        }


    }
    else if(rle_state == RLE_STATE_COUNTER) {
        ret = rle_prev_v;
        rle_cnt--;
        if(rle_cnt == 0) {
            ret = get_bits(rle_in, rle_rdp, rle_bpp);
            rle_prev_v = ret;
            rle_rdp += rle_bpp;
            rle_state = RLE_STATE_SINGLE;
        }
    }

    return ret;
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
static int32_t unicode_list_compare(const void * ref, const void * element)
{
    return ((int32_t)(*(uint16_t *)ref)) - ((int32_t)(*(uint16_t *)element));
}
