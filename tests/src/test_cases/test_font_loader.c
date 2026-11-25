/**
 * @file test_font_loader.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#if LV_BUILD_TEST
#include "../../lvgl.h"

#include "unity/unity.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int compare_fonts(lv_font_t * f1, lv_font_t * f2);
void test_font_loader_with_cache(void);
void test_font_loader_no_cache(void);
void test_font_loader_from_buffer(void);
void test_font_loader_dynamic(void);
void test_font_loader_corrupt_kern(void);
void test_font_loader_corrupt_bitmap(void);
void test_font_loader_kern_class_out_of_range(void);
void test_font_loader_kern_mapping_shorter_than_glyphs(void);
void test_font_loader_corrupt_cmap(void);
void test_font_loader_corrupt_bit_width(void);
void test_font_loader_truncated(void);
void test_font_loader_dynamic_from_buffer(void);
void test_font_loader_rejects_out_of_bounds_subtable(void);
void test_font_loader_rejects_huge_cmap_subtable_count(void);
void test_font_loader_mapped(void);
void test_font_loader_mapped_dynamic(void);
void test_font_loader_map_not_implemented(void);
void test_font_loader_mapped_glyph_bitmap_len(void);
void test_font_loader_mapped_rejects_corrupt_fonts(void);
void test_font_loader_mapped_kern_class_out_of_range(void);
void test_font_loader_mapped_kern_mapping_shorter_than_glyphs(void);
void test_font_loader_mapped_compressed_bitmap_bounded(void);
static void map_drv_cleanup(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* fonts converted to C structs using the LVGL Font Converter */
extern lv_font_t test_font_1;
extern lv_font_t test_font_2;
extern lv_font_t test_font_3;

/* font binaries converted to plain C arrays */
extern uint8_t const test_font_1_buf[6876];
extern uint8_t const test_font_2_buf[7252];
extern uint8_t const test_font_3_buf[4892];

static lv_font_t * font_1_bin = NULL;
static lv_font_t * font_2_bin = NULL;
static lv_font_t * font_3_bin = NULL;

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */

    /*A failing assert skips the rest of the test: release what it left behind*/
    lv_obj_clean(lv_screen_active());
    lv_binfont_destroy(font_1_bin);
    lv_binfont_destroy(font_2_bin);
    lv_binfont_destroy(font_3_bin);
    font_1_bin = NULL;
    font_2_bin = NULL;
    font_3_bin = NULL;
    map_drv_cleanup();
}

static void common(void)
{
    compare_fonts(&test_font_1, font_1_bin);
    compare_fonts(&test_font_2, font_2_bin);
    compare_fonts(&test_font_3, font_3_bin);

    /* create labels for testing */
    lv_obj_t * scr = lv_screen_active();
    lv_obj_t * label1 = lv_label_create(scr);
    lv_obj_t * label2 = lv_label_create(scr);
    lv_obj_t * label3 = lv_label_create(scr);

    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_label_set_text(label1, "The quick brown fox jumped over the lazy dog");
    lv_obj_set_style_text_font(label1, font_1_bin, 0);
    lv_label_set_text(label2, "The quick brown fox jumped over the lazy dog");
    lv_obj_set_style_text_font(label2, font_2_bin, 0);
    lv_label_set_text(label3, "The quick brown fox jumped over the lazy dog");
    lv_obj_set_style_text_font(label3, font_3_bin, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("font_loader_1.png");

    lv_obj_clean(lv_screen_active());

    lv_binfont_destroy(font_1_bin);
    lv_binfont_destroy(font_2_bin);
    lv_binfont_destroy(font_3_bin);
    font_1_bin = NULL;
    font_2_bin = NULL;
    font_3_bin = NULL;
}

void test_font_loader_with_cache(void)
{
    /*Test with cache ('A' has cache)*/

    font_1_bin = lv_binfont_create("A:src/test_assets/test_font_1.fnt");
    TEST_ASSERT_NOT_NULL(font_1_bin);

    font_2_bin = lv_binfont_create("A:src/test_assets/test_font_2.fnt");
    TEST_ASSERT_NOT_NULL(font_2_bin);

    font_3_bin = lv_binfont_create("A:src/test_assets/test_font_3.fnt");
    TEST_ASSERT_NOT_NULL(font_3_bin);

    common();
}

void test_font_loader_no_cache(void)
{
    /*Test without cache ('B' has NO cache)*/

    font_1_bin = lv_binfont_create("B:src/test_assets/test_font_1.fnt");
    TEST_ASSERT_NOT_NULL(font_1_bin);

    font_2_bin = lv_binfont_create("B:src/test_assets/test_font_2.fnt");
    TEST_ASSERT_NOT_NULL(font_2_bin);

    font_3_bin = lv_binfont_create("B:src/test_assets/test_font_3.fnt");
    TEST_ASSERT_NOT_NULL(font_3_bin);

    common();
}

void test_font_loader_from_buffer(void)
{
    /*Test with memfs*/

    font_1_bin = lv_binfont_create_from_buffer((void *)&test_font_1_buf, sizeof(test_font_1_buf));
    TEST_ASSERT_NOT_NULL(font_1_bin);

    font_2_bin = lv_binfont_create_from_buffer((void *)&test_font_2_buf, sizeof(test_font_2_buf));
    TEST_ASSERT_NOT_NULL(font_2_bin);

    font_3_bin = lv_binfont_create_from_buffer((void *)&test_font_3_buf, sizeof(test_font_3_buf));
    TEST_ASSERT_NOT_NULL(font_3_bin);

    common();
}

void test_font_loader_dynamic(void)
{
    /*Load the glyph bitmaps on demand instead of loading them all into RAM*/

    lv_binfont_dsc_t dsc = {
        .dynamic_glyph_load = true,
    };

    dsc.path = "A:src/test_assets/test_font_1.fnt";
    font_1_bin = lv_binfont_create_ex(&dsc);
    TEST_ASSERT_NOT_NULL(font_1_bin);

    dsc.path = "A:src/test_assets/test_font_2.fnt";
    font_2_bin = lv_binfont_create_ex(&dsc);
    TEST_ASSERT_NOT_NULL(font_2_bin);

    dsc.path = "A:src/test_assets/test_font_3.fnt";
    font_3_bin = lv_binfont_create_ex(&dsc);
    TEST_ASSERT_NOT_NULL(font_3_bin);

    /*The rendered result has to be identical to the fully loaded fonts*/
    common();
}

void test_font_loader_dynamic_from_buffer(void)
{
    /*Dynamic loading from a memory mapped file*/

    lv_binfont_dsc_t dsc = {
        .dynamic_glyph_load = true,
    };

    dsc.buffer = test_font_1_buf;
    dsc.buffer_size = sizeof(test_font_1_buf);
    font_1_bin = lv_binfont_create_ex(&dsc);
    TEST_ASSERT_NOT_NULL(font_1_bin);

    dsc.buffer = test_font_2_buf;
    dsc.buffer_size = sizeof(test_font_2_buf);
    font_2_bin = lv_binfont_create_ex(&dsc);
    TEST_ASSERT_NOT_NULL(font_2_bin);

    dsc.buffer = test_font_3_buf;
    dsc.buffer_size = sizeof(test_font_3_buf);
    font_3_bin = lv_binfont_create_ex(&dsc);
    TEST_ASSERT_NOT_NULL(font_3_bin);

    common();
}

#if LV_USE_FONT_COMPRESSED
/*Request a glyph through the normal (decompressing) path so the bitmap
 *readers actually run on the loaded font.*/
static void request_glyph_bitmap(lv_font_t * font, uint32_t letter)
{
    lv_font_glyph_dsc_t g_dsc;
    TEST_ASSERT_TRUE(lv_font_get_glyph_dsc(font, &g_dsc, letter, 0));

    lv_draw_buf_t * buf = lv_draw_buf_create(g_dsc.box_w, g_dsc.box_h, LV_COLOR_FORMAT_A8, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /*Must return without reading past the glyph's stored data.*/
    TEST_ASSERT_NOT_NULL(lv_font_get_glyph_bitmap(&g_dsc, buf));

    lv_font_glyph_release_draw_data(&g_dsc);
    lv_draw_buf_destroy(buf);
}
#endif /*LV_USE_FONT_COMPRESSED*/

void test_font_loader_corrupt_bit_width(void)
{
    /*A glyph header field wider than 32 bits cannot be returned by read_bits().*/
    const char * path = "A:src/test_assets/test_font_bit_width_oob.fnt";

    TEST_ASSERT_NULL(lv_binfont_create(path));

    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = true,
    };
    TEST_ASSERT_NULL(lv_binfont_create_ex(&dsc));
}

void test_font_loader_corrupt_kern(void)
{
    const char * path = "A:src/test_assets/test_font_corrupt_kern.fnt";

    TEST_ASSERT_NULL(lv_binfont_create(path));

    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = true,
    };
    TEST_ASSERT_NULL(lv_binfont_create_ex(&dsc));
}

/*A plain bitmap whose box does not fit its stored data is rejected.*/
void test_font_loader_corrupt_bitmap(void)
{
    const char * path = "A:src/test_assets/test_font_bitmap_oob.fnt";

    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = true,
    };
    TEST_ASSERT_NULL(lv_binfont_create(path));
    TEST_ASSERT_NULL(lv_binfont_create_ex(&dsc));

#if LV_USE_FONT_COMPRESSED
    /*A truncated compressed stream is bounded at read time: the glyph has to be
     *requested for the compressed reader to run, and doing so must not crash.*/
    const char * path2 = "A:src/test_assets/test_font_bitmap_rle_oob.fnt";
    lv_font_t * font = lv_binfont_create(path2);
    TEST_ASSERT_NOT_NULL(font);
    request_glyph_bitmap(font, 'A');
    lv_binfont_destroy(font);
#endif
}

/*Return a copy of test_font_1_buf and the offset of the class kerning data in it*/
static uint8_t * dup_font_1_with_kern(uint32_t * kern_ofs)
{
    uint8_t * buf = lv_malloc(sizeof(test_font_1_buf));
    TEST_ASSERT_NOT_NULL(buf);
    lv_memcpy(buf, test_font_1_buf, sizeof(test_font_1_buf));

    uint32_t i;
    for(i = 0; i + 8 < sizeof(test_font_1_buf); i++) {
        if(lv_memcmp(&buf[i], "kern", 4) == 0) break;
    }
    TEST_ASSERT_LESS_THAN_UINT32(sizeof(test_font_1_buf) - 8, i);

    /*"kern", format(1) + padding(3), mapping length(2), rows(1), cols(1), left[], right[], values[]*/
    TEST_ASSERT_EQUAL_UINT8(3, buf[i + 4]);
    *kern_ofs = i + 8;
    return buf;
}

void test_font_loader_kern_class_out_of_range(void)
{
    uint32_t ofs;
    uint8_t * buf = dup_font_1_with_kern(&ofs);

    /*The unmodified font loads*/
    lv_font_t * font = lv_binfont_create_from_buffer(buf, sizeof(test_font_1_buf));
    TEST_ASSERT_NOT_NULL(font);
    lv_binfont_destroy(font);

    uint16_t mapping_len = buf[ofs] | (buf[ofs + 1] << 8);
    uint8_t rows = buf[ofs + 2];
    uint8_t cols = buf[ofs + 3];
    uint8_t * left = &buf[ofs + 4];
    uint8_t * right = &buf[ofs + 4 + mapping_len];

    /*A left class greater than the number of rows would index past class_pair_values*/
    left[mapping_len - 1] = rows + 1;
    TEST_ASSERT_NULL(lv_binfont_create_from_buffer(buf, sizeof(test_font_1_buf)));
    left[mapping_len - 1] = 0;

    /*Same for a right class greater than the number of columns*/
    right[mapping_len - 1] = cols + 1;
    TEST_ASSERT_NULL(lv_binfont_create_from_buffer(buf, sizeof(test_font_1_buf)));
    right[mapping_len - 1] = 0;

    font = lv_binfont_create_from_buffer(buf, sizeof(test_font_1_buf));
    TEST_ASSERT_NOT_NULL(font);
    lv_binfont_destroy(font);

    lv_free(buf);
}

void test_font_loader_kern_mapping_shorter_than_glyphs(void)
{
    uint32_t ofs;
    uint8_t * buf = dup_font_1_with_kern(&ofs);

    /*The class mappings are indexed by glyph id, so they must cover every glyph*/
    uint16_t mapping_len = buf[ofs] | (buf[ofs + 1] << 8);
    mapping_len--;
    buf[ofs] = mapping_len & 0xff;
    buf[ofs + 1] = mapping_len >> 8;
    TEST_ASSERT_NULL(lv_binfont_create_from_buffer(buf, sizeof(test_font_1_buf)));

    lv_free(buf);
}

/*A cmap must not reference glyphs that the loca table doesn't describe.*/
void test_font_loader_corrupt_cmap(void)
{
    const char * path = "A:src/test_assets/test_font_cmap_glyph_oob.fnt";

    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = true,
    };
    TEST_ASSERT_NULL(lv_binfont_create(path));
    TEST_ASSERT_NULL(lv_binfont_create_ex(&dsc));
}

void test_font_loader_truncated(void)
{
    const char * path = "A:src/test_assets/test_font_truncated.fnt";

    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = true,
    };
    TEST_ASSERT_NULL(lv_binfont_create(path));
    TEST_ASSERT_NULL(lv_binfont_create_ex(&dsc));
}

/*The subtable data has to stay inside the cmap section. A crafted font can point a
 *subtable past the end of the section while the read itself stays inside the file, which
 *used to load silently and expose data from the following table.*/
void test_font_loader_rejects_out_of_bounds_subtable(void)
{
    const char * path = "A:src/test_assets/test_font_cmap_data_oob.fnt";

    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = true,
    };
    TEST_ASSERT_NULL(lv_binfont_create(path));
    TEST_ASSERT_NULL(lv_binfont_create_ex(&dsc));
}

/*The subtable count is read from the file, so it must not be used as an allocation size
 *before it is bounded to the section it belongs to.*/
void test_font_loader_rejects_huge_cmap_subtable_count(void)
{
    const char * path = "A:src/test_assets/test_font_count_oob.fnt";

    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = true,
    };
    TEST_ASSERT_NULL(lv_binfont_create(path));
    TEST_ASSERT_NULL(lv_binfont_create_ex(&dsc));
}

/*A drive that serves each file from an exact-size heap copy, so ASan catches a read past the
 *end of a mapping and a use of the mapping after close. It maps files (unless `map_off`) and
 *counts what reaches it per handle.*/
#define MAP_DRV_LETTER 'X'
#define MAP_DRV_MAX_FILES 4

typedef struct {
    bool used;
    uint8_t * buf;
    uint32_t size;
    uint32_t pos;
    uint32_t reads;     /**< read_cb calls on this handle*/
    uint32_t maps;      /**< map_cb calls that succeeded on this handle*/
    int32_t live_maps;  /**< mappings not unmapped yet*/
} map_drv_file_t;

static lv_fs_drv_t map_drv;
static bool map_drv_registered;
static map_drv_file_t map_drv_files[MAP_DRV_MAX_FILES];
static struct {
    bool map_off;                   /**< map_cb returns NOT_IMP*/
    const uint8_t * src;            /**< content of "X:buf"*/
    uint32_t src_size;
    uint32_t opens;
    uint32_t closes;
    uint32_t maps;                  /**< successful map_cb calls on every handle*/
    uint32_t reads;                 /**< read_cb calls on every handle*/
    uint32_t reads_on_mapped;       /**< read_cb calls on closed handles that had a mapping*/
    uint32_t closed_while_mapped;   /**< closes with a mapping still alive*/
    uint32_t bad_unmaps;            /**< unmaps of a pointer outside the file or never mapped*/
} map_drv_rec;

/*Font buffers the kern tests modify, released in tearDown*/
static uint8_t * map_test_buf;

static void * map_drv_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    if(mode != LV_FS_MODE_RD) return NULL;

    map_drv_file_t * f = NULL;
    for(uint32_t i = 0; i < MAP_DRV_MAX_FILES; i++) {
        if(!map_drv_files[i].used) {
            f = &map_drv_files[i];
            break;
        }
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "too many files open on the mapping drive");
    lv_memzero(f, sizeof(*f));

    if(lv_strcmp(path, "buf") == 0) {
        f->size = map_drv_rec.src_size;
        f->buf = lv_malloc(f->size);
        TEST_ASSERT_NOT_NULL(f->buf);
        lv_memcpy(f->buf, map_drv_rec.src, f->size);
    }
    else {
        char real_path[256];
        lv_snprintf(real_path, sizeof(real_path), "A:%s", path);
        lv_fs_file_t src;
        if(lv_fs_open(&src, real_path, LV_FS_MODE_RD) != LV_FS_RES_OK) return NULL;
        lv_fs_seek(&src, 0, LV_FS_SEEK_END);
        lv_fs_tell(&src, &f->size);
        lv_fs_seek(&src, 0, LV_FS_SEEK_SET);
        f->buf = lv_malloc(f->size);
        TEST_ASSERT_NOT_NULL(f->buf);
        uint32_t br = 0;
        lv_fs_read(&src, f->buf, f->size, &br);
        lv_fs_close(&src);
        TEST_ASSERT_EQUAL_UINT32(f->size, br);
    }

    f->used = true;
    map_drv_rec.opens++;
    return f;
}

static lv_fs_res_t map_drv_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    map_drv_file_t * f = file_p;
    if(f->live_maps != 0) map_drv_rec.closed_while_mapped++;
    if(f->maps > 0) map_drv_rec.reads_on_mapped += f->reads;
    lv_free(f->buf);
    lv_memzero(f, sizeof(*f));
    map_drv_rec.closes++;
    return LV_FS_RES_OK;
}

static lv_fs_res_t map_drv_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    map_drv_file_t * f = file_p;
    f->reads++;
    map_drv_rec.reads++;
    uint32_t n = f->pos < f->size ? LV_MIN(btr, f->size - f->pos) : 0;
    lv_memcpy(buf, f->buf + f->pos, n);
    f->pos += n;
    if(br) *br = n;
    return LV_FS_RES_OK;
}

static lv_fs_res_t map_drv_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    map_drv_file_t * f = file_p;
    if(whence == LV_FS_SEEK_SET) f->pos = pos;
    else if(whence == LV_FS_SEEK_CUR) f->pos += pos;
    else f->pos = f->size + pos;
    return LV_FS_RES_OK;
}

static lv_fs_res_t map_drv_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    *pos_p = ((map_drv_file_t *)file_p)->pos;
    return LV_FS_RES_OK;
}

static lv_fs_res_t map_drv_map(lv_fs_drv_t * drv, void * file_p, uint32_t offset, uint32_t len, const void ** ptr)
{
    LV_UNUSED(drv);
    map_drv_file_t * f = file_p;
    if(map_drv_rec.map_off) return LV_FS_RES_NOT_IMP;
    if(offset > f->size || len > f->size - offset) return LV_FS_RES_INV_PARAM;
    f->maps++;
    f->live_maps++;
    map_drv_rec.maps++;
    *ptr = f->buf + offset;
    return LV_FS_RES_OK;
}

static lv_fs_res_t map_drv_unmap(lv_fs_drv_t * drv, void * file_p, const void * ptr, uint32_t len)
{
    LV_UNUSED(drv);
    map_drv_file_t * f = file_p;
    const uint8_t * p = ptr;
    if(p < f->buf || p > f->buf + f->size || len > (uint32_t)(f->buf + f->size - p) || f->live_maps <= 0) {
        map_drv_rec.bad_unmaps++;
        return LV_FS_RES_INV_PARAM;
    }
    f->live_maps--;
    return LV_FS_RES_OK;
}

static void map_drv_register(bool map_off)
{
    if(map_drv_registered) lv_fs_remove_drive(MAP_DRV_LETTER);
    lv_memzero(&map_drv_rec, sizeof(map_drv_rec));
    lv_memzero(map_drv_files, sizeof(map_drv_files));
    map_drv_rec.map_off = map_off;

    lv_fs_drv_init(&map_drv);
    map_drv.letter = MAP_DRV_LETTER;
    map_drv.open_cb = map_drv_open;
    map_drv.close_cb = map_drv_close;
    map_drv.read_cb = map_drv_read;
    map_drv.seek_cb = map_drv_seek;
    map_drv.tell_cb = map_drv_tell;
    map_drv.map_cb = map_drv_map;
    map_drv.unmap_cb = map_drv_unmap;
    lv_fs_drv_register(&map_drv);
    map_drv_registered = true;
}

static void map_drv_cleanup(void)
{
    if(map_drv_registered) lv_fs_remove_drive(MAP_DRV_LETTER);
    map_drv_registered = false;
    for(uint32_t i = 0; i < MAP_DRV_MAX_FILES; i++) {
        lv_free(map_drv_files[i].buf);
    }
    lv_memzero(map_drv_files, sizeof(map_drv_files));
    lv_free(map_test_buf);
    map_test_buf = NULL;
}

/*Reads on handles that had a mapping, open ones included*/
static uint32_t map_drv_reads_on_mapped(void)
{
    uint32_t reads = map_drv_rec.reads_on_mapped;
    for(uint32_t i = 0; i < MAP_DRV_MAX_FILES; i++) {
        if(map_drv_files[i].used && map_drv_files[i].maps > 0) reads += map_drv_files[i].reads;
    }
    return reads;
}

/*Every file opened on the drive is closed, and no mapping outlived its file*/
static void assert_map_drv_released(void)
{
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(map_drv_rec.opens, map_drv_rec.closes, "files left open");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, map_drv_rec.closed_while_mapped, "closed with a live mapping");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, map_drv_rec.bad_unmaps, "unmapped a pointer it didn't map");
}

static lv_font_t * create_font(const char * path, bool dynamic)
{
    lv_binfont_dsc_t dsc = {
        .path = path,
        .dynamic_glyph_load = dynamic,
    };
    return lv_binfont_create_ex(&dsc);
}

static void create_mapped_fonts(bool dynamic)
{
    font_1_bin = create_font("X:src/test_assets/test_font_1.fnt", dynamic);
    TEST_ASSERT_NOT_NULL(font_1_bin);
    font_2_bin = create_font("X:src/test_assets/test_font_2.fnt", dynamic);
    TEST_ASSERT_NOT_NULL(font_2_bin);
    font_3_bin = create_font("X:src/test_assets/test_font_3.fnt", dynamic);
    TEST_ASSERT_NOT_NULL(font_3_bin);
}

/*A font on a drive that maps files is parsed from the mapping: same font as from stdio, and
 *nothing read through the drive. The glyph bitmaps don't point into the mapping: the drive
 *frees the file at close, ASan would catch a use while rendering.*/
void test_font_loader_mapped(void)
{
    map_drv_register(false);

    create_mapped_fonts(false);
    TEST_ASSERT_GREATER_THAN_UINT32(0, map_drv_rec.maps);
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_reads_on_mapped());
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_rec.reads);
    assert_map_drv_released();

    common();
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_rec.reads);
    assert_map_drv_released();
}

/*Dynamic glyph loading keeps the file open and draws the glyphs from the mapping*/
void test_font_loader_mapped_dynamic(void)
{
    map_drv_register(false);

    create_mapped_fonts(true);
    TEST_ASSERT_GREATER_THAN_UINT32(0, map_drv_rec.maps);
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_rec.reads);

    /*Renders every glyph of the label and destroys the fonts*/
    common();
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_rec.reads);
    assert_map_drv_released();
}

/*A drive that can't map a file falls back to reading it*/
void test_font_loader_map_not_implemented(void)
{
    map_drv_register(true);

    create_mapped_fonts(false);
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_rec.maps);
    TEST_ASSERT_GREATER_THAN_UINT32(0, map_drv_rec.reads);
    common();
    assert_map_drv_released();

    create_mapped_fonts(true);
    common();
    assert_map_drv_released();
}

/*A mapped font knows the length of its glyph bitmaps as a read one does*/
void test_font_loader_mapped_glyph_bitmap_len(void)
{
    static const char * const names[] = {"test_font_1.fnt", "test_font_2.fnt", "test_font_3.fnt"};
    map_drv_register(false);

    for(uint32_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        for(uint32_t dynamic = 0; dynamic < 2; dynamic++) {
            char path[64];
            lv_snprintf(path, sizeof(path), "A:src/test_assets/%s", names[i]);
            font_1_bin = create_font(path, dynamic);
            TEST_ASSERT_NOT_NULL(font_1_bin);
            path[0] = MAP_DRV_LETTER;
            font_2_bin = create_font(path, dynamic);
            TEST_ASSERT_NOT_NULL(font_2_bin);

            const lv_font_fmt_txt_dsc_t * read_dsc = font_1_bin->dsc;
            const lv_font_fmt_txt_dsc_t * mapped_dsc = font_2_bin->dsc;
            TEST_ASSERT_EQUAL_MESSAGE(read_dsc->are_glyphs_dynamic_loaded, mapped_dsc->are_glyphs_dynamic_loaded, path);
            TEST_ASSERT_EQUAL_UINT32_MESSAGE(read_dsc->glyph_bitmap_len, mapped_dsc->glyph_bitmap_len, path);
            if(!dynamic) TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(0, mapped_dsc->glyph_bitmap_len, path);

            lv_binfont_destroy(font_1_bin);
            lv_binfont_destroy(font_2_bin);
            font_1_bin = NULL;
            font_2_bin = NULL;
        }
    }

    TEST_ASSERT_GREATER_THAN_UINT32(0, map_drv_rec.maps);
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_reads_on_mapped());
    assert_map_drv_released();
}

/*Every check the loader makes on a read file holds on a mapped one, and the error paths
 *release the mapping before closing the file*/
void test_font_loader_mapped_rejects_corrupt_fonts(void)
{
    static const char * const paths[] = {
        "X:src/test_assets/test_font_bit_width_oob.fnt",
        "X:src/test_assets/test_font_corrupt_kern.fnt",
        "X:src/test_assets/test_font_bitmap_oob.fnt",
        "X:src/test_assets/test_font_cmap_glyph_oob.fnt",
        "X:src/test_assets/test_font_truncated.fnt",
        "X:src/test_assets/test_font_cmap_data_oob.fnt",
        "X:src/test_assets/test_font_count_oob.fnt",
    };
    map_drv_register(false);

    for(uint32_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        uint32_t maps = map_drv_rec.maps;
        TEST_ASSERT_NULL_MESSAGE(lv_binfont_create(paths[i]), paths[i]);
        TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(maps, map_drv_rec.maps, paths[i]);
        TEST_ASSERT_NULL_MESSAGE(create_font(paths[i], true), paths[i]);
        assert_map_drv_released();
    }

    TEST_ASSERT_EQUAL_UINT32(0, map_drv_reads_on_mapped());
}

/*Load a kern test font from "X:buf", mapped or read*/
static lv_font_t * create_from_map_drv(const uint8_t * buf, uint32_t size, bool map_off)
{
    map_drv_register(map_off);
    map_drv_rec.src = buf;
    map_drv_rec.src_size = size;
    return lv_binfont_create("X:buf");
}

void test_font_loader_mapped_kern_class_out_of_range(void)
{
    for(uint32_t map_off = 0; map_off < 2; map_off++) {
        uint32_t ofs;
        uint8_t * buf = dup_font_1_with_kern(&ofs);
        map_test_buf = buf;
        uint32_t size = sizeof(test_font_1_buf);

        uint16_t mapping_len = buf[ofs] | (buf[ofs + 1] << 8);
        uint8_t rows = buf[ofs + 2];
        uint8_t cols = buf[ofs + 3];
        uint8_t * left = &buf[ofs + 4];
        uint8_t * right = &buf[ofs + 4 + mapping_len];

        /*The unmodified font loads*/
        font_1_bin = create_from_map_drv(buf, size, map_off);
        TEST_ASSERT_NOT_NULL(font_1_bin);
        if(map_off) TEST_ASSERT_EQUAL_UINT32(0, map_drv_rec.maps);
        else TEST_ASSERT_GREATER_THAN_UINT32(0, map_drv_rec.maps);
        lv_binfont_destroy(font_1_bin);
        font_1_bin = NULL;
        assert_map_drv_released();

        left[mapping_len - 1] = rows + 1;
        TEST_ASSERT_NULL(create_from_map_drv(buf, size, map_off));
        assert_map_drv_released();
        left[mapping_len - 1] = 0;

        right[mapping_len - 1] = cols + 1;
        TEST_ASSERT_NULL(create_from_map_drv(buf, size, map_off));
        assert_map_drv_released();

        lv_free(buf);
        map_test_buf = NULL;
    }
}

void test_font_loader_mapped_kern_mapping_shorter_than_glyphs(void)
{
    for(uint32_t map_off = 0; map_off < 2; map_off++) {
        uint32_t ofs;
        uint8_t * buf = dup_font_1_with_kern(&ofs);
        map_test_buf = buf;

        uint16_t mapping_len = buf[ofs] | (buf[ofs + 1] << 8);
        mapping_len--;
        buf[ofs] = mapping_len & 0xff;
        buf[ofs + 1] = mapping_len >> 8;

        TEST_ASSERT_NULL(create_from_map_drv(buf, sizeof(test_font_1_buf), map_off));
        if(!map_off) TEST_ASSERT_GREATER_THAN_UINT32(0, map_drv_rec.maps);
        assert_map_drv_released();

        lv_free(buf);
        map_test_buf = NULL;
    }
}

/*A truncated compressed glyph is bounded by the file's end when the file is mapped too: the
 *mapping ends exactly where the heap copy does*/
void test_font_loader_mapped_compressed_bitmap_bounded(void)
{
#if LV_USE_FONT_COMPRESSED
    map_drv_register(false);

    for(uint32_t dynamic = 0; dynamic < 2; dynamic++) {
        font_1_bin = create_font("X:src/test_assets/test_font_bitmap_rle_oob.fnt", dynamic);
        TEST_ASSERT_NOT_NULL(font_1_bin);
        request_glyph_bitmap(font_1_bin, 'A');
        lv_binfont_destroy(font_1_bin);
        font_1_bin = NULL;
    }

    TEST_ASSERT_GREATER_THAN_UINT32(0, map_drv_rec.maps);
    TEST_ASSERT_EQUAL_UINT32(0, map_drv_reads_on_mapped());
    assert_map_drv_released();
#else
    TEST_PASS();
#endif
}

void test_font_loader_reload(void)
{
    /*Reload a font which is being used by a label*/
    lv_obj_t * scr = lv_screen_active();
    lv_obj_t * label = lv_label_create(scr);
    lv_obj_center(label);
    lv_label_set_text(label, "The quick brown fox jumped over the lazy dog");

    lv_font_t style_font;
    lv_font_t * font;
    font = lv_binfont_create("A:src/test_assets/test_font_2.fnt");
    TEST_ASSERT_NOT_NULL(font);
    lv_memcpy(&style_font, font, sizeof(lv_font_t));

    lv_obj_set_style_text_font(label, &style_font, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("font_loader_2.png");

    lv_binfont_destroy(font);

    font = lv_binfont_create("A:src/test_assets/test_font_3.fnt");
    TEST_ASSERT_NOT_NULL(font);
    lv_memcpy(&style_font, font, sizeof(lv_font_t));

    lv_obj_report_style_change(NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("font_loader_3.png");

    lv_obj_delete(label);

    lv_binfont_destroy(font);
}

static int compare_fonts(lv_font_t * f1, lv_font_t * f2)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(f1, "font not null");
    TEST_ASSERT_NOT_NULL_MESSAGE(f2, "font not null");

    //    Skip these test because -Wpedantic tells
    //    ISO C forbids passing argument 1 of ‘TEST_ASSERT_EQUAL_PTR_MESSAGE’ between function pointer and ‘void *’
    //    TEST_ASSERT_EQUAL_PTR_MESSAGE(f1->get_glyph_dsc, f2->get_glyph_dsc, "glyph_dsc");
    //    TEST_ASSERT_EQUAL_PTR_MESSAGE(f1->get_glyph_bitmap, f2->get_glyph_bitmap, "glyph_bitmap");

    TEST_ASSERT_EQUAL_INT_MESSAGE(f1->line_height, f2->line_height, "line_height");
    TEST_ASSERT_EQUAL_INT_MESSAGE(f1->base_line, f2->base_line, "base_line");
    TEST_ASSERT_EQUAL_INT_MESSAGE(f1->subpx, f2->subpx, "subpx");
    lv_font_fmt_txt_dsc_t * dsc1 = (lv_font_fmt_txt_dsc_t *)f1->dsc;
    lv_font_fmt_txt_dsc_t * dsc2 = (lv_font_fmt_txt_dsc_t *)f2->dsc;

    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->kern_scale, dsc2->kern_scale, "kern_scale");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->cmap_num, dsc2->cmap_num, "cmap_num");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->bpp, dsc2->bpp, "bpp");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->kern_classes, dsc2->kern_classes, "kern_classes");
    TEST_ASSERT_EQUAL_INT_MESSAGE(dsc1->bitmap_format, dsc2->bitmap_format, "bitmap_format");

    // cmaps
    int total_glyphs = 0;
    for(int i = 0; i < dsc1->cmap_num; ++i) {
        lv_font_fmt_txt_cmap_t * cmaps1 = (lv_font_fmt_txt_cmap_t *)&dsc1->cmaps[i];
        lv_font_fmt_txt_cmap_t * cmaps2 = (lv_font_fmt_txt_cmap_t *)&dsc2->cmaps[i];

        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->range_start, cmaps2->range_start, "range_start");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->range_length, cmaps2->range_length, "range_length");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->glyph_id_start, cmaps2->glyph_id_start, "glyph_id_start");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->type, cmaps2->type, "type");
        TEST_ASSERT_EQUAL_INT_MESSAGE(cmaps1->list_length, cmaps2->list_length, "list_length");

        if(cmaps1->unicode_list != NULL && cmaps2->unicode_list != NULL) {
            TEST_ASSERT_TRUE_MESSAGE(cmaps1->unicode_list && cmaps2->unicode_list, "unicode_list");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)cmaps1->unicode_list,
                (uint8_t *)cmaps2->unicode_list,
                sizeof(uint16_t) * cmaps1->list_length,
                "unicode_list");
            total_glyphs += cmaps1->list_length;
        }
        else {
            total_glyphs += cmaps1->range_length;
            TEST_ASSERT_EQUAL_PTR_MESSAGE(cmaps1->unicode_list, cmaps2->unicode_list, "unicode_list");
        }

        if(cmaps1->glyph_id_ofs_list != NULL && cmaps2->glyph_id_ofs_list != NULL) {
            uint8_t * ids1 = (uint8_t *)cmaps1->glyph_id_ofs_list;
            uint8_t * ids2 = (uint8_t *)cmaps2->glyph_id_ofs_list;

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(ids1, ids2, cmaps1->list_length, "glyph_id_ofs_list");
        }
        else {
            TEST_ASSERT_EQUAL_PTR_MESSAGE(cmaps1->glyph_id_ofs_list, cmaps2->glyph_id_ofs_list, "glyph_id_ofs_list");
        }
    }

    // kern_dsc
    if(dsc1->kern_classes == 1 && dsc2->kern_classes == 1) {
        lv_font_fmt_txt_kern_classes_t * kern1 = (lv_font_fmt_txt_kern_classes_t *)dsc1->kern_dsc;
        lv_font_fmt_txt_kern_classes_t * kern2 = (lv_font_fmt_txt_kern_classes_t *)dsc2->kern_dsc;
        if(kern1 != NULL && kern2 != NULL) {
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->right_class_cnt, kern2->right_class_cnt, "right_class_cnt");
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->left_class_cnt, kern2->left_class_cnt, "left_class_cnt");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)kern1->left_class_mapping,
                (uint8_t *)kern2->left_class_mapping,
                kern1->left_class_cnt,
                "left_class_mapping");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)kern1->right_class_mapping,
                (uint8_t *)kern2->right_class_mapping,
                kern1->right_class_cnt,
                "right_class_mapping");

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *)kern1->class_pair_values,
                (uint8_t *)kern2->class_pair_values,
                kern1->right_class_cnt * kern1->left_class_cnt,
                "class_pair_values");
        }
        else {
            TEST_ASSERT_EQUAL_PTR_MESSAGE(kern1, kern2, "kern");
        }
    }
    else if(dsc1->kern_classes == 0 && dsc2->kern_classes == 0) {
        lv_font_fmt_txt_kern_pair_t * kern1 = (lv_font_fmt_txt_kern_pair_t *)dsc1->kern_dsc;
        lv_font_fmt_txt_kern_pair_t * kern2 = (lv_font_fmt_txt_kern_pair_t *)dsc2->kern_dsc;
        if(kern1 != NULL && kern2 != NULL) {
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->glyph_ids_size, kern2->glyph_ids_size, "glyph_ids_size");
            TEST_ASSERT_EQUAL_INT_MESSAGE(kern1->pair_cnt, kern2->pair_cnt, "pair_cnt");

            int ids_size;

            if(kern1->glyph_ids_size == 0) {
                ids_size = sizeof(int8_t) * 2 * kern1->pair_cnt;
            }
            else {
                ids_size = sizeof(int16_t) * 2 * kern1->pair_cnt;
            }

            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(kern1->glyph_ids, kern2->glyph_ids, ids_size, "glyph_ids");
            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                (uint8_t *) kern1->values,
                (uint8_t *) kern2->values,
                kern1->pair_cnt,
                "glyph_values");
        }
    }

    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc1 = (lv_font_fmt_txt_glyph_dsc_t *)dsc1->glyph_dsc;
    lv_font_fmt_txt_glyph_dsc_t * glyph_dsc2 = (lv_font_fmt_txt_glyph_dsc_t *)dsc2->glyph_dsc;

    for(int i = 0; i < total_glyphs; ++i) {
        /*A dynamically loaded font has no `glyph_bitmap` array, the screenshot comparison of the
         *caller covers its glyphs instead*/
        if(i < total_glyphs - 1 && !dsc2->are_glyphs_dynamic_loaded) {
            int size1 = glyph_dsc1[i + 1].bitmap_index - glyph_dsc1[i].bitmap_index;

            if(size1 > 0) {
                TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
                    dsc1->glyph_bitmap + glyph_dsc1[i].bitmap_index,
                    dsc2->glyph_bitmap + glyph_dsc2[i].bitmap_index,
                    size1 - 1, "glyph_bitmap");
            }
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].adv_w, glyph_dsc2[i].adv_w, "adv_w");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].box_w, glyph_dsc2[i].box_w, "box_w");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].box_h, glyph_dsc2[i].box_h, "box_h");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].ofs_x, glyph_dsc2[i].ofs_x, "ofs_x");
        TEST_ASSERT_EQUAL_INT_MESSAGE(glyph_dsc1[i].ofs_y, glyph_dsc2[i].ofs_y, "ofs_y");
    }

    LV_LOG_INFO("No differences found!");
    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif // LV_BUILD_TEST
