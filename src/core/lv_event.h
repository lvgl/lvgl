/**
 * @file lv_templ.h
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
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;
struct _lv_event_dsc_t;

/**
 * Type of event being sent to the object.
 */
typedef enum {
    LV_EVENT_ALL = 0,

    /** Input device events*/
    LV_EVENT_PRESSED,             /**< The object has been pressed*/
    LV_EVENT_PRESSING,            /**< The object is being pressed (called continuously while pressing)*/
    LV_EVENT_PRESS_LOST,          /**< The object is still being pressed but slid cursor/finger off of the object */
    LV_EVENT_SHORT_CLICKED,       /**< The object was pressed for a short period of time, then released it. Not called if scrolled.*/
    LV_EVENT_LONG_PRESSED,        /**< Object has been pressed for at least `long_press_time`.  Not called if scrolled.*/
    LV_EVENT_LONG_PRESSED_REPEAT, /**< Called after `long_press_time` in every `long_press_repeat_time` ms.  Not called if scrolled.*/
    LV_EVENT_CLICKED,             /**< Called on release if not scrolled (regardless to long press)*/
    LV_EVENT_RELEASED,            /**< Called in every cases when the object has been released*/
    LV_EVENT_SCROLL_BEGIN,        /**< Scrolling begins*/
    LV_EVENT_SCROLL_END,          /**< Scrolling ends*/
    LV_EVENT_SCROLL,              /**< Scrolling*/
    LV_EVENT_GESTURE,             /**< A gesture is detected. Get the gesture with `lv_indev_get_gesture_dir(lv_indev_get_act());` */
    LV_EVENT_KEY,                 /**< A key is sent to the object. Get the key with `lv_indev_get_key(lv_indev_get_act());`*/
    LV_EVENT_FOCUSED,             /**< The object is focused*/
    LV_EVENT_DEFOCUSED,           /**< The object is defocused*/
    LV_EVENT_LEAVE,               /**< The object is defocused but still selected*/
    LV_EVENT_HIT_TEST,            /**< Perform advanced hit-testing*/

    /** Drawing events*/
    LV_EVENT_COVER_CHECK,        /**< Check if the object fully covers an area. The event parameter is `lv_cover_check_info_t *`.*/
    LV_EVENT_REFR_EXT_DRAW_SIZE, /**< Get the required extra draw area around the object (e.g. for shadow). The event parameter is `lv_coord_t *` to store the size.*/
    LV_EVENT_DRAW_MAIN_BEGIN,    /**< Starting the main drawing phase*/
    LV_EVENT_DRAW_MAIN,          /**< Perform the main drawing*/
    LV_EVENT_DRAW_MAIN_END,      /**< Finishing the main drawing phase*/
    LV_EVENT_DRAW_POST_BEGIN,    /**< Starting the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST,          /**< Perform the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST_END,      /**< Finishing the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_PART_BEGIN,    /**< Starting to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. */
    LV_EVENT_DRAW_PART_END,      /**< Finishing to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. */

    /** Special events*/
    LV_EVENT_VALUE_CHANGED,       /**< The object's value has changed (i.e. slider moved)*/
    LV_EVENT_INSERT,              /**< A text is inserted to the object. The event data is `char *` being inserted.*/
    LV_EVENT_REFRESH,             /**< Notify the object to refresh something on it (for the user)*/
    LV_EVENT_READY,               /**< A process has finished*/
    LV_EVENT_CANCEL,              /**< A process has been cancelled */

    /** Other events*/
    LV_EVENT_DELETE,              /**< Object is being deleted*/
    LV_EVENT_CHILD_CHANGED,       /**< Child was removed, added, or its size, position were changed */
    LV_EVENT_CHILD_CREATED,       /**< Child was created, always bubbles up to all parents*/
    LV_EVENT_CHILD_DELETED,       /**< Child was deleted, always bubbles up to all parents*/
    LV_EVENT_SCREEN_UNLOAD_START, /**< A screen unload started, fired immediately when scr_load is called*/
    LV_EVENT_SCREEN_LOAD_START,   /**< A screen load started, fired when the screen change delay is expired*/
    LV_EVENT_SCREEN_LOADED,       /**< A screen was loaded*/
    LV_EVENT_SCREEN_UNLOADED,     /**< A screen was unloaded*/
    LV_EVENT_SIZE_CHANGED,        /**< Object coordinates/size have changed*/
    LV_EVENT_STYLE_CHANGED,       /**< Object's style has changed*/
    LV_EVENT_LAYOUT_CHANGED,      /**< The children position has changed due to a layout recalculation*/
    LV_EVENT_GET_SELF_SIZE,       /**< Get the internal size of a widget*/

    _LV_EVENT_LAST                /** Number of default events*/
} lv_event_code_t;

typedef struct _lv_event_t {
    struct _lv_obj_t * target;
    struct _lv_obj_t * current_target;
    lv_event_code_t code;
    void * user_data;
    void * param;
    struct _lv_event_t * prev;
    uint8_t deleted : 1;
} lv_event_t;

/**
 * @brief Event callback.
 * Events are used to notify the user of some action being taken on the object.
 * For details, see ::lv_event_t.
 */
typedef void (*lv_event_cb_t)(lv_event_t * e);

/**
 * Used as the event parameter of ::LV_EVENT_HIT_TEST to check if an `point` can click the object or not.
 * `res` should be set like this:
 *   - If already set to `false` an other event wants that point non clickable. If you want to respect it leave it as `false` or set `true` to overwrite it.
 *   - If already set `true` and `point` shouldn't be clickable set to `false`
 *   - If already set to `true` you agree that `point` can click the object leave it as `true`
 */
typedef struct {
    const lv_point_t * point;   /**< A point relative to screen to check if it can click the object or not*/
    bool res;                   /**< true: `point` can click the object; false: it cannot*/
} lv_hit_test_info_t;

/**
 * Used as the event parameter of ::LV_EVENT_COVER_CHECK to check if an area is covered by the object or not.
 * In the event use `const lv_area_t * area = lv_event_get_cover_area(e)` to get the area to check
 * and `lv_event_set_cover_res(e, res)` to set the result.
 */
typedef struct {
    lv_cover_res_t res;
    const lv_area_t * area;
} lv_cover_check_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Send an event to the object
 * @param obj           pointer to an object
 * @param event_code    the type of the event from `lv_event_t`
 * @param param         arbitrary data depending on the widget type and the event. (Usually `NULL`)
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event_code
 */
lv_res_t lv_event_send(struct _lv_obj_t * obj, lv_event_code_t event_code, void * param);

/**
 * Used by the widgets internally to call the ancestor widget types's event handler
 * @param class_p   pointer to the class of the widget (NOT the ancestor class)
 * @param e         pointer to the event descriptor
 * @return          LV_RES_OK: the taget object was not deleted in the event; LV_RES_INV: it was deleted in the event_code
 */
lv_res_t lv_obj_event_base(const lv_obj_class_t * class_p, lv_event_t * e);

/**
 * Get the object originally targeted by the event. It's the same even if the event is bubbled.
 * @param e     pointer to the event descriptor
 * @return      the target of the event_code
 */
struct _lv_obj_t * lv_event_get_target(lv_event_t * e);

/**
 * Get the current target of the event. It's the object which event handler being called.
 * If the event is not bubbled it's the same as "normal" target.
 * @param e     pointer to the event descriptor
 * @return      pointer to the current target of the event_code
 */
struct _lv_obj_t * lv_event_get_current_target(lv_event_t * e);

/**
 * Get the event code of an event
 * @param e     pointer to the event descriptor
 * @return      the event code. (E.g. `LV_EVENT_CLICKED`, `LV_EVENT_FOCUSED`, etc)
 */
lv_event_code_t lv_event_get_code(lv_event_t * e);

/**
 * Get the parameter passed when the event was sent
 * @param e     pointer to the event descriptor
 * @return      pointer to the parameter
 */
void * lv_event_get_param(lv_event_t * e);

/**
 * Get the user_data passed when the event was registered on the object
 * @param e     pointer to the event descriptor
 * @return      pointer to the user_data
 */
void * lv_event_get_user_data(lv_event_t * e);


/**
 * Register a new, custom event ID.
 * It can be used the same way as e.g. `LV_EVENT_CLICKED` to send custom events
 * @return      the new event id
 * @example
 * uint32_t LV_EVENT_MINE = 0;
 * ...
 * e = lv_event_register_id();
 * ...
 * lv_event_send(obj, LV_EVENT_MINE, &some_data);
 */
uint32_t lv_event_register_id(void);

/**
 * Nested events can be called and one of them might belong to an object that is being deleted.
 * Mark this object's `event_temp_data` deleted to know that its `lv_event_send` should return `LV_RES_INV`
 * @param obj pointer to an object to mark as deleted
 */
void _lv_event_mark_deleted(struct _lv_obj_t * obj);


/**
 * Add an event handler function for an object.
 * Used by the user to react on event which happens with the object.
 * An object can have multiple event handler. They will be called in the same order as they were added.
 * @param obj       pointer to an object
 * @param filter    and event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be sued the receive all the events.
 * @param event_cb  the new event function
 * @param           user_data custom data data will be available in `event_cb`
 * @return          a pointer the event descriptor. Can be used in ::lv_obj_remove_event_dsc
 */
struct _lv_event_dsc_t * lv_obj_add_event_cb(struct _lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter,
                                             void * user_data);

/**
 * Remove an event handler function for an object.
 * @param obj       pointer to an object
 * @param event_cb  the event function to remove, or `NULL` to remove the the firstly added event callback
 * @return          true if any event handlers were removed
 */
bool lv_obj_remove_event_cb(struct _lv_obj_t * obj, lv_event_cb_t event_cb);

/**
 * Remove an event handler function with a specific user_data from an object.
 * @param obj               pointer to an object
 * @param event_cb          the event function to remove, or `NULL` only `user_data` matters.
 * @param event_user_data   the user_data specified in ::lv_obj_add_event_cb
 * @return                  true if any event handlers were removed
 */
bool lv_obj_remove_event_cb_with_user_data(struct _lv_obj_t * obj, lv_event_cb_t event_cb,
                                           const void * event_user_data);

/**
 * DEPRACTED because doesn't work if multiple event handlers are added to an object.
 * Remove an event handler function for an object.
 * @param obj       pointer to an object
 * @param event_dsc pointer to an event descriptor to remove (returned by ::lv_obj_add_event_cb)
 * @return          true if any event handlers were removed
 */
bool lv_obj_remove_event_dsc(struct _lv_obj_t * obj, struct _lv_event_dsc_t * event_dsc);

/**
 * Get the input device passed as parameter to indev related events.
 * @param e     pointer to an event
 * @return      the indev that triggered the event or NULL if called on a not indev related event
 */
lv_indev_t * lv_event_get_indev(lv_event_t * e);

/**
 * Get the part draw descriptor passed as parameter to `LV_EVENT_DRAW_PART_BEGIN/END`.
 * @param e     pointer to an event
 * @return      the part draw descriptor to hook the drawing or NULL if called on an unrelated event
 */
lv_obj_draw_part_dsc_t * lv_event_get_draw_part_dsc(lv_event_t * e);

/**
 * Get the clip area passed as parameter to draw events events.
 * Namely: `LV_EVENT_DRAW_MAIN/POST`, `LV_EVENT_DRAW_MAIN/POST_BEGIN`, `LV_EVENT_DRAW_MAIN/POST_END`
 * @param e     pointer to an event
 * @return      the clip area to use during drawing or NULL if called on an unrelated event
 */
const lv_area_t * lv_event_get_clip_area(lv_event_t * e);

/**
 * Get the old area of the object before its size was changed. Can be used in `LV_EVENT_SIZE_CHANGED`
 * @param e     pointer to an event
 * @return      the old absolute area of the object or NULL if called on an unrelated event
 */
const lv_area_t * lv_event_get_old_size(lv_event_t * e);

/**
 * Get the key passed as parameter to an event. Can be used in `LV_EVENT_KEY`
 * @param e     pointer to an event
 * @return      the triggering key or NULL if called on an unrelated event
 */
uint32_t lv_event_get_key(lv_event_t * e);

/**
 * Get the animation descriptor of a scrolling. Can be used in `LV_EVENT_SCROLL_BEGIN`
 * @param e     pointer to an event
 * @return      the animation that will scroll the object. (can be modified as required)
 */
lv_anim_t * lv_event_get_scroll_anim(lv_event_t * e);

/**
 * Set the new extra draw size. Can be used in `LV_EVENT_REFR_EXT_DRAW_SIZE`
 * @param e     pointer to an event
 * @param size  The new extra draw size
 */
void lv_event_set_ext_draw_size(lv_event_t * e, lv_coord_t size);

/**
 * Get a pointer to an `lv_point_t` variable in which the self size should be saved (width in `point->x` and height `point->y`).
 * Can be used in `LV_EVENT_GET_SELF_SIZE`
 * @param e     pointer to an event
 * @return      pointer to `lv_point_t` or NULL if called on an unrelated event
 */
lv_point_t * lv_event_get_self_size_info(lv_event_t * e);

/**
 * Get a pointer to an `lv_hit_test_info_t` variable in which the hit test result should be saved. Can be used in `LV_EVENT_HIT_TEST`
 * @param e     pointer to an event
 * @return      pointer to `lv_hit_test_info_t` or NULL if called on an unrelated event
 */
lv_hit_test_info_t * lv_event_get_hit_test_info(lv_event_t * e);

/**
 * Get a pointer to an area which should be examined whether the object fully covers it or not.
 * Can be used in `LV_EVENT_HIT_TEST`
 * @param e     pointer to an event
 * @return      an area with absolute coordinates to check
 */
const lv_area_t * lv_event_get_cover_area(lv_event_t * e);

/**
 * Set the result of cover checking. Can be used in `LV_EVENT_COVER_CHECK`
 * @param e     pointer to an event
 * @param res   an element of ::lv_cover_check_info_t
 */
void lv_event_set_cover_res(lv_event_t * e, lv_cover_res_t res);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_EVENT_H*/
