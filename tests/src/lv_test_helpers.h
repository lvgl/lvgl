#ifndef LV_TEST_HELPERS_H
#define LV_TEST_HELPERS_H

#include "lv_test_conf.h"
#include "../lvgl.h"

#ifdef LVGL_CI_USING_SYS_HEAP
/* Skip checking heap as we don't have the info available */
#define LV_HEAP_CHECK(x) do {} while(0)
/* Pick a non-zero value */
#define lv_test_get_free_mem() (65536)
#else
#define LV_HEAP_CHECK(x) x

static inline uint32_t lv_test_get_free_mem(void)
{
    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);
    return m1.free_size;
}
#endif /* LVGL_CI_USING_SYS_HEAP */

#define CANVAS_WIDTH_TO_STRIDE(w, px_size) ((((w) * (px_size) + (LV_DRAW_BUF_STRIDE_ALIGN - 1)) / LV_DRAW_BUF_STRIDE_ALIGN) * LV_DRAW_BUF_STRIDE_ALIGN)

void lv_test_wait(uint32_t ms);

#endif /*LV_TEST_HELPERS_H*/
