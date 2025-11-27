/**
 * @file test_flex_size_content_perf.c
 *
 * Performance tests for SIZE_CONTENT propagation in nested flex containers.
 * These tests measure layout performance to ensure the ancestor propagation
 * patch doesn't introduce significant performance regression.
 */

#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <time.h>

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

static void simple_style(lv_obj_t * obj)
{
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN);
}

/**
 * Helper to get current time in microseconds
 */
static uint64_t get_time_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

/**
 * Test: Measure layout time for simple flex container
 * This establishes a baseline for comparison.
 */
void test_perf_simple_flex(void)
{
    const int iterations = 1000;
    uint64_t start, end;

    /* Warm up */
    for(int i = 0; i < 10; i++) {
        lv_obj_t * parent = lv_obj_create(active_screen);
        lv_obj_set_size(parent, 300, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
        simple_style(parent);

        lv_obj_t * child = lv_obj_create(parent);
        lv_obj_set_size(child, 60, 80);
        simple_style(child);

        lv_obj_update_layout(active_screen);
        lv_obj_delete(parent);
    }

    /* Timed run */
    start = get_time_us();
    for(int i = 0; i < iterations; i++) {
        lv_obj_t * parent = lv_obj_create(active_screen);
        lv_obj_set_size(parent, 300, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
        simple_style(parent);

        lv_obj_t * child = lv_obj_create(parent);
        lv_obj_set_size(child, 60, 80);
        simple_style(child);

        lv_obj_update_layout(active_screen);
        lv_obj_delete(parent);
    }
    end = get_time_us();

    uint64_t total_us = end - start;
    uint64_t per_iter_us = total_us / iterations;

    LV_LOG_USER("Simple flex layout: %llu us total, %llu us per iteration", total_us, per_iter_us);

    /* Should complete 1000 iterations in under 1 second */
    TEST_ASSERT_LESS_THAN_UINT64(1000000, total_us);
}

/**
 * Test: Measure layout time for nested SIZE_CONTENT containers
 * This tests the performance impact of ancestor propagation.
 */
void test_perf_nested_size_content(void)
{
    const int iterations = 1000;
    uint64_t start, end;

    /* Warm up */
    for(int i = 0; i < 10; i++) {
        lv_obj_t * outer = lv_obj_create(active_screen);
        lv_obj_set_size(outer, 300, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(outer, LV_FLEX_FLOW_COLUMN);
        simple_style(outer);

        lv_obj_t * inner = lv_obj_create(outer);
        lv_obj_set_size(inner, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(inner, LV_FLEX_FLOW_ROW);
        simple_style(inner);

        lv_obj_t * child = lv_obj_create(inner);
        lv_obj_set_size(child, 60, 80);
        simple_style(child);

        lv_obj_update_layout(active_screen);
        lv_obj_delete(outer);
    }

    /* Timed run */
    start = get_time_us();
    for(int i = 0; i < iterations; i++) {
        lv_obj_t * outer = lv_obj_create(active_screen);
        lv_obj_set_size(outer, 300, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(outer, LV_FLEX_FLOW_COLUMN);
        simple_style(outer);

        lv_obj_t * inner = lv_obj_create(outer);
        lv_obj_set_size(inner, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(inner, LV_FLEX_FLOW_ROW);
        simple_style(inner);

        lv_obj_t * child = lv_obj_create(inner);
        lv_obj_set_size(child, 60, 80);
        simple_style(child);

        lv_obj_update_layout(active_screen);
        lv_obj_delete(outer);
    }
    end = get_time_us();

    uint64_t total_us = end - start;
    uint64_t per_iter_us = total_us / iterations;

    LV_LOG_USER("Nested SIZE_CONTENT layout: %llu us total, %llu us per iteration", total_us, per_iter_us);

    /* Should complete 1000 iterations in under 2 seconds (allowing some overhead) */
    TEST_ASSERT_LESS_THAN_UINT64(2000000, total_us);
}

/**
 * Test: Measure layout time for deeply nested SIZE_CONTENT (5 levels)
 * This stress-tests the ancestor propagation.
 */
void test_perf_deep_nested_size_content(void)
{
    const int iterations = 500;
    const int depth = 5;
    uint64_t start, end;

    /* Warm up */
    for(int i = 0; i < 5; i++) {
        lv_obj_t * containers[depth + 1];

        containers[0] = lv_obj_create(active_screen);
        lv_obj_set_size(containers[0], 400, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(containers[0], LV_FLEX_FLOW_COLUMN);
        simple_style(containers[0]);

        for(int d = 1; d <= depth; d++) {
            containers[d] = lv_obj_create(containers[d - 1]);
            lv_obj_set_size(containers[d], LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_flex_flow(containers[d], d % 2 == 0 ? LV_FLEX_FLOW_COLUMN : LV_FLEX_FLOW_ROW);
            simple_style(containers[d]);
        }

        /* Leaf child with fixed size */
        lv_obj_t * leaf = lv_obj_create(containers[depth]);
        lv_obj_set_size(leaf, 50, 100);
        simple_style(leaf);

        lv_obj_update_layout(active_screen);
        lv_obj_delete(containers[0]);
    }

    /* Timed run */
    start = get_time_us();
    for(int i = 0; i < iterations; i++) {
        lv_obj_t * containers[depth + 1];

        containers[0] = lv_obj_create(active_screen);
        lv_obj_set_size(containers[0], 400, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(containers[0], LV_FLEX_FLOW_COLUMN);
        simple_style(containers[0]);

        for(int d = 1; d <= depth; d++) {
            containers[d] = lv_obj_create(containers[d - 1]);
            lv_obj_set_size(containers[d], LV_PCT(100), LV_SIZE_CONTENT);
            lv_obj_set_flex_flow(containers[d], d % 2 == 0 ? LV_FLEX_FLOW_COLUMN : LV_FLEX_FLOW_ROW);
            simple_style(containers[d]);
        }

        /* Leaf child with fixed size */
        lv_obj_t * leaf = lv_obj_create(containers[depth]);
        lv_obj_set_size(leaf, 50, 100);
        simple_style(leaf);

        lv_obj_update_layout(active_screen);
        lv_obj_delete(containers[0]);
    }
    end = get_time_us();

    uint64_t total_us = end - start;
    uint64_t per_iter_us = total_us / iterations;

    LV_LOG_USER("Deep nested (5 levels) SIZE_CONTENT layout: %llu us total, %llu us per iteration",
                total_us, per_iter_us);

    /* Should complete 500 iterations in under 3 seconds */
    TEST_ASSERT_LESS_THAN_UINT64(3000000, total_us);
}

/**
 * Test: Measure layout time for many siblings with SIZE_CONTENT parent
 * Tests horizontal scaling with many children.
 */
void test_perf_many_siblings_size_content(void)
{
    const int iterations = 100;
    const int num_children = 20;
    uint64_t start, end;

    /* Warm up */
    for(int i = 0; i < 5; i++) {
        lv_obj_t * parent = lv_obj_create(active_screen);
        lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
        simple_style(parent);

        for(int c = 0; c < num_children; c++) {
            lv_obj_t * child = lv_obj_create(parent);
            lv_obj_set_size(child, 40, 30 + (c % 5) * 10);
            simple_style(child);
        }

        lv_obj_update_layout(active_screen);
        lv_obj_delete(parent);
    }

    /* Timed run */
    start = get_time_us();
    for(int i = 0; i < iterations; i++) {
        lv_obj_t * parent = lv_obj_create(active_screen);
        lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
        simple_style(parent);

        for(int c = 0; c < num_children; c++) {
            lv_obj_t * child = lv_obj_create(parent);
            lv_obj_set_size(child, 40, 30 + (c % 5) * 10);
            simple_style(child);
        }

        lv_obj_update_layout(active_screen);
        lv_obj_delete(parent);
    }
    end = get_time_us();

    uint64_t total_us = end - start;
    uint64_t per_iter_us = total_us / iterations;

    LV_LOG_USER("Many siblings (20 children) SIZE_CONTENT layout: %llu us total, %llu us per iteration",
                total_us, per_iter_us);

    /* Should complete 100 iterations in under 2 seconds */
    TEST_ASSERT_LESS_THAN_UINT64(2000000, total_us);
}

#endif
