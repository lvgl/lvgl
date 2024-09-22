/**
 * @file lv_libinput_private.h
 *
 */

#ifndef LV_LIBINPUT_PRIVATE_H
#define LV_LIBINPUT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_libinput.h"

#if LV_USE_LIBINPUT

#if LV_LIBINPUT_XKB
#include "lv_xkb_private.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_libinput_event_t {
    lv_indev_state_t pressed;
    int key_val;
    lv_point_t point;
};

struct lv_libinput_t {
    int fd;
    struct pollfd fds[1];

    /* The points array is implemented as a circular LIFO queue */
    lv_libinput_event_t points[LV_LIBINPUT_MAX_EVENTS]; /* Event buffer */
    lv_libinput_event_t slots[2]; /* Realtime state of up to 2 fingers to handle multitouch */

    /* Pointer devices work a bit differently in libinput which requires us to store their last known state */
    lv_point_t pointer_position;
    bool pointer_button_down;

    int start; /* Index of start of event queue */
    int end; /* Index of end of queue*/
    lv_libinput_event_t last_event; /* Report when no new events
                                   * to keep indev state consistent
                                   */
    bool deinit; /* Tell worker thread to quit */
    pthread_mutex_t event_lock;
    pthread_t worker_thread;

    struct libinput * libinput_context;
    struct libinput_device * libinput_device;

#if LV_LIBINPUT_XKB
    lv_xkb_t xkb;
#endif /* LV_LIBINPUT_XKB */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LIBINPUT */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LIBINPUT_PRIVATE_H*/
