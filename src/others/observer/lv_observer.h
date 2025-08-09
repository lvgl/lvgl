/**
 * @file lv_observer.h
 *
 */

#ifndef LV_OBSERVER_H
#define LV_OBSERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj.h"
#if LV_USE_OBSERVER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Values for lv_subject_t's `type` field
 */
typedef enum {
    LV_SUBJECT_TYPE_INVALID =   0,   /**< indicates Subject not initialized yet */
    LV_SUBJECT_TYPE_NONE =      1,   /**< a null value like None or NILt */
    LV_SUBJECT_TYPE_INT =       2,   /**< an int32_t */
    LV_SUBJECT_TYPE_FLOAT =     3,   /**< a float, requires `LV_USE_FLOAT 1` */
    LV_SUBJECT_TYPE_POINTER =   4,   /**< a void pointer */
    LV_SUBJECT_TYPE_COLOR   =   5,   /**< an lv_color_t */
    LV_SUBJECT_TYPE_GROUP  =    6,   /**< an array of Subjects */
    LV_SUBJECT_TYPE_STRING  =   7,   /**< a char pointer */
} lv_subject_type_t;

/**
 * A common type to handle all the various observable types in the same way
 */
typedef union {
    int32_t num;           /**< Integer number (opacity, enums, booleans or "normal" numbers) */
    const void * pointer;  /**< Constant pointer  (string buffer, format string, font, cone text, etc.) */
    lv_color_t color;      /**< Color */
#if LV_USE_FLOAT
    float float_v;         /**< Floating point value*/
#endif
} lv_subject_value_t;

/**
 * The Subject (an observable value)
 */
typedef struct {
    lv_ll_t subs_ll;                     /**< Subscribers */
    lv_subject_value_t value;            /**< Current value */
    lv_subject_value_t prev_value;       /**< Previous value */
    void * user_data;                    /**< Additional parameter, can be used freely by user */
    uint32_t type                 :  4;  /**< One of the LV_SUBJECT_TYPE_... values */
    uint32_t size                 : 24;  /**< String buffer size or group length */
    uint32_t notify_restart_query :  1;  /**< If an Observer was deleted during notification,
                                          * start notifying from the beginning. */
} lv_subject_t;

/**
  * Callback called to notify Observer that Subject's value has changed
  * @param observer     pointer to Observer
  * @param subject      pointer to Subject being observed
  */
typedef void (*lv_observer_cb_t)(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an integer-type Subject.
 * @param subject   pointer to Subject
 * @param value     initial value
 */
void lv_subject_init_int(lv_subject_t * subject, int32_t value);

/**
 * Set value of an integer Subject and notify Observers.
 * @param subject   pointer to Subject
 * @param value     new value
 */
void lv_subject_set_int(lv_subject_t * subject, int32_t value);

/**
 * Get current value of an integer Subject.
 * @param subject   pointer to Subject
 * @return          current value
 */
int32_t lv_subject_get_int(lv_subject_t * subject);

/**
 * Get previous value of an integer Subject.
 * @param subject   pointer to Subject
 * @return          current value
 */
int32_t lv_subject_get_previous_int(lv_subject_t * subject);

#if LV_USE_FLOAT

/**
 * Initialize an float-type Subject.
 * @param subject   pointer to Subject
 * @param value     initial value
 */
void lv_subject_init_float(lv_subject_t * subject, float value);

/**
 * Set value of an float Subject and notify Observers.
 * @param subject   pointer to Subject
 * @param value     new value
 */
void lv_subject_set_float(lv_subject_t * subject, float value);

/**
 * Get current value of an float Subject.
 * @param subject   pointer to Subject
 * @return          current value
 */
float lv_subject_get_float(lv_subject_t * subject);

/**
 * Get previous value of an float Subject.
 * @param subject   pointer to Subject
 * @return          current value
 */
float lv_subject_get_previous_float(lv_subject_t * subject);

#endif /*LV_USE_FLOAT*/

/**
 * Initialize a string-type Subject.
 * @param subject   pointer to Subject
 * @param buf       pointer to buffer to store string
 * @param prev_buf  pointer to buffer to store previous string; can be NULL if not used
 * @param size      size of buffer(s)
 * @param value     initial value of string, e.g. "hello"
 * @note            A string Subject stores its own copy of the string, not just the pointer.
 */
void lv_subject_init_string(lv_subject_t * subject, char * buf, char * prev_buf, size_t size, const char * value);

/**
 * Copy a string to a Subject and notify Observers if it changed.
 * @param subject   pointer to Subject
 * @param buf       new string
 */
void lv_subject_copy_string(lv_subject_t * subject, const char * buf);

/**
 * Format a new string, updating Subject, and notify Observers if it changed.
 * @param subject   pointer to Subject
 * @param format    format string
 */
void lv_subject_snprintf(lv_subject_t * subject, const char * format, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Get current value of a string Subject.
 * @param subject   pointer to Subject
 * @return          pointer to buffer containing current value
 */
const char * lv_subject_get_string(lv_subject_t * subject);

/**
 * Get previous value of a string Subject.
 * @param subject   pointer to Subject
 * @return          pointer to buffer containing previous value
 * @note            NULL will be returned if NULL was passed in `lv_subject_init_string()`
 *                  as `prev_buf`.
 */
const char * lv_subject_get_previous_string(lv_subject_t * subject);

/**
 * Initialize a pointer-type Subject.
 * @param subject   pointer to Subject
 * @param value     initial value
 */
void lv_subject_init_pointer(lv_subject_t * subject, void * value);

/**
 * Set value of a pointer Subject and notify Observers (regardless of whether it changed).
 * @param subject   pointer to Subject
 * @param ptr       new value
 */
void lv_subject_set_pointer(lv_subject_t * subject, void * ptr);

/**
 * Get current value of a pointer Subject.
 * @param subject   pointer to Subject
 * @return          current value
 */
const void * lv_subject_get_pointer(lv_subject_t * subject);

/**
 * Get previous value of a pointer Subject.
 * @param subject   pointer to Subject
 * @return          previous value
 */
const void * lv_subject_get_previous_pointer(lv_subject_t * subject);

/**
 * Initialize a color-type Subject.
 * @param subject   pointer to Subject
 * @param color     initial value
 */
void lv_subject_init_color(lv_subject_t * subject, lv_color_t color);

/**
 * Set value of a color Subject and notify Observers if it changed.
 * @param subject   pointer to Subject
 * @param color     new value
 */
void lv_subject_set_color(lv_subject_t * subject, lv_color_t color);

/**
 * Get current value of a color Subject.
 * @param subject   pointer to Subject
 * @return          current value
 */
lv_color_t lv_subject_get_color(lv_subject_t * subject);

/**
 * Get previous value of a color Subject.
 * @param subject   pointer to Subject
 * @return          previous value
 */
lv_color_t lv_subject_get_previous_color(lv_subject_t * subject);

/**
 * Initialize a Group-type Subject.
 * @param group_subject  pointer to Group-type Subject
 * @param list           list of other Subject addresses; when any of these have values
                             updated, Observers of `group_subject` will be notified.
 * @param list_len       number of elements in `list[]`
 */
void lv_subject_init_group(lv_subject_t * group_subject, lv_subject_t * list[], uint32_t list_len);

/**
 * Remove all Observers from a Subject and free allocated memory, and delete
 * any associated Widget-Binding events.  This leaves `subject` "disconnected" from
 * all Observers and all associated Widget events established through Widget Binding.
 * @param subject   pointer to Subject
 * @note            This can safely be called regardless of whether any Observers
 *                  added with `lv_subject_add_observer_obj()` or bound to a Widget Property
 *                  with one of the `..._bind_...()` functions.
 */
void lv_subject_deinit(lv_subject_t * subject);

/**
 * Get an element from Subject Group's list.
 * @param subject   pointer to Group-type Subject
 * @param index     index of element to get
 * @return          pointer to indexed Subject from list, or NULL if index is out of bounds
 */
lv_subject_t * lv_subject_get_group_element(lv_subject_t * subject, int32_t index);

/**
 * Add Observer to Subject. When Subject's value changes `observer_cb` will be called.
 * @param subject       pointer to Subject
 * @param observer_cb   notification callback
 * @param user_data     optional user data
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_subject_add_observer(lv_subject_t * subject, lv_observer_cb_t observer_cb, void * user_data);

/**
 * Add Observer to Subject for a Widget.
 * When the Widget is deleted, Observer will be unsubscribed from Subject automatically.
 * @param subject       pointer to Subject
 * @param observer_cb   notification callback
 * @param obj           pointer to Widget
 * @param user_data     optional user data
 * @return              pointer to newly-created Observer
 * @note                Do not call `lv_observer_remove()` on Observers created this way.
 *                      Only clean up such Observers by either:
 *                      - deleting the Widget, or
 *                      - calling `lv_subject_deinit()` to gracefully de-couple and
 *                        remove all Observers.
 */
lv_observer_t * lv_subject_add_observer_obj(lv_subject_t * subject, lv_observer_cb_t observer_cb, lv_obj_t * obj,
                                            void * user_data);

/**
 * Add an Observer to a Subject and also save a target pointer.
 * @param subject       pointer to Subject
 * @param observer_cb   notification callback
 * @param target        any pointer (NULL is okay)
 * @param user_data     optional user data
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_subject_add_observer_with_target(lv_subject_t * subject, lv_observer_cb_t observer_cb,
                                                    void * target, void * user_data);

/**
 * Remove Observer from its Subject.
 * @param observer      pointer to Observer
 */
void lv_observer_remove(lv_observer_t * observer);

/**
 * Remove Observers associated with Widget `obj` from specified `subject` or all Subjects.
 * @param obj       pointer to Widget whose Observers should be removed
 * @param subject   Subject to remove Widget from, or NULL to remove from all Subjects
 * @note            This function can be used e.g. when a Widget's Subject(s) needs to
 *                  be replaced by other Subject(s)
 */
void lv_obj_remove_from_subject(lv_obj_t * obj, lv_subject_t * subject);

/**
 * Get target of an Observer.
 * @param observer      pointer to Observer
 * @return              pointer to saved target
 */
void * lv_observer_get_target(lv_observer_t * observer);

/**
 * Get target Widget of Observer.
 * This is the same as `lv_observer_get_target()`, except it returns `target`
 * as an `lv_obj_t *`.
 * @param observer      pointer to Observer
 * @return              pointer to saved Widget target
 */
lv_obj_t * lv_observer_get_target_obj(lv_observer_t * observer);

/**
 * Get Observer's user data.
 * @param observer      pointer to Observer
 * @return              void pointer to saved user data
*/
void * lv_observer_get_user_data(const lv_observer_t * observer);

/**
 * Notify all Observers of Subject.
 * @param subject       pointer to Subject
 */
void lv_subject_notify(lv_subject_t * subject);

/**
 * Add an event handler to increment (or decrement) the value of a subject on a trigger.
 * @param obj       pointer to a widget
 * @param subject   pointer to a subject to change
 * @param trigger   the trigger on which the subject should be changed
 * @param step      value to add on trigger
 * @param min       the minimum value
 * @param max       the maximum value
 */
void lv_obj_add_subject_increment_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, int32_t step,
                                        int32_t min, int32_t max);

/**
 * Set the value of an integer subject.
 * @param obj       pointer to a widget
 * @param subject   pointer to a subject to change
 * @param trigger   the trigger on which the subject should be changed
 * @param value     the value to set
 */
void lv_obj_add_subject_set_int_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, int32_t value);


#if LV_USE_FLOAT
/**
 * Set the value of a float subject.
 * @param obj       pointer to a widget
 * @param subject   pointer to a subject to change
 * @param trigger   the trigger on which the subject should be changed
 * @param value     the value to set
 */
void lv_obj_add_subject_set_float_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, float value);
#endif

/**
 * Set the value of a string subject.
 * @param obj       pointer to a widget
 * @param subject   pointer to a subject to change
 * @param trigger   the trigger on which the subject should be changed
 * @param value     the value to set
 */
void lv_obj_add_subject_set_string_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger,
                                         const char * value);

/**
 * Disable a style if a subject's value is not equal to a reference value
 * @param obj           pointer to Widget
 * @param style         pointer to a style
 * @param selector      pointer to a selector
 * @param subject       pointer to Subject
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector,
                                  lv_subject_t * subject, int32_t ref_value);

/**
 * Set Widget's flag(s) if an integer Subject's value is equal to a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param flag          flag(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_flag_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Set Widget's flag(s) if an integer Subject's value is not equal to a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param flag          flag(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_flag_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag,
                                           int32_t ref_value);

/**
 * Set Widget's flag(s) if an integer Subject's value is greater than a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param flag          flag(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_flag_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Set Widget's flag(s) if an integer Subject's value is greater than or equal to a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param flag          flag(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_flag_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Set Widget's flag(s) if an integer Subject's value is less than a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param flag          flag(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_flag_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);

/**
 * Set Widget's flag(s) if an integer Subject's value is less than or equal to a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param flag          flag(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_OBJ_FLAG_HIDDEN`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_flag_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value);


/**
 * Set Widget's state(s) if an integer Subject's value is equal to a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param state         state(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_STATE_CHECKED`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_state_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Set a Widget's state(s) if an integer Subject's value is not equal to a reference value, clear flag otherwise
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param state         state(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_STATE_CHECKED`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_state_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state,
                                            int32_t ref_value);

/**
 * Set Widget's state(s) if an integer Subject's value is greater than a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param state         state(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_STATE_CHECKED`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_state_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Set Widget's state(s) if an integer Subject's value is greater than or equal to a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param state         state(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_STATE_CHECKED`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_state_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Set Widget's state(s) if an integer Subject's value is less than a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param state         state(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_STATE_CHECKED`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_state_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Set Widget's state(s) if an integer Subject's value is less than or equal to a reference value, clear flag otherwise.
 * @param obj           pointer to Widget
 * @param subject       pointer to Subject
 * @param state         state(s) (can be bit-wise OR-ed) to set or clear (e.g. `LV_STATE_CHECKED`)
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_state_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value);

/**
 * Set an integer Subject to 1 when a Widget is checked and set it 0 when unchecked, and
 * clear Widget's checked state when Subject's value changes to 0 and set it when non-zero.
 * @param obj       pointer to Widget
 * @param subject   pointer to a Subject
 * @return          pointer to newly-created Observer
 * @note            Ensure Widget's `LV_OBJ_FLAG_CHECKABLE` flag is set.
 */
lv_observer_t * lv_obj_bind_checked(lv_obj_t * obj, lv_subject_t * subject);

#if LV_USE_LABEL
/**
 * Bind an integer, string, or pointer Subject to a Label.
 * @param obj       pointer to Label
 * @param subject   pointer to Subject
 * @param fmt       optional printf-like format string with 1 format specifier (e.g. "%d °C")
 *                  or NULL to bind to the value directly.
 * @return          pointer to newly-created Observer
 * @note            `fmt == NULL` can be used only with string and pointer Subjects.
 * @note            If Subject is a pointer and `fmt == NULL`, pointer must point
 *                  to a `\0` terminated string.
 */
lv_observer_t * lv_label_bind_text(lv_obj_t * obj, lv_subject_t * subject, const char * fmt);
#endif

#if LV_USE_ARC
/**
 * Bind an integer subject to an Arc's value.
 * @param obj       pointer to Arc
 * @param subject   pointer to Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_arc_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

#if LV_USE_SLIDER
/**
 * Bind an integer Subject to a Slider's value.
 * @param obj       pointer to Slider
 * @param subject   pointer to Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_slider_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

#if LV_USE_ROLLER
/**
 * Bind an integer Subject to a Roller's value.
 * @param obj       pointer to Roller
 * @param subject   pointer to Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_roller_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

#if LV_USE_DROPDOWN
/**
 * Bind an integer Subject to a Dropdown's value.
 * @param obj       pointer to Dropdown
 * @param subject   pointer to Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_dropdown_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OBSERVER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBSERVER_H*/
