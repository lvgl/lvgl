/**
 * @file lv_observable.h
 *
 */

#ifndef LV_OBSERVABLE_H
#define LV_OBSERVABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

struct _lv_observer_t;

typedef enum {
    LV_SUBJECT_TYPE_NONE =      0,
    LV_SUBJECT_TYPE_INT =       1,   /**< an int32_t*/
    LV_SUBJECT_TYPE_POINTER =   2,   /**< a void pointer*/
    LV_SUBJECT_TYPE_COLOR   =   3,   /**< an lv_color_t*/
    LV_SUBJECT_TYPE_GROUP  =    4,   /**< an array of subjects*/
    LV_SUBJECT_TYPE_STRING  =   5,   /**< a char pointer*/
} lv_subject_type_t;


/**
 * A common type to handle all the various observable types in the same way
 */
typedef union {
    int32_t num; /**< Integer number (opacity, enums, booleans or "normal" numbers)*/
    const void * ptr; /**< Constant pointer  (string buffer, format string, font, cone text, etc)*/
    lv_color_t color; /**< Color */
} lv_subject_value_t;

/**
 * The subject (an observable value)
 */
typedef struct {
    lv_ll_t subs_ll;                /**< Subscribers*/
    uint32_t type   : 4;
    uint32_t size   : 28;           /**< Might be used to store a size related to `type`*/
    lv_subject_value_t value;       /**< Actual value*/
} lv_subject_t;

/**
  * Callback called when the observed value changes
  * @param s the lv_observer_t object created when the object was subscribed to the value
  */
typedef void (*lv_observer_cb_t)(lv_subject_t * subject, struct _lv_observer_t * observer);

/**
 * The observer object: a descriptor returned when subscribing LVGL widgets to subjects
 */
typedef struct _lv_observer_t {
    lv_subject_t * subject;             /**< The observed value */
    lv_observer_cb_t cb;                /**< Callback that should be called when the value changes*/
    lv_obj_t * target;                  /**< LVGL widget which has subscribed for the value*/
    void * user_data;                   /**< Additional parameter supplied when subscribing*/
    uint32_t auto_free_user_data : 1;    /**< Automatically free user data when the observer is removed */
} lv_observer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an int type subject
 * @param subject address of the subject
 * @param value initial value
 */
void lv_subject_init_int(lv_subject_t * subject, int32_t value);

/**
 * Initialize a pointer type subject
 * @param subject address of the subject
 * @param value initial value
 */
void lv_subject_init_ptr(lv_subject_t * subject, void * value);

/**
 * Initialize a string type subject
 * @param subject address of the subject
 * @param buf pointer to a buffer containing the string value
 * @param size size of the buffer
 */
void lv_subject_init_string(lv_subject_t * subject, char * buf, size_t size);

/**
 * Initialize a color type subject
 * @param subject address of the subject
 * @param color initial value
 */
void lv_subject_init_color(lv_subject_t * subject, lv_color_t color);


lv_observer_t * lv_subject_add_observer(lv_subject_t * subject, lv_observer_cb_t cb, void * user_data);

/**
 * Subscribe an LVGL widget to a subject
 * @param subject address of the subject
 * @param cb callback to call when the value of the subject changes
 * @param obj the LVGL object what will be affected by the change
 * @param data1 optional additional parameter stored in the observer object
 * @param data2 optional additional parameter stored in the observer object
 * @return the observer object
 * @note This is a generic interface, which is normally not called directly. For specific purposes there are specific wrappers.
 * @note The observer object is allocated on the heap. It will be freed automatically when the object is deleted or when it is unsubscibed from the subject.
 */
lv_observer_t * lv_subject_add_observer_obj(lv_subject_t * subject, lv_observer_cb_t cb, lv_obj_t * obj,
                                            void * user_data);

/**
 * Remove observer from the list of subscribers
 * @param observer address of the observer object returned when a widget was subscribed to the subject
 */
void lv_observer_remove(lv_observer_t * observer);

void lv_observer_add_flag(lv_observer_t * observer, lv_observer_flag_t flag);

void lv_observer_remove_flag(lv_observer_t * observer, lv_observer_flag_t flag);

/**
 * Remove all observers of a specified widget from the list of observers
 * @param subject address of the subject
 * @param obj the LVGL widget
 */
void lv_subject_remove_all_obj(lv_subject_t * subject, lv_obj_t * obj);

/**
 * Remove all observers of a subject
 * @param subject address of the subject
 */
void lv_subject_unsubscribe_all(lv_subject_t * subject);

/**
 * Update the value of an int type subject
 * @param subject address of the subject
 * @param value new value
 * @note automatically notifies all observers
 */
void lv_subject_set_int(lv_subject_t * subject, int32_t value);

/**
 * Update the value of a pointer type subject
 * @param subject address of the subject
 * @param value new value
 * @note automatically notifies all observers
 */
void lv_subject_set_ptr(lv_subject_t * subject, void * value);

/**
 * Update the value of a color type subject
 * @param subject address of the subject
 * @param color new value
 * @note automatically notifies all observers
 */
void lv_subject_set_color(lv_subject_t * subject, lv_color_t color);

/**
 * Copy the string into the buffer of a string type subject
 * @param subject address of the subject
 * @param buf string to copy
 * @note automatically notifies all observers
 */
void lv_subject_copy_string(lv_subject_t * subject, char * buf);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBSERVABLE_H*/
