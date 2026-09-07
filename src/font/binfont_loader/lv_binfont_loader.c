/**
 * @file lv_binfont_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"
#include "../fmt_txt/lv_font_fmt_txt_private.h"
#include "../../fs/lv_fs_private.h"

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
    int16_t underline_position;
    uint16_t underline_thickness;
} font_header_bin_t;

typedef struct {
    lv_font_fmt_txt_glyph_loader_t base;    /**< Must be the first member*/
    lv_fs_file_t file;                      /**< The font file, kept open for the font's lifetime*/
    uint8_t * bitmap_buf;                   /**< Scratch buffer, fits the largest glyph of the font*/
    uint32_t * glyph_offset_table;          /**< `loca_count` offsets, relative to `glyph_start`*/
    uint32_t loca_count;                    /**< Number of glyphs*/
    uint32_t glyph_start;                   /**< Position of the glyph table in the file*/
    uint32_t glyph_length;                  /**< Length of the glyph table in bytes*/
    uint32_t glyph_nbits;                   /**< Bits of the glyph header preceding the bitmap*/
    uint32_t bitmap_buf_size;               /**< Size of `bitmap_buf` in bytes*/
} lv_binfont_glyph_loader_t;

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
static bool load_font(lv_fs_file_t * fp, lv_font_t * font, bool dynamic_glyph_load);
int32_t load_kern(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc, uint8_t format, uint32_t start);

static int read_bits_signed(bit_iterator_t * it, int n_bits, lv_fs_res_t * res);
static unsigned int read_bits(bit_iterator_t * it, int n_bits, lv_fs_res_t * res);

static bool read_glyph_bitmap(lv_fs_file_t * fp, uint32_t pos, uint32_t nbits, uint8_t * buf, uint32_t buf_size);
static const uint8_t * load_glyph_bitmap_cb(lv_font_fmt_txt_glyph_loader_t * loader, uint32_t gid);
static lv_binfont_glyph_loader_t * glyph_loader_create(lv_fs_file_t * fp,
                                                       uint32_t * glyph_offset_table,
                                                       uint32_t loca_count, uint32_t glyph_start, uint32_t glyph_length,
                                                       uint32_t glyph_nbits, uint32_t max_bmp_size);
static void glyph_loader_delete(lv_binfont_glyph_loader_t * loader);

static lv_font_t * binfont_font_create_cb(const lv_font_info_t * info, const void * src);
static void binfont_font_delete_cb(lv_font_t * font);
static void * binfont_font_dup_src_cb(const void * src);
static void binfont_font_free_src_cb(void * src);

/**********************
 *      MACROS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

const lv_font_class_t lv_binfont_font_class = {
    .create_cb = binfont_font_create_cb,
    .delete_cb = binfont_font_delete_cb,
    .dup_src_cb = binfont_font_dup_src_cb,
    .free_src_cb = binfont_font_free_src_cb,
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_font_t * lv_binfont_create(const char * path)
{
    LV_CHECK_ARG(path != NULL, return NULL);

    const lv_binfont_dsc_t dsc = {
        .path = path,
    };

    return lv_binfont_create_ex(&dsc);
}

lv_font_t * lv_binfont_create_ex(const lv_binfont_dsc_t * dsc)
{
    LV_CHECK_ARG(dsc != NULL, return NULL);
#if LV_USE_FS_MEMFS
    LV_CHECK_ARG(dsc->path != NULL || dsc->buffer != NULL, return NULL);
#else
    LV_CHECK_ARG(dsc->path != NULL, return NULL);
#endif

    const char * path = dsc->path;
    lv_fs_path_ex_t mempath;
    if(LV_USE_FS_MEMFS && path == NULL) {
        lv_fs_make_path_from_buffer(&mempath, LV_FS_MEMFS_LETTER, (void *)dsc->buffer, dsc->buffer_size, "bin");
        path = (const char *)&mempath;
    }

    lv_fs_file_t file;
    lv_fs_res_t fs_res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(fs_res != LV_FS_RES_OK) return NULL;

    lv_font_t * font = lv_zalloc(sizeof(*font));
    LV_ASSERT_MALLOC(font);
    if(!font) {
        LV_LOG_ERROR("Failed to allocate memory for font");
        lv_fs_close(&file);
        return NULL;
    }


    bool loaded = load_font(&file, font, dsc->dynamic_glyph_load);

    const lv_font_fmt_txt_dsc_t * font_dsc = font->dsc;
    bool file_owned_by_loader = font_dsc != NULL && font_dsc->are_glyphs_dynamic_loaded;

    if(!loaded) {
        LV_LOG_WARN("Error loading font file: %s", path);
        lv_binfont_destroy(font);
        if(!file_owned_by_loader) lv_fs_close(&file);
        return NULL;
    }

    if(!file_owned_by_loader) lv_fs_close(&file);

    return font;
}

#if LV_USE_FS_MEMFS
lv_font_t * lv_binfont_create_from_buffer(void * buffer, uint32_t size)
{
    LV_CHECK_ARG(buffer != NULL, return NULL);

    const lv_binfont_dsc_t dsc = {
        .buffer = buffer,
        .buffer_size = size,
    };

    return lv_binfont_create_ex(&dsc);
}
#endif

void lv_binfont_destroy(lv_font_t * font)
{
    if(font == NULL) return;

    const lv_font_fmt_txt_dsc_t * dsc = font->dsc;
    if(dsc == NULL) return;

    if(dsc->kern_classes == 0) {
        const lv_font_fmt_txt_kern_pair_t * kern_dsc = dsc->kern_dsc;
        if(NULL != kern_dsc) {
            lv_free((void *)kern_dsc->glyph_ids);
            lv_free((void *)kern_dsc->values);
            lv_free((void *)kern_dsc);
        }
    }
    else {
        const lv_font_fmt_txt_kern_classes_t * kern_dsc = dsc->kern_dsc;
        if(NULL != kern_dsc) {
            lv_free((void *)kern_dsc->class_pair_values);
            lv_free((void *)kern_dsc->left_class_mapping);
            lv_free((void *)kern_dsc->right_class_mapping);
            lv_free((void *)kern_dsc);
        }
    }

    const lv_font_fmt_txt_cmap_t * cmaps = dsc->cmaps;
    if(NULL != cmaps) {
        for(int i = 0; i < dsc->cmap_num; ++i) {
            lv_free((void *)cmaps[i].glyph_id_ofs_list);
            lv_free((void *)cmaps[i].unicode_list);
        }
        lv_free((void *)cmaps);
    }

    if(dsc->are_glyphs_dynamic_loaded) {
        glyph_loader_delete((lv_binfont_glyph_loader_t *)dsc->glyph_bitmap);
    }
    else {
        lv_free((void *)dsc->glyph_bitmap);
    }

    lv_free((void *)dsc->glyph_dsc);
    lv_free((void *)dsc);
    lv_free(font);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bit_iterator_t init_bit_iterator(lv_fs_file_t * fp)
{
    LV_ASSERT(fp != NULL);
    bit_iterator_t it;
    it.fp = fp;
    it.bit_pos = -1;
    it.byte_value = 0;
    return it;
}

static unsigned int read_bits(bit_iterator_t * it, int n_bits, lv_fs_res_t * res)
{
    LV_ASSERT(it != NULL);
    LV_ASSERT(res != NULL);
    unsigned int value = 0;
    while(n_bits--) {
        it->byte_value = it->byte_value << 1;
        it->bit_pos--;

        if(it->bit_pos < 0) {
            it->bit_pos = 7;
            *res = lv_fs_read(it->fp, &(it->byte_value), 1, NULL);
            if(*res != LV_FS_RES_OK) {
                return 0;
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
    LV_ASSERT(it != NULL);
    LV_ASSERT(res != NULL);
    unsigned int value = read_bits(it, n_bits, res);
    if(value & (1 << (n_bits - 1))) {
        value |= ~0u << n_bits;
    }
    return value;
}

static int read_label(lv_fs_file_t * fp, int start, const char * label)
{
    LV_ASSERT(fp != NULL);
    LV_ASSERT(label != NULL);
    lv_fs_seek(fp, start, LV_FS_SEEK_SET);

    uint32_t length;
    char buf[4];

    if(lv_fs_read(fp, &length, 4, NULL) != LV_FS_RES_OK
       || lv_fs_read(fp, buf, 4, NULL) != LV_FS_RES_OK
       || lv_memcmp(label, buf, 4) != 0) {
        LV_LOG_WARN("Error reading '%s' label.", label);
        return -1;
    }

    return length;
}

static bool load_cmaps_tables(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc,
                              uint32_t cmaps_start, cmap_table_bin_t * cmap_table)
{
    LV_ASSERT(fp != NULL);
    LV_ASSERT(font_dsc != NULL);
    LV_ASSERT(cmap_table != NULL);
    if(lv_fs_read(fp, cmap_table, font_dsc->cmap_num * sizeof(cmap_table_bin_t), NULL) != LV_FS_RES_OK) {
        return false;
    }

    for(unsigned int i = 0; i < font_dsc->cmap_num; ++i) {
        lv_fs_res_t res = lv_fs_seek(fp, cmaps_start + cmap_table[i].data_offset, LV_FS_SEEK_SET);
        if(res != LV_FS_RES_OK) {
            return false;
        }

        lv_font_fmt_txt_cmap_t * cmap = (lv_font_fmt_txt_cmap_t *) & (font_dsc->cmaps[i]);

        cmap->range_start = cmap_table[i].range_start;
        cmap->range_length = cmap_table[i].range_length;
        cmap->glyph_id_start = cmap_table[i].glyph_id_start;
        cmap->type = cmap_table[i].format_type;

        switch(cmap_table[i].format_type) {
            case LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL: {
                    uint32_t ids_size = (uint32_t)(sizeof(uint8_t) * cmap_table[i].data_entries_count);
                    uint8_t * glyph_id_ofs_list = lv_malloc(ids_size);
                    LV_ASSERT_MALLOC(glyph_id_ofs_list);
                    if(!glyph_id_ofs_list) {
                        return false;
                    }

                    cmap->glyph_id_ofs_list = glyph_id_ofs_list;

                    if(lv_fs_read(fp, glyph_id_ofs_list, ids_size, NULL) != LV_FS_RES_OK) {
                        return false;
                    }

                    cmap->list_length = cmap->range_length;
                    break;
                }
            case LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY:
                break;
            case LV_FONT_FMT_TXT_CMAP_SPARSE_FULL:
            case LV_FONT_FMT_TXT_CMAP_SPARSE_TINY: {
                    uint32_t list_size = sizeof(uint16_t) * cmap_table[i].data_entries_count;
                    uint16_t * unicode_list = (uint16_t *)lv_malloc(list_size);
                    LV_ASSERT_MALLOC(unicode_list);
                    if(!unicode_list) {
                        return false;
                    }

                    cmap->unicode_list = unicode_list;
                    cmap->list_length = cmap_table[i].data_entries_count;

                    if(lv_fs_read(fp, unicode_list, list_size, NULL) != LV_FS_RES_OK) {
                        return false;
                    }

                    if(cmap_table[i].format_type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
                        uint16_t * buf = lv_malloc(sizeof(*buf) * cmap->list_length);
                        LV_ASSERT_MALLOC(buf);
                        if(!buf) {
                            return false;
                        }

                        cmap->glyph_id_ofs_list = buf;

                        if(lv_fs_read(fp, buf, sizeof(uint16_t) * cmap->list_length, NULL) != LV_FS_RES_OK) {
                            return false;
                        }
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
    LV_ASSERT(fp != NULL);
    LV_ASSERT(font_dsc != NULL);
    int32_t cmaps_length = read_label(fp, cmaps_start, "cmap");
    if(cmaps_length < 0) {
        return -1;
    }


    uint32_t cmaps_subtables_count;
    if(lv_fs_read(fp, &cmaps_subtables_count, sizeof(uint32_t), NULL) != LV_FS_RES_OK) {
        return -1;
    }

    lv_font_fmt_txt_cmap_t * cmaps = lv_zalloc(cmaps_subtables_count * sizeof(*cmaps));
    LV_ASSERT_MALLOC(cmaps);
    if(!cmaps) {
        return -1;
    }

    cmap_table_bin_t * cmaps_tables = lv_malloc(cmaps_subtables_count * sizeof(*cmaps_tables));
    LV_ASSERT_MALLOC(cmaps_tables);
    if(!cmaps_tables) {
        lv_free(cmaps);
        return -1;
    }

    font_dsc->cmaps = cmaps;
    font_dsc->cmap_num = cmaps_subtables_count;
    bool success = load_cmaps_tables(fp, font_dsc, cmaps_start, cmaps_tables);

    lv_free(cmaps_tables);

    return success ? cmaps_length : -1;
}

static bool read_glyph_bitmap(lv_fs_file_t * fp, uint32_t pos, uint32_t nbits, uint8_t * buf, uint32_t buf_size)
{
    LV_ASSERT(fp != NULL);
    LV_ASSERT(buf != NULL);
    LV_ASSERT(buf_size > 0);


    lv_fs_res_t res = lv_fs_seek(fp, pos, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) return false;

    bit_iterator_t bit_it = init_bit_iterator(fp);

    /*Skip the glyph header*/
    read_bits(&bit_it, nbits, &res);
    if(res != LV_FS_RES_OK) return false;

    if(nbits % 8 == 0) {  /*Fast path: the bitmap is byte aligned in the file*/
        return lv_fs_read(fp, buf, buf_size, NULL) == LV_FS_RES_OK;
    }

    for(uint32_t k = 0; k < buf_size - 1; ++k) {
        buf[k] = read_bits(&bit_it, 8, &res);
        if(res != LV_FS_RES_OK) return false;
    }

    buf[buf_size - 1] = read_bits(&bit_it, 8 - nbits % 8, &res);
    if(res != LV_FS_RES_OK) return false;

    /*The last fragment should be on the MSB but read_bits() will place it to the LSB*/
    buf[buf_size - 1] = buf[buf_size - 1] << (nbits % 8);

    return true;
}

static const uint8_t * load_glyph_bitmap_cb(lv_font_fmt_txt_glyph_loader_t * base, uint32_t gid)
{
    LV_ASSERT(base != NULL);
    lv_binfont_glyph_loader_t * loader = (lv_binfont_glyph_loader_t *)base;

    if(gid >= loader->loca_count) {
        LV_LOG_WARN("Invalid glyph index: %" LV_PRIu32 " >= %" LV_PRIu32, gid, loader->loca_count);
        return NULL;
    }

    uint32_t offset = loader->glyph_offset_table[gid];
    uint32_t next_offset = gid < loader->loca_count - 1 ? loader->glyph_offset_table[gid + 1] : loader->glyph_length;
    uint32_t header_size = loader->glyph_nbits / 8;

    if(next_offset < offset + header_size) {
        LV_LOG_WARN("Invalid glyph offsets for glyph %" LV_PRIu32, gid);
        return NULL;
    }

    uint32_t bmp_size = next_offset - offset - header_size;
    if(bmp_size == 0 || bmp_size > loader->bitmap_buf_size) {
        LV_LOG_WARN("Invalid bitmap size for glyph %" LV_PRIu32, gid);
        return NULL;
    }

    if(!read_glyph_bitmap(&loader->file, loader->glyph_start + offset, loader->glyph_nbits,
                          loader->bitmap_buf, bmp_size)) {
        return NULL;
    }

    return loader->bitmap_buf;
}

static void glyph_loader_delete(lv_binfont_glyph_loader_t * loader)
{
    if(loader == NULL) return;

    lv_fs_close(&loader->file);
    lv_mutex_delete(&loader->base.lock);
    lv_free(loader->bitmap_buf);
    loader->bitmap_buf = NULL;
    lv_free(loader->glyph_offset_table);
    loader->glyph_offset_table = NULL;
    lv_free(loader);
}

static lv_binfont_glyph_loader_t * glyph_loader_create(lv_fs_file_t * fp,
                                                       uint32_t * glyph_offset_table,
                                                       uint32_t loca_count, uint32_t glyph_start, uint32_t glyph_length,
                                                       uint32_t glyph_nbits, uint32_t max_bmp_size)
{
    LV_ASSERT(fp != NULL);
    LV_ASSERT(glyph_offset_table != NULL);

    lv_binfont_glyph_loader_t * loader = lv_malloc_zeroed(sizeof(lv_binfont_glyph_loader_t));
    LV_ASSERT_MALLOC(loader);
    if(loader == NULL) {
        LV_LOG_ERROR("Failed to allocate memory for dynamic glyph loader");
        return NULL;
    }

    loader->bitmap_buf = lv_malloc(LV_MAX(max_bmp_size, 1));
    LV_ASSERT_MALLOC(loader->bitmap_buf);
    if(loader->bitmap_buf == NULL) {
        LV_LOG_ERROR("Failed to allocate memory for scratch glyph buffer");
        lv_free(loader);
        return NULL;
    }

    if(lv_mutex_init(&loader->base.lock) != LV_RESULT_OK) {
        LV_LOG_WARN("Couldn't create the mutex of the glyph loader");
        lv_free(loader->bitmap_buf);
        lv_free(loader);
        return NULL;
    }

    loader->base.get_glyph_bitmap_cb = load_glyph_bitmap_cb;
    loader->file = *fp;
    loader->glyph_offset_table = glyph_offset_table;
    loader->loca_count = loca_count;
    loader->glyph_start = glyph_start;
    loader->glyph_length = glyph_length;
    loader->glyph_nbits = glyph_nbits;
    loader->bitmap_buf_size = max_bmp_size;
    return loader;
}

/**
 * Read the glyph table of the font.
 * @param dynamic_glyph_load    if `true` only the glyph descriptors are read and the bitmaps are
 *                              left in the file. `max_bmp_size` then receives the size of the
 *                              largest glyph bitmap, otherwise it's set to 0.
 * @return                      the length of the glyph table in bytes or -1 on error
 */
static int32_t load_glyph(lv_fs_file_t * fp, lv_font_fmt_txt_dsc_t * font_dsc,
                          uint32_t start, uint32_t * glyph_offset_table, uint32_t loca_count, font_header_bin_t * header,
                          bool dynamic_glyph_load, uint32_t * max_bmp_size)
{
    LV_ASSERT(fp != NULL);
    LV_ASSERT(font_dsc != NULL);
    LV_ASSERT(glyph_offset_table != NULL);
    LV_ASSERT(header != NULL);
    LV_ASSERT(max_bmp_size != NULL);

    int32_t glyph_length = read_label(fp, start, "glyf");
    if(glyph_length < 0) {
        return -1;
    }

    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc = lv_zalloc(loca_count * sizeof(*glyph_dsc));
    LV_ASSERT_MALLOC(glyph_dsc);
    if(!glyph_dsc) {
        return -1;
    }

    font_dsc->glyph_dsc = glyph_dsc;

    int32_t nbits = header->advance_width_bits + 2 * header->xy_bits + 2 * header->wh_bits;
    int32_t cur_bmp_size = 0;
    *max_bmp_size = 0;

    for(unsigned int i = 0; i < loca_count; ++i) {
        lv_font_fmt_txt_glyph_dsc_t * gdsc = &glyph_dsc[i];

        lv_fs_res_t res = lv_fs_seek(fp, start + glyph_offset_table[i], LV_FS_SEEK_SET);
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

        int next_offset = (i < loca_count - 1) ? glyph_offset_table[i + 1] : (uint32_t)glyph_length;
        int bmp_size = next_offset - glyph_offset_table[i] - nbits / 8;

        if(i == 0) {
            gdsc->adv_w = 0;
            gdsc->box_w = 0;
            gdsc->box_h = 0;
            gdsc->ofs_x = 0;
            gdsc->ofs_y = 0;
        }

        gdsc->bitmap_index = cur_bmp_size;

        if(gdsc->box_w * gdsc->box_h == 0) continue;

        if(bmp_size <= 0) {
            LV_LOG_WARN("Invalid bitmap size of glyph %u", i);
            return -1;
        }

        if(dynamic_glyph_load) {
            /* store the biggest bmp size so that we can allocate a buffer for it*/
            *max_bmp_size = LV_MAX(*max_bmp_size, (uint32_t)bmp_size);
        }
        else {
            cur_bmp_size += bmp_size;
        }
    }

    if(dynamic_glyph_load) {
        return glyph_length;
    }

    uint8_t * glyph_bmp = (uint8_t *)lv_malloc(LV_MAX(cur_bmp_size, 1));
    LV_ASSERT_MALLOC(glyph_bmp);
    if(!glyph_bmp) {
        return -1;
    }

    font_dsc->glyph_bitmap = glyph_bmp;
    font_dsc->are_glyphs_dynamic_loaded = false;

    for(unsigned int i = 1; i < loca_count; ++i) {
        if(glyph_dsc[i].box_w * glyph_dsc[i].box_h == 0) {
            continue;
        }

        int next_offset = (i < loca_count - 1) ? glyph_offset_table[i + 1] : (uint32_t)glyph_length;
        int bmp_size = next_offset - glyph_offset_table[i] - nbits / 8;

        if(!read_glyph_bitmap(fp, start + glyph_offset_table[i], nbits,
                              &glyph_bmp[glyph_dsc[i].bitmap_index], bmp_size)) {
            return -1;
        }
    }
    return glyph_length;
}

static void release_glyph_cb(const lv_font_t * font, lv_font_glyph_dsc_t * glyph_dsc)
{
    LV_UNUSED(font);
    LV_UNUSED(glyph_dsc);
    /*No custom memory management needed*/
}

/*
 * Loads a `lv_font_t` from a binary file, given a `lv_fs_file_t`.
 *
 * Memory allocations on `lvgl_load_font` should be immediately zeroed and
 * the pointer should be set on the `lv_font_t` data before any possible return.
 *
 * When something fails, it returns `false` and the memory on the `lv_font_t`
 * still needs to be freed using `lv_binfont_destroy`.
 *
 * `lv_binfont_destroy` will assume that all non-null pointers are allocated and
 * should be freed.
 */
static bool load_font(lv_fs_file_t * fp, lv_font_t * font, bool dynamic_glyph_load)
{
    LV_ASSERT(fp != NULL);
    LV_ASSERT(font != NULL);
    lv_font_fmt_txt_dsc_t * font_dsc = lv_zalloc(sizeof(*font_dsc));
    LV_ASSERT_MALLOC(font_dsc);
    if(!font_dsc) {
        return false;
    }

    font->dsc = font_dsc;

    /*header*/
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
    font->release_glyph = release_glyph_cb;
    font->subpx = font_header.subpixels_mode;
    font->underline_position = (int8_t) font_header.underline_position;
    font->underline_thickness = (int8_t) font_header.underline_thickness;

    font_dsc->bpp = font_header.bits_per_pixel;
    font_dsc->kern_scale = font_header.kerning_scale;
    font_dsc->bitmap_format = font_header.compression_id;

    /*cmaps*/
    uint32_t cmaps_start = header_length;
    int32_t cmaps_length = load_cmaps(fp, font_dsc, cmaps_start);
    if(cmaps_length < 0) {
        return false;
    }

    /*loca*/
    uint32_t loca_start = cmaps_start + cmaps_length;
    int32_t loca_length = read_label(fp, loca_start, "loca");
    if(loca_length < 0) {
        return false;
    }

    uint32_t loca_count;
    if(lv_fs_read(fp, &loca_count, sizeof(uint32_t), NULL) != LV_FS_RES_OK) {
        return false;
    }

    uint32_t * glyph_offset_table = lv_malloc(sizeof(uint32_t) * loca_count);
    LV_ASSERT_MALLOC(glyph_offset_table);
    if(!glyph_offset_table) {
        LV_LOG_ERROR("Failed to load allocate glyph offset table");
        return false;
    }

    switch(font_header.index_to_loc_format) {
        case 0:
            for(unsigned int i = 0; i < loca_count; ++i) {
                uint16_t offset;
                if(lv_fs_read(fp, &offset, sizeof(uint16_t), NULL) != LV_FS_RES_OK) {
                    lv_free(glyph_offset_table);
                    return false;
                }
                glyph_offset_table[i] = offset;
            }
            break;
        case 1:
            if(lv_fs_read(fp, glyph_offset_table, loca_count * sizeof(uint32_t), NULL) != LV_FS_RES_OK) {
                lv_free(glyph_offset_table);
                return false;
            }
            break;
        default:
            LV_LOG_WARN("Unknown index_to_loc_format: %d.", font_header.index_to_loc_format);
            lv_free(glyph_offset_table);
            return false;
    }

    /*glyph*/
    uint32_t glyph_start = loca_start + loca_length;
    uint32_t max_bmp_size = 0;
    int32_t glyph_length = load_glyph(fp, font_dsc, glyph_start, glyph_offset_table, loca_count, &font_header,
                                      dynamic_glyph_load, &max_bmp_size);

    if(glyph_length < 0) {
        lv_free(glyph_offset_table);
        return false;
    }

    if(dynamic_glyph_load) {
        uint32_t glyph_nbits = font_header.advance_width_bits + 2 * font_header.xy_bits + 2 * font_header.wh_bits;

        lv_binfont_glyph_loader_t * loader = glyph_loader_create(fp,
                                                                 glyph_offset_table,
                                                                 loca_count,
                                                                 glyph_start,
                                                                 (uint32_t)glyph_length,
                                                                 glyph_nbits,
                                                                 max_bmp_size);

        /*On success the loader takes over `fp` and `glyph_offset_table`*/
        if(!loader) {
            lv_free(glyph_offset_table);
            return false;
        }
        font_dsc->glyph_bitmap = (const uint8_t *)loader;
        font_dsc->are_glyphs_dynamic_loaded = true;
    }
    else {
        lv_free(glyph_offset_table);
    }

    /*kerning*/
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
    LV_ASSERT(fp != NULL);
    LV_ASSERT(font_dsc != NULL);
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

    if(0 == kern_format_type) { /*sorted pairs*/
        lv_font_fmt_txt_kern_pair_t * kern_pair = lv_zalloc(sizeof(*kern_pair));
        LV_ASSERT_MALLOC(kern_pair);
        if(!kern_pair) {
            return -1;
        }

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

        uint8_t * glyph_ids = lv_malloc(ids_size);
        LV_ASSERT_MALLOC(glyph_ids);
        int8_t * values = lv_malloc(glyph_entries);
        LV_ASSERT_MALLOC(values);
        if(!glyph_ids || !values) {
            lv_free(values);
            lv_free(glyph_ids);
            return -1;
        }

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
    else if(3 == kern_format_type) { /*array M*N of classes*/

        lv_font_fmt_txt_kern_classes_t * kern_classes = lv_zalloc(sizeof(*kern_classes));
        LV_ASSERT_MALLOC(kern_classes);
        if(!kern_classes) {
            return -1;
        }

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

        uint8_t * kern_left = lv_malloc(kern_class_mapping_length);
        LV_ASSERT_MALLOC(kern_left);
        uint8_t * kern_right = lv_malloc(kern_class_mapping_length);
        LV_ASSERT_MALLOC(kern_right);
        int8_t * kern_values = lv_malloc(kern_values_length);
        LV_ASSERT_MALLOC(kern_values);

        if(!kern_left || !kern_right || !kern_values) {
            lv_free(kern_left);
            lv_free(kern_right);
            lv_free(kern_values);
            return -1;
        }

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

static lv_font_t * binfont_font_create_cb(const lv_font_info_t * info, const void * src)
{
    LV_ASSERT(info != NULL);
    LV_ASSERT(src != NULL);
    const lv_binfont_font_src_t * font_src = src;

    if(info->size != font_src->font_size) return NULL;

    const lv_binfont_dsc_t dsc = {
        .path = font_src->path,
        .buffer = font_src->buffer,
        .buffer_size = font_src->buffer_size,
        .dynamic_glyph_load = font_src->dynamic_glyph_load,
    };

    return lv_binfont_create_ex(&dsc);
}

static void binfont_font_delete_cb(lv_font_t * font)
{
    lv_binfont_destroy(font);
}

static void * binfont_font_dup_src_cb(const void * src)
{
    LV_ASSERT(src != NULL);
    const lv_binfont_font_src_t * font_src = src;

    lv_binfont_font_src_t * new_src = lv_malloc_zeroed(sizeof(lv_binfont_font_src_t));
    LV_ASSERT_MALLOC(new_src);
    if(!new_src) {
        return NULL;
    }
    *new_src = *font_src;

    if(font_src->path) {
        new_src->path = lv_strdup(font_src->path);
    }

    return new_src;
}

static void binfont_font_free_src_cb(void * src)
{
    LV_ASSERT(src != NULL);
    lv_binfont_font_src_t * font_src = src;

    lv_free((char *)font_src->path);
    font_src->path = NULL;
    lv_free(font_src);
}
