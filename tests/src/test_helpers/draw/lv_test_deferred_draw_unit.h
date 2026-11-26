/**
* @file lv_test_deferred_draw_unit.h
*
* A fake main draw unit that queues its fills instead of drawing them, like a GPU
* unit that builds a command list. A queued fill is only "done" after the unit's
* `wait_for_finish_cb` ran, so the tests can check that LVGL waits before it reads
* or flushes a buffer. Every other draw type goes to the software fallback.
*/
#if LV_BUILD_TEST

#ifndef LV_TEST_DEFERRED_DRAW_UNIT_H
#define LV_TEST_DEFERRED_DRAW_UNIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <lvgl/lvgl.h>
#include <lvgl_private/lvgl_private.h>

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register the queuing unit as the main draw unit, with the software unit as fallback.
 * Resets the counters.
 */
void deferred_draw_unit_create(void);

/**
 * Unregister and free the queuing unit. Safe to call when it wasn't created.
 */
void deferred_draw_unit_delete(void);

/**
 * @return  true if a fill was queued after the last wait
 */
bool deferred_draw_unit_has_pending(void);

/**
 * @return  the number of fills the unit took
 */
uint32_t deferred_draw_unit_get_fill_cnt(void);

/**
 * @return  the number of times the unit was waited for
 */
uint32_t deferred_draw_unit_get_wait_cnt(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_DEFERRED_DRAW_UNIT_H*/

#endif /*LV_BUILD_TEST*/
