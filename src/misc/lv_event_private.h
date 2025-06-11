/**
 * @file lv_event_private.h
 *
 */

#ifndef LV_EVENT_PRIVATE_H
#define LV_EVENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_event.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_event_dsc_t {
    lv_event_cb_t cb;
    void * user_data;
    uint32_t filter;
};

struct _lv_event_t {
    void * current_target;
    void * original_target;
    lv_event_code_t code;
    void * user_data;
    void * param;
    lv_event_t * prev;
    uint8_t deleted : 1;
    uint8_t stop_processing : 1;
    uint8_t stop_bubbling : 1;
    uint8_t stop_trickling : 1;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_event_push(lv_event_t * e);

void lv_event_pop(lv_event_t * e);

/**
 * Nested events can be called and one of them might belong to an object that is being deleted.
 * Mark this object's `event_temp_data` deleted to know that its `lv_obj_send_event` should return `LV_RESULT_INVALID`
 * @param target     pointer to an event target which was deleted
 */
void lv_event_mark_deleted(void * target);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_EVENT_PRIVATE_H*/
