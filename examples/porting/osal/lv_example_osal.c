/**
 * @file lv_example_osal.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_examples.h"

#if LV_BUILD_EXAMPLES
#include "../../../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void counter_button_event_cb(lv_event_t * e);
static void increment_thread_entry(void * user_data);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_thread_sync_t press_sync;
static lv_thread_t increment_thread;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @title Thread-safe counter with OSAL sync
 * @brief Count button clicks from a worker thread using `lv_thread_sync_t`.
 *
 * A button is aligned near the center of the active screen and its
 * `LV_EVENT_CLICKED` callback signals an `lv_thread_sync_t`. A worker
 * thread created with `lv_thread_init` at `LV_THREAD_PRIO_MID` creates a
 * counter label under `lv_lock` / `lv_unlock`, then loops on
 * `lv_thread_sync_wait`, incrementing a press counter and updating the
 * label text on each signal. The locking pairs keep label updates safe
 * across the UI and worker threads.
 */
void lv_example_osal(void)
{
    lv_obj_t * counter_button;

    counter_button = lv_button_create(lv_screen_active());
    lv_obj_align(counter_button, LV_ALIGN_CENTER, 0, -15);
    lv_obj_add_event_cb(counter_button, counter_button_event_cb, LV_EVENT_CLICKED, NULL);

    if(lv_thread_sync_init(&press_sync) != LV_RESULT_OK) {
        LV_LOG_ERROR("Error initializing thread sync");
    }

    if(lv_thread_init(&increment_thread, "inc_th", LV_THREAD_PRIO_MID, increment_thread_entry, 2048,
                      NULL) != LV_RESULT_OK) {
        LV_LOG_ERROR("Error initializing thread");
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void counter_button_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    if(lv_thread_sync_signal(&press_sync) != LV_RESULT_OK) {
        LV_LOG_ERROR("Error signaling thread sync");
    }
}

static void increment_thread_entry(void * user_data)
{
    LV_UNUSED(user_data);
    lv_obj_t * counter_label;
    uint32_t press_count = 0;

    lv_lock();
    counter_label = lv_label_create(lv_screen_active());
    lv_obj_align(counter_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text_fmt(counter_label, "Pressed %" LV_PRIu32 " times", press_count);
    lv_unlock();

    while(true) {
        if(lv_thread_sync_wait(&press_sync) != LV_RESULT_OK) {
            LV_LOG_ERROR("Error awaiting thread sync");
        }
        press_count += 1;

        lv_lock();
        lv_label_set_text_fmt(counter_label, "Pressed %" LV_PRIu32 " times", press_count);
        lv_unlock();
    }
}


#endif
