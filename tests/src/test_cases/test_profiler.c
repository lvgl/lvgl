#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#define OUTPUT_LINE_MAX 8
#define OUTPUT_BUF_MAX 128

static uint32_t profiler_tick = 0;
static int output_line = 0;
static char output_buf[OUTPUT_LINE_MAX][OUTPUT_BUF_MAX];

static uint32_t get_tick_cb(void)
{
    return profiler_tick++;
}

static void flush_cb(const char * buf)
{
    TEST_ASSERT_LESS_THAN(OUTPUT_LINE_MAX, output_line);
    TEST_ASSERT_LESS_THAN(OUTPUT_BUF_MAX, strlen(buf));

    lv_strcpy(output_buf[output_line], buf);
    output_line++;
}

void setUp(void)
{
    lv_profiler_builtin_config_t config;
    lv_profiler_builtin_config_init(&config);
    config.buf_size = 1024;
    config.tick_per_sec = 1; /* One second is equal to 1000000 microseconds */
    config.tick_get_cb = get_tick_cb;
    config.flush_cb = flush_cb;
    lv_profiler_builtin_init(&config);
}

void tearDown(void)
{
    lv_profiler_builtin_uninit();
}

void test_profiler_normal(void)
{
    /* enable profier */
    lv_profiler_builtin_set_enable(true);

    /* reset */
    profiler_tick = 0;
    output_line = 0;
    lv_memzero(output_buf, sizeof(output_buf));

    /* test profiler */
    LV_PROFILER_BEGIN;
    LV_PROFILER_END;
    LV_PROFILER_BEGIN_TAG("custom_tag");
    LV_PROFILER_END_TAG("custom_tag");

    /* flush output */
    lv_profiler_builtin_flush();

    /* check output */
    TEST_ASSERT_EQUAL_INT(output_line, 4);
    TEST_ASSERT_EQUAL_INT(profiler_tick, 4);
    TEST_ASSERT_EQUAL_STRING(output_buf[0], "   LVGL-1 [0] 0.000000: tracing_mark_write: B|1|test_profiler_normal\n");
    TEST_ASSERT_EQUAL_STRING(output_buf[1], "   LVGL-1 [0] 1.000000: tracing_mark_write: E|1|test_profiler_normal\n");
    TEST_ASSERT_EQUAL_STRING(output_buf[2], "   LVGL-1 [0] 2.000000: tracing_mark_write: B|1|custom_tag\n");
    TEST_ASSERT_EQUAL_STRING(output_buf[3], "   LVGL-1 [0] 3.000000: tracing_mark_write: E|1|custom_tag\n");
}

void test_profiler_disable(void)
{
    /* disable profier */
    lv_profiler_builtin_set_enable(false);

    /* reset */
    profiler_tick = 0;
    output_line = 0;
    lv_memzero(output_buf, sizeof(output_buf));

    /* test profiler */
    LV_PROFILER_BEGIN;
    LV_PROFILER_END;
    LV_PROFILER_BEGIN_TAG("custom_tag");
    LV_PROFILER_END_TAG("custom_tag");

    /* flush output */
    lv_profiler_builtin_flush();

    /* check output */
    TEST_ASSERT_EQUAL_INT(output_line, 0);
    TEST_ASSERT_EQUAL_INT(profiler_tick, 0);
    TEST_ASSERT_EQUAL_CHAR(output_buf[1][0], '\0');
    TEST_ASSERT_EQUAL_CHAR(output_buf[2][0], '\0');
    TEST_ASSERT_EQUAL_CHAR(output_buf[3][0], '\0');
    TEST_ASSERT_EQUAL_CHAR(output_buf[4][0], '\0');
}

#endif
