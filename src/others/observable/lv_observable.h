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
//#include "../../misc/lv_ll.h"

/*********************
 *      DEFINES
 *********************/

#define LV_SUBJECT_TYPE_INT     0 /**< an int32_t*/
#define LV_SUBJECT_TYPE_POINTER 1 /**< a void pointer*/
#define LV_SUBJECT_TYPE_COLOR   2 /**< an lv_color_t*/
#define LV_SUBJECT_TYPE_FLOAT   3 /**< a float*/
#define LV_SUBJECT_TYPE_BITMASK 4 /**< an array of up to 30 binary flags*/
#define LV_SUBJECT_TYPE_STRING  256 /**< a char pointer*/

#define LV_OBSERVER_BITMASK_NEGATE  0x80000000UL /**< negate result of bitmask comparison*/
#define LV_OBSERVER_BITMASK_ALL_SET 0x40000000UL /**< check if all masked bits are set*/

/**********************
 *      TYPEDEFS
 **********************/

 /**
  * Callback called when the observed value changes
  * @param s the lv_observer_t object created when the object was subscribed to the value
  */
typedef void (*lv_observer_cb_t)(void* s);

/**
 * A common type to handle all the various observable types in the same way
 */
typedef union {
    int32_t num; /**< Integer number (opacity, enums, booleans or "normal" numbers)*/
    float fnum; /**< Floating point number */
    const void* ptr; /**< Constant pointer  (string buffer, format string, font, cone text, etc)*/
    lv_color_t color; /**< Color */
} lv_subject_value_t;

/**
 * The subject (an observable value)
 */
typedef struct {
    lv_ll_t subs_ll; /**< Subscribers*/
    size_t type; /**< For elementary types the type id, for strings LV_SUBJECT_TYPE_STRING + the length of the string*/
    lv_subject_value_t value; /**< Actual value*/
} lv_subject_t;

/**
 * The observer object: a descriptor returned when subscribing LVGL widgets to subjects
 */
typedef struct {
    lv_subject_t* subject; /**< The observed value */
    lv_observer_cb_t cb; /**< Callback that should be called when the value changes*/
    lv_obj_t* obj; /**< LVGL widget which has subscribed for the value*/
    void* data1; /**< Additional parameter supplied when subscribing*/
    void* data2; /**< Additional parameter supplied when subscribing*/
} lv_observer_t;

typedef uint32_t lv_bitmask_pattern_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an int type subject
 * @param subject address of the subject
 * @param value initial value
 */
void lv_subject_init_int(lv_subject_t* subject, int32_t value);

/**
 * Initialize a pointer type subject
 * @param subject address of the subject
 * @param value initial value
 */
void lv_subject_init_ptr(lv_subject_t* subject, void* value);

/**
 * Initialize a string type subject
 * @param subject address of the subject
 * @param buf pointer to a buffer containing the string value
 * @param size size of the buffer
 */
void lv_subject_init_string(lv_subject_t* subject, char* buf, size_t size);

/**
 * Initialize a color type subject
 * @param subject address of the subject
 * @param color initial value
 */
void lv_subject_init_color(lv_subject_t* subject, lv_color_t color);

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
void* lv_subject_subscribe_obj(lv_subject_t* subject, lv_observer_cb_t cb, lv_obj_t* obj, void* data1, void* data2);

/**
 * Remove observer from the list of subscribers
 * @param observer address of the observer object returned when a widget was subscribed to the subject
 */
void lv_observer_unsubscribe(lv_observer_t* observer);

/**
 * Remove all observers of a specified widget from the list of observers
 * @param subject address of the subject
 * @param obj the LVGL widget
 */
void lv_subject_unsubscribe_obj(lv_subject_t* subject, lv_obj_t* obj);

/**
 * Remove all observers of a subject
 * @param subject address of the subject
 */
void lv_subject_unsubscribe_all(lv_subject_t* subject);

/**
 * Update the value of an int type subject
 * @param subject address of the subject
 * @param value new value
 * @note automatically notifies all observers
 */
void lv_subject_set_int(lv_subject_t* subject, int32_t value);

/**
 * Update the value of a pointer type subject
 * @param subject address of the subject
 * @param value new value
 * @note automatically notifies all observers
 */
void lv_subject_set_ptr(lv_subject_t* subject, void* value);

/**
 * Update the value of a color type subject
 * @param subject address of the subject
 * @param color new value
 * @note automatically notifies all observers
 */
void lv_subject_set_color(lv_subject_t* subject, lv_color_t color);

/**
 * Copy the string into the buffer of a string type subject
 * @param subject address of the subject
 * @param buf string to copy 
 * @note automatically notifies all observers
 */
void lv_subject_copy_string(lv_subject_t* subject, char* buf);

/**
 * Set bitmask and value comparison pattern of the observer
 * @param observer address of the observer
 * @param pattern pattern used to mask out interested bits from the bitmask's value and compare it with a constant value
 * @note The value
 */
void lv_observer_bitmask_set_pattern(lv_observer_t* observer, lv_bitmask_pattern_t pattern);

/**
 * Bind bitmask to object flag
 * @param subject address of the subject
 * @param obj the LVGL object what will be affected by the change
 * @param flag the affected flag
 * @param pattern pattern used to mask out interested bits from the bitmask's value and compare it with a constant value
 */
void* lv_bind_bitmask_to_obj_flag(lv_subject_t* subject, lv_obj_t* obj, lv_obj_flag_t flag, lv_bitmask_pattern_t pattern);

/**
 * Bind bitmask to object state
 * @param subject address of the subject
 * @param obj the LVGL object what will be affected by the change
 * @param state the affected state
 * @param pattern pattern used to mask out interested bits from the bitmask's value and compare it with a constant value
 */
void* lv_bind_bitmask_to_obj_state(lv_subject_t* subject, lv_obj_t* obj, lv_state_t flag, lv_bitmask_pattern_t pattern);

/**
 * Bind int value to a local style property
 * @param subject address of the subject
 * @param obj the LVGL object what will be affected by the change
 * @param prop the style property
 * @param selector the part/state selector
 */
void* lv_bind_int_to_obj_local_style_prop(lv_subject_t* subject, lv_obj_t* obj, lv_style_prop_t prop, lv_style_selector_t selector);

/**
 * Bind string value to a label text
 * @param subject address of the subject
 * @param obj the LVGL object what will be affected by the change
 */
void* lv_bind_string_to_label_text(lv_subject_t* subject, lv_obj_t* obj);

/**
 * Bind int value to a label text using a predefined format string
 * @param subject address of the subject
 * @param obj the LVGL object what will be affected by the change
 * @param fmt the format string
 */
void* lv_bind_int_to_label_text_fmt(lv_subject_t* subject, lv_obj_t* obj, const char* fmt);

/**
 * Bind and int value to a label text using a variable format string
 * @param subject address of the subject
 * @param fmt the address of the format string (an observable string value)
 * @param obj the LVGL object what will be affected by the change
 * @note If either the value or the formwat string changes the label's text will be updated.
 */
void* lv_bind_formatted_int_to_label_text(lv_subject_t* subject, lv_subject_t* fmt, lv_obj_t* obj);

/**
 * Generic method to subscribe to an int value with a user-defined callback
 * @param subject address of the subject
 * @param cb the callback called when the subject changes
 * @param obj the LVGL object what will be affected by the change
 * @param data1 optional additional parameter stored in the observer object
 * @param data2 optional additional parameter stored in the observer object
 */
void* lv_bind_int_to_callback(lv_subject_t* subject, lv_observer_cb_t cb, lv_obj_t* obj, void* data1, void* data2);

/**********************
 *      MACROS
 **********************/

/**
 * Macro for bitmasks: check if no bit is set
 */
#define LV_BITMASK_NOT_SET(mask) ((uint32_t)(mask))

/**
 * Macro for bitmasks: check if any bit is set
 */
#define LV_BITMASK_ANY_SET(mask) ((uint32_t)(mask) | LV_OBSERVER_BITMASK_NEGATE)

/**
 * Macro for bitmasks: check if all bits are set
 */
#define LV_BITMASK_ALL_SET(mask) ((uint32_t)(mask) | LV_OBSERVER_BITMASK_ALL_SET)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBSERVABLE_H*/
