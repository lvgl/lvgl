/**
 * @file lv_test_helpers.h
 *
 */

#ifndef LV_TEST_HELPERS_H
#define LV_TEST_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../config/lv_conf_internal.h"
#if LV_USE_TEST

#include "../../lv_types.h"
#include "../../stdlib/lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Emulate a delay. It's not real delay, but it tricks LVGL to think that the
 * required time has been elapsed.
 * `lv_timer_handler` is called after each millisecond, meaning all the events
 * will be fired inside this function.
 * At the end the animations and display will be also updated.
 * @param ms    the number of milliseconds to pass
 */
void lv_test_wait(uint32_t ms);

/**
 * Emulates some time passing.
 * Update the animations and the display only once at the end.
 * @param ms    the number of milliseconds to pass
 */
void lv_test_fast_forward(uint32_t ms);

/**
 * Heap usage sample to compare against a later one. See `lv_test_get_mem_usage()`.
 */
typedef struct {
    size_t used_size;   /**< Allocated bytes */
    size_t used_cnt;    /**< Number of allocated blocks */
} lv_test_mem_usage_t;

#if LV_USE_STDLIB_MALLOC != LV_STDLIB_BUILTIN
/* Skip checking heap as we don't have the info available */
#define LV_HEAP_CHECK(x) do {} while(0)
/* Pick a non-zero value */
#define lv_test_get_free_mem() (65536)
/* Constant values, so that comparing two samples never reports a leak */
#define lv_test_get_used_mem() (0)
#define lv_test_get_mem_usage() ((lv_test_mem_usage_t) {0})
#else
#define LV_HEAP_CHECK(x) x

static inline size_t lv_test_get_free_mem(void)
{
    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);
    return m1.free_size;
}

/**
 * Get the number of allocated bytes.
 * Prefer this over `lv_test_get_free_mem()` for leak checks: the free size also
 * shrinks when the heap gets fragmented, as every new block consumes a header.
 */
static inline size_t lv_test_get_used_mem(void)
{
    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);
    return m1.cur_used;
}

/**
 * Take a heap usage sample for `TEST_ASSERT_MEM_LEAK_LESS_THAN()`.
 * The block count is exact, while the allocated size can differ by a few bytes
 * between two identical sequences of allocations: the allocator leaves the
 * remainder in the block when it is too small to be split off.
 */
static inline lv_test_mem_usage_t lv_test_get_mem_usage(void)
{
    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);
    lv_test_mem_usage_t usage = { .used_size = m1.cur_used, .used_cnt = m1.used_cnt };
    return usage;
}
#endif /* LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN */

/**********************
 *      MACROS
 **********************/

#define LV_TEST_WIDTH_TO_STRIDE(w, px_size) ((((w) * (px_size) + (LV_DRAW_BUF_STRIDE_ALIGN - 1)) / LV_DRAW_BUF_STRIDE_ALIGN) * LV_DRAW_BUF_STRIDE_ALIGN)

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_HELPERS_H*/
