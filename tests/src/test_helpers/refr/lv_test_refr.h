/**
* @file lv_test_refr.h
*
* Shared fixture of the `test_refr_*` test cases: a display that records what it
* was asked to flush and sync, a copy of the rendered screen, and the shorthands
* to build a predictable scene on it.
*/
#if LV_BUILD_TEST

#ifndef LV_TEST_REFR_H
#define LV_TEST_REFR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

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

/*The height `refr_row_rounding_cb()` rounds up to unless a test asks for another one*/
#define REFR_ROW_ROUNDING_DEFAULT   4

#define REFR_COLOR_RED      0xFFFF0000
#define REFR_COLOR_GREEN    0xFF00FF00
#define REFR_COLOR_BLUE     0xFF0000FF
#define REFR_COLOR_WHITE    0xFFFFFFFF
#define REFR_COLOR_BLACK    0xFF000000

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
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

    /*The whole screen, assembled from the flushed areas. Only allocated for the color
     *formats that `refr_px_read()` can decode*/
    uint8_t * full_frame_buffer;
    uint32_t full_frame_buffer_stride;
    uint32_t full_frame_buffer_px_size;
} refr_ctx_t;

/**********************
 * GLOBAL VARIABLES
 **********************/

extern refr_ctx_t refr_ctx;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void refr_ctx_reset(void);

/**
 * Create the display under test and reset the context that describes it.
 * @param w         horizontal resolution
 * @param h         vertical resolution
 * @param cf        color format of the display
 * @param mode      render mode of the display
 * @param buf_cnt   number of draw buffers to create, 1..3
 * @param buf_h     height of a draw buffer
 * @return          the created display
 */
lv_display_t * refr_disp_create(int32_t w, int32_t h, lv_color_format_t cf,
                                lv_display_render_mode_t mode, uint32_t buf_cnt, int32_t buf_h);

/**
 * Delete the display under test with its draw buffers and the full frame buffer.
 * Safe to call when nothing was created.
 */
void refr_disp_delete(void);

void refr_hide_sysmon(lv_display_t * disp);

/*Refresh the display under test without touching the other displays*/
void refr_frame(void);

/*The default callbacks of the display under test*/
void refr_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
void refr_flush_wait_cb(lv_display_t * disp);
void refr_sync_cb(lv_display_t * disp, const lv_area_t * area);
void refr_sync_wait_cb(lv_display_t * disp);

/*Read a pixel of a buffer holding an image in the display's color format*/
uint32_t refr_px_read(const uint8_t * buf, uint32_t stride, lv_color_format_t cf, int32_t x, int32_t y);

/*Pixel of the assembled screen image*/
uint32_t refr_screen_px(int32_t x, int32_t y);

/*Pixel of one of the display's draw buffers*/
uint32_t refr_buf_px(const lv_draw_buf_t * buf, int32_t x, int32_t y);

/*Subscribe to every display event that the refreshing sends*/
void refr_log_events(lv_display_t * disp);

bool refr_event_is_seen(lv_event_code_t code);
uint32_t refr_event_get_count(lv_event_code_t code);
int32_t refr_event_get_index(lv_event_code_t code);

/*Forget the flushed areas, the synced areas and the logged events*/
void refr_log_reset(void);

/**
 * The number of rows that fit into the draw buffer in partial mode.
 * The draw buffers are padded to LV_DRAW_BUF_ALIGN, so this can not be derived
 * from the requested buffer height.
 */
uint32_t refr_max_row_of(int32_t area_w, int32_t area_h);

/*The flushed chunks have to tile the given area from top to bottom, without gaps,
 *and only the last one may be marked as the last part*/
void refr_check_flushed_chunks(const lv_area_t * area, uint32_t max_row);

lv_obj_t * refr_screen(void);

/*A screen sized, opaque, square cornered background so that the pixels are predictable*/
void refr_screen_set_color(uint32_t argb);

/*An opaque, square cornered rectangle*/
lv_obj_t * refr_rect_create(lv_obj_t * parent, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t argb);

/*Counting how many times a widget was drawn. The user data is an `uint32_t *`*/
void refr_draw_main_counter_cb(lv_event_t * e);

/**
 * The height the invalidated areas are rounded up to by `refr_row_rounding_cb()`.
 * `refr_ctx_reset()` restores `REFR_ROW_ROUNDING_DEFAULT`.
 * @param rows  the height the areas are rounded up to
 */
void refr_set_row_rounding(int32_t rows);

/*Round the height of the invalidated areas up to a multiple of the row rounding,
 *like the drivers of the displays that can only be updated in blocks*/
void refr_row_rounding_cb(lv_event_t * e);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_REFR_H*/

#endif /*LV_BUILD_TEST*/
