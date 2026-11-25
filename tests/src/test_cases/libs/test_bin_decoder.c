#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

static void memdrv_cleanup(void);
static void by_area_cleanup(void);

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    memdrv_cleanup();
    by_area_cleanup();
}

static void create_image(const void * src)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, src);
    lv_obj_center(img);
}

static void bin_decoder(const void * src, const char * screenshot)
{
    lv_image_cache_drop(src);
    create_image(src);
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        lv_obj_clean(lv_screen_active());
        create_image(src);

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
}

static void create_image_tile(const void * src)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, src);
    lv_obj_center(img);
    lv_obj_set_size(img, 275, 175);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
}

static lv_image_dsc_t * get_image_dsc(void)
{
#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 2
    static LV_ATTRIBUTE_MEM_ALIGN uint8_t image_map[IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(lv_color32_t)] = { 0 };
    static lv_image_dsc_t image_dsc = { 0 };

    image_dsc.header.magic = LV_IMAGE_HEADER_MAGIC,
    image_dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    image_dsc.header.w = IMAGE_WIDTH;
    image_dsc.header.h = IMAGE_HEIGHT;
    image_dsc.header.stride = IMAGE_WIDTH * sizeof(lv_color32_t);
    image_dsc.header.flags = 0;
    image_dsc.data_size = sizeof(image_map);
    image_dsc.data = image_map;

    return &image_dsc;
}

static lv_image_decoder_dsc_t * get_image_decoder_dsc(void)
{
    static lv_image_decoder_dsc_t decoder_dsc = { 0 };
    decoder_dsc.src_type = LV_IMAGE_SRC_FILE;
    decoder_dsc.src = NULL;
    decoder_dsc.header.flags = 0;
    return &decoder_dsc;
}

void bin_decoder_tile(const void * src, const char * screenshot)
{
    create_image_tile(src);
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        lv_obj_clean(lv_screen_active());
        create_image_tile(src);

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
}

void test_bin_decoder_i4(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    bin_decoder(&test_image_cogwheel_i4, "libs/bin_decoder_1.png");
}
void test_bin_decoder_i4_tile(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    bin_decoder_tile(&test_image_cogwheel_i4, "libs/bin_decoder_2.png");
}
void test_bin_decoder_argb8888(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    bin_decoder(&test_image_cogwheel_argb8888, "libs/bin_decoder_3.png");
}
void test_bin_decoder_argb8888_tile(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    bin_decoder_tile(&test_image_cogwheel_argb8888, "libs/bin_decoder_4.png");
}
void test_bin_decoder_bin_file(void)
{
    bin_decoder("A:src/test_files/binimages/cogwheel.ARGB8888.bin", "libs/cogwheel.ARGB8888.png");
}
void test_bin_decoder_flush_cache(void)
{
#if LV_BIN_DECODER_RAM_LOAD == 1
    LV_IMAGE_DECLARE(test_I1_NONE_align64);
    LV_IMAGE_DECLARE(test_I2_NONE_align64);
    LV_IMAGE_DECLARE(test_I4_NONE_align64);
    LV_IMAGE_DECLARE(test_I8_NONE_align64);
    LV_IMAGE_DECLARE(test_A1_NONE_align64);
    LV_IMAGE_DECLARE(test_A2_NONE_align64);
    LV_IMAGE_DECLARE(test_A4_NONE_align64);
    LV_IMAGE_DECLARE(test_A8_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB565A8_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB565_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB888_NONE_align64);
    LV_IMAGE_DECLARE(test_XRGB8888_NONE_align64);
    LV_IMAGE_DECLARE(test_ARGB8888_NONE_align64);

    const lv_image_dsc_t * img_dscs[] = {
        &test_I1_NONE_align64,
        &test_I2_NONE_align64,
        &test_I4_NONE_align64,
        &test_I8_NONE_align64,
        &test_A1_NONE_align64,
        &test_A2_NONE_align64,
        &test_A4_NONE_align64,
        &test_A8_NONE_align64,
        &test_RGB565A8_NONE_align64,
        &test_RGB565_NONE_align64,
        &test_RGB888_NONE_align64,
        &test_XRGB8888_NONE_align64,
        &test_ARGB8888_NONE_align64,
    };
    const lv_image_decoder_args_t args = {
        .no_cache = true,
        .premultiply = false,
        .stride_align = false,
        .use_indexed = true,
        .flush_cache = true,
    };

    for(unsigned long i = 0; i < sizeof(img_dscs) / sizeof(img_dscs[0]); i++) {
        lv_image_decoder_dsc_t decoder_dsc;
        lv_result_t res = lv_image_decoder_open(&decoder_dsc, img_dscs[i], &args);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_image_decoder_close(&decoder_dsc);
    }
#endif
}

static void render_transformed_image(const void * src)
{
    lv_image_cache_drop(src);

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, src);
    lv_obj_center(img);
    lv_image_set_pivot(img, 0, 0);
    lv_image_set_rotation(img, 300);
    lv_image_set_scale(img, 400);
    lv_refr_now(NULL);

    lv_obj_clean(lv_screen_active());
    lv_image_cache_drop(src);
}

void test_bin_decoder_transformed_partial_decode(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    render_transformed_image(&test_image_cogwheel_i4);
    render_transformed_image("A:src/test_files/binimages/cogwheel.I4.bin");
    render_transformed_image("A:src/test_files/binimages/cogwheel.RGB565.bin");
    render_transformed_image("A:src/test_files/binimages/cogwheel.RGB565A8.bin");
}

void test_bin_decoder_open_stride_zero_handling(void)
{
    /* Test open image with stride zero */
    lv_image_dsc_t * image_dsc = get_image_dsc();
    image_dsc->header.stride = 0;
    lv_image_decoder_dsc_t * decoder_dsc = get_image_decoder_dsc();
    decoder_dsc->src = image_dsc;
    decoder_dsc->src_type = LV_IMAGE_SRC_VARIABLE;
    decoder_dsc->header.stride = image_dsc->header.w * sizeof(lv_color32_t);

    lv_result_t result = lv_bin_decoder_open(NULL, decoder_dsc);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, result);

    /* Test open image with stride zero for indexed image */
    image_dsc = get_image_dsc();
    image_dsc->header.stride = 0;
    image_dsc->header.cf = LV_COLOR_FORMAT_I1;
    decoder_dsc = get_image_decoder_dsc();
    decoder_dsc->src = image_dsc;
    decoder_dsc->src_type = LV_IMAGE_SRC_VARIABLE;
    decoder_dsc->header.stride = image_dsc->header.w * sizeof(lv_color32_t);
    decoder_dsc->args.use_indexed = 1;
    result = lv_bin_decoder_open(NULL, decoder_dsc);
#if LV_BIN_DECODER_RAM_LOAD == 0
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);
#else
    TEST_ASSERT_EQUAL(LV_RESULT_OK, result);
#endif

    lv_bin_decoder_close(decoder_dsc->decoder, decoder_dsc);
}

/*---------------------------------------------------------------------------------------------
 * Images on a drive that can map files.
 * Drive 'R' serves one file from memory, at a chosen alignment, and records what passes through
 * it. Everything it holds is released in tearDown so a failing assert doesn't leak it.
 *-------------------------------------------------------------------------------------------*/

#define MEMDRV_LETTER 'R'

typedef struct {
    uint32_t pos;
    int32_t live_maps;
    bool mapped;
    uint32_t read_bytes;
} memdrv_file_t;

static struct {
    lv_fs_drv_t drv;
    bool registered;
    uint8_t * storage;
    const uint8_t * data;   /*The file, somewhere inside `storage`*/
    uint8_t * pristine;     /*A copy of the file to check it's never written through the mapping*/
    uint32_t size;
    bool can_map;           /*map_cb answers LV_FS_RES_NOT_IMP when false*/
    uint32_t mapped_handle_read_bytes;  /*Read through handles that were also mapped*/
    uint32_t maps;
    uint32_t unmaps;
    uint32_t bad_unmaps;    /*Unmaps of a pointer or length map_cb didn't give*/
    uint32_t closes_with_live_maps;
    uint32_t path_id;
} memdrv;

static uint8_t * snapshot_ref;
static uint8_t * snapshot_cmp;

static void memdrv_cleanup(void)
{
    if(memdrv.registered) lv_fs_remove_drive(MEMDRV_LETTER);
    if(memdrv.storage) lv_free(memdrv.storage);
    if(memdrv.pristine) lv_free(memdrv.pristine);
    uint32_t path_id = memdrv.path_id;
    lv_memzero(&memdrv, sizeof(memdrv));
    memdrv.path_id = path_id;   /*Keep paths unique across tests so no cache hands back an old image*/

    if(snapshot_ref) lv_free(snapshot_ref);
    if(snapshot_cmp) lv_free(snapshot_cmp);
    snapshot_ref = NULL;
    snapshot_cmp = NULL;
}

static void * memdrv_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    LV_UNUSED(path);
    if(mode != LV_FS_MODE_RD) return NULL;
    return lv_malloc_zeroed(sizeof(memdrv_file_t));
}

static lv_fs_res_t memdrv_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    memdrv_file_t * f = file_p;
    if(f->live_maps != 0) memdrv.closes_with_live_maps++;
    if(f->mapped) memdrv.mapped_handle_read_bytes += f->read_bytes;
    lv_free(f);
    return LV_FS_RES_OK;
}

static lv_fs_res_t memdrv_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    memdrv_file_t * f = file_p;
    uint32_t n = f->pos < memdrv.size ? LV_MIN(btr, memdrv.size - f->pos) : 0;
    lv_memcpy(buf, memdrv.data + f->pos, n);
    f->pos += n;
    f->read_bytes += n;
    if(br) *br = n;
    return LV_FS_RES_OK;
}

static lv_fs_res_t memdrv_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    memdrv_file_t * f = file_p;
    if(whence == LV_FS_SEEK_SET) f->pos = pos;
    else if(whence == LV_FS_SEEK_CUR) f->pos += pos;
    else f->pos = memdrv.size + pos;
    return LV_FS_RES_OK;
}

static lv_fs_res_t memdrv_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    *pos_p = ((memdrv_file_t *)file_p)->pos;
    return LV_FS_RES_OK;
}

static lv_fs_res_t memdrv_map(lv_fs_drv_t * drv, void * file_p, uint32_t offset, uint32_t len, const void ** ptr)
{
    LV_UNUSED(drv);
    LV_UNUSED(len);
    if(!memdrv.can_map) return LV_FS_RES_NOT_IMP;
    ((memdrv_file_t *)file_p)->live_maps++;
    ((memdrv_file_t *)file_p)->mapped = true;
    memdrv.maps++;
    *ptr = memdrv.data + offset;
    return LV_FS_RES_OK;
}

static lv_fs_res_t memdrv_unmap(lv_fs_drv_t * drv, void * file_p, const void * ptr, uint32_t len)
{
    LV_UNUSED(drv);
    const uint8_t * p = ptr;
    if(p < memdrv.data || p + len > memdrv.data + memdrv.size) memdrv.bad_unmaps++;
    ((memdrv_file_t *)file_p)->live_maps--;
    memdrv.unmaps++;
    return LV_FS_RES_OK;
}

/**
 * Serve `path` on drive 'R'. The pixel data (right after the image header) lands at an address
 * that is a multiple of LV_DRAW_BUF_ALIGN plus `misalign` bytes.
 */
static void memdrv_load(const char * path, bool can_map, uint32_t misalign)
{
    uint32_t size = 0;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_path_get_size(path, &size));

    memdrv.storage = lv_malloc(size + 2 * LV_DRAW_BUF_ALIGN + misalign);
    TEST_ASSERT_NOT_NULL(memdrv.storage);
    lv_uintptr_t pixels = (lv_uintptr_t)memdrv.storage + sizeof(lv_image_header_t);
    lv_uintptr_t aligned = (pixels + LV_DRAW_BUF_ALIGN - 1) / LV_DRAW_BUF_ALIGN * LV_DRAW_BUF_ALIGN;
    uint8_t * data = memdrv.storage + (aligned - pixels) + misalign;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_load_to_buf(data, size, path));

    memdrv.pristine = lv_malloc(size);
    TEST_ASSERT_NOT_NULL(memdrv.pristine);
    lv_memcpy(memdrv.pristine, data, size);

    memdrv.data = data;
    memdrv.size = size;
    memdrv.can_map = can_map;

    if(!memdrv.registered) {
        lv_fs_drv_init(&memdrv.drv);
        memdrv.drv.letter = MEMDRV_LETTER;
        memdrv.drv.open_cb = memdrv_open;
        memdrv.drv.close_cb = memdrv_close;
        memdrv.drv.read_cb = memdrv_read;
        memdrv.drv.seek_cb = memdrv_seek;
        memdrv.drv.tell_cb = memdrv_tell;
        memdrv.drv.map_cb = memdrv_map;
        memdrv.drv.unmap_cb = memdrv_unmap;
        lv_fs_drv_register(&memdrv.drv);
        memdrv.registered = true;
    }
}

/*Replace the served file, keeping the drive and resetting the counters*/
static void memdrv_reload(const char * path, bool can_map, uint32_t misalign)
{
    lv_free(memdrv.storage);
    lv_free(memdrv.pristine);
    memdrv.storage = NULL;
    memdrv.pristine = NULL;
    memdrv.mapped_handle_read_bytes = 0;
    memdrv.maps = 0;
    memdrv.unmaps = 0;
    memdrv.bad_unmaps = 0;
    memdrv.closes_with_live_maps = 0;
    memdrv_load(path, can_map, misalign);
}

/*The bytes of the frame that hold pixels*/
static uint32_t frame_size(void)
{
    lv_draw_buf_t * fb = lv_display_get_buf_active(lv_display_get_default());
    return fb->header.stride * fb->header.h;
}

/**
 * Draw `src` centered on a cleared screen and copy the frame into `*snapshot`.
 * The image is deleted and dropped from the cache afterwards, so the decoder has closed it.
 */
static void render_to(const char * src, uint8_t ** snapshot)
{
    lv_obj_clean(lv_screen_active());
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, src);
    lv_obj_center(img);
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_draw_buf_t * fb = lv_display_get_buf_active(lv_display_get_default());
    if(*snapshot == NULL) *snapshot = lv_malloc(frame_size());
    TEST_ASSERT_NOT_NULL(*snapshot);
    lv_memcpy(*snapshot, fb->data, frame_size());

    lv_obj_clean(lv_screen_active());
    lv_image_cache_drop(src);
    lv_refr_now(NULL);
}

/*A path on 'R' no cache has seen yet*/
static const char * memdrv_path(void)
{
    static char path[32];
    lv_snprintf(path, sizeof(path), "%c:img%" LV_PRIu32 ".bin", MEMDRV_LETTER, memdrv.path_id++);
    return path;
}

static void assert_same_frame(const char * what)
{
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(snapshot_ref, snapshot_cmp, frame_size(), what);
}

/**
 * Every mapping was released, each before its file was closed, with what map_cb gave,
 * and nothing was written through a mapping (it's read-only)
 */
static void assert_mappings_released(const char * what)
{
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(memdrv.maps, memdrv.unmaps, what);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, memdrv.bad_unmaps, what);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, memdrv.closes_with_live_maps, what);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(memdrv.pristine, memdrv.data, memdrv.size, what);
}

static const char * const true_color_files[] = {
    "A:src/test_files/binimages/cogwheel.ARGB8888.bin",
    "A:src/test_files/binimages/cogwheel.XRGB8888.bin",
    "A:src/test_files/binimages/cogwheel.RGB888.bin",
    "A:src/test_files/binimages/cogwheel.RGB565.bin",
    "A:src/test_files/binimages/cogwheel.RGB565A8.bin",
};

void test_bin_decoder_map_true_color(void)
{
    for(size_t i = 0; i < sizeof(true_color_files) / sizeof(true_color_files[0]); i++) {
        const char * file = true_color_files[i];

        /*The reference: the same file through a drive that can't map*/
        render_to(file, &snapshot_ref);

        /*The pixels are taken from the mapping: the decoder's file only reads the header.
         *Other decoders probing the file (e.g. FFmpeg) read through handles they don't map.*/
        memdrv_reload(file, true, 0);
        render_to(memdrv_path(), &snapshot_cmp);
        assert_same_frame(file);
        TEST_ASSERT_TRUE_MESSAGE(memdrv.maps > 0, file);
        TEST_ASSERT_LESS_OR_EQUAL_UINT32_MESSAGE(2 * sizeof(lv_image_header_t), memdrv.mapped_handle_read_bytes, file);
        assert_mappings_released(file);
    }
}

void test_bin_decoder_map_draws_something(void)
{
    /*Guards the comparisons above: an empty screen differs from the drawn image*/
    render_to(true_color_files[0], &snapshot_ref);
    lv_obj_clean(lv_screen_active());
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
    lv_draw_buf_t * fb = lv_display_get_buf_active(lv_display_get_default());
    TEST_ASSERT_NOT_EQUAL(0, lv_memcmp(snapshot_ref, fb->data, frame_size()));
}

void test_bin_decoder_map_unaligned(void)
{
    /*A mapping that doesn't meet LV_DRAW_BUF_ALIGN still draws the same pixels*/
    for(size_t i = 0; i < sizeof(true_color_files) / sizeof(true_color_files[0]); i++) {
        const char * file = true_color_files[i];
        render_to(file, &snapshot_ref);

        for(uint32_t misalign = 1; misalign <= 3; misalign++) {
            memdrv_reload(file, true, misalign);
            render_to(memdrv_path(), &snapshot_cmp);
            assert_same_frame(file);
            assert_mappings_released(file);
        }
    }
}

void test_bin_decoder_map_not_implemented(void)
{
    /*A drive whose map_cb declines the file falls back to reading it*/
    for(size_t i = 0; i < sizeof(true_color_files) / sizeof(true_color_files[0]); i++) {
        const char * file = true_color_files[i];
        render_to(file, &snapshot_ref);

        memdrv_reload(file, false, 0);
        render_to(memdrv_path(), &snapshot_cmp);
        assert_same_frame(file);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, memdrv.maps, file);
    }
}

void test_bin_decoder_map_other_formats(void)
{
    /*Indexed, alpha-only and compressed images draw the same from a drive that can map*/
    static const char * const files[] = {
        "A:src/test_files/binimages/cogwheel.I1.bin",
        "A:src/test_files/binimages/cogwheel.I2.bin",
        "A:src/test_files/binimages/cogwheel.I4.bin",
        "A:src/test_files/binimages/cogwheel.I8.bin",
        "A:src/test_files/binimages/cogwheel.A8.bin",
        /*Not L8: unmapped L8 files don't open with LV_BIN_DECODER_RAM_LOAD, mapped ones draw like a variable*/
        "A:src/test_files/lz4_compressed/cogwheel.ARGB8888.bin",
        "A:src/test_files/lz4_compressed/cogwheel.RGB565A8.bin",
        "A:src/test_files/lz4_compressed/cogwheel.I4.bin",
        "A:src/test_files/rle_compressed/cogwheel.A8.bin",
    };
    for(size_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        render_to(files[i], &snapshot_ref);

        memdrv_reload(files[i], true, 0);
        render_to(memdrv_path(), &snapshot_cmp);
        assert_same_frame(files[i]);
        assert_mappings_released(files[i]);
    }
}

void test_bin_decoder_map_no_leak(void)
{
    /*Drawing a mapped image again and again keeps the memory and the mappings balanced*/
    memdrv_load(true_color_files[4], true, 0);
    const char * path = memdrv_path();
    render_to(path, &snapshot_ref);
    render_to(path, &snapshot_cmp); /*Allocates the snapshot outside the measured window*/

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        render_to(path, &snapshot_cmp);
    }
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
    assert_mappings_released("repeated draws");
}
/*---------------------------------------------------------------------------------------------
 * Decoding by area.
 * Without LV_BIN_DECODER_RAM_LOAD the draw path decodes indexed images line by line, whatever
 * holds them: a variable, a mapped file or a file read through lv_fs. Each line must equal the
 * same line of the image decoded whole.
 *-------------------------------------------------------------------------------------------*/

static lv_image_decoder_dsc_t by_area_dsc;
static bool by_area_dsc_open;
static uint8_t * by_area_whole;     /*The image decoded whole, ARGB8888, `w * 4` bytes a line*/
static uint8_t * by_area_file_buf;  /*A file served from memory through memfs*/

static void by_area_cleanup(void)
{
    if(by_area_dsc_open) lv_image_decoder_close(&by_area_dsc);
    by_area_dsc_open = false;
    if(by_area_whole) lv_free(by_area_whole);
    by_area_whole = NULL;
    if(by_area_file_buf) lv_free(by_area_file_buf);
    by_area_file_buf = NULL;
}

/*Open `src` with the indexed image converted to ARGB8888 and keep a copy of the whole image*/
static void by_area_open(const void * src)
{
    lv_image_cache_drop(src);
    const lv_image_decoder_args_t args = { .no_cache = true, .use_indexed = false };
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_open(&by_area_dsc, src, &args));
    by_area_dsc_open = true;

    const lv_draw_buf_t * whole = by_area_dsc.decoded;
    TEST_ASSERT_NOT_NULL_MESSAGE(whole, "LV_BIN_DECODER_RAM_LOAD is needed for the whole image");
    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_ARGB8888, whole->header.cf);

    uint32_t w = by_area_dsc.header.w;
    uint32_t h = by_area_dsc.header.h;
    by_area_whole = lv_malloc(w * h * 4);
    TEST_ASSERT_NOT_NULL(by_area_whole);
    for(uint32_t y = 0; y < h; y++) {
        lv_memcpy(by_area_whole + y * w * 4, whole->data + y * whole->header.stride, w * 4);
    }
}

/*Decode the open image line by line and compare every line with the whole image*/
static void by_area_check_lines(void)
{
    int32_t w = by_area_dsc.header.w;
    int32_t h = by_area_dsc.header.h;
    const lv_area_t full = {0, 0, w - 1, h - 1};
    lv_area_t decoded_area = {LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MIN};

    int32_t lines = 0;
    while(lv_bin_decoder_get_area(by_area_dsc.decoder, &by_area_dsc, &full, &decoded_area) == LV_RESULT_OK) {
        TEST_ASSERT_EQUAL_INT32(lines, decoded_area.y1);
        TEST_ASSERT_EQUAL_INT32(decoded_area.y1, decoded_area.y2);
        TEST_ASSERT_EQUAL_INT32(0, decoded_area.x1);
        TEST_ASSERT_EQUAL_INT32(w - 1, decoded_area.x2);
        TEST_ASSERT_EQUAL_MEMORY(by_area_whole + decoded_area.y1 * w * 4, by_area_dsc.decoded->data, w * 4);
        lines++;
    }
    TEST_ASSERT_EQUAL_INT32(h, lines);
}

void test_bin_decoder_by_area_variable(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    by_area_open(&test_image_cogwheel_i4);
    by_area_check_lines();
}

void test_bin_decoder_by_area_mapped_file(void)
{
    const char * file = "A:src/test_files/binimages/cogwheel.I4.bin";
    uint32_t size = 0;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_path_get_size(file, &size));
    by_area_file_buf = lv_malloc(size);
    TEST_ASSERT_NOT_NULL(by_area_file_buf);
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_load_to_buf(by_area_file_buf, size, file));

    /*A buffer-backed file is always mapped*/
    static lv_fs_path_ex_t path;
    lv_fs_make_path_from_buffer(&path, LV_FS_MEMFS_LETTER, by_area_file_buf, size, "bin");
    by_area_open((const char *)&path);
    by_area_check_lines();
}

void test_bin_decoder_by_area_file(void)
{
    by_area_open("A:src/test_files/binimages/cogwheel.I4.bin");
    by_area_check_lines();
}
#endif
