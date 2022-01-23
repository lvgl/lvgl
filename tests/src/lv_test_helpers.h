#ifndef LV_TEST_HELPERS_H
#define LV_TEST_HELPERS_H

#ifdef LVGL_CI_USING_SYS_HEAP
/* Skip checking heap as we don't have the info available */
#define LV_HEAP_CHECK(x) do {} while(0)
#else
#define LV_HEAP_CHECK(x) x
#endif /* LVGL_CI_USING_SYS_HEAP */

#endif /*LV_TEST_HELPERS_H*/


