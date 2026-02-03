/**
 * @file lv_event.h
 *
 */

#ifndef LV_EVENT_H
#define LV_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_types.h"
#include "../lv_conf_internal.h"

#include "lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_event_cb_t)(lv_event_t * e);

/**
 * Type of event being sent to Widget
 */
typedef enum {
    LV_EVENT_ALL = 0,

    /** Input device events*/
    LV_EVENT_PRESSED,             /**< Widget has been pressed */
    LV_EVENT_PRESSING,            /**< Widget is being pressed (sent continuously while pressing)*/
    LV_EVENT_PRESS_LOST,          /**< Widget is still being pressed but slid cursor/finger off Widget */
    LV_EVENT_SHORT_CLICKED,       /**< Widget was pressed for a short period of time, then released. Not sent if scrolled. */
    LV_EVENT_SINGLE_CLICKED,      /**< Sent for first short click within a small distance and short time */
    LV_EVENT_DOUBLE_CLICKED,      /**< Sent for second short click within small distance and short time */
    LV_EVENT_TRIPLE_CLICKED,      /**< Sent for third short click within small distance and short time */
    LV_EVENT_LONG_PRESSED,        /**< Object has been pressed for at least `long_press_time`.  Not sent if scrolled. */
    LV_EVENT_LONG_PRESSED_REPEAT, /**< Sent after `long_press_time` in every `long_press_repeat_time` ms.  Not sent if scrolled. */
    LV_EVENT_CLICKED,             /**< Sent on release if not scrolled (regardless to long press)*/
    LV_EVENT_RELEASED,            /**< Sent in every cases when Widget has been released */
    LV_EVENT_SCROLL_BEGIN,        /**< Scrolling begins. The event parameter is a pointer to the animation of the scroll. Can be modified */
    LV_EVENT_SCROLL_THROW_BEGIN,
    LV_EVENT_SCROLL_END,          /**< Scrolling ends */
    LV_EVENT_SCROLL,              /**< Scrolling */
    LV_EVENT_GESTURE,             /**< A gesture is detected. Get gesture with `lv_indev_get_gesture_dir(lv_indev_active());` */
    LV_EVENT_KEY,                 /**< A key is sent to Widget. Get key with `lv_indev_get_key(lv_indev_active());`*/
    LV_EVENT_ROTARY,              /**< An encoder or wheel was rotated. Get rotation count with `lv_event_get_rotary_diff(e);`*/
    LV_EVENT_FOCUSED,             /**< Widget received focus */
    LV_EVENT_DEFOCUSED,           /**< Widget's focus has been lost */
    LV_EVENT_LEAVE,               /**< Widget's focus has been lost but is still selected */
    LV_EVENT_HIT_TEST,            /**< Perform advanced hit-testing */
    LV_EVENT_INDEV_RESET,         /**< Indev has been reset */
    LV_EVENT_HOVER_OVER,          /**< Indev hover over object */
    LV_EVENT_HOVER_LEAVE,         /**< Indev hover leave object */

    /** Drawing events */
    LV_EVENT_COVER_CHECK,         /**< Check if Widget fully covers an area. The event parameter is `lv_cover_check_info_t *`. */
    LV_EVENT_REFR_EXT_DRAW_SIZE,  /**< Get required extra draw area around Widget (e.g. for shadow). The event parameter is `int32_t *` to store the size. */
    LV_EVENT_DRAW_MAIN_BEGIN,     /**< Starting the main drawing phase */
    LV_EVENT_DRAW_MAIN,           /**< Perform the main drawing */
    LV_EVENT_DRAW_MAIN_END,       /**< Finishing the main drawing phase */
    LV_EVENT_DRAW_POST_BEGIN,     /**< Starting the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST,           /**< Perform the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST_END,       /**< Finishing the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_TASK_ADDED,     /**< Adding a draw task. The `LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS` flag needs to be set */

    /** Special events */
    LV_EVENT_VALUE_CHANGED,       /**< Widget's value has changed (i.e. slider moved)*/
    LV_EVENT_INSERT,              /**< Text has been inserted into Widget. The event data is `char *` being inserted. */
    LV_EVENT_REFRESH,             /**< Notify Widget to refresh something on it (for user)*/
    LV_EVENT_READY,               /**< A process has finished */
    LV_EVENT_CANCEL,              /**< A process has been cancelled */
    LV_EVENT_STATE_CHANGED,       /**< The state of the widget changed*/

    /** Other events */
    LV_EVENT_CREATE,              /**< Object is being created */
    LV_EVENT_DELETE,              /**< Object is being deleted */
    LV_EVENT_CHILD_CHANGED,       /**< Child was removed, added, or its size, position were changed */
    LV_EVENT_CHILD_CREATED,       /**< Child was created, always bubbles up to all parents */
    LV_EVENT_CHILD_DELETED,       /**< Child was deleted, always bubbles up to all parents */
    LV_EVENT_SCREEN_UNLOAD_START, /**< A screen unload started, fired immediately when scr_load is called */
    LV_EVENT_SCREEN_LOAD_START,   /**< A screen load started, fired when the screen change delay is expired */
    LV_EVENT_SCREEN_LOADED,       /**< A screen was loaded */
    LV_EVENT_SCREEN_UNLOADED,     /**< A screen was unloaded */
    LV_EVENT_SIZE_CHANGED,        /**< Object coordinates/size have changed */
    LV_EVENT_STYLE_CHANGED,       /**< Object's style has changed */
    LV_EVENT_LAYOUT_CHANGED,      /**< A child's position position has changed due to a layout recalculation */
    LV_EVENT_GET_SELF_SIZE,       /**< Get internal size of a widget */

    /** Events of optional LVGL components */
    LV_EVENT_INVALIDATE_AREA,     /**< An area is invalidated (marked for redraw). `lv_event_get_param(e)`
                                   * returns a pointer to an `lv_area_t` object with the coordinates of the
                                   * area to be invalidated.  The area can be freely modified if needed to
                                   * adapt it a special requirement of the display. Usually needed with
                                   * monochrome displays to invalidate `N x 8` rows or columns in one pass. */
    LV_EVENT_RESOLUTION_CHANGED,  /**< Sent when the resolution changes due to `lv_display_set_resolution()` or `lv_display_set_rotation()`. */
    LV_EVENT_COLOR_FORMAT_CHANGED,/**< Sent as a result of any call to `lv_display_set_color_format()`. */
    LV_EVENT_REFR_REQUEST,        /**< Sent when something happened that requires redraw. */
    LV_EVENT_REFR_START,          /**< Sent before a refreshing cycle starts. Sent even if there is nothing to redraw. */
    LV_EVENT_REFR_READY,          /**< Sent when refreshing has been completed (after rendering and calling flush callback). Sent even if no redraw happened. */
    LV_EVENT_RENDER_START,        /**< Sent just before rendering begins. */
    LV_EVENT_RENDER_READY,        /**< Sent after rendering has been completed. */
    LV_EVENT_FLUSH_START,         /**< Sent before flush callback is called. */
    LV_EVENT_FLUSH_FINISH,        /**< Sent after flush callback call has returned. */
    LV_EVENT_FLUSH_WAIT_START,    /**< Sent before flush wait callback is called. */
    LV_EVENT_FLUSH_WAIT_FINISH,   /**< Sent after flush wait callback call has returned. */
    LV_EVENT_UPDATE_LAYOUT_COMPLETED,    /**< Sent after layout update completes*/

    LV_EVENT_VSYNC,
    LV_EVENT_VSYNC_REQUEST,
#if LV_USE_TRANSLATION
    LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, /**< Sent when the translation language changed. */
#endif /*LV_USE_TRANSLATION*/

    LV_EVENT_LAST,                 /** Number of default events */

    LV_EVENT_PREPROCESS = 0x8000,   /** This is a flag that can be set with an event so it's processed
                                      before the class default event processing */
    LV_EVENT_MARKED_DELETING = 0x10000,
} lv_event_code_t;

typedef struct {
    lv_array_t array;
    uint8_t is_traversing: 1;          /**< True: the list is being nested traversed */
    uint8_t has_marked_deleting: 1;    /**< True: the list has marked deleting objects
                                         when some of events are marked as deleting */
} lv_event_list_t;

/**
 * @brief Event callback.
 * Events are used to notify the user of some action being taken on Widget.
 * For details, see ::lv_event_t.
 */

lv_result_t lv_event_send(lv_event_list_t * list, lv_event_t * e, bool preprocess);

lv_event_dsc_t * lv_event_add(lv_event_list_t * list, lv_event_cb_t cb, lv_event_code_t filter, void * user_data);
bool lv_event_remove_dsc(lv_event_list_t * list, lv_event_dsc_t * dsc);

uint32_t lv_event_get_count(lv_event_list_t * list);

lv_event_dsc_t * lv_event_get_dsc(lv_event_list_t * list, uint32_t index);

lv_event_cb_t lv_event_dsc_get_cb(lv_event_dsc_t * dsc);

void * lv_event_dsc_get_user_data(lv_event_dsc_t * dsc);

bool lv_event_remove(lv_event_list_t * list, uint32_t index);

void lv_event_remove_all(lv_event_list_t * list);

/**
 * Get Widget originally targeted by the event. It's the same even if event was bubbled.
 * @param e     pointer to the event descriptor
 * @return      the target of the event_code
 */
void * lv_event_get_target(lv_event_t * e);

/**
 * Get current target of the event. It's the Widget for which the event handler being called.
 * If the event is not bubbled it's the same as "normal" target.
 * @param e     pointer to the event descriptor
 * @return      pointer to the current target of the event_code
 */
void * lv_event_get_current_target(lv_event_t * e);

/**
 * Get event code of an event.
 * @param e     pointer to the event descriptor
 * @return      the event code. (E.g. `LV_EVENT_CLICKED`, `LV_EVENT_FOCUSED`, etc)
 */
lv_event_code_t lv_event_get_code(lv_event_t * e);

/**
 * Get parameter passed when event was sent.
 * @param e     pointer to the event descriptor
 * @return      pointer to the parameter
 */
void * lv_event_get_param(lv_event_t * e);

/**
 * Get user_data passed when event was registered on Widget.
 * @param e     pointer to the event descriptor
 * @return      pointer to the user_data
 */
void * lv_event_get_user_data(lv_event_t * e);

/**
 * Stop event from bubbling.
 * This is only valid when called in the middle of an event processing chain.
 * @param e     pointer to the event descriptor
 */
void lv_event_stop_bubbling(lv_event_t * e);

/**
 * Stop event from trickling down to children.
 * This is only valid when called in the middle of an event processing chain.
 * @param e     pointer to the event descriptor
 */
void lv_event_stop_trickling(lv_event_t * e);

/**
 * Stop processing this event.
 * This is only valid when called in the middle of an event processing chain.
 * @param e     pointer to the event descriptor
 */
void lv_event_stop_processing(lv_event_t * e);

/**
 * Helper function typically used in LV_EVENT_DELETE
 * to free the event's user_data
 * @param e     pointer to an event descriptor
 */
void lv_event_free_user_data_cb(lv_event_t * e);


/**
 * Register a new, custom event ID.
 * It can be used the same way as e.g. `LV_EVENT_CLICKED` to send custom events
 * @return      the new event id
 *
 * Example:
 * @code
 * uint32_t LV_EVENT_MINE = 0;
 * ...
 * e = lv_event_register_id();
 * ...
 * lv_obj_send_event(obj, LV_EVENT_MINE, &some_data);
 * @endcode
 */
uint32_t lv_event_register_id(void);

/**
 * Get the name of an event code.
 * @param code  the event code
 * @return      the name of the event code as a string
 */
const char * lv_event_code_get_name(lv_event_code_t code);

#if LV_USE_EXT_DATA
/**
 * Set external data and its destructor for an event descriptor.
 * This allows associating custom data with an event callback that will be automatically cleaned up
 * when the event descriptor is removed or destroyed.
 * @param dsc         pointer to an event descriptor (from lv_obj_add_event_cb)
 * @param data        pointer to the external data to associate with the event descriptor
 * @param free_cb     function pointer to a destructor that will be called to clean up the external data.
 *                    The destructor will receive the data pointer as its parameter.
 */
void lv_event_desc_set_external_data(lv_event_dsc_t * dsc, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVENT_H */
