/**
 * @file lv_font_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include <stdbool.h>

#include "../lvgl.h"
#include "../lv_misc/lv_fs.h"
#include "lv_font_loader.h"

#if LV_USE_FILESYSTEM


/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_fs_file_t * fp;
    int8_t bit_pos;
    uint8_t byte_value;
} bit_iterator_t;

typedef struct font_header_bin {
    uint32_t version;
    uint16_t tables_count;
    uint16_t font_size;
    uint16_t ascent;
    int16_t descent;
    uint16_t typo_ascent;
    int16_t typo_descent;
    uint16_t typo_line_gap;
    int16_t min_y;
    int16_t max_y;
    uint16_t default_advance_width;
    uint16_t kerning_scale;
    uint8_t index_to_loc_format;
    uint8_t glyph_id_format;
    uint8_t advance_width_format;
    uint8_t bits_per_pixel;
    uint8_t xy_bits;
    uint8_t wh_bits;
    uint8_t advance_width_bits;
    uint8_t compression_id;
    uint8_t subpixels_mode;
    uint8_t padding;
} font_header_bin_t;

typedef struct cmap_table_bin {
    uint32_t data_offset;
    uint32_t range_start;
    uint16_t range_length;
    uint16_t glyph_id_start;
    uint16_t data_entries_count;
    uint8_t format_type;
    uint8_t padding;
} cmap_table_bin_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static bit_iterator_t init_bit_iterator(lv_fs_file_t * fp);
static bool lvgl_load_font(lv_fs_file_t * fp, lv_font_t * font);
int32_t load_kern(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc, uint8_t format, uint32_t start);

static int read_bits_signed(bit_iterator_t * it, int n_bits, lv_fs_res_t * res);
static int read_bits(bit_iterator_t * it, int n_bits, lv_fs_res_t * res);


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Loads a `lv_font_t` object from a binary font file
 * @param font_name filename where the font file is located
 * @return a pointer to the font or NULL in case of error
 */
lv_font_t * lv_font_load(const char * font_name)
{
    bool success = false;

    lv_font_t * font = lv_mem_alloc(sizeof(lv_font_t));
    memset(font, 0, sizeof(lv_font_t));

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, font_name, LV_FS_MODE_RD);

    if(res == LV_FS_RES_OK) {
        success = lvgl_load_font(&file, font);
    }

    if(!success) {
        LV_LOG_WARN("Error loading font file: %s\n", font_name);
        /*
         * When `lvgl_load_font` fails it can leak some pointers.
         * All non-null pointers can be assumed as allocated and
         * `lv_font_free` should free them correctly.
         */
        lv_font_free(font);
        font = NULL;
    }

    lv_fs_close(&file);

    return font;
}


/**
 * Frees the memory allocated by the `lv_font_load()` function
 * @param font lv_font_t object created by the lv_font_load function
 */
void lv_font_free(lv_font_t * font)
{
    if(NULL != font) {
        lv_font_fmt_txt_dsc_t * dsc = (lv_font_fmt_txt_dsc_t *) font->dsc;

        if(NULL != dsc) {

            if(dsc->kern_classes == 0) {
                lv_font_fmt_txt_kern_pair_t * kern_dsc =
                    (lv_font_fmt_txt_kern_pair_t *) dsc->kern_dsc;

                if(NULL != kern_dsc) {
                    if(kern_dsc->glyph_ids)
                        lv_mem_free((void *) kern_dsc->glyph_ids);

                    if(kern_dsc->values)
                        lv_mem_free((void *) kern_dsc->values);

                    lv_mem_free((void *) kern_dsc);
                }
            }
            else {
                lv_font_fmt_txt_kern_classes_t * kern_dsc =
                    (lv_font_fmt_txt_kern_classes_t *) dsc->kern_dsc;

                if(NULL != kern_dsc) {
                    if(kern_dsc->class_pair_values)
                        lv_mem_free((void *) kern_dsc->class_pair_values);

                    if(kern_dsc->left_class_mapping)
                        lv_mem_free((void *) kern_dsc->left_class_mapping);

                    if(kern_dsc->right_class_mapping)
                        lv_mem_free((void *) kern_dsc->right_class_mapping);

                    lv_mem_free((void *) kern_dsc);
                }
            }

            lv_font_fmt_txt_cmap_t * cmaps =
                (lv_font_fmt_txt_cmap_t *) dsc->cmaps;

            if(NULL != cmaps) {
                for(int i = 0; i < dsc->cmap_num; ++i) {
                    if(NULL != cmaps[i].glyph_id_ofs_list)
                        lv_mem_free((void *) cmaps[i].glyph_id_ofs_list);
                    if(NULL != cmaps[i].unicode_list)
                        lv_mem_free((void *) cmaps[i].unicode_list);
                }
                lv_mem_free(cmaps);
            }

            if(NULL != dsc->glyph_bitmap) {
                lv_mem_free((void *) dsc->glyph_bitmap);
            }
            if(NULL != dsc->glyph_dsc) {
                lv_mem_free((void *) dsc->glyph_dsc);
            }
            lv_mem_free(dsc);
        }
        lv_mem_free(font);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static bit_iterator_t init_bit_iterator(lv_fs_file_t * fp)
{
    bit_iterator_t it;
    it.fp = fp;
    it.bit_pos = -1;
    it.byte_value = 0;
    return it;
}

static int read_bits(bit_iterator_t * it, int n_bits, lv_fs_res_t * res)
{
    int value = 0;
    while(n_bits--) {
        it->byte_value = it->byte_value << 1;
        it->bit_pos--;

        if(it->bit_pos < 0) {
            it->bit_pos = 7;
            *res = lv_fs_read(it->fp, &(it->byte_value), 1, NULL);
            if(*res != LV_FS_RES_OK) {
                return -1;
            }
        }
        int8_t bit = (it->byte_value & 0x80) ? 1 : 0;

        value |= (bit << n_bits);
    }
    *res = LV_FS_RES_OK;
    return value;
}

static int read_bits_signed(bit_iterator_t * it, int n_bits, lv_fs_res_t * res)
{
    int value = read_bits(it, n_bits, res);
    if(value & (1 << (n_bits - 1))) {
        for(int bit = n_bits; bit < 16; ++bit) {
            value |= (1 << bit);
        }
    }
    return value;
}

static int read_label(lv_fs_file_t * fp, int start, const char * label)
{
    lv_fs_seek(fp, start);

    uint32_t length;
    char buf[4];

    if(lv_fs_read(fp, &length, 4, NULL) != LV_FS_RES_OK
       || lv_fs_read(fp, buf, 4, NULL) != LV_FS_RES_OK
       || memcmp(label, buf, 4) != 0) {
        LV_LOG_WARN("Error reading '%s' label.", label);
        return -1;
    }

    return length;
}

static bool load_cmaps_tables(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc,
                              uint32_t cmaps_start, cmap_table_bin_t * cmap_table)
{
    for(unsigned int i = 0; i < font_dsc->cmap_num; ++i) {
        if(lv_fs_read(fp, &cmap_table[i], sizeof(cmap_table_bin_t), NULL) != LV_FS_RES_OK) {
            return false;
        }

        lv_font_fmt_txt_cmap_t * cmap = (lv_font_fmt_txt_cmap_t *) & (font_dsc->cmaps[i]);

        cmap->range_start = cmap_table[i].range_start;
        cmap->range_length = cmap_table[i].range_length;
        cmap->glyph_id_start = cmap_table[i].glyph_id_start;
    }

    for(unsigned int i = 0; i < font_dsc->cmap_num; ++i) {
        lv_fs_res_t res = lv_fs_seek(fp, cmaps_start + cmap_table[i].data_offset);
        if(res != LV_FS_RES_OK) {
            return false;
        }

        lv_font_fmt_txt_cmap_t * cmap = (lv_font_fmt_txt_cmap_t *) & (font_dsc->cmaps[i]);

        switch(cmap_table[i].format_type) {
            case 0: {
                    uint8_t ids_size = sizeof(uint8_t) * cmap_table[i].data_entries_count;
                    uint8_t * glyph_id_ofs_list = lv_mem_alloc(ids_size);

                    cmap->glyph_id_ofs_list = glyph_id_ofs_list;

                    if(lv_fs_read(fp, glyph_id_ofs_list, ids_size, NULL) != LV_FS_RES_OK) {
                        return false;
                    }

                    cmap->type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL;
                    cmap->list_length = cmap->range_length;
                    cmap->unicode_list = NULL;
                    break;
                }
            case 2:
                cmap->type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY;
                cmap->list_length = 0;
                cmap->unicode_list = NULL;
                cmap->glyph_id_ofs_list = NULL;
                break;
            case 1:
            case 3: {
                    uint32_t list_size = sizeof(uint16_t) * cmap_table[i].data_entries_count;
                    uint16_t * unicode_list = (uint16_t *) lv_mem_alloc(list_size);

                    cmap->unicode_list = unicode_list;
                    cmap->list_length = cmap_table[i].data_entries_count;

                    if(lv_fs_read(fp, unicode_list, list_size, NULL) != LV_FS_RES_OK) {
                        return false;
                    }

                    if(cmap_table[i].format_type == 1) {
                        uint16_t * buf = lv_mem_alloc(sizeof(uint16_t) * cmap->list_length);

                        cmap->type = LV_FONT_FMT_TXT_CMAP_SPARSE_FULL;
                        cmap->glyph_id_ofs_list = buf;

                        if(lv_fs_read(fp, buf, sizeof(uint16_t) * cmap->list_length, NULL) != LV_FS_RES_OK) {
                            return false;
                        }
                    }
                    else {
                        cmap->type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY;
                        cmap->glyph_id_ofs_list = NULL;
                    }
                    break;
                }
            default:
                LV_LOG_WARN("Unknown cmaps format type %d.", cmap_table[i].format_type);
                return false;
        }
    }
    return true;
}

static int32_t load_cmaps(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc, uint32_t cmaps_start)
{
    int32_t cmaps_length = read_label(fp, cmaps_start, "cmap");
    if(cmaps_length < 0) {
        return -1;
    }

    uint32_t cmaps_subtables_count;
    if(lv_fs_read(fp, &cmaps_subtables_count, sizeof(uint32_t), NULL) != LV_FS_RES_OK) {
        return -1;
    }

    lv_font_fmt_txt_cmap_t * cmaps =
        lv_mem_alloc(cmaps_subtables_count * sizeof(lv_font_fmt_txt_cmap_t));

    memset(cmaps, 0, cmaps_subtables_count * sizeof(lv_font_fmt_txt_cmap_t));

    font_dsc->cmaps = cmaps;
    font_dsc->cmap_num = cmaps_subtables_count;

    cmap_table_bin_t * cmaps_tables = lv_mem_alloc(sizeof(cmap_table_bin_t) * font_dsc->cmap_num);

    bool success = load_cmaps_tables(fp, font_dsc, cmaps_start, cmaps_tables);

    lv_mem_free(cmaps_tables);

    return success ? cmaps_length : -1;
}

static int32_t load_glyph(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc,
                          uint32_t start, uint32_t * glyph_offset, uint32_t loca_count, font_header_bin_t * header)
{
    int32_t glyph_length = read_label(fp, start, "glyf");
    if(glyph_length < 0) {
        return -1;
    }

    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc = (lv_font_fmt_txt_glyph_dsc_t *)
                                              lv_mem_alloc(loca_count * sizeof(lv_font_fmt_txt_glyph_dsc_t));

    memset(glyph_dsc, 0, loca_count * sizeof(lv_font_fmt_txt_glyph_dsc_t));

    font_dsc->glyph_dsc = glyph_dsc;

    int cur_bmp_size = 0;

    for(unsigned int i = 0; i < loca_count; ++i) {
        lv_font_fmt_txt_glyph_dsc_t * gdsc = &glyph_dsc[i];

        lv_fs_res_t res = lv_fs_seek(fp, start + glyph_offset[i]);
        if(res != LV_FS_RES_OK) {
            return -1;
        }

        bit_iterator_t bit_it = init_bit_iterator(fp);

        if(header->advance_width_bits == 0) {
            gdsc->adv_w = header->default_advance_width;
        }
        else {
            gdsc->adv_w = read_bits(&bit_it, header->advance_width_bits, &res);
            if(res != LV_FS_RES_OK) {
                return -1;
            }
        }

        if(header->advance_width_format == 0) {
            gdsc->adv_w *= 16;
        }

        gdsc->ofs_x = read_bits_signed(&bit_it, header->xy_bits, &res);
        if(res != LV_FS_RES_OK) {
            return -1;
        }

        gdsc->ofs_y = read_bits_signed(&bit_it, header->xy_bits, &res);
        if(res != LV_FS_RES_OK) {
            return -1;
        }

        gdsc->box_w = read_bits(&bit_it, header->wh_bits, &res);
        if(res != LV_FS_RES_OK) {
            return -1;
        }

        gdsc->box_h = read_bits(&bit_it, header->wh_bits, &res);
        if(res != LV_FS_RES_OK) {
            return -1;
        }

        int nbits = header->advance_width_bits + 2 * header->xy_bits + 2 * header->wh_bits;
        int next_offset = (i < loca_count - 1) ? glyph_offset[i + 1] : (uint32_t)(glyph_length - 1);
        int bmp_size = next_offset - glyph_offset[i] - nbits / 8;

        if(i == 0) {
            gdsc->adv_w = 0;
            gdsc->box_w = 0;
            gdsc->box_h = 0;
            gdsc->ofs_x = 0;
            gdsc->ofs_y = 0;
        }

        gdsc->bitmap_index = cur_bmp_size;
        if(gdsc->box_w * gdsc->box_h != 0) {
            cur_bmp_size += bmp_size;
        }
    }

    uint8_t * glyph_bmp = (uint8_t *) lv_mem_alloc(sizeof(uint8_t) * cur_bmp_size);

    font_dsc->glyph_bitmap = glyph_bmp;

    cur_bmp_size = 0;

    for(unsigned int i = 1; i < loca_count; ++i) {
        lv_fs_res_t res = lv_fs_seek(fp, start + glyph_offset[i]);
        if(res != LV_FS_RES_OK) {
            return -1;
        }
        bit_iterator_t bit_it = init_bit_iterator(fp);

        int nbits = header->advance_width_bits + 2 * header->xy_bits + 2 * header->wh_bits;

        read_bits(&bit_it, nbits, &res);
        if(res != LV_FS_RES_OK) {
            return -1;
        }

        if(glyph_dsc[i].box_w * glyph_dsc[i].box_h == 0) {
            continue;
        }

        int next_offset = (i < loca_count - 1) ? glyph_offset[i + 1] : (uint32_t)(glyph_length - 1);
        int bmp_size = next_offset - glyph_offset[i] - nbits / 8;

        for(int k = 0; k < bmp_size; ++k) {
            glyph_bmp[cur_bmp_size + k] = read_bits(&bit_it, 8, &res);
            if(res != LV_FS_RES_OK) {
                return -1;
            }
        }
        cur_bmp_size += bmp_size;
    }
    return glyph_length;
}

/*
 * Loads a `lv_font_t` from a binary file, given a `lv_fs_file_t`.
 *
 * Memory allocations on `lvgl_load_font` should be immediately zeroed and
 * the pointer should be set on the `lv_font_t` data before any possible return.
 *
 * When something fails, it returns `false` and the memory on the `lv_font_t`
 * still needs to be freed using `lv_font_free`.
 *
 * `lv_font_free` will assume that all non-null pointers are allocated and
 * should be freed.
 */
static bool lvgl_load_font(lv_fs_file_t * fp, lv_font_t * font)
{
    lv_font_fmt_txt_dsc_t * font_dsc = (lv_font_fmt_txt_dsc_t *)
                                       lv_mem_alloc(sizeof(lv_font_fmt_txt_dsc_t));

    memset(font_dsc, 0, sizeof(lv_font_fmt_txt_dsc_t));

    font->dsc = font_dsc;

    /* header */
    int32_t header_length = read_label(fp, 0, "head");
    if(header_length < 0) {
        return false;
    }

    font_header_bin_t font_header;
    if(lv_fs_read(fp, &font_header, sizeof(font_header_bin_t), NULL) != LV_FS_RES_OK) {
        return false;
    }

    font->base_line = -font_header.descent;
    font->line_height = font_header.ascent - font_header.descent;
    font->get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt;
    font->get_glyph_bitmap = lv_font_get_bitmap_fmt_txt;
    font->subpx = font_header.subpixels_mode;

    font_dsc->bpp = font_header.bits_per_pixel;
    font_dsc->kern_scale = font_header.kerning_scale;
    font_dsc->bitmap_format = font_header.compression_id;

    /* cmaps */
    uint32_t cmaps_start = header_length;
    int32_t cmaps_length = load_cmaps(fp, font_dsc, cmaps_start);
    if(cmaps_length < 0) {
        return false;
    }

    /* loca */
    uint32_t loca_start = cmaps_start + cmaps_length;
    int32_t loca_length = read_label(fp, loca_start, "loca");
    if(loca_length < 0) {
        return false;
    }

    uint32_t loca_count;
    if(lv_fs_read(fp, &loca_count, sizeof(uint32_t), NULL) != LV_FS_RES_OK) {
        return false;
    }

    bool failed = false;
    uint32_t * glyph_offset = lv_mem_alloc(sizeof(uint32_t) * (loca_count + 1));

    for(unsigned int i = 0; i < loca_count; ++i) {
        if(font_header.index_to_loc_format == 0) {
            uint16_t offset;
            if(lv_fs_read(fp, &offset, sizeof(uint16_t), NULL) != LV_FS_RES_OK) {
                failed = true;
                break;
            }
            glyph_offset[i] = offset;
        }
        else if(font_header.index_to_loc_format == 1) {
            uint32_t offset;
            if(lv_fs_read(fp, &offset, sizeof(uint32_t), NULL) != LV_FS_RES_OK) {
                failed = true;
                break;
            }
            glyph_offset[i] = offset;
        }
        else {
            LV_LOG_WARN("Unknown index_to_loc_format: %d.", font_header.index_to_loc_format);
            failed = true;
            break;
        }
    }

    if(failed) {
        lv_mem_free(glyph_offset);
        return false;
    }

    /* glyph */
    uint32_t glyph_start = loca_start + loca_length;
    int32_t glyph_length = load_glyph(
                               fp, font_dsc, glyph_start, glyph_offset, loca_count, &font_header);

    lv_mem_free(glyph_offset);

    if(glyph_length < 0) {
        return false;
    }

    if(font_header.tables_count < 4) {
        font_dsc->kern_dsc = NULL;
        font_dsc->kern_classes = 0;
        font_dsc->kern_scale = 0;
        return true;
    }

    uint32_t kern_start = glyph_start + glyph_length;

    int32_t kern_length = load_kern(fp, font_dsc, font_header.glyph_id_format, kern_start);

    return kern_length >= 0;
}

int32_t load_kern(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc, uint8_t format, uint32_t start)
{
    int32_t kern_length = read_label(fp, start, "kern");
    if(kern_length < 0) {
        return -1;
    }

    uint8_t kern_format_type;
    int32_t padding;
    if(lv_fs_read(fp, &kern_format_type, sizeof(uint8_t), NULL) != LV_FS_RES_OK ||
       lv_fs_read(fp, &padding, 3 * sizeof(uint8_t), NULL) != LV_FS_RES_OK) {
        return -1;
    }

    if(0 == kern_format_type) { /* sorted pairs */
        lv_font_fmt_txt_kern_pair_t * kern_pair = lv_mem_alloc(sizeof(lv_font_fmt_txt_kern_pair_t));

        memset(kern_pair, 0, sizeof(lv_font_fmt_txt_kern_pair_t));

        font_dsc->kern_dsc = kern_pair;
        font_dsc->kern_classes = 0;

        uint32_t glyph_entries;
        if(lv_fs_read(fp, &glyph_entries, sizeof(uint32_t), NULL) != LV_FS_RES_OK) {
            return -1;
        }

        int ids_size;
        if(format == 0) {
            ids_size = sizeof(int8_t) * 2 * glyph_entries;
        }
        else {
            ids_size = sizeof(int16_t) * 2 * glyph_entries;
        }

        uint8_t * glyph_ids = lv_mem_alloc(ids_size);
        int8_t * values = lv_mem_alloc(glyph_entries);

        kern_pair->glyph_ids_size = format;
        kern_pair->pair_cnt = glyph_entries;
        kern_pair->glyph_ids = glyph_ids;
        kern_pair->values = values;

        if(lv_fs_read(fp, glyph_ids, ids_size, NULL) != LV_FS_RES_OK) {
            return -1;
        }

        if(lv_fs_read(fp, values, glyph_entries, NULL) != LV_FS_RES_OK) {
            return -1;
        }
    }
    else if(3 == kern_format_type) { /* array M*N of classes */

        lv_font_fmt_txt_kern_classes_t * kern_classes = lv_mem_alloc(sizeof(lv_font_fmt_txt_kern_classes_t));

        memset(kern_classes, 0, sizeof(lv_font_fmt_txt_kern_classes_t));

        font_dsc->kern_dsc = kern_classes;
        font_dsc->kern_classes = 1;

        uint16_t kern_class_mapping_length;
        uint8_t kern_table_rows;
        uint8_t kern_table_cols;

        if(lv_fs_read(fp, &kern_class_mapping_length, sizeof(uint16_t), NULL) != LV_FS_RES_OK ||
           lv_fs_read(fp, &kern_table_rows, sizeof(uint8_t), NULL) != LV_FS_RES_OK ||
           lv_fs_read(fp, &kern_table_cols, sizeof(uint8_t), NULL) != LV_FS_RES_OK) {
            return -1;
        }

        int kern_values_length = sizeof(int8_t) * kern_table_rows * kern_table_cols;

        uint8_t * kern_left = lv_mem_alloc(kern_class_mapping_length);
        uint8_t * kern_right = lv_mem_alloc(kern_class_mapping_length);
        int8_t * kern_values = lv_mem_alloc(kern_values_length);

        kern_classes->left_class_mapping  = kern_left;
        kern_classes->right_class_mapping = kern_right;
        kern_classes->left_class_cnt = kern_table_rows;
        kern_classes->right_class_cnt = kern_table_cols;
        kern_classes->class_pair_values = kern_values;

        if(lv_fs_read(fp, kern_left, kern_class_mapping_length, NULL) != LV_FS_RES_OK ||
           lv_fs_read(fp, kern_right, kern_class_mapping_length, NULL) != LV_FS_RES_OK ||
           lv_fs_read(fp, kern_values, kern_values_length, NULL) != LV_FS_RES_OK) {
            return -1;
        }
    }
    else {
        LV_LOG_WARN("Unknown kern_format_type: %d", kern_format_type);
        return -1;
    }

    return kern_length;
}

#endif /*LV_USE_FILESYSTEM*/

