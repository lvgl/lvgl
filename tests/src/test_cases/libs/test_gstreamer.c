#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_GSTREAMER

#include <unistd.h>

/* Generated with:
 * gst-launch-1.0 videotestsrc num-buffers=15 pattern=solid-color \
 *     foreground-color=0xff20a040 ! video/x-raw,width=64,height=64,framerate=15/1 \
 *   ! theoraenc quality=63 keyframe-force=1 keyframe-freq=1 ! oggmux \
 *   ! filesink location=src/test_assets/test_video_solid.ogv
 */
#define TEST_VIDEO_PATH     "src/test_assets/test_video_solid.ogv"
#define TEST_VIDEO_SIZE     64
#define TEST_VIDEO_DURATION 1000
#define TEST_VIDEO_R        0x20
#define TEST_VIDEO_G        0xa0
#define TEST_VIDEO_B        0x40
#define TEST_VIDEO_COLOR_TOLERANCE 8

static uint32_t event_cnt[LV_GSTREAMER_STREAM_STATE_END + 1];
static lv_color_format_t original_color_format;

void setUp(void)
{
    lv_memzero(event_cnt, sizeof(event_cnt));
    original_color_format = lv_display_get_color_format(lv_display_get_default());
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    lv_display_set_color_format(lv_display_get_default(), original_color_format);
}

static void stream_event_cb(lv_event_t * e)
{
    lv_gstreamer_stream_state_t state = lv_gstreamer_get_stream_state(e);
    TEST_ASSERT(state >= LV_GSTREAMER_STREAM_STATE_START && state <= LV_GSTREAMER_STREAM_STATE_END);
    event_cnt[state]++;
}

/* GStreamer decodes on the real clock in its own threads, so unlike
 * lv_test_wait() these have to burn real time while LVGL's simulated tick and
 * its timers keep running. */
static void pump(uint32_t ms)
{
    while(ms--) {
        usleep(1000);
        lv_tick_inc(1);
        lv_timer_handler();
    }
    lv_refr_now(NULL);
}

static bool pump_until(const uint32_t * counter, uint32_t target, uint32_t timeout_ms)
{
    for(uint32_t i = 0; i < timeout_ms; i++) {
        if(*counter >= target) break;
        usleep(1000);
        lv_tick_inc(1);
        lv_timer_handler();
    }
    lv_refr_now(NULL);
    return *counter >= target;
}

static void assert_frame_is_video_color(const lv_image_dsc_t * frame)
{
    const uint8_t * px = frame->data + frame->header.stride * (TEST_VIDEO_SIZE / 2);

    switch(frame->header.cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            px += (TEST_VIDEO_SIZE / 2) * 4;
            TEST_ASSERT_INT_WITHIN(TEST_VIDEO_COLOR_TOLERANCE, TEST_VIDEO_B, px[0]);
            TEST_ASSERT_INT_WITHIN(TEST_VIDEO_COLOR_TOLERANCE, TEST_VIDEO_G, px[1]);
            TEST_ASSERT_INT_WITHIN(TEST_VIDEO_COLOR_TOLERANCE, TEST_VIDEO_R, px[2]);
            break;
        case LV_COLOR_FORMAT_RGB565: {
                px += (TEST_VIDEO_SIZE / 2) * 2;
                uint16_t color;
                lv_memcpy(&color, px, sizeof(color));
                TEST_ASSERT_INT_WITHIN(1, TEST_VIDEO_R >> 3, (color >> 11) & 0x1f);
                TEST_ASSERT_INT_WITHIN(1, TEST_VIDEO_G >> 2, (color >> 5) & 0x3f);
                TEST_ASSERT_INT_WITHIN(1, TEST_VIDEO_B >> 3, color & 0x1f);
                break;
            }
        default:
            TEST_FAIL_MESSAGE("frame decoded into an unexpected color format");
    }
}

static lv_obj_t * create_player(void)
{
    lv_obj_t * player = lv_gstreamer_create(lv_screen_active());
    lv_obj_center(player);
    lv_obj_add_event_cb(player, stream_event_cb, LV_EVENT_STATE_CHANGED, NULL);
    return player;
}

void test_gstreamer_without_source(void)
{
    lv_obj_t * player = create_player();

    TEST_ASSERT_EQUAL(LV_GSTREAMER_STATE_NULL, lv_gstreamer_get_state(player));
    TEST_ASSERT_EQUAL(0, lv_gstreamer_get_duration(player));
    TEST_ASSERT_EQUAL(0, lv_gstreamer_get_position(player));
    TEST_ASSERT_EQUAL(0, lv_gstreamer_get_volume(player));

    /* None of these may crash without a pipeline */
    lv_gstreamer_play(player);
    lv_gstreamer_pause(player);
    lv_gstreamer_stop(player);
    lv_gstreamer_set_position(player, 100);
    lv_gstreamer_set_rate(player, 512);
    lv_gstreamer_set_volume(player, 50);
    pump(50);

    TEST_ASSERT_EQUAL(LV_GSTREAMER_STATE_NULL, lv_gstreamer_get_state(player));
    TEST_ASSERT_NULL(lv_image_get_src(player));
}

void test_gstreamer_invalid_source(void)
{
    lv_obj_t * player = create_player();

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gstreamer_set_src(player, "no_such_factory", NULL, NULL));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gstreamer_set_src(player, LV_GSTREAMER_FACTORY_TEST_VIDEO, NULL, NULL));
    /* The source cannot be replaced once set */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_gstreamer_set_src(player, LV_GSTREAMER_FACTORY_TEST_VIDEO, NULL, NULL));
}

void test_gstreamer_stream_state_of_invalid_event(void)
{
    TEST_ASSERT_EQUAL(LV_GSTREAMER_STREAM_STATE_INVALID, lv_gstreamer_get_stream_state(NULL));
}

void test_gstreamer_missing_file(void)
{
    lv_obj_t * player = create_player();
    TEST_ASSERT_EQUAL(LV_RESULT_OK,
                      lv_gstreamer_set_src(player, LV_GSTREAMER_FACTORY_FILE, LV_GSTREAMER_PROPERTY_FILE, "no_such_file.ogv"));

    lv_gstreamer_play(player);
    pump(200);

    /* The pipeline reports the error on its bus, no frame is ever produced */
    TEST_ASSERT_EQUAL(0, event_cnt[LV_GSTREAMER_STREAM_STATE_START]);
    TEST_ASSERT_NULL(lv_image_get_src(player));
}

void test_gstreamer_videotestsrc(void)
{
    lv_obj_t * player = create_player();
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gstreamer_set_src(player, LV_GSTREAMER_FACTORY_TEST_VIDEO, NULL, NULL));

    lv_gstreamer_play(player);
    TEST_ASSERT_TRUE(pump_until(&event_cnt[LV_GSTREAMER_STREAM_STATE_START], 1, 3000));

    TEST_ASSERT_EQUAL(LV_GSTREAMER_STATE_PLAYING, lv_gstreamer_get_state(player));

    /* The widget adopts the size of the video */
    TEST_ASSERT_EQUAL(320, lv_obj_get_width(player));
    TEST_ASSERT_EQUAL(240, lv_obj_get_height(player));

    const lv_image_dsc_t * frame = lv_image_get_src(player);
    TEST_ASSERT_NOT_NULL(frame);
    TEST_ASSERT_EQUAL(320, frame->header.w);
    TEST_ASSERT_EQUAL(240, frame->header.h);

    lv_gstreamer_stop(player);
    pump(50);
    TEST_ASSERT_EQUAL(LV_GSTREAMER_STATE_READY, lv_gstreamer_get_state(player));
    TEST_ASSERT_EQUAL(1, event_cnt[LV_GSTREAMER_STREAM_STATE_STOP]);
}

void test_gstreamer_file_playback(void)
{
    lv_obj_t * player = create_player();
    TEST_ASSERT_EQUAL(LV_RESULT_OK,
                      lv_gstreamer_set_src(player, LV_GSTREAMER_FACTORY_FILE, LV_GSTREAMER_PROPERTY_FILE, TEST_VIDEO_PATH));

    lv_gstreamer_play(player);
    TEST_ASSERT_TRUE(pump_until(&event_cnt[LV_GSTREAMER_STREAM_STATE_START], 1, 3000));

    TEST_ASSERT_EQUAL(TEST_VIDEO_SIZE, lv_obj_get_width(player));
    TEST_ASSERT_EQUAL(TEST_VIDEO_SIZE, lv_obj_get_height(player));
    TEST_ASSERT_EQUAL(TEST_VIDEO_DURATION, lv_gstreamer_get_duration(player));

    const lv_image_dsc_t * frame = lv_image_get_src(player);
    TEST_ASSERT_NOT_NULL(frame);
    TEST_ASSERT_EQUAL(lv_display_get_color_format(lv_display_get_default()), frame->header.cf);
    assert_frame_is_video_color(frame);

    lv_gstreamer_pause(player);
    uint32_t paused_at = lv_gstreamer_get_position(player);
    pump(200);
    TEST_ASSERT_EQUAL(LV_GSTREAMER_STATE_PAUSED, lv_gstreamer_get_state(player));
    TEST_ASSERT_EQUAL(paused_at, lv_gstreamer_get_position(player));

    lv_gstreamer_set_position(player, 500);
    /* The flushing seek is asynchronous, the position is only up to date once
     * the pipeline has prerolled the frame it seeked to */
    pump(200);
    TEST_ASSERT_INT_WITHIN(100, 500, lv_gstreamer_get_position(player));

    lv_gstreamer_play(player);
    TEST_ASSERT_TRUE(pump_until(&event_cnt[LV_GSTREAMER_STREAM_STATE_END], 1, 3000));
}

void test_gstreamer_follows_display_color_format(void)
{
#if LV_USE_DRAW_NANOVG
    TEST_IGNORE_MESSAGE("The NanoVG headless test display only supports XRGB8888/ARGB8888");
#else
    /* Restored by tearDown() */
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_RGB565);

    lv_obj_t * player = create_player();
    TEST_ASSERT_EQUAL(LV_RESULT_OK,
                      lv_gstreamer_set_src(player, LV_GSTREAMER_FACTORY_FILE, LV_GSTREAMER_PROPERTY_FILE, TEST_VIDEO_PATH));

    lv_gstreamer_play(player);
    TEST_ASSERT_TRUE(pump_until(&event_cnt[LV_GSTREAMER_STREAM_STATE_START], 1, 3000));

    const lv_image_dsc_t * frame = lv_image_get_src(player);
    TEST_ASSERT_NOT_NULL(frame);
    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_RGB565, frame->header.cf);
    TEST_ASSERT_EQUAL(lv_draw_buf_width_to_stride(TEST_VIDEO_SIZE, LV_COLOR_FORMAT_RGB565), frame->header.stride);
    /* The pipeline really produced RGB16, the frame is not just labelled as such */
    assert_frame_is_video_color(frame);
#endif
}

void test_gstreamer_delete_while_playing(void)
{
    size_t mem_before = lv_test_get_free_mem();

    for(uint32_t i = 0; i < 5; i++) {
        lv_obj_t * player = create_player();
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_gstreamer_set_src(player, LV_GSTREAMER_FACTORY_TEST_VIDEO, NULL, NULL));
        lv_gstreamer_play(player);
        TEST_ASSERT_TRUE(pump_until(&event_cnt[LV_GSTREAMER_STREAM_STATE_START], i + 1, 3000));
        lv_obj_delete(player);
        pump(20);
    }

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 64);
}

#else

void setUp(void) { }
void tearDown(void) { }
void test_gstreamer_without_source(void) { }
void test_gstreamer_invalid_source(void) { }
void test_gstreamer_stream_state_of_invalid_event(void) { }
void test_gstreamer_missing_file(void) { }
void test_gstreamer_videotestsrc(void) { }
void test_gstreamer_file_playback(void) { }
void test_gstreamer_follows_display_color_format(void) { }
void test_gstreamer_delete_while_playing(void) { }

#endif /* LV_USE_GSTREAMER */
#endif /* LV_BUILD_TEST */
