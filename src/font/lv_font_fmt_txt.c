/**
 * @file lv_font_fmt_txt.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_font.h"
#include "lv_font_fmt_txt_private.h"
#include "../core/lv_global.h"
#include "../misc/lv_assert.h"
#include "../misc/lv_types.h"
#include "../misc/lv_log.h"
#include "../misc/lv_utils.h"
#include "../stdlib/lv_mem.h"

/*********************
 *      DEFINES
 *********************/
#if LV_USE_FONT_COMPRESSED
    #define font_rle LV_GLOBAL_DEFAULT()->font_fmt_rle
#endif /*LV_USE_FONT_COMPRESSED*/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t gid_left;
    uint32_t gid_right;
} kern_pair_ref_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter);
static int8_t get_kern_value(const lv_font_t * font, uint32_t gid_left, uint32_t gid_right);
static int unicode_list_compare(const void * ref, const void * element);
static int kern_pair_8_compare(const void * ref, const void * element);
static int kern_pair_16_compare(const void * ref, const void * element);

#if LV_USE_FONT_COMPRESSED
    static void decompress(const uint8_t * in, uint8_t * out, int32_t w, int32_t h, uint8_t bpp, bool prefilter);
    static inline void decompress_line(uint8_t * out, int32_t w);
    static inline uint8_t get_bits(const uint8_t * in, uint32_t bit_pos, uint8_t len);
    static inline void rle_init(const uint8_t * in,  uint8_t bpp);
    static inline uint8_t rle_next(void);
#endif /*LV_USE_FONT_COMPRESSED*/

static lv_font_t * builtin_font_create_cb(const lv_font_info_t * info, const void * src);
static void builtin_font_delete_cb(lv_font_t * font);
static void * builtin_font_dup_src_cb(const void * src);
static void builtin_font_free_src_cb(void * src);

/**********************
 *  STATIC VARIABLES
 **********************/

static const uint8_t opa4_table[16] = {0,  17, 34,  51,
                                       68, 85, 102, 119,
                                       136, 153, 170, 187,
                                       204, 221, 238, 255
                                      };

#if LV_USE_FONT_COMPRESSED
static const uint8_t opa3_table[8] = {0, 36, 73, 109, 146, 182, 218, 255};
#endif

static const uint8_t opa2_table[4] = {0, 85, 170, 255};

const lv_font_class_t lv_builtin_font_class = {
    .create_cb = builtin_font_create_cb,
    .delete_cb = builtin_font_delete_cb,
    .dup_src_cb = builtin_font_dup_src_cb,
    .free_src_cb = builtin_font_free_src_cb,
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const void * lv_font_get_bitmap_fmt_txt(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf)
{
    const lv_font_t * font = g_dsc->resolved_font;

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t gid = g_dsc->gid.index;
    if(!gid) return NULL;

    const lv_font_fmt_txt_glyph_dsc_t * gdsc = &fdsc->glyph_dsc[gid];

    if(g_dsc->req_raw_bitmap) return &fdsc->glyph_bitmap[gdsc->bitmap_index];

    uint8_t * bitmap_out = draw_buf->data;
    int32_t gsize = (int32_t) gdsc->box_w * gdsc->box_h;
    if(gsize == 0) return NULL;

    uint16_t stride_in = g_dsc->stride;

    if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
        const uint8_t * bitmap_in = &fdsc->glyph_bitmap[gdsc->bitmap_index];
        uint8_t * bitmap_out_tmp = bitmap_out;
        int32_t i = 0;
        int32_t x, y;
        uint32_t stride_out = lv_draw_buf_width_to_stride(gdsc->box_w, LV_COLOR_FORMAT_A8);
        if(fdsc->bpp == 1) {
            for(y = 0; y < gdsc->box_h; y ++) {
                uint16_t line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                for(x = 0; x < gdsc->box_w; x++, i++) {
                    i = i & 0x7;
                    if(i == 0) bitmap_out_tmp[x] = (*bitmap_in) & 0x80 ? 0xff : 0x00;
                    else if(i == 1) bitmap_out_tmp[x] = (*bitmap_in) & 0x40 ? 0xff : 0x00;
                    else if(i == 2) bitmap_out_tmp[x] = (*bitmap_in) & 0x20 ? 0xff : 0x00;
                    else if(i == 3) bitmap_out_tmp[x] = (*bitmap_in) & 0x10 ? 0xff : 0x00;
                    else if(i == 4) bitmap_out_tmp[x] = (*bitmap_in) & 0x08 ? 0xff : 0x00;
                    else if(i == 5) bitmap_out_tmp[x] = (*bitmap_in) & 0x04 ? 0xff : 0x00;
                    else if(i == 6) bitmap_out_tmp[x] = (*bitmap_in) & 0x02 ? 0xff : 0x00;
                    else if(i == 7) {
                        bitmap_out_tmp[x] = (*bitmap_in) & 0x01 ? 0xff : 0x00;
                        line_rem--;
                        bitmap_in++;
                    }
                }
                /*Handle stride*/
                if(stride_in) {
                    i = 0;  /*If there is a stride start from the next byte in the next line*/
                    bitmap_in += line_rem;
                }
                bitmap_out_tmp += stride_out;
            }
        }
        else if(fdsc->bpp == 2) {
            for(y = 0; y < gdsc->box_h; y ++) {
                uint16_t line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                for(x = 0; x < gdsc->box_w; x++, i++) {
                    i = i & 0x3;
                    if(i == 0) bitmap_out_tmp[x] = opa2_table[(*bitmap_in) >> 6];
                    else if(i == 1) bitmap_out_tmp[x] = opa2_table[((*bitmap_in) >> 4) & 0x3];
                    else if(i == 2) bitmap_out_tmp[x] = opa2_table[((*bitmap_in) >> 2) & 0x3];
                    else if(i == 3) {
                        bitmap_out_tmp[x] = opa2_table[((*bitmap_in) >> 0) & 0x3];
                        line_rem--;
                        bitmap_in++;
                    }
                }

                /*Handle stride*/
                if(stride_in) {
                    i = 0;  /*If there is a stride start from the next byte in the next line*/
                    bitmap_in += line_rem;
                }
                bitmap_out_tmp += stride_out;
            }

        }
        else if(fdsc->bpp == 4) {
            for(y = 0; y < gdsc->box_h; y ++) {
                uint16_t line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                for(x = 0; x < gdsc->box_w; x++, i++) {
                    i = i & 0x1;
                    if(i == 0) {
                        bitmap_out_tmp[x] = opa4_table[(*bitmap_in) >> 4];
                    }
                    else if(i == 1) {
                        bitmap_out_tmp[x] = opa4_table[(*bitmap_in) & 0xF];
                        line_rem--;
                        bitmap_in++;
                    }
                }

                /*Handle stride*/
                if(stride_in) {
                    i = 0;  /*If there is a stride start from the next byte in the next line*/
                    bitmap_in += line_rem;
                }
                bitmap_out_tmp += stride_out;
            }
        }
        else if(fdsc->bpp == 8) {
            for(y = 0; y < gdsc->box_h; y ++) {
                uint16_t line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                for(x = 0; x < gdsc->box_w; x++, i++) {
                    bitmap_out_tmp[x] = *bitmap_in;
                    line_rem--;
                    bitmap_in++;
                }
                bitmap_out_tmp += stride_out;
                bitmap_in += line_rem;
            }
        }

        lv_draw_buf_flush_cache(draw_buf, NULL);
        return draw_buf;
    }
    /*Handle compressed bitmap*/
    else {
#if LV_USE_FONT_COMPRESSED
        bool prefilter = fdsc->bitmap_format == LV_FONT_FMT_TXT_COMPRESSED;
        decompress(&fdsc->glyph_bitmap[gdsc->bitmap_index], bitmap_out, gdsc->box_w, gdsc->box_h,
                   (uint8_t)fdsc->bpp, prefilter);
        lv_draw_buf_flush_cache(draw_buf, NULL);
        return draw_buf;
#else /*!LV_USE_FONT_COMPRESSED*/
        LV_LOG_WARN("Compressed fonts is used but LV_USE_FONT_COMPRESSED is not enabled in lv_conf.h");
        return NULL;
#endif
    }

    /*If not returned earlier then the letter is not found in this font*/
    return NULL;
}

bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter,
                                   uint32_t unicode_letter_next)
{
    /*It fixes a strange compiler optimization issue: https://github.com/lvgl/lvgl/issues/4370*/
    bool is_tab = unicode_letter == '\t';
    if(is_tab) {
        unicode_letter = ' ';
    }
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
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

    uint32_t adv_w = gdsc->adv_w;
    if(is_tab) adv_w *= 2;

    adv_w += kv;
    adv_w  = (adv_w + (1 << 3)) >> 4;

    dsc_out->adv_w = adv_w;
    dsc_out->box_h = gdsc->box_h;
    dsc_out->box_w = gdsc->box_w;
    dsc_out->ofs_x = gdsc->ofs_x;
    dsc_out->ofs_y = gdsc->ofs_y;

    if(fdsc->stride == 0) dsc_out->stride = 0;
    else {
        /*e.g. font_dsc stride ==  4 means align to 4 byte boundary.
         *In glyph_dsc store the actual line length in bytes*/
        dsc_out->stride = LV_ROUND_UP(dsc_out->box_w, fdsc->stride);
    }

    dsc_out->format = (uint8_t)fdsc->bpp;
    dsc_out->is_placeholder = false;
    dsc_out->gid.index = gid;

    if(is_tab) dsc_out->box_w = dsc_out->box_w * 2;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t get_glyph_dsc_id(const lv_font_t * font, uint32_t letter)
{
    if(letter == '\0') return 0;

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;

    uint16_t i;
    for(i = 0; i < fdsc->cmap_num; i++) {

        /*Relative code point*/
        uint32_t rcp = letter - fdsc->cmaps[i].range_start;
        if(rcp >= fdsc->cmaps[i].range_length) continue;
        uint32_t glyph_id = 0;
        if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY) {
            glyph_id = fdsc->cmaps[i].glyph_id_start + rcp;
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL) {
            const uint8_t * gid_ofs_8 = fdsc->cmaps[i].glyph_id_ofs_list;
            /* The first character is always valid and should have offset = 0
             * However if a character is missing it also has offset=0.
             * So if there is a 0 not on the first position then it's a missing character */
            if(gid_ofs_8[rcp] == 0 && letter != fdsc->cmaps[i].range_start) continue;
            glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_8[rcp];
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY) {
            uint16_t key = rcp;
            uint16_t * p = lv_utils_bsearch(&key, fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length,
                                            sizeof(fdsc->cmaps[i].unicode_list[0]), unicode_list_compare);

            if(p) {
                lv_uintptr_t ofs = p - fdsc->cmaps[i].unicode_list;
                glyph_id = fdsc->cmaps[i].glyph_id_start + (uint32_t) ofs;
            }
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
            uint16_t key = rcp;
            uint16_t * p = lv_utils_bsearch(&key, fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length,
                                            sizeof(fdsc->cmaps[i].unicode_list[0]), unicode_list_compare);

            if(p) {
                lv_uintptr_t ofs = p - fdsc->cmaps[i].unicode_list;
                const uint16_t * gid_ofs_16 = fdsc->cmaps[i].glyph_id_ofs_list;
                glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_16[ofs];
            }
        }

        return glyph_id;
    }

    return 0;

}

static int8_t get_kern_value(const lv_font_t * font, uint32_t gid_left, uint32_t gid_right)
{
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;

    int8_t value = 0;

    if(fdsc->kern_classes == 0) {
        /*Kern pairs*/
        const lv_font_fmt_txt_kern_pair_t * kdsc = fdsc->kern_dsc;
        if(kdsc->glyph_ids_size == 0) {
            /*Use binary search to find the kern value.
             *The pairs are ordered left_id first, then right_id secondly.*/
            const uint16_t * g_ids = kdsc->glyph_ids;
            kern_pair_ref_t g_id_both = {gid_left, gid_right};
            uint16_t * kid_p = lv_utils_bsearch(&g_id_both, g_ids, kdsc->pair_cnt, 2, kern_pair_8_compare);

            /*If the `g_id_both` were found get its index from the pointer*/
            if(kid_p) {
                lv_uintptr_t ofs = kid_p - g_ids;
                value = kdsc->values[ofs];
            }
        }
        else if(kdsc->glyph_ids_size == 1) {
            /*Use binary search to find the kern value.
             *The pairs are ordered left_id first, then right_id secondly.*/
            const uint32_t * g_ids = kdsc->glyph_ids;
            kern_pair_ref_t g_id_both = {gid_left, gid_right};
            uint32_t * kid_p = lv_utils_bsearch(&g_id_both, g_ids, kdsc->pair_cnt, 4, kern_pair_16_compare);

            /*If the `g_id_both` were found get its index from the pointer*/
            if(kid_p) {
                lv_uintptr_t ofs = kid_p - g_ids;
                value = kdsc->values[ofs];
            }

        }
        else {
            /*Invalid value*/
        }
    }
    else {
        /*Kern classes*/
        const lv_font_fmt_txt_kern_classes_t * kdsc = fdsc->kern_dsc;
        uint8_t left_class = kdsc->left_class_mapping[gid_left];
        uint8_t right_class = kdsc->right_class_mapping[gid_right];

        /*If class = 0, kerning not exist for that glyph
         *else got the value form `class_pair_values` 2D array*/
        if(left_class > 0 && right_class > 0) {
            value = kdsc->class_pair_values[(left_class - 1) * kdsc->right_class_cnt + (right_class - 1)];
        }

    }
    return value;
}

static int kern_pair_8_compare(const void * ref, const void * element)
{
    const kern_pair_ref_t * ref8_p = ref;
    const uint8_t * element8_p = element;

    /*If the MSB is different it will matter. If not return the diff. of the LSB*/
    if(ref8_p->gid_left != element8_p[0]) return ref8_p->gid_left - element8_p[0];
    else return ref8_p->gid_right - element8_p[1];
}

static int kern_pair_16_compare(const void * ref, const void * element)
{
    const kern_pair_ref_t * ref16_p = ref;
    const uint16_t * element16_p = element;

    /*If the MSB is different it will matter. If not return the diff. of the LSB*/
    if(ref16_p->gid_left != element16_p[0]) return ref16_p->gid_left - element16_p[0];
    else return ref16_p->gid_right - element16_p[1];
}

#if LV_USE_FONT_COMPRESSED

/**
 * The compress a glyph's bitmap
 * @param in the compressed bitmap
 * @param out buffer to store the result
 * @param px_num number of pixels in the glyph (width * height)
 * @param bpp bit per pixel (bpp = 3 will be converted to bpp = 4)
 * @param prefilter true: the lines are XORed
 */
static void decompress(const uint8_t * in, uint8_t * out, int32_t w, int32_t h, uint8_t bpp, bool prefilter)
{
    const lv_opa_t * opa_table;
    switch(bpp) {
        case 2:
            opa_table = opa2_table;
            break;
        case 3:
            opa_table = opa3_table;
            break;
        case 4:
            opa_table = opa4_table;
            break;
        default:
            LV_LOG_WARN("%d bpp is not handled", bpp);
            return;
    }

    rle_init(in, bpp);

    uint8_t * line_buf1 = lv_malloc(w);

    uint8_t * line_buf2 = NULL;

    if(prefilter) {
        line_buf2 = lv_malloc(w);
    }

    decompress_line(line_buf1, w);

    int32_t y;
    int32_t x;
    uint32_t stride = lv_draw_buf_width_to_stride(w, LV_COLOR_FORMAT_A8);

    for(x = 0; x < w; x++) {
        out[x] = opa_table[line_buf1[x]];
    }
    out += stride;

    for(y = 1; y < h; y++) {
        if(prefilter) {
            decompress_line(line_buf2, w);

            for(x = 0; x < w; x++) {
                line_buf1[x] = line_buf2[x] ^ line_buf1[x];
                out[x] = opa_table[line_buf1[x]];
            }
        }
        else {
            decompress_line(line_buf1, w);

            for(x = 0; x < w; x++) {
                out[x] = opa_table[line_buf1[x]];
            }
        }
        out += stride;
    }

    lv_free(line_buf1);
    lv_free(line_buf2);
}

/**
 * Decompress one line. Store one pixel per byte
 * @param out output buffer
 * @param w width of the line in pixel count
 */
static inline void decompress_line(uint8_t * out, int32_t w)
{
    int32_t i;
    for(i = 0; i < w; i++) {
        out[i] = rle_next();
    }
}

/**
 * Read bits from an input buffer. The read can cross byte boundary.
 * @param in the input buffer to read from.
 * @param bit_pos index of the first bit to read.
 * @param len number of bits to read (must be <= 8).
 * @return the read bits
 */
static inline uint8_t get_bits(const uint8_t * in, uint32_t bit_pos, uint8_t len)
{
    uint8_t bit_mask;
    switch(len) {
        case 1:
            bit_mask = 0x1;
            break;
        case 2:
            bit_mask = 0x3;
            break;
        case 3:
            bit_mask = 0x7;
            break;
        case 4:
            bit_mask = 0xF;
            break;
        case 8:
            bit_mask = 0xFF;
            break;
        default:
            bit_mask = (uint16_t)((uint16_t) 1 << len) - 1;
    }

    uint32_t byte_pos = bit_pos >> 3;
    bit_pos = bit_pos & 0x7;

    if(bit_pos + len >= 8) {
        uint16_t in16 = (in[byte_pos] << 8) + in[byte_pos + 1];
        return (in16 >> (16 - bit_pos - len)) & bit_mask;
    }
    else {
        return (in[byte_pos] >> (8 - bit_pos - len)) & bit_mask;
    }
}

static inline void rle_init(const uint8_t * in,  uint8_t bpp)
{
    lv_font_fmt_rle_t * rle = &font_rle;
    rle->in = in;
    rle->bpp = bpp;
    rle->state = RLE_STATE_SINGLE;
    rle->rdp = 0;
    rle->prev_v = 0;
    rle->count = 0;
}

static inline uint8_t rle_next(void)
{
    uint8_t v = 0;
    uint8_t ret = 0;
    lv_font_fmt_rle_t * rle = &font_rle;

    if(rle->state == RLE_STATE_SINGLE) {
        ret = get_bits(rle->in, rle->rdp, rle->bpp);
        if(rle->rdp != 0 && rle->prev_v == ret) {
            rle->count = 0;
            rle->state = RLE_STATE_REPEATED;
        }

        rle->prev_v = ret;
        rle->rdp += rle->bpp;
    }
    else if(rle->state == RLE_STATE_REPEATED) {
        v = get_bits(rle->in, rle->rdp, 1);
        rle->count++;
        rle->rdp += 1;
        if(v == 1) {
            ret = rle->prev_v;
            if(rle->count == 11) {
                rle->count = get_bits(rle->in, rle->rdp, 6);
                rle->rdp += 6;
                if(rle->count != 0) {
                    rle->state = RLE_STATE_COUNTER;
                }
                else {
                    ret = get_bits(rle->in, rle->rdp, rle->bpp);
                    rle->prev_v = ret;
                    rle->rdp += rle->bpp;
                    rle->state = RLE_STATE_SINGLE;
                }
            }
        }
        else {
            ret = get_bits(rle->in, rle->rdp, rle->bpp);
            rle->prev_v = ret;
            rle->rdp += rle->bpp;
            rle->state = RLE_STATE_SINGLE;
        }

    }
    else if(rle->state == RLE_STATE_COUNTER) {
        ret = rle->prev_v;
        rle->count--;
        if(rle->count == 0) {
            ret = get_bits(rle->in, rle->rdp, rle->bpp);
            rle->prev_v = ret;
            rle->rdp += rle->bpp;
            rle->state = RLE_STATE_SINGLE;
        }
    }

    return ret;
}
#endif /*LV_USE_FONT_COMPRESSED*/

/** Code Comparator.
 *
 *  Compares the value of both input arguments.
 *
 *  @param[in]  pRef        Pointer to the reference.
 *  @param[in]  pElement    Pointer to the element to compare.
 *
 *  @return Result of comparison.
 *  @retval < 0   Reference is less than element.
 *  @retval = 0   Reference is equal to element.
 *  @retval > 0   Reference is greater than element.
 *
 */
static int unicode_list_compare(const void * ref, const void * element)
{
    return (*(uint16_t *)ref) - (*(uint16_t *)element);
}

static lv_font_t * builtin_font_create_cb(const lv_font_info_t * info, const void * src)
{
    const lv_builtin_font_src_t * font_src = src;

    /**
     * If a crash occurs here, please check whether the last font in
     * the lv_builtin_font_src array is set to NULL as required to mark the end of the array.
     */
    while(font_src->font_p) {
        if(info->size == font_src->size) {
            return (lv_font_t *)font_src->font_p;
        }
        font_src++;
    }

    LV_LOG_WARN("No built-in font found with size: %" LV_PRIu32, info->size);
    return NULL;
}

static void builtin_font_delete_cb(lv_font_t * font)
{
    /*Nothing to delete*/
    LV_UNUSED(font);
}

static void * builtin_font_dup_src_cb(const void * src)
{
    const lv_builtin_font_src_t * font_src = src;
    uint32_t len = 0;

    /*Measure the size of the source data*/

    /**
     * If a crash occurs here, please check whether the last font in
     * the lv_builtin_font_src array is set to NULL as required to mark the end of the array.
     */
    while(font_src->font_p) {
        len++;
        font_src++;
    }

    if(len == 0) {
        LV_LOG_WARN("No source data found");
        return NULL;
    }

    lv_builtin_font_src_t * new_src = lv_malloc_zeroed(sizeof(lv_builtin_font_src_t) * (len + 1));
    LV_ASSERT_MALLOC(new_src);
    lv_memcpy(new_src, src, sizeof(lv_builtin_font_src_t) * len);

    return new_src;
}

static void builtin_font_free_src_cb(void * src)
{
    lv_free(src);
}
