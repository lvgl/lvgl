#if LV_BUILD_TEST

#include <lvgl/lvgl.h>
#include <lvgl_private/lvgl_private.h>

#include "unity/unity.h"

/*********************
 *      DEFINES
 *********************/

/* A GPU based draw unit doesn't use the CPU draw buffer */
#if LV_USE_DRAW_NANOVG || LV_USE_DRAW_OPENGLES
    #define ASSERT_PX_EQ(expected, actual)              do { (void)(expected); (void)(actual); } while(0)
    #define ASSERT_PX_WITHIN(delta, expected, actual)   do { (void)(expected); (void)(actual); } while(0)
    #define ASSERT_PX_TRUE(condition)                   do { (void)(condition); } while(0)
#else
    #define ASSERT_PX_EQ(expected, actual)              TEST_ASSERT_EQUAL_HEX32(expected, actual)
    #define ASSERT_PX_WITHIN(delta, expected, actual)   TEST_ASSERT_UINT32_WITHIN(delta, expected, actual)
    #define ASSERT_PX_TRUE(condition)                   TEST_ASSERT_TRUE(condition)
#endif

#define REFR_LOG_MAX        64

#define REFR_COLOR_RED      0xFFFF0000
#define REFR_COLOR_GREEN    0xFF00FF00
#define REFR_COLOR_BLUE     0xFF0000FF
#define REFR_COLOR_WHITE    0xFFFFFFFF
#define REFR_COLOR_BLACK    0xFF000000

/**********************
 *      TYPEDEFS
 **********************/

struct {
    lv_display_t * disp;
    lv_draw_buf_t * buf[3];
    uint32_t buf_cnt;
    lv_color_format_t cf;
    int32_t hor_res;
    int32_t ver_res;

    /*Flushing*/
    uint32_t flush_cnt;
    lv_area_t flush_area[REFR_LOG_MAX];
    uint8_t flush_is_last[REFR_LOG_MAX];
    const lv_draw_buf_t * flush_buf[REFR_LOG_MAX];
    uint32_t flush_wait_cnt;
    bool defer_flush_ready;

    /*Syncing*/
    uint32_t sync_cnt;
    lv_area_t sync_area[REFR_LOG_MAX];
    uint32_t sync_wait_cnt;
    bool defer_sync_ready;

    /*Events*/
    uint32_t ev_cnt;
    lv_event_code_t ev[REFR_LOG_MAX];

    /*A copy of the whole screen, assembled from the flushed areas*/
    uint8_t * mirror;
    uint32_t mirror_stride;
    uint32_t mirror_px_size;
} refr_ctx;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t px_size_of(lv_color_format_t cf);
static void mirror_copy(lv_display_t * disp, const lv_area_t * area, const uint8_t * px_map);
static void log_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

static int32_t row_rounding = 4;

static void refr_ctx_reset(void)
{
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
}

static void refr_hide_sysmon(lv_display_t * disp)
{
#if LV_USE_SYSMON && LV_USE_PERF_MONITOR
#if LV_USE_PERF_MONITOR
    lv_sysmon_hide_performance(disp);
#endif
#if LV_USE_MEM_MONITOR
    lv_sysmon_hide_memory(disp);
#endif
#else
    LV_UNUSED(disp);
#endif
}

/**
 * The height the invalidated areas are rounded up to by `refr_row_rounding_cb()`.
 * @param rows  the height the areas are rounded up to
 */
static void refr_set_row_rounding(int32_t rows)
{
    row_rounding = rows;
}

static uint32_t px_size_of(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB8888:
            return 4;
        case LV_COLOR_FORMAT_RGB888:
            return 3;
        case LV_COLOR_FORMAT_RGB565:
            return 2;
        default:
            return 0; /*Not mirrored*/
    }
}

/*Read a pixel of a buffer holding an image in the display's color format*/
static uint32_t refr_px_read(const uint8_t * buf, uint32_t stride, lv_color_format_t cf, int32_t x, int32_t y)
{
    uint32_t px_size = px_size_of(cf);
    const uint8_t * p = buf + (uint32_t)y * stride + (uint32_t)x * px_size;
    switch(cf) {
        case LV_COLOR_FORMAT_XRGB8888:
            return 0xFF000000 | ((uint32_t)p[2] << 16) | ((uint32_t)p[1] << 8) | p[0];
        case LV_COLOR_FORMAT_ARGB8888:
            return ((uint32_t)p[3] << 24) | ((uint32_t)p[2] << 16) | ((uint32_t)p[1] << 8) | p[0];
        case LV_COLOR_FORMAT_RGB888:
            return 0xFF000000 | ((uint32_t)p[2] << 16) | ((uint32_t)p[1] << 8) | p[0];
        case LV_COLOR_FORMAT_RGB565: {
                uint16_t c = (uint16_t)(p[0] | ((uint32_t)p[1] << 8));
                uint32_t r = ((c >> 11) & 0x1F) << 3;
                uint32_t gg = ((c >> 5) & 0x3F) << 2;
                uint32_t b = (c & 0x1F) << 3;
                return 0xFF000000 | (r << 16) | (gg << 8) | b;
            }
        default:
            return 0;
    }
}

/*Pixel of the assembled screen image*/
static uint32_t refr_screen_px(int32_t x, int32_t y)
{
    TEST_ASSERT_NOT_NULL(refr_ctx.mirror);
    TEST_ASSERT_TRUE(x >= 0 && x < refr_ctx.hor_res && y >= 0 && y < refr_ctx.ver_res);
    return refr_px_read(refr_ctx.mirror, refr_ctx.mirror_stride, refr_ctx.cf, x, y);
}

/*Pixel of one of the display's draw buffers*/
static uint32_t refr_buf_px(const lv_draw_buf_t * buf, int32_t x, int32_t y)
{
    return refr_px_read(buf->data, buf->header.stride, refr_ctx.cf, x, y);
}

static void mirror_copy(lv_display_t * disp, const lv_area_t * area, const uint8_t * px_map)
{
    if(refr_ctx.mirror == NULL) return;

    /*The flushed area carries the display offset, the mirror does not*/
    lv_area_t a = *area;
    lv_area_move(&a, -disp->offset_x, -disp->offset_y);
    if(a.x1 < 0 || a.y1 < 0 || a.x2 >= refr_ctx.hor_res || a.y2 >= refr_ctx.ver_res) return;

    uint32_t px_size = refr_ctx.mirror_px_size;
    uint32_t src_stride = disp->buf_act->header.stride;
    bool partial = lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_PARTIAL;
    int32_t w = lv_area_get_width(&a);
    int32_t h = lv_area_get_height(&a);
    int32_t y;

    for(y = 0; y < h; y++) {
        const uint8_t * src;
        if(partial) src = px_map + (uint32_t)y * src_stride;
        else src = px_map + (uint32_t)(a.y1 + y) * src_stride + (uint32_t)a.x1 * px_size;

        lv_memcpy(refr_ctx.mirror + (uint32_t)(a.y1 + y) * refr_ctx.mirror_stride + (uint32_t)a.x1 * px_size,
                  src, (uint32_t)w * px_size);
    }
}

static void refr_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    if(refr_ctx.flush_cnt < REFR_LOG_MAX) {
        refr_ctx.flush_area[refr_ctx.flush_cnt] = *area;
        refr_ctx.flush_is_last[refr_ctx.flush_cnt] = lv_display_flush_is_last(disp);
        refr_ctx.flush_buf[refr_ctx.flush_cnt] = disp->buf_act;
    }
    refr_ctx.flush_cnt++;

    mirror_copy(disp, area, px_map);

    if(!refr_ctx.defer_flush_ready) lv_display_flush_ready(disp);
}

static void refr_flush_wait_cb(lv_display_t * disp)
{
    LV_UNUSED(disp);
    refr_ctx.flush_wait_cnt++;
}

static void refr_sync_cb(lv_display_t * disp, const lv_area_t * area)
{
    if(refr_ctx.sync_cnt < REFR_LOG_MAX) refr_ctx.sync_area[refr_ctx.sync_cnt] = *area;
    refr_ctx.sync_cnt++;
    if(!refr_ctx.defer_sync_ready) lv_display_sync_ready(disp);
}

static void refr_sync_wait_cb(lv_display_t * disp)
{
    LV_UNUSED(disp);
    refr_ctx.sync_wait_cnt++;
}

/*Recording the display events*/

static void log_event_cb(lv_event_t * e)
{
    if(refr_ctx.ev_cnt < REFR_LOG_MAX) refr_ctx.ev[refr_ctx.ev_cnt] = lv_event_get_code(e);
    refr_ctx.ev_cnt++;
}

static void refr_log_events(lv_display_t * disp)
{
    static const lv_event_code_t codes[] = {
        LV_EVENT_REFR_START, LV_EVENT_REFR_READY,
        LV_EVENT_RENDER_START, LV_EVENT_RENDER_READY,
        LV_EVENT_FLUSH_START, LV_EVENT_FLUSH_FINISH,
        LV_EVENT_FLUSH_WAIT_START, LV_EVENT_FLUSH_WAIT_FINISH,
        LV_EVENT_SYNC_START, LV_EVENT_SYNC_FINISH,
        LV_EVENT_SYNC_WAIT_START, LV_EVENT_SYNC_WAIT_FINISH,
    };
    size_t i;
    for(i = 0; i < sizeof(codes) / sizeof(codes[0]); i++) {
        lv_display_add_event_cb(disp, log_event_cb, codes[i], NULL);
    }
}

static bool refr_event_seen(lv_event_code_t code)
{
    uint32_t i;
    for(i = 0; i < refr_ctx.ev_cnt && i < REFR_LOG_MAX; i++) {
        if(refr_ctx.ev[i] == code) return true;
    }
    return false;
}

static uint32_t refr_event_count(lv_event_code_t code)
{
    uint32_t i;
    uint32_t cnt = 0;
    for(i = 0; i < refr_ctx.ev_cnt && i < REFR_LOG_MAX; i++) {
        if(refr_ctx.ev[i] == code) cnt++;
    }
    return cnt;
}

static int32_t refr_event_index(lv_event_code_t code)
{
    uint32_t i;
    for(i = 0; i < refr_ctx.ev_cnt && i < REFR_LOG_MAX; i++) {
        if(refr_ctx.ev[i] == code) return (int32_t)i;
    }
    return -1;
}

static void refr_log_reset(void)
{
    refr_ctx.flush_cnt = 0;
    refr_ctx.flush_wait_cnt = 0;
    refr_ctx.sync_cnt = 0;
    refr_ctx.sync_wait_cnt = 0;
    refr_ctx.ev_cnt = 0;
}

static lv_display_t * refr_disp_create(int32_t w, int32_t h, lv_color_format_t cf,
                                       lv_display_render_mode_t mode, uint32_t buf_cnt, int32_t buf_h)
{
    uint32_t i;

    refr_ctx_reset();
    refr_ctx.cf = cf;
    refr_ctx.hor_res = w;
    refr_ctx.ver_res = h;
    refr_ctx.buf_cnt = buf_cnt;

    lv_display_t * disp = lv_display_create(w, h);
    TEST_ASSERT_NOT_NULL(disp);
    lv_display_set_color_format(disp, cf);

    for(i = 0; i < buf_cnt; i++) {
        refr_ctx.buf[i] = lv_draw_buf_create(w, buf_h, cf, LV_STRIDE_AUTO);
        TEST_ASSERT_NOT_NULL(refr_ctx.buf[i]);
    }
    lv_display_set_draw_buffers(disp, refr_ctx.buf[0], buf_cnt > 1 ? refr_ctx.buf[1] : NULL);
    if(buf_cnt > 2) lv_display_set_3rd_draw_buffer(disp, refr_ctx.buf[2]);
    lv_display_set_render_mode(disp, mode);
    lv_display_set_flush_cb(disp, refr_flush_cb);

    refr_hide_sysmon(disp);

    refr_ctx.mirror_px_size = px_size_of(cf);
    if(refr_ctx.mirror_px_size) {
        refr_ctx.mirror_stride = (uint32_t)w * refr_ctx.mirror_px_size;
        refr_ctx.mirror = lv_malloc_zeroed(refr_ctx.mirror_stride * (uint32_t)h);
        TEST_ASSERT_NOT_NULL(refr_ctx.mirror);
    }

    refr_ctx.disp = disp;
    return disp;
}

static void refr_disp_delete(void)
{
    uint32_t i;
    if(refr_ctx.disp) {
        lv_display_delete(refr_ctx.disp);
        refr_ctx.disp = NULL;
    }
    for(i = 0; i < refr_ctx.buf_cnt; i++) {
        if(refr_ctx.buf[i]) lv_draw_buf_destroy(refr_ctx.buf[i]);
        refr_ctx.buf[i] = NULL;
    }
    if(refr_ctx.mirror) {
        lv_free(refr_ctx.mirror);
        refr_ctx.mirror = NULL;
    }
}

/*Refresh the display under test without touching the other displays*/
static void refr_frame(void)
{
    lv_display_refr_timer(lv_display_get_refr_timer(refr_ctx.disp));
}

/**
 * The number of rows that fit into the draw buffer in partial mode.
 * The draw buffers are padded to LV_DRAW_BUF_ALIGN, so this can not be derived
 * from the requested buffer height.
 */
static uint32_t refr_max_row_of(int32_t area_w, int32_t area_h)
{
    uint32_t stride = lv_draw_buf_width_to_stride((uint32_t)area_w, refr_ctx.cf);
    uint32_t overhead = LV_COLOR_INDEXED_PALETTE_SIZE(refr_ctx.cf) * sizeof(lv_color32_t);
    uint32_t max_row = (refr_ctx.disp->buf_act->data_size - overhead) / stride;
    if(max_row > (uint32_t)area_h) max_row = (uint32_t)area_h;
    return max_row;
}

/*The flushed chunks have to tile the given area from top to bottom, without gaps,
 *and only the last one may be marked as the last part*/
static void refr_check_flushed_chunks(const lv_area_t * area, uint32_t max_row)
{
    uint32_t expected_cnt = ((uint32_t)lv_area_get_height(area) + max_row - 1) / max_row;
    TEST_ASSERT_EQUAL_UINT32(expected_cnt, refr_ctx.flush_cnt);

    uint32_t i;
    for(i = 0; i < refr_ctx.flush_cnt; i++) {
        lv_area_t expected;
        expected.x1 = area->x1;
        expected.x2 = area->x2;
        expected.y1 = area->y1 + (int32_t)(i * max_row);
        expected.y2 = expected.y1 + (int32_t)max_row - 1;
        if(expected.y2 > area->y2) expected.y2 = area->y2;
        TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.flush_area[i]));
        /*Only the last part of the last area is marked*/
        TEST_ASSERT_EQUAL(i == refr_ctx.flush_cnt - 1, refr_ctx.flush_is_last[i] != 0);
    }
}

static lv_obj_t * refr_screen(void)
{
    return lv_display_get_screen_active(refr_ctx.disp);
}

/*A screen sized, opaque, square cornered background so that the pixels are predictable*/
static void refr_screen_set_color(uint32_t argb)
{
    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scr, lv_color_hex(argb), 0);
}

/*An opaque, square cornered rectangle*/
static lv_obj_t * refr_rect_create(lv_obj_t * parent, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t argb)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(argb), 0);
    return obj;
}

/*Counting how many times a widget was drawn*/
static void refr_draw_main_counter_cb(lv_event_t * e)
{
    uint32_t * cnt = lv_event_get_user_data(e);
    (*cnt)++;
}

/*Round the height of the invalidated areas up to a multiple of `row_rounding`,
 *like the drivers of the displays that can only be updated in blocks*/
static void refr_row_rounding_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    lv_area_t * area = lv_event_get_invalidated_area(e);
    int32_t h = lv_area_get_height(area);
    int32_t rounded = ((h + row_rounding - 1) / row_rounding) * row_rounding;
    int32_t max_y = lv_display_get_vertical_resolution(disp) - 1;
    area->y2 = LV_MIN(area->y1 + rounded - 1, max_y);
}

void setUp(void)
{
    refr_ctx_reset();
}

void tearDown(void)
{
    refr_disp_delete();
}

void test_refr_inv_area_without_display(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    lv_area_t area = {0, 0, 9, 9};

    lv_display_set_default(NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(NULL, &area));
    lv_display_set_default(disp_def);
}

void test_refr_inv_area_uses_default_display(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);

    lv_display_set_default(refr_ctx.disp);
    refr_ctx.disp->inv_p = 0;

    lv_area_t area = {10, 20, 29, 39};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(NULL, &area));
    lv_display_set_default(disp_def);

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&area, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_disabled(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_ctx.disp->inv_p = 0;

    lv_display_enable_invalidation(refr_ctx.disp, false);
    TEST_ASSERT_FALSE(lv_display_is_invalidation_enabled(refr_ctx.disp));

    lv_area_t area = {0, 0, 9, 9};
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);

    /*Nothing is rendered while invalidation is disabled*/
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);

    lv_display_enable_invalidation(refr_ctx.disp, true);
    TEST_ASSERT_TRUE(lv_display_is_invalidation_enabled(refr_ctx.disp));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
}

void test_refr_inv_area_null_area_clears_the_buffer(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);

    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {50, 50, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.disp->inv_p);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, NULL));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);

    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

void test_refr_inv_area_out_of_screen(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_ctx.disp->inv_p = 0;

    lv_area_t right = {100, 10, 120, 20};
    lv_area_t below = {10, 100, 20, 120};
    lv_area_t left = {-50, 10, -1, 20};
    lv_area_t above = {10, -50, 20, -1};

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &right));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &below));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &left));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &above));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);
}

void test_refr_inv_area_clipped_to_screen(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_ctx.disp->inv_p = 0;

    lv_area_t area = { -20, -30, 120, 130};
    lv_area_t expected = {0, 0, 99, 99};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.disp->inv_areas[0]));

    /*The caller's area is not modified*/
    lv_area_t untouched = { -20, -30, 120, 130};
    TEST_ASSERT_TRUE(lv_area_is_equal(&untouched, &area));
}

void test_refr_inv_area_i1_rounded_to_byte_boundary(void)
{
    refr_disp_create(64, 32, LV_COLOR_FORMAT_I1, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 32);
    refr_ctx.disp->inv_p = 0;

    lv_area_t area = {11, 5, 27, 9};
    lv_area_t expected = {8, 5, 31, 9};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.disp->inv_areas[0]));

    /*Already aligned areas are kept*/
    refr_ctx.disp->inv_p = 0;
    lv_area_t aligned = {8, 0, 15, 1};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &aligned));
    TEST_ASSERT_TRUE(lv_area_is_equal(&aligned, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_full_mode_takes_the_whole_screen(void)
{
    refr_disp_create(100, 80, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_FULL, 1, 80);
    refr_ctx.disp->inv_p = 0;

    lv_area_t small = {10, 10, 19, 19};
    lv_area_t full = {0, 0, 99, 79};

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &small));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.disp->inv_areas[0]));

    /*More invalidations do not add more areas*/
    lv_area_t other = {50, 50, 59, 59};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &other));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_contained_area_is_not_stored(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_ctx.disp->inv_p = 0;

    lv_area_t big = {10, 10, 59, 59};
    lv_area_t inside = {20, 20, 29, 29};
    lv_area_t same = {10, 10, 59, 59};
    lv_area_t overlapping = {50, 50, 79, 79};

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &big));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &inside));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &same));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);

    /*Only partially covered areas are stored*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &overlapping));
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.disp->inv_p);
}

void test_refr_inv_area_buffer_overflow_falls_back_to_full_screen(void)
{
    refr_disp_create(400, 400, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 400);
    refr_ctx.disp->inv_p = 0;

    /*Fill the invalid area buffer with distinct, non-overlapping areas*/
    uint32_t i;
    for(i = 0; i < LV_INV_BUF_SIZE; i++) {
        lv_area_t a;
        a.x1 = (int32_t)(i % 20) * 20;
        a.y1 = (int32_t)(i / 20) * 20;
        a.x2 = a.x1 + 9;
        a.y2 = a.y1 + 9;
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &a));
    }
    TEST_ASSERT_EQUAL_UINT32(LV_INV_BUF_SIZE, refr_ctx.disp->inv_p);

    /*One more area does not fit, so the whole screen is invalidated instead*/
    lv_area_t extra = {300, 300, 309, 309};
    lv_area_t full = {0, 0, 399, 399};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &extra));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_event_can_modify_the_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_set_row_rounding(4);
    lv_display_add_event_cb(refr_ctx.disp, refr_row_rounding_cb, LV_EVENT_INVALIDATE_AREA, NULL);
    refr_ctx.disp->inv_p = 0;

    lv_area_t area = {10, 8, 19, 12};      /*5 rows*/
    lv_area_t expected = {10, 8, 19, 15};  /*rounded up to 8 rows*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.disp->inv_areas[0]));
}

static void delete_disp_event_cb(lv_event_t * e)
{
    lv_display_delete(lv_event_get_current_target(e));
    refr_ctx.disp = NULL;
}

void test_refr_inv_area_display_deleted_in_event(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    lv_display_add_event_cb(refr_ctx.disp, delete_disp_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    lv_area_t area = {10, 10, 19, 19};
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_NULL(refr_ctx.disp);
}

void test_refr_join_overlapping_areas(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*Two overlapping areas are cheaper to redraw together*/
    lv_area_t a1 = {10, 10, 49, 49};
    lv_area_t a2 = {20, 20, 59, 59};
    lv_area_t joined = {10, 10, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&joined, &refr_ctx.flush_area[0]));
}

void test_refr_dont_join_distant_areas(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*Joining these would cover much more than the two areas together*/
    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {90, 90, 99, 99};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&a1, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(lv_area_is_equal(&a2, &refr_ctx.flush_area[1]));
    /*Only the very last part of the very last area is flagged*/
    TEST_ASSERT_FALSE(refr_ctx.flush_is_last[0]);
    TEST_ASSERT_TRUE(refr_ctx.flush_is_last[1]);
}

void test_refr_join_contained_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*The small area is stored first, so it is not filtered out by lv_inv_area()*/
    lv_area_t small = {20, 20, 29, 29};
    lv_area_t big = {10, 10, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &small);
    lv_inv_area(refr_ctx.disp, &big);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&big, &refr_ctx.flush_area[0]));
}

void test_refr_invalid_areas_are_cleared_after_refreshing(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);
    uint32_t i;
    for(i = 0; i < LV_INV_BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, refr_ctx.disp->inv_area_joined[i]);
        TEST_ASSERT_EQUAL_INT32(0, refr_ctx.disp->inv_areas[i].x1);
        TEST_ASSERT_EQUAL_INT32(0, refr_ctx.disp->inv_areas[i].y2);
    }
}

void test_refr_top_obj_invalid_arguments(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_area_t area = {0, 0, 9, 9};
    TEST_ASSERT_NULL(lv_refr_get_top_obj(NULL, refr_screen()));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&area, NULL));
}

void test_refr_top_obj_finds_the_covering_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());

    /*Fully inside the object*/
    lv_area_t inside = {20, 20, 29, 29};
    TEST_ASSERT_EQUAL_PTR(obj, lv_refr_get_top_obj(&inside, refr_screen()));

    /*Only partially on the object, so the screen covers it*/
    lv_area_t crossing = {5, 5, 20, 20};
    TEST_ASSERT_EQUAL_PTR(refr_screen(), lv_refr_get_top_obj(&crossing, refr_screen()));

    /*Searching from the object itself finds the object*/
    TEST_ASSERT_EQUAL_PTR(obj, lv_refr_get_top_obj(&inside, obj));

    /*Outside of the given object*/
    lv_area_t outside = {55, 55, 60, 60};
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&outside, obj));
}

void test_refr_top_obj_skips_the_hidden_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_obj_set_hidden(obj, true);

    lv_area_t inside = {20, 20, 29, 29};
    TEST_ASSERT_EQUAL_PTR(refr_screen(), lv_refr_get_top_obj(&inside, refr_screen()));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));
}

void test_refr_top_obj_skips_the_transparent_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t inside = {20, 20, 29, 29};

    /*A not fully opaque object can not cover anything*/
    lv_obj_set_style_opa(obj, LV_OPA_50, 0);
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));

    /*Neither can an object that does not fill its area*/
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));
}

void test_refr_top_obj_skips_the_layered_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t inside = {20, 20, 29, 29};

    /*Objects drawn on their own layer are blended afterwards, they can not cover*/
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_SIMPLE, lv_obj_get_layer_type(obj));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));

    lv_obj_set_style_opa_layered(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_transform_rotation(obj, 300, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));
}

void test_refr_top_obj_skips_the_rounded_corner(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_radius(obj, 15, 0);
    lv_obj_update_layout(refr_screen());

    /*The corner of the object is not covered by the rounded background*/
    lv_area_t corner = {10, 10, 14, 14};
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&corner, obj));

    /*The middle still is*/
    lv_area_t middle = {25, 25, 34, 34};
    TEST_ASSERT_EQUAL_PTR(obj, lv_refr_get_top_obj(&middle, obj));
}

void test_refr_top_obj_prefers_the_topmost_child(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * bottom = refr_rect_create(refr_screen(), 0, 0, 60, 60, REFR_COLOR_RED);
    lv_obj_t * top = refr_rect_create(refr_screen(), 0, 0, 60, 60, REFR_COLOR_GREEN);
    lv_obj_update_layout(refr_screen());

    lv_area_t area = {10, 10, 19, 19};
    TEST_ASSERT_EQUAL_PTR(top, lv_refr_get_top_obj(&area, refr_screen()));

    /*With the topmost one hidden the one below is used*/
    lv_obj_set_hidden(top, true);
    TEST_ASSERT_EQUAL_PTR(bottom, lv_refr_get_top_obj(&area, refr_screen()));
}

void test_refr_partial_mode_splits_the_area(void)
{
    const int32_t w = 100;
    const int32_t h = 100;
    const int32_t buf_h = 10;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, buf_h);
    refr_screen_set_color(REFR_COLOR_BLUE);
    refr_frame();
    refr_log_reset();

    uint32_t max_row = refr_max_row_of(w, h);
    TEST_ASSERT_LESS_THAN_UINT32((uint32_t)h, max_row);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The screen does not fit into the buffer, so it is rendered in chunks*/
    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, max_row);
    LV_UNUSED(buf_h);

    /*The chunks together make up the whole screen*/
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(0, 0));
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(w - 1, h - 1));
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(w / 2, h / 2));
}

void test_refr_partial_mode_handles_the_remainder_rows(void)
{
    const int32_t w = 100;
    const int32_t h = 95;
    const int32_t buf_h = 10;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, buf_h);
    refr_screen_set_color(REFR_COLOR_GREEN);
    refr_frame();
    refr_log_reset();

    uint32_t max_row = refr_max_row_of(w, h);
    TEST_ASSERT_TRUE(max_row < (uint32_t)h);
    /*The last chunk is smaller than the others*/
    TEST_ASSERT_NOT_EQUAL(0, (uint32_t)h % max_row);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, max_row);
    TEST_ASSERT_EQUAL_INT32((int32_t)((uint32_t)h % max_row),
                            lv_area_get_height(&refr_ctx.flush_area[refr_ctx.flush_cnt - 1]));

    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(w - 1, h - 1));
    LV_UNUSED(buf_h);
}

void test_refr_partial_mode_small_area_is_not_split(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 10);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*An area that fits in the buffer is rendered in one go*/
    lv_area_t area = {10, 10, 59, 14};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&area, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(refr_ctx.flush_is_last[0]);
}

void test_refr_partial_mode_rounding_event_limits_the_rows(void)
{
    const int32_t w = 100;
    const int32_t h = 100;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 10);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    uint32_t max_row = refr_max_row_of(w, h);
    TEST_ASSERT_TRUE(max_row > 2);

    /*Drivers can round the rendered area. Rounding the height up to a multiple
     *that does not fit twice leaves exactly that many usable rows.*/
    uint32_t rounded_max_row = max_row - 1;
    refr_set_row_rounding((int32_t)rounded_max_row);
    lv_display_add_event_cb(refr_ctx.disp, refr_row_rounding_cb, LV_EVENT_INVALIDATE_AREA, NULL);
    refr_log_reset();

    lv_obj_invalidate(refr_screen());
    refr_frame();

    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, rounded_max_row);
}

void test_refr_partial_mode_narrow_area_gets_more_rows(void)
{
    const int32_t w = 100;
    const int32_t h = 200;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 10);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*A narrow area needs less memory per row, so more rows fit into the buffer*/
    const int32_t narrow_w = w / 4;
    lv_area_t narrow = {0, 0, narrow_w - 1, h - 1};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &narrow);
    refr_frame();

    uint32_t narrow_max_row = refr_max_row_of(narrow_w, h);
    uint32_t full_max_row = refr_max_row_of(w, h);
    TEST_ASSERT_TRUE(narrow_max_row > full_max_row);
    refr_check_flushed_chunks(&narrow, narrow_max_row);
}

void test_refr_direct_mode_renders_every_area_separately(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {90, 90, 99, 99};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    /*No splitting in direct mode, one flush per invalid area*/
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&a1, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(lv_area_is_equal(&a2, &refr_ctx.flush_area[1]));
}

void test_refr_full_mode_always_renders_the_whole_screen(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_FULL, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();

    lv_area_t full = {0, 0, 99, 99};
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(refr_ctx.flush_is_last[0]);

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(15, 15));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(90, 90));
}

void test_refr_without_active_screen(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_obj_t * scr = refr_screen();
    lv_obj_invalidate(scr);
    TEST_ASSERT_TRUE(refr_ctx.disp->inv_p > 0);

    /*Without an active screen there is nothing to render*/
    refr_ctx.disp->act_scr = NULL;
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);

    refr_ctx.disp->act_scr = scr;
}

void test_refr_nothing_to_do(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    refr_log_events(refr_ctx.disp);
    refr_log_reset();

    /*Without an invalid area nothing is rendered, but the refresh events are sent*/
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_REFR_START));
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_REFR_READY));
    TEST_ASSERT_FALSE(refr_event_seen(LV_EVENT_RENDER_START));
    TEST_ASSERT_FALSE(refr_event_seen(LV_EVENT_RENDER_READY));
}

void test_refr_refreshed_area_is_stored(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 16);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    lv_area_t area = {8, 8, 23, 23};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_TRUE(lv_area_is_equal(&area, &refr_ctx.disp->refreshed_area));
}

void test_refr_render_to_rgb565(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_RGB565, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();

    ASSERT_PX_EQ(0xFFF80000, refr_screen_px(15, 15)); /*Red, quantized to 5 bits*/
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(50, 50));
}

void test_refr_render_to_rgb888(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_RGB888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_GREEN);
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(15, 15));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(50, 50));
}

void test_refr_transparent_screen_is_cleared(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_ARGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);

    /*Nothing covers the display, so the buffer has to be cleared before rendering*/
    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(lv_display_get_layer_bottom(refr_ctx.disp), LV_OPA_TRANSP, 0);

    /*Fill the buffer with garbage to prove that it gets cleared*/
    lv_memset(refr_ctx.buf[0]->data, 0xAB, refr_ctx.buf[0]->data_size);

    refr_rect_create(scr, 10, 10, 20, 20, REFR_COLOR_RED);
    lv_obj_invalidate(scr);
    refr_frame();

    ASSERT_PX_EQ(0x00000000, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(15, 15));
}

void test_refr_transparent_screen_clears_only_the_invalid_area(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_ARGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);

    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(lv_display_get_layer_bottom(refr_ctx.disp), LV_OPA_TRANSP, 0);

    lv_obj_t * obj = refr_rect_create(scr, 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_invalidate(scr);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(10, 10));

    /*Invalidate an area away from the rectangle. The rectangle must survive in the buffer.*/
    lv_area_t far_area = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &far_area);
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[0], 10, 10));
    ASSERT_PX_EQ(0x00000000, refr_buf_px(refr_ctx.buf[0], 50, 50));
    LV_UNUSED(obj);
}

void test_refr_indexed_format_is_not_tiled(void)
{
    /*Indexed formats can not be rendered in tiles, the palette is shared*/
    refr_disp_create(64, 32, LV_COLOR_FORMAT_I1, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 32);
    lv_display_set_tile_cnt(refr_ctx.disp, 4);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    lv_area_t full = {0, 0, 63, 31};
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.flush_area[0]));
}

void test_refr_indexed_format_partial_mode(void)
{
    /*The palette is stored in the buffer, its space must not be used for rows*/
    const int32_t w = 848;
    const int32_t h = 64;
    refr_disp_create(w, h, LV_COLOR_FORMAT_I1, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 8);

    uint32_t stride = lv_draw_buf_width_to_stride((uint32_t)w, LV_COLOR_FORMAT_I1);
    uint32_t overhead = LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I1) * sizeof(lv_color32_t);
    uint32_t data_size = refr_ctx.disp->buf_act->data_size;
    uint32_t max_row = refr_max_row_of(w, h);

    /*The rendered rows and the palette together have to fit into the buffer*/
    TEST_ASSERT_TRUE(max_row * stride + overhead <= data_size);
    TEST_ASSERT_TRUE((max_row + 1) * stride + overhead > data_size);
    TEST_ASSERT_TRUE(max_row < (uint32_t)h);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, max_row);
}

void test_refr_single_buffer_waits_before_rendering(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    TEST_ASSERT_FALSE(lv_display_is_double_buffered(refr_ctx.disp));
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);
    refr_ctx.defer_flush_ready = true;
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*Rendering can only start once the previous flush is over*/
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_wait_cnt);
    TEST_ASSERT_EQUAL_PTR(refr_ctx.buf[0], refr_ctx.disp->buf_act);
}

void test_refr_double_buffer_partial_swaps_on_every_flush(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 2, 16);
    TEST_ASSERT_TRUE(lv_display_is_double_buffered(refr_ctx.disp));
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    const lv_draw_buf_t * first = refr_ctx.disp->buf_act;
    const lv_draw_buf_t * second = first == refr_ctx.buf[0] ? refr_ctx.buf[1] : refr_ctx.buf[0];

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The buffers alternate between the chunks*/
    TEST_ASSERT_TRUE(refr_ctx.flush_cnt > 1);
    uint32_t i;
    for(i = 0; i < refr_ctx.flush_cnt; i++) {
        TEST_ASSERT_EQUAL_PTR(i % 2 == 0 ? first : second, refr_ctx.flush_buf[i]);
    }
}

void test_refr_double_buffer_direct_swaps_only_after_the_last_area(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_draw_buf_t * buf_before = refr_ctx.disp->buf_act;
    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {50, 50, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    /*Both areas of a frame go to the same buffer*/
    TEST_ASSERT_EQUAL_PTR(buf_before, refr_ctx.flush_buf[0]);
    TEST_ASSERT_EQUAL_PTR(buf_before, refr_ctx.flush_buf[1]);
    /*The swap happens after the frame is complete*/
    TEST_ASSERT_NOT_EQUAL(buf_before, refr_ctx.disp->buf_act);
}

void test_refr_triple_buffer_rotates_the_buffers(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 3, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    /*The buffers are used in a round robin fashion*/
    uint32_t start;
    for(start = 0; start < 3; start++) {
        if(refr_ctx.disp->buf_act == refr_ctx.buf[start]) break;
    }
    TEST_ASSERT_TRUE(start < 3);

    uint32_t i;
    for(i = 1; i <= 4; i++) {
        lv_obj_invalidate(refr_screen());
        refr_frame();
        TEST_ASSERT_EQUAL_PTR(refr_ctx.buf[(start + i) % 3], refr_ctx.disp->buf_act);
    }
}

void test_refr_full_mode_double_buffer(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_FULL, 2, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    const lv_draw_buf_t * first = refr_ctx.disp->buf_act;
    const lv_draw_buf_t * second = first == refr_ctx.buf[0] ? refr_ctx.buf[1] : refr_ctx.buf[0];

    /*Full mode always renders everything, so both buffers get a complete image*/
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_PTR(first, refr_ctx.flush_buf[0]);

    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_PTR(second, refr_ctx.flush_buf[1]);

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[0], 15, 15));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[1], 15, 15));
}

void test_refr_buffers_with_explicit_stride(void)
{
    /*A driver may hand over a frame buffer with padding at the end of the rows*/
    const int32_t w = 60;
    const int32_t h = 40;
    const uint32_t stride = 64 * 4;
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
    refr_ctx.cf = LV_COLOR_FORMAT_XRGB8888;
    refr_ctx.hor_res = w;
    refr_ctx.ver_res = h;

    lv_display_t * disp = lv_display_create(w, h);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);
    /*The draw buffer is padded to LV_DRAW_BUF_ALIGN*/
    uint32_t buf_size = LV_ROUND_UP(stride * (uint32_t)h, LV_DRAW_BUF_ALIGN);
    uint8_t * raw = lv_malloc_zeroed(buf_size + LV_DRAW_BUF_ALIGN);
    TEST_ASSERT_NOT_NULL(raw);
    uint8_t * aligned = lv_draw_buf_align(raw, LV_COLOR_FORMAT_XRGB8888);
    lv_display_set_buffers_with_stride(disp, aligned, NULL, buf_size, stride, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, refr_flush_cb);
    refr_hide_sysmon(disp);
    refr_ctx.disp = disp;
    TEST_ASSERT_EQUAL_UINT32(0, disp->stride_is_auto);

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();

    /*The stride is kept when the layer's draw buffer is reshaped*/
    TEST_ASSERT_EQUAL_UINT32(stride, disp->buf_act->header.stride);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_px_read(aligned, stride, LV_COLOR_FORMAT_XRGB8888, 15, 15));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_px_read(aligned, stride, LV_COLOR_FORMAT_XRGB8888, 50, 30));

    lv_display_delete(disp);
    refr_ctx.disp = NULL;
    lv_free(raw);
}

void test_refr_raw_buffers_use_automatic_stride(void)
{
    const int32_t w = 60;
    const int32_t h = 40;
    uint32_t stride = lv_draw_buf_width_to_stride(w, LV_COLOR_FORMAT_XRGB8888);
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
    refr_ctx.cf = LV_COLOR_FORMAT_XRGB8888;
    refr_ctx.hor_res = w;
    refr_ctx.ver_res = h;

    lv_display_t * disp = lv_display_create(w, h);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);
    uint32_t buf_size = LV_ROUND_UP(stride * (uint32_t)h, LV_DRAW_BUF_ALIGN);
    uint8_t * raw = lv_malloc_zeroed(buf_size + LV_DRAW_BUF_ALIGN);
    TEST_ASSERT_NOT_NULL(raw);
    uint8_t * aligned = lv_draw_buf_align(raw, LV_COLOR_FORMAT_XRGB8888);
    lv_display_set_buffers(disp, aligned, NULL, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, refr_flush_cb);
    refr_hide_sysmon(disp);
    refr_ctx.disp = disp;
    TEST_ASSERT_EQUAL_UINT32(1, disp->stride_is_auto);

    refr_screen_set_color(REFR_COLOR_BLUE);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(stride, disp->buf_act->header.stride);
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_px_read(aligned, stride, LV_COLOR_FORMAT_XRGB8888, 30, 20));

    lv_display_delete(disp);
    refr_ctx.disp = NULL;
    lv_free(raw);
}

void test_refr_tiled_rendering(void)
{
    refr_disp_create(80, 80, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 80);
    lv_display_set_tile_cnt(refr_ctx.disp, 4);
    TEST_ASSERT_EQUAL_UINT32(4, lv_display_get_tile_cnt(refr_ctx.disp));

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 0, 0, 80, 80, REFR_COLOR_GREEN);
    /*A rectangle crossing every tile boundary*/
    refr_rect_create(refr_screen(), 30, 0, 20, 80, REFR_COLOR_RED);
    /*An object on a layer of its own, so that the tiles have nested layers*/
    lv_obj_t * layered = refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_BLUE);
    lv_obj_set_style_opa_layered(layered, 128, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Tiling is invisible from the outside: one flush with the complete image*/
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    lv_area_t full = {0, 0, 79, 79};
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.flush_area[0]));

    int32_t y;
    for(y = 25; y < 80; y += 5) {
        ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(10, y));
        ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, y));
        ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(70, y));
    }
}

void test_refr_tiled_rendering_partial_mode(void)
{
    refr_disp_create(80, 80, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 20);
    lv_display_set_tile_cnt(refr_ctx.disp, 2);

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 0, 0, 80, 80, REFR_COLOR_BLUE);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(4, refr_ctx.flush_cnt);
    int32_t y;
    for(y = 0; y < 80; y += 5) {
        ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(40, y));
    }
}

void test_refr_flush_area_carries_the_display_offset(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_physical_resolution(refr_ctx.disp, 128, 128);
    lv_display_set_offset(refr_ctx.disp, 5, 7);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_area_t area = {10, 20, 29, 39};
    lv_area_t expected = {15, 27, 34, 46};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.flush_area[0]));
}

void test_refr_event_order(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    refr_log_events(refr_ctx.disp);
    refr_log_reset();

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_event_count(LV_EVENT_REFR_START));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_count(LV_EVENT_RENDER_START));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_count(LV_EVENT_FLUSH_START));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_count(LV_EVENT_FLUSH_FINISH));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_count(LV_EVENT_RENDER_READY));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_count(LV_EVENT_REFR_READY));

    TEST_ASSERT_TRUE(refr_event_index(LV_EVENT_REFR_START) < refr_event_index(LV_EVENT_RENDER_START));
    TEST_ASSERT_TRUE(refr_event_index(LV_EVENT_RENDER_START) < refr_event_index(LV_EVENT_FLUSH_START));
    TEST_ASSERT_TRUE(refr_event_index(LV_EVENT_FLUSH_START) < refr_event_index(LV_EVENT_FLUSH_FINISH));
    TEST_ASSERT_TRUE(refr_event_index(LV_EVENT_FLUSH_FINISH) < refr_event_index(LV_EVENT_RENDER_READY));
    TEST_ASSERT_TRUE(refr_event_index(LV_EVENT_RENDER_READY) < refr_event_index(LV_EVENT_REFR_READY));
}

void test_refr_flush_wait_events(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);
    refr_ctx.defer_flush_ready = true;
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    refr_log_events(refr_ctx.disp);
    refr_log_reset();

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_wait_cnt);
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_FLUSH_WAIT_START));
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_FLUSH_WAIT_FINISH));
    /*The frame is complete, so the driver was told that this was the last flush*/
    TEST_ASSERT_EQUAL(1, refr_ctx.disp->flushing_last);
}

void test_refr_flush_wait_is_skipped_when_not_flushing(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*The flush callback reports readiness right away, so there is nothing to wait for*/
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_wait_cnt);
}

void test_refr_without_flush_cb(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_flush_cb(refr_ctx.disp, NULL);
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);

    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Rendering still happens, only the flushing is skipped*/
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_buf_px(refr_ctx.buf[0], 30, 30));
}

void test_refr_sync_areas_of_double_buffered_direct_mode(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Render a red rectangle into one of the buffers only*/
    const lv_draw_buf_t * rendered_buf = refr_ctx.disp->buf_act;
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t obj_area = {0, 0, 19, 19};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &obj_area);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(rendered_buf, 10, 10));

    const lv_draw_buf_t * next_buf = refr_ctx.disp->buf_act;
    TEST_ASSERT_NOT_EQUAL(rendered_buf, next_buf);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_buf_px(next_buf, 10, 10));

    /*Refresh a different area. Before rendering, the area updated in the previous
     *frame is copied to the buffer that is now being rendered.*/
    lv_area_t other = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &other);
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(next_buf, 10, 10));
}

void test_refr_sync_area_is_dropped_when_redrawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    refr_log_reset();

    /*The previous frame covered the whole screen. Everything around the newly
     *rendered area still has to be synchronized.*/
    lv_area_t area = {10, 10, 29, 29};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(4, refr_ctx.sync_cnt);
    uint32_t i;
    for(i = 0; i < refr_ctx.sync_cnt; i++) {
        lv_area_t common;
        TEST_ASSERT_FALSE(lv_area_intersect(&common, &refr_ctx.sync_area[i], &area));
    }

    refr_log_reset();
    /*Redraw exactly the same area: nothing is left to synchronize*/
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.sync_cnt);
}

void test_refr_sync_area_is_split_when_partially_redrawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t big = {10, 10, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &big);
    refr_frame();
    refr_log_reset();

    /*Cut a stripe out of the previous area, the rest still has to be synchronized*/
    lv_area_t stripe = {10, 20, 49, 29};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &stripe);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.sync_cnt);
    uint32_t i;
    for(i = 0; i < refr_ctx.sync_cnt; i++) {
        /*The synchronized parts are inside the old area but outside the new one*/
        TEST_ASSERT_TRUE(lv_area_is_in(&refr_ctx.sync_area[i], &big, 0));
        lv_area_t common;
        TEST_ASSERT_FALSE(lv_area_intersect(&common, &refr_ctx.sync_area[i], &stripe));
    }
}

void test_refr_joined_areas_are_synced_once(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*These two areas are joined before rendering, so only the joined area is
     *remembered for the next synchronization*/
    lv_area_t a1 = {10, 10, 39, 39};
    lv_area_t a2 = {20, 20, 49, 49};
    lv_area_t joined = {10, 10, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();
    refr_log_reset();

    lv_area_t far_area = {55, 0, 63, 5};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &far_area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&joined, &refr_ctx.sync_area[0]));
}

void test_refr_sync_area_outside_the_display_is_skipped(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_area_t area = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();
    refr_log_reset();

    /*The display got smaller, the remembered area is not on it anymore*/
    lv_display_set_resolution(refr_ctx.disp, 32, 32);
    refr_ctx.hor_res = 32;
    refr_ctx.ver_res = 32;

    lv_area_t small = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &small);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.sync_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
}

void test_refr_sync_cb_with_single_buffer(void)
{
    /*A sync callback is used even without double buffering*/
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 16);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    lv_display_set_sync_wait_cb(refr_ctx.disp, refr_sync_wait_cb);
    refr_ctx.defer_sync_ready = true;
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t a1 = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    refr_frame();
    refr_log_reset();

    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&a1, &refr_ctx.sync_area[0]));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_wait_cnt);
    TEST_ASSERT_EQUAL(0, refr_ctx.disp->syncing_last);
}

void test_refr_sync_events_and_offset(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_physical_resolution(refr_ctx.disp, 128, 128);
    lv_display_set_offset(refr_ctx.disp, 3, 4);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_log_events(refr_ctx.disp);

    lv_area_t a1 = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    refr_frame();
    refr_log_reset();

    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_cnt);
    lv_area_t expected = {3, 4, 12, 13};
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.sync_area[0]));
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_SYNC_START));
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_SYNC_FINISH));
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_SYNC_WAIT_START));
    TEST_ASSERT_TRUE(refr_event_seen(LV_EVENT_SYNC_WAIT_FINISH));
}

void test_refr_no_sync_without_double_buffer_or_sync_cb(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_log_events(refr_ctx.disp);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t a1 = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    refr_frame();
    refr_log_reset();

    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.sync_cnt);
    TEST_ASSERT_FALSE(refr_event_seen(LV_EVENT_SYNC_START));
}

void test_refr_triple_buffer_sync(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 3, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Render a rectangle into one buffer only*/
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t obj_area = {0, 0, 19, 19};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &obj_area);
    refr_frame();

    /*Two more frames, each of them refreshing an unrelated area. The rectangle
     *has to be propagated to all of the three buffers.*/
    uint32_t i;
    for(i = 0; i < 2; i++) {
        lv_area_t other = {40, 40, 59, 59};
        refr_ctx.disp->inv_p = 0;
        lv_inv_area(refr_ctx.disp, &other);
        refr_frame();
    }

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[0], 10, 10));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[1], 10, 10));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[2], 10, 10));
}

void test_refr_now_refreshes_the_given_display(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_log_reset();

    lv_refr_now(refr_ctx.disp);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);

    /*Nothing left to do*/
    refr_log_reset();
    lv_refr_now(refr_ctx.disp);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

void test_refr_now_refreshes_every_display(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_log_reset();

    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
}

void test_refr_now_without_refresh_timer(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_display_delete_refr_timer(refr_ctx.disp);
    TEST_ASSERT_NULL(lv_display_get_refr_timer(refr_ctx.disp));
    refr_log_reset();

    /*Without a refresh timer the display is never refreshed*/
    lv_refr_now(refr_ctx.disp);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

void test_refr_timer_without_argument_uses_the_default_display(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(refr_ctx.disp);
    lv_obj_invalidate(refr_screen());
    lv_display_refr_timer(NULL);
    lv_display_set_default(disp_def);

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
}

void test_refr_timer_without_display(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(NULL);
    /*Nothing to refresh, but it must not crash*/
    lv_display_refr_timer(NULL);
    lv_display_set_default(disp_def);
}

void test_refr_display_without_draw_buffer(void)
{
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
    lv_display_t * disp = lv_display_create(64, 64);
    refr_ctx.disp = disp;
    lv_display_set_flush_cb(disp, refr_flush_cb);
    TEST_ASSERT_NULL(lv_display_get_buf_active(disp));

    lv_obj_invalidate(lv_display_get_screen_active(disp));
    lv_display_refr_timer(lv_display_get_refr_timer(disp));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

void test_refr_get_and_set_disp_refreshing(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    /*The last refreshed display is remembered*/
    TEST_ASSERT_EQUAL_PTR(refr_ctx.disp, lv_refr_get_disp_refreshing());

    lv_display_t * disp_def = lv_display_get_default();
    lv_refr_set_disp_refreshing(disp_def);
    TEST_ASSERT_EQUAL_PTR(disp_def, lv_refr_get_disp_refreshing());
}

static bool rendering_in_progress_while_drawing;
static lv_display_t * disp_refreshing_while_drawing;

static void draw_main_state_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_display_t * disp = lv_obj_get_display(obj);
    rendering_in_progress_while_drawing = disp->rendering_in_progress;
    disp_refreshing_while_drawing = lv_refr_get_disp_refreshing();
}

void test_refr_rendering_in_progress_flag(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    rendering_in_progress_while_drawing = false;
    disp_refreshing_while_drawing = NULL;
    lv_obj_add_event_cb(obj, draw_main_state_cb, LV_EVENT_DRAW_MAIN, NULL);

    TEST_ASSERT_FALSE(refr_ctx.disp->rendering_in_progress);
    refr_frame();

    TEST_ASSERT_TRUE(rendering_in_progress_while_drawing);
    TEST_ASSERT_EQUAL_PTR(refr_ctx.disp, disp_refreshing_while_drawing);
    /*The flag is cleared when the rendering is over*/
    TEST_ASSERT_FALSE(refr_ctx.disp->rendering_in_progress);
}

static uint32_t refr_start_cnt;
static lv_event_code_t last_event_before_delete;

static void delete_on_refr_start_cb(lv_event_t * e)
{
    refr_start_cnt++;
    last_event_before_delete = lv_event_get_code(e);
    lv_display_delete(lv_event_get_current_target(e));
    refr_ctx.disp = NULL;
}

void test_refr_display_deleted_in_refr_start(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_start_cnt = 0;
    lv_display_add_event_cb(refr_ctx.disp, delete_on_refr_start_cb, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(refr_ctx.disp, delete_on_refr_start_cb, LV_EVENT_REFR_READY, NULL);

    lv_display_t * disp = refr_ctx.disp;
    lv_obj_invalidate(lv_display_get_screen_active(disp));
    lv_display_refr_timer(lv_display_get_refr_timer(disp));

    /*The refresh is aborted, so LV_EVENT_REFR_READY is never sent*/
    TEST_ASSERT_EQUAL_UINT32(1, refr_start_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_REFR_START, last_event_before_delete);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    TEST_ASSERT_NULL(refr_ctx.disp);
}

void test_refr_bottom_layer_is_drawn_when_nothing_covers(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);

    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_TRANSP, 0);

    lv_obj_t * bottom = lv_display_get_layer_bottom(refr_ctx.disp);
    lv_obj_set_style_bg_opa(bottom, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(bottom, lv_color_hex(REFR_COLOR_BLUE), 0);

    lv_obj_invalidate(scr);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(30, 30));

    /*An opaque screen covers the bottom layer, so it is not drawn at all*/
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scr, lv_color_hex(REFR_COLOR_RED), 0);
    lv_obj_invalidate(scr);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(30, 30));
}

void test_refr_top_and_sys_layers_are_always_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    /*The screen covers everything below, but the top and sys layers are above it*/
    refr_rect_create(refr_screen(), 0, 0, 64, 64, REFR_COLOR_RED);
    refr_rect_create(lv_display_get_layer_top(refr_ctx.disp), 0, 0, 20, 20, REFR_COLOR_GREEN);
    refr_rect_create(lv_display_get_layer_sys(refr_ctx.disp), 30, 0, 20, 20, REFR_COLOR_BLUE);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(10, 10));
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(40, 10));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(10, 40));
}

void test_refr_previous_screen_drawn_below_the_active_one(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(refr_ctx.disp);
    lv_obj_t * prev = lv_obj_create(NULL);
    lv_display_set_default(disp_def);

    lv_obj_remove_style_all(prev);
    lv_obj_set_style_bg_opa(prev, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(prev, lv_color_hex(REFR_COLOR_GREEN), 0);

    refr_screen_set_color(REFR_COLOR_RED);

    /*Both screens cover the display. The active one is drawn last, so it wins.*/
    refr_ctx.disp->prev_scr = prev;
    refr_ctx.disp->draw_prev_over_act = 0;
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(30, 30));

    /*With draw_prev_over_act the previous screen is drawn on top*/
    refr_ctx.disp->draw_prev_over_act = 1;
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(30, 30));

    refr_ctx.disp->prev_scr = NULL;
    refr_ctx.disp->draw_prev_over_act = 0;
    lv_obj_delete(prev);
}

void test_refr_screen_load_animation(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(refr_ctx.disp);

    refr_screen_set_color(REFR_COLOR_RED);
    lv_obj_t * next = lv_obj_create(NULL);
    lv_obj_remove_style_all(next);
    lv_obj_set_style_bg_opa(next, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(next, lv_color_hex(REFR_COLOR_GREEN), 0);

    lv_screen_load_anim(next, LV_SCREEN_LOAD_ANIM_MOVE_LEFT, 100, 0, false);

    /*Half way through the animation both screens are on the display*/
    lv_tick_inc(50);
    lv_timer_handler();
    refr_frame();
    TEST_ASSERT_NOT_NULL(lv_display_get_screen_prev(refr_ctx.disp));

    bool red_seen = false;
    bool green_seen = false;
    int32_t x;
    for(x = 0; x < 64; x++) {
        uint32_t px = refr_screen_px(x, 30);
        if(px == REFR_COLOR_RED) red_seen = true;
        if(px == REFR_COLOR_GREEN) green_seen = true;
    }
    ASSERT_PX_TRUE(red_seen);
    ASSERT_PX_TRUE(green_seen);

    /*At the end only the new screen is left*/
    lv_tick_inc(100);
    lv_timer_handler();
    refr_frame();
    TEST_ASSERT_NULL(lv_display_get_screen_prev(refr_ctx.disp));
    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(30, 30));

    lv_display_set_default(disp_def);
}

void test_refr_hidden_object_is_not_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_set_hidden(obj, true);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));
}

void test_refr_fully_transparent_layer_is_not_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_set_style_opa_layered(obj, LV_OPA_TRANSP, 0);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));
}

void test_refr_object_outside_the_clip_area_is_not_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    lv_obj_t * layered = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_GREEN);
    lv_obj_set_style_opa_layered(layered, LV_OPA_50, 0);

    uint32_t draw_cnt = 0;
    uint32_t layered_draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_add_event_cb(layered, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &layered_draw_cnt);
    lv_obj_update_layout(refr_screen());

    /*Refresh an area far from the objects*/
    lv_area_t area = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, layered_draw_cnt);
}

void test_refr_transformed_object_outside_the_clip_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 450, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_update_layout(refr_screen());

    /*The transformed area of the object is not in the refreshed area*/
    lv_area_t area = {60, 60, 79, 79};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
}

void test_refr_transformed_object_corner_of_the_bounding_box(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 450, 0);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_update_layout(refr_screen());

    /*The corner of the bounding box of a rotated square is empty. It overlaps the
     *transformed area, but no part of the object is rendered there.*/
    lv_area_t corner = {22, 22, 24, 24};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &corner);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(23, 23));
}

void test_refr_layered_opacity_is_blended(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_WHITE);
    lv_obj_set_style_opa_layered(obj, 128, 0);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t px = refr_screen_px(15, 15);
    uint32_t r = (px >> 16) & 0xFF;
    /*White over black with 50% opacity*/
    ASSERT_PX_WITHIN(4, 128, r);
}

void test_refr_simple_layer_is_split_into_chunks(void)
{
    /*A simple layer is rendered in chunks that fit into LV_DRAW_LAYER_SIMPLE_BUF_SIZE*/
    const int32_t w = 200;
    const int32_t h = 100;
    refr_disp_create(w + 20, h + 20, LV_COLOR_FORMAT_RGB565, LV_DISPLAY_RENDER_MODE_DIRECT, 1, h + 20);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_obj_t * obj = refr_rect_create(refr_screen(), 0, 0, w, h, REFR_COLOR_WHITE);
    lv_obj_set_style_opa_layered(obj, 128, 0);
    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The object is opaque, so the chunks are rendered without an alpha channel*/
    uint32_t px_size = lv_color_format_get_size(LV_COLOR_FORMAT_RGB565);
    uint32_t max_rgb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / px_size;
    uint32_t expected_rgb = ((uint32_t)h + max_rgb_rows - 1) / max_rgb_rows;
    TEST_ASSERT_EQUAL_UINT32(expected_rgb, draw_cnt);

    /*Every chunk is blended the same way*/
    uint32_t reference = refr_screen_px(w / 2, 0);
    ASSERT_PX_WITHIN(12, 128, (reference >> 16) & 0xFF);
    int32_t y;
    for(y = 0; y < h; y += 7) {
        ASSERT_PX_EQ(reference, refr_screen_px(w / 2, y));
    }

    /*With a rounded corner the layer needs an alpha channel, so the chunks are smaller*/
    draw_cnt = 0;
    lv_obj_set_style_radius(obj, 20, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t max_argb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / sizeof(lv_color32_t);
    uint32_t expected_argb = ((uint32_t)h + max_argb_rows - 1) / max_argb_rows;
    TEST_ASSERT_EQUAL_UINT32(expected_argb, draw_cnt);
    TEST_ASSERT_TRUE(expected_argb >= expected_rgb);
}

void test_refr_bitmap_mask_forces_an_alpha_layer(void)
{
    /*An object with a bitmap mask always needs an alpha channel, even if it is
     *opaque. So its layer is split into the smaller, ARGB sized chunks.*/
    const int32_t w = 200;
    const int32_t h = 100;
    refr_disp_create(w + 20, h + 20, LV_COLOR_FORMAT_RGB565, LV_DISPLAY_RENDER_MODE_DIRECT, 1, h + 20);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_obj_t * obj = refr_rect_create(refr_screen(), 0, 0, w, h, REFR_COLOR_WHITE);
    /*Keep the object on a layer of its own in every configuration*/
    lv_obj_set_style_opa_layered(obj, 128, 0);
    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t px_size = lv_color_format_get_size(LV_COLOR_FORMAT_RGB565);
    uint32_t max_rgb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / px_size;
    TEST_ASSERT_EQUAL_UINT32(((uint32_t)h + max_rgb_rows - 1) / max_rgb_rows, draw_cnt);

    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    lv_obj_set_style_bitmap_mask_src(obj, &test_image_cogwheel_a8, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_SIMPLE, lv_obj_get_layer_type(obj));

    draw_cnt = 0;
    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t max_argb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / sizeof(lv_color32_t);
    TEST_ASSERT_EQUAL_UINT32(((uint32_t)h + max_argb_rows - 1) / max_argb_rows, draw_cnt);
}

void test_refr_transformed_object_uses_a_layer(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 900, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, draw_cnt);
    /*A square rotated by 90 degrees around its center covers the same area*/
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));
}

void test_refr_rotation_is_normalized(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);

    /*Angles out of the 0..3600 range are wrapped around*/
    lv_obj_set_style_transform_rotation(obj, 3600 + 900, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));

    lv_obj_set_style_transform_rotation(obj, -900, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));
}

void test_refr_scaled_and_skewed_object(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 40, 40, 20, 20, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);

    /*Twice as large around its center*/
    lv_obj_set_style_transform_scale_x(obj, 2 * LV_SCALE_NONE, 0);
    lv_obj_set_style_transform_scale_y(obj, 2 * LV_SCALE_NONE, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    lv_obj_invalidate(refr_screen());
    refr_frame();
    /*The object covers twice as much as before*/
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(35, 50));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 65));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));

    /*A skewed object is rendered too. How it looks depends on the renderer,
     *only the fact that it is drawn is checked here.*/
    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_set_style_transform_scale_x(obj, LV_SCALE_NONE, 0);
    lv_obj_set_style_transform_scale_y(obj, LV_SCALE_NONE, 0);
    lv_obj_set_style_transform_skew_x(obj, 20, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, draw_cnt);
}

void test_refr_zero_scale_is_not_drawn(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_scale_x(obj, 0, 0);

    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(50, 50));
}

void test_refr_children_are_clipped_to_the_parent(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_BLUE);
    refr_rect_create(parent, 20, 20, 60, 60, REFR_COLOR_RED);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The child is visible inside the parent*/
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, 40));
    /*and clipped outside of it*/
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(60, 60));
}

static void ext_draw_size_cb(lv_event_t * e)
{
    lv_event_set_ext_draw_size(e, 40);
}

void test_refr_overflow_visible_extends_the_clip_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_BLUE);
    lv_obj_add_event_cb(parent, ext_draw_size_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_refresh_ext_draw_size(parent);
    refr_rect_create(parent, 20, 20, 60, 60, REFR_COLOR_RED);

    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(60, 60));

    /*With overflow visible the children may draw on the extended draw area*/
    lv_obj_set_overflow_visible(parent, true);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(60, 60));
}

void test_refr_children_of_a_transparent_parent_are_skipped(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 60, 60, REFR_COLOR_BLUE);
    lv_obj_t * child = refr_rect_create(parent, 0, 0, 20, 20, REFR_COLOR_RED);

    uint32_t child_draw_cnt = 0;
    lv_obj_add_event_cb(child, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &child_draw_cnt);

    /*A completely transparent parent makes the whole subtree invisible*/
    lv_obj_set_style_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, child_draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));
}

void test_refr_clip_corner_splits_the_children(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 60, 60, REFR_COLOR_BLUE);
    lv_obj_set_style_radius(parent, 20, 0);
    lv_obj_set_style_clip_corner(parent, true, 0);

    lv_obj_t * child = refr_rect_create(parent, 0, 0, 60, 60, REFR_COLOR_RED);
    uint32_t child_draw_cnt = 0;
    lv_obj_add_event_cb(child, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &child_draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The children are rendered three times: the top, the middle and the bottom part*/
    TEST_ASSERT_EQUAL_UINT32(3, child_draw_cnt);
    /*The corner of the child is cut off, the middle is not*/
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(11, 11));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, 40));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, 12));

    /*Without a radius there is nothing to clip*/
    child_draw_cnt = 0;
    lv_obj_set_style_radius(parent, 0, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, child_draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(11, 11));
}

void test_refr_post_draw_events_are_sent(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_BLUE);
    lv_obj_t * child = refr_rect_create(parent, 0, 0, 10, 10, REFR_COLOR_RED);

    uint32_t parent_post = 0;
    uint32_t child_post = 0;
    lv_obj_add_event_cb(parent, refr_draw_main_counter_cb, LV_EVENT_DRAW_POST, &parent_post);
    lv_obj_add_event_cb(child, refr_draw_main_counter_cb, LV_EVENT_DRAW_POST, &child_post);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Both the object with children and the leaf object get the post draw events*/
    TEST_ASSERT_EQUAL_UINT32(1, parent_post);
    TEST_ASSERT_EQUAL_UINT32(1, child_post);
}

void test_refr_obj_redraw_invalid_arguments(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_layer_t layer;
    lv_obj_redraw(NULL, refr_screen());
    lv_obj_redraw(&layer, NULL);
    lv_obj_refr(NULL, refr_screen());
    lv_obj_refr(&layer, NULL);
}

#if LV_DRAW_TRANSFORM_USE_MATRIX

/*The pixels of a rotated display are not where the logical coordinates say,
 *so the draw buffer is checked directly*/
static void check_rotated_rect(const lv_draw_buf_t * buf, const lv_area_t * logical_area, uint32_t color)
{
    lv_area_t physical = *logical_area;
    lv_display_rotate_area(refr_ctx.disp, &physical);

    ASSERT_PX_EQ(color, refr_buf_px(buf, physical.x1, physical.y1));
    ASSERT_PX_EQ(color, refr_buf_px(buf, physical.x2, physical.y2));
    ASSERT_PX_EQ(color, refr_buf_px(buf, (physical.x1 + physical.x2) / 2,
                                    (physical.y1 + physical.y2) / 2));
}

#endif /*LV_DRAW_TRANSFORM_USE_MATRIX*/

void test_refr_matrix_display_rotation(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    /*A square display so that every rotation uses the same buffer size*/
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_matrix_rotation(refr_ctx.disp, true);
    TEST_ASSERT_TRUE(lv_display_get_matrix_rotation(refr_ctx.disp));

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);

    static const lv_display_rotation_t rotations[] = {
        LV_DISPLAY_ROTATION_0, LV_DISPLAY_ROTATION_90,
        LV_DISPLAY_ROTATION_180, LV_DISPLAY_ROTATION_270,
    };
    lv_area_t logical = {0, 0, 19, 19};

    size_t i;
    for(i = 0; i < sizeof(rotations) / sizeof(rotations[0]); i++) {
        lv_display_set_rotation(refr_ctx.disp, rotations[i]);
        lv_obj_invalidate(refr_screen());
        refr_frame();

        /*The buffer is always the size of the unrotated display*/
        TEST_ASSERT_EQUAL_INT32(lv_display_get_original_horizontal_resolution(refr_ctx.disp),
                                (int32_t)refr_ctx.disp->buf_act->header.w);
        check_rotated_rect(refr_ctx.disp->buf_act, &logical, REFR_COLOR_RED);
    }

    lv_display_set_rotation(refr_ctx.disp, LV_DISPLAY_ROTATION_0);
    lv_display_set_matrix_rotation(refr_ctx.disp, false);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_matrix_sync_areas_are_rotated(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_matrix_rotation(refr_ctx.disp, true);
    lv_display_set_rotation(refr_ctx.disp, LV_DISPLAY_ROTATION_90);

    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Render a rectangle into one buffer only*/
    const lv_draw_buf_t * rendered_buf = refr_ctx.disp->buf_act;
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t logical = {0, 0, 19, 19};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &logical);
    refr_frame();
    check_rotated_rect(rendered_buf, &logical, REFR_COLOR_RED);

    /*Refresh an unrelated area. The rectangle has to be copied to the other
     *buffer at its rotated position.*/
    const lv_draw_buf_t * next_buf = refr_ctx.disp->buf_act;
    TEST_ASSERT_NOT_EQUAL(rendered_buf, next_buf);
    lv_area_t other = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &other);
    refr_frame();
    check_rotated_rect(next_buf, &logical, REFR_COLOR_RED);

    lv_display_set_rotation(refr_ctx.disp, LV_DISPLAY_ROTATION_0);
    lv_display_set_matrix_rotation(refr_ctx.disp, false);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_obj_matrix_is_applied_without_a_layer(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_translate(&matrix, 40, 30);
    lv_obj_set_transform(obj, &matrix);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*A fully opaque transformed object is drawn directly with a matrix*/
    TEST_ASSERT_EQUAL_UINT32(1, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(60, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));

    lv_obj_set_transform(obj, NULL);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_obj_matrix_not_invertible(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    /*A matrix that collapses everything to a line can not be inverted*/
    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_scale(&matrix, 1.0f, 0.0f);
    lv_obj_set_transform(obj, &matrix);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));

    lv_obj_set_transform(obj, NULL);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_rotated_obj_clipped_by_the_invalid_area(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 900, 0);

    /*The whole object is inside the clip area, so the matrix can be used*/
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));

    /*Now only a part of the object is refreshed, so it is rendered to a layer*/
    lv_area_t part = {40, 40, 59, 59};
    refr_log_reset();
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &part);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

#endif /*LV_BUILD_TEST*/
