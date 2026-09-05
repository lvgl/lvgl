/**
* @file lv_test_refr.c
*
*/
#if LV_BUILD_TEST

/*********************
 *      INCLUDES
 *********************/

#include "lv_test_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool cf_is_readable(lv_color_format_t cf);
static void full_frame_buffer_copy(lv_display_t * disp, const lv_area_t * area, const uint8_t * px_map);
static void log_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

static int32_t row_rounding = REFR_ROW_ROUNDING_DEFAULT;

/**********************
 *      MACROS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

refr_ctx_t refr_ctx;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void refr_ctx_reset(void)
{
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
    row_rounding = REFR_ROW_ROUNDING_DEFAULT;
}

void refr_hide_sysmon(lv_display_t * disp)
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

void refr_set_row_rounding(int32_t rows)
{
    row_rounding = rows;
}

uint32_t refr_px_read(const uint8_t * buf, uint32_t stride, lv_color_format_t cf, int32_t x, int32_t y)
{
    uint32_t px_size = lv_color_format_get_size(cf);
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

uint32_t refr_screen_px(int32_t x, int32_t y)
{
    TEST_ASSERT_NOT_NULL(refr_ctx.full_frame_buffer);
    TEST_ASSERT_TRUE(x >= 0 && x < refr_ctx.hor_res && y >= 0 && y < refr_ctx.ver_res);
    return refr_px_read(refr_ctx.full_frame_buffer, refr_ctx.full_frame_buffer_stride, refr_ctx.cf, x, y);
}

uint32_t refr_buf_px(const lv_draw_buf_t * buf, int32_t x, int32_t y)
{
    return refr_px_read(buf->data, buf->header.stride, refr_ctx.cf, x, y);
}

void refr_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    if(refr_ctx.flush_cnt < REFR_LOG_MAX) {
        refr_ctx.flush_area[refr_ctx.flush_cnt] = *area;
        refr_ctx.flush_is_last[refr_ctx.flush_cnt] = lv_display_flush_is_last(disp);
        refr_ctx.flush_buf[refr_ctx.flush_cnt] = disp->buf_act;
    }
    refr_ctx.flush_cnt++;

    full_frame_buffer_copy(disp, area, px_map);

    if(!refr_ctx.defer_flush_ready) lv_display_flush_ready(disp);
}

void refr_flush_wait_cb(lv_display_t * disp)
{
    LV_UNUSED(disp);
    refr_ctx.flush_wait_cnt++;
}

void refr_partial_sync_cb(lv_display_t * disp, const lv_area_t * area)
{
    if(refr_ctx.partial_sync_cnt < REFR_LOG_MAX) refr_ctx.partial_sync_area[refr_ctx.partial_sync_cnt] = *area;
    refr_ctx.partial_sync_cnt++;
    if(!refr_ctx.defer_partial_sync_ready) lv_display_partial_sync_ready(disp);
}

void refr_partial_sync_wait_cb(lv_display_t * disp)
{
    LV_UNUSED(disp);
    refr_ctx.partial_sync_wait_cnt++;
}

/*Recording the display events*/

void refr_log_events(lv_display_t * disp)
{
    static const lv_event_code_t codes[] = {
        LV_EVENT_REFR_START, LV_EVENT_REFR_READY,
        LV_EVENT_RENDER_START, LV_EVENT_RENDER_READY,
        LV_EVENT_FLUSH_START, LV_EVENT_FLUSH_FINISH,
        LV_EVENT_FLUSH_WAIT_START, LV_EVENT_FLUSH_WAIT_FINISH,
        LV_EVENT_PARTIAL_SYNC_START, LV_EVENT_PARTIAL_SYNC_FINISH,
        LV_EVENT_PARTIAL_SYNC_WAIT_START, LV_EVENT_PARTIAL_SYNC_WAIT_FINISH,
    };
    size_t i;
    for(i = 0; i < sizeof(codes) / sizeof(codes[0]); i++) {
        lv_display_add_event_cb(disp, log_event_cb, codes[i], NULL);
    }
}

bool refr_event_is_seen(lv_event_code_t code)
{
    uint32_t i;
    for(i = 0; i < refr_ctx.ev_cnt && i < REFR_LOG_MAX; i++) {
        if(refr_ctx.ev[i] == code) return true;
    }
    return false;
}

uint32_t refr_event_get_count(lv_event_code_t code)
{
    uint32_t i;
    uint32_t cnt = 0;
    for(i = 0; i < refr_ctx.ev_cnt && i < REFR_LOG_MAX; i++) {
        if(refr_ctx.ev[i] == code) cnt++;
    }
    return cnt;
}

int32_t refr_event_get_index(lv_event_code_t code)
{
    uint32_t i;
    for(i = 0; i < refr_ctx.ev_cnt && i < REFR_LOG_MAX; i++) {
        if(refr_ctx.ev[i] == code) return (int32_t)i;
    }
    return -1;
}

void refr_log_reset(void)
{
    refr_ctx.flush_cnt = 0;
    refr_ctx.flush_wait_cnt = 0;
    refr_ctx.partial_sync_cnt = 0;
    refr_ctx.partial_sync_wait_cnt = 0;
    refr_ctx.ev_cnt = 0;
}

lv_display_t * refr_disp_create(int32_t w, int32_t h, lv_color_format_t cf,
                                lv_display_render_mode_t mode, uint32_t buf_cnt, int32_t buf_h)
{
    uint32_t i;

    TEST_ASSERT_TRUE(buf_cnt >= 1 && buf_cnt <= 3);

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

    if(cf_is_readable(cf)) {
        refr_ctx.full_frame_buffer_px_size = lv_color_format_get_size(cf);
        refr_ctx.full_frame_buffer_stride = (uint32_t)w * refr_ctx.full_frame_buffer_px_size;
        refr_ctx.full_frame_buffer = lv_malloc_zeroed(refr_ctx.full_frame_buffer_stride * (uint32_t)h);
        TEST_ASSERT_NOT_NULL(refr_ctx.full_frame_buffer);
    }

    refr_ctx.disp = disp;
    return disp;
}

void refr_disp_delete(void)
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
    if(refr_ctx.full_frame_buffer) {
        lv_free(refr_ctx.full_frame_buffer);
        refr_ctx.full_frame_buffer = NULL;
    }
}

void refr_frame(void)
{
    lv_display_refr_timer(lv_display_get_refr_timer(refr_ctx.disp));
}

uint32_t refr_max_row_of(int32_t area_w, int32_t area_h)
{
    uint32_t stride = lv_draw_buf_width_to_stride((uint32_t)area_w, refr_ctx.cf);
    uint32_t overhead = LV_COLOR_INDEXED_PALETTE_SIZE(refr_ctx.cf) * sizeof(lv_color32_t);
    uint32_t max_row = (refr_ctx.disp->buf_act->data_size - overhead) / stride;
    if(max_row > (uint32_t)area_h) max_row = (uint32_t)area_h;
    return max_row;
}

void refr_check_flushed_chunks(const lv_area_t * area, uint32_t max_row)
{
    uint32_t expected_cnt = ((uint32_t)lv_area_get_height(area) + max_row - 1) / max_row;
    TEST_ASSERT_EQUAL_UINT32(expected_cnt, refr_ctx.flush_cnt);
    /*Above `REFR_LOG_MAX` the flushed areas are not recorded any more*/
    TEST_ASSERT_TRUE(refr_ctx.flush_cnt <= REFR_LOG_MAX);

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

lv_obj_t * refr_screen(void)
{
    return lv_display_get_screen_active(refr_ctx.disp);
}

void refr_screen_set_color(uint32_t argb)
{
    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scr, lv_color_hex(argb), 0);
}

lv_obj_t * refr_rect_create(lv_obj_t * parent, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t argb)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(argb), 0);
    return obj;
}

void refr_draw_main_counter_cb(lv_event_t * e)
{
    uint32_t * cnt = lv_event_get_user_data(e);
    (*cnt)++;
}

void refr_row_rounding_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    lv_area_t * area = lv_event_get_invalidated_area(e);
    int32_t h = lv_area_get_height(area);
    int32_t rounded = ((h + row_rounding - 1) / row_rounding) * row_rounding;
    int32_t max_y = lv_display_get_vertical_resolution(disp) - 1;
    area->y2 = LV_MIN(area->y1 + rounded - 1, max_y);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Only the color formats that `refr_px_read()` can decode are kept in the full frame buffer*/
static bool cf_is_readable(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_RGB565:
            return true;
        default:
            return false;
    }
}

static void full_frame_buffer_copy(lv_display_t * disp, const lv_area_t * area, const uint8_t * px_map)
{
    if(refr_ctx.full_frame_buffer == NULL) return;

    /*The flushed area carries the display offset, the full frame buffer does not*/
    lv_area_t a = *area;
    lv_area_move(&a, -disp->offset_x, -disp->offset_y);
    if(a.x1 < 0 || a.y1 < 0 || a.x2 >= refr_ctx.hor_res || a.y2 >= refr_ctx.ver_res) return;

    uint32_t px_size = refr_ctx.full_frame_buffer_px_size;
    uint32_t src_stride = disp->buf_act->header.stride;
    bool partial = lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_PARTIAL;
    int32_t w = lv_area_get_width(&a);
    int32_t h = lv_area_get_height(&a);
    int32_t y;

    for(y = 0; y < h; y++) {
        const uint8_t * src;
        if(partial) src = px_map + (uint32_t)y * src_stride;
        else src = px_map + (uint32_t)(a.y1 + y) * src_stride + (uint32_t)a.x1 * px_size;

        lv_memcpy(refr_ctx.full_frame_buffer + (uint32_t)(a.y1 + y) * refr_ctx.full_frame_buffer_stride +
                  (uint32_t)a.x1 * px_size,
                  src, (uint32_t)w * px_size);
    }
}

static void log_event_cb(lv_event_t * e)
{
    if(refr_ctx.ev_cnt < REFR_LOG_MAX) refr_ctx.ev[refr_ctx.ev_cnt] = lv_event_get_code(e);
    refr_ctx.ev_cnt++;
}

#endif /*LV_BUILD_TEST*/
