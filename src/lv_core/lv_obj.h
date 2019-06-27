/**
 * @file lv_obj.h
 *
 */

#ifndef LV_OBJ_H
#define LV_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stddef.h>
#include <stdbool.h>
#include "lv_style.h"
#include "../lv_misc/lv_types.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_ll.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_log.h"
#include "../lv_hal/lv_hal.h"

/*********************
 *      DEFINES
 *********************/

/*Error check of lv_conf.h*/
#if LV_HOR_RES_MAX == 0 || LV_VER_RES_MAX == 0
#error "LittlevGL: LV_HOR_RES_MAX and LV_VER_RES_MAX must be greater than 0"
#endif

#if LV_ANTIALIAS > 1
#error "LittlevGL: LV_ANTIALIAS can be only 0 or 1"
#endif

#define LV_MAX_ANCESTOR_NUM 8

#define LV_EXT_CLICK_AREA_OFF 0
#define LV_EXT_CLICK_AREA_TINY 1
#define LV_EXT_CLICK_AREA_FULL 2

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;

enum {
    LV_DESIGN_DRAW_MAIN,
    LV_DESIGN_DRAW_POST,
    LV_DESIGN_COVER_CHK,
};
typedef uint8_t lv_design_mode_t;

typedef bool (*lv_design_cb_t)(struct _lv_obj_t * obj, const lv_area_t * mask_p, lv_design_mode_t mode);

enum {
    LV_EVENT_PRESSED,             /*The object has been pressed*/
    LV_EVENT_PRESSING,            /*The object is being pressed (called continuously while pressing)*/
    LV_EVENT_PRESS_LOST,          /*Still pressing but slid from the objects*/
    LV_EVENT_SHORT_CLICKED,       /*Released before long press time. Not called if dragged.*/
    LV_EVENT_LONG_PRESSED,        /*Pressing for `LV_INDEV_LONG_PRESS_TIME` time.  Not called if dragged.*/
    LV_EVENT_LONG_PRESSED_REPEAT, /*Called after `LV_INDEV_LONG_PRESS_TIME` in every
                                     `LV_INDEV_LONG_PRESS_REP_TIME` ms.  Not called if dragged.*/
    LV_EVENT_CLICKED,             /*Called on release if not dragged (regardless to long press)*/
    LV_EVENT_RELEASED,            /*Called in every cases when the object has been released*/
    LV_EVENT_DRAG_BEGIN,
    LV_EVENT_DRAG_END,
    LV_EVENT_DRAG_THROW_BEGIN,
    LV_EVENT_KEY,
    LV_EVENT_FOCUSED,
    LV_EVENT_DEFOCUSED,
    LV_EVENT_VALUE_CHANGED,
    LV_EVENT_INSERT,
    LV_EVENT_REFRESH,
    LV_EVENT_APPLY,  /*"Ok", "Apply" or similar specific button has clicked*/
    LV_EVENT_CANCEL, /*"Close", "Cancel" or similar specific button has clicked*/
    LV_EVENT_DELETE,
};
typedef uint8_t lv_event_t;

typedef void (*lv_event_cb_t)(struct _lv_obj_t * obj, lv_event_t event);

enum {
    /*General signals*/
    LV_SIGNAL_CLEANUP,
    LV_SIGNAL_CHILD_CHG,
    LV_SIGNAL_CORD_CHG,
    LV_SIGNAL_PARENT_SIZE_CHG,
    LV_SIGNAL_STYLE_CHG,
    LV_SIGNAL_REFR_EXT_DRAW_PAD,
    LV_SIGNAL_GET_TYPE,

    /*Input device related*/
    LV_SIGNAL_PRESSED,
    LV_SIGNAL_PRESSING,
    LV_SIGNAL_PRESS_LOST,
    LV_SIGNAL_RELEASED,
    LV_SIGNAL_LONG_PRESS,
    LV_SIGNAL_LONG_PRESS_REP,
    LV_SIGNAL_DRAG_BEGIN,
    LV_SIGNAL_DRAG_END,

    /*Group related*/
    LV_SIGNAL_FOCUS,
    LV_SIGNAL_DEFOCUS,
    LV_SIGNAL_CONTROL,
    LV_SIGNAL_GET_EDITABLE,
};
typedef uint8_t lv_signal_t;

typedef lv_res_t (*lv_signal_cb_t)(struct _lv_obj_t * obj, lv_signal_t sign, void * param);

enum {
    LV_ALIGN_CENTER = 0,
    LV_ALIGN_IN_TOP_LEFT,
    LV_ALIGN_IN_TOP_MID,
    LV_ALIGN_IN_TOP_RIGHT,
    LV_ALIGN_IN_BOTTOM_LEFT,
    LV_ALIGN_IN_BOTTOM_MID,
    LV_ALIGN_IN_BOTTOM_RIGHT,
    LV_ALIGN_IN_LEFT_MID,
    LV_ALIGN_IN_RIGHT_MID,
    LV_ALIGN_OUT_TOP_LEFT,
    LV_ALIGN_OUT_TOP_MID,
    LV_ALIGN_OUT_TOP_RIGHT,
    LV_ALIGN_OUT_BOTTOM_LEFT,
    LV_ALIGN_OUT_BOTTOM_MID,
    LV_ALIGN_OUT_BOTTOM_RIGHT,
    LV_ALIGN_OUT_LEFT_TOP,
    LV_ALIGN_OUT_LEFT_MID,
    LV_ALIGN_OUT_LEFT_BOTTOM,
    LV_ALIGN_OUT_RIGHT_TOP,
    LV_ALIGN_OUT_RIGHT_MID,
    LV_ALIGN_OUT_RIGHT_BOTTOM,
};
typedef uint8_t lv_align_t;

#if LV_USE_OBJ_REALIGN
typedef struct
{
    const struct _lv_obj_t * base;
    lv_coord_t xofs;
    lv_coord_t yofs;
    lv_align_t align;
    uint8_t auto_realign : 1;
    uint8_t origo_align : 1; /*1: the oigo (center of the object) was aligned with
                                `lv_obj_align_origo`*/
} lv_reailgn_t;
#endif

enum {
    LV_DRAG_DIR_HOR = 0x1,
    LV_DRAG_DIR_VER = 0x2,
    LV_DRAG_DIR_ALL = 0x3, /* Should be the bitwise OR of the above */
};

typedef uint8_t lv_drag_dir_t;

typedef struct _lv_obj_t
{
    struct _lv_obj_t * par; /*Pointer to the parent object*/
    lv_ll_t child_ll;       /*Linked list to store the children objects*/

    lv_area_t coords; /*Coordinates of the object (x1, y1, x2, y2)*/

    lv_event_cb_t event_cb;
    lv_signal_cb_t signal_cb; /*Object type specific signal function*/
    lv_design_cb_t design_cb; /*Object type specific design function*/

    void * ext_attr;            /*Object type specific extended data*/
    const lv_style_t * style_p; /*Pointer to the object's style*/

#if LV_USE_GROUP != 0
    void * group_p; /*Pointer to the group of the object*/
#endif

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    uint8_t ext_click_pad_hor;
    uint8_t ext_click_pad_ver;
#endif

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_click_pad;
#endif

    /*Attributes and states*/
    uint8_t click : 1;          /*1: Can be pressed by an input device*/
    uint8_t drag : 1;           /*1: Enable the dragging*/
    uint8_t drag_throw : 1;     /*1: Enable throwing with drag*/
    uint8_t drag_parent : 1;    /*1: Parent will be dragged instead*/
    uint8_t hidden : 1;         /*1: Object is hidden*/
    uint8_t top : 1;            /*1: If the object or its children is clicked it goes to the foreground*/
    uint8_t opa_scale_en : 1;   /*1: opa_scale is set*/
    uint8_t parent_event : 1;   /*1: Send the object's events to the parent too. */
    lv_drag_dir_t drag_dir : 2; /* Which directions the object can be dragged in */
    uint8_t reserved : 6;       /*Reserved for future use*/
    uint8_t protect;            /*Automatically happening actions can be prevented. 'OR'ed values from
                                   `lv_protect_t`*/
    lv_opa_t opa_scale;         /*Scale down the opacity by this factor. Effects all children as well*/

    lv_coord_t ext_draw_pad; /*EXTtend the size in every direction for drawing. */

#if LV_USE_OBJ_REALIGN
    lv_reailgn_t realign;
#endif

#if LV_USE_USER_DATA
    lv_obj_user_data_t user_data;
#endif

} lv_obj_t;

/*Protect some attributes (max. 8 bit)*/
enum {
    LV_PROTECT_NONE      = 0x00,
    LV_PROTECT_CHILD_CHG = 0x01,   /*Disable the child change signal. Used by the library*/
    LV_PROTECT_PARENT    = 0x02,   /*Prevent automatic parent change (e.g. in lv_page)*/
    LV_PROTECT_POS       = 0x04,   /*Prevent automatic positioning (e.g. in lv_cont layout)*/
    LV_PROTECT_FOLLOW    = 0x08,   /*Prevent the object be followed in automatic ordering (e.g. in
                                      lv_cont PRETTY layout)*/
    LV_PROTECT_PRESS_LOST = 0x10,  /*If the `indev` was pressing this object but swiped out while
                                      pressing do not search other object.*/
    LV_PROTECT_CLICK_FOCUS = 0x20, /*Prevent focusing the object by clicking on it*/
};
typedef uint8_t lv_protect_t;

/*Used by `lv_obj_get_type()`. The object's and its ancestor types are stored here*/
typedef struct
{
    const char * type[LV_MAX_ANCESTOR_NUM]; /*[0]: the actual type, [1]: ancestor, [2] #1's ancestor
                                               ... [x]: "lv_obj" */
} lv_obj_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the 'lv' library.
 */
void lv_init(void);

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, const lv_obj_t * copy);

/**
 * Delete 'obj' and all of its children
 * @param obj pointer to an object to delete
 * @return LV_RES_INV because the object is deleted
 */
lv_res_t lv_obj_del(lv_obj_t * obj);

/**
 * Delete all children of an object
 * @param obj pointer to an object
 */
void lv_obj_clean(lv_obj_t * obj);

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(const lv_obj_t * obj);

/*=====================
 * Setter functions
 *====================*/

/*--------------------
 * Parent/children set
 *--------------------*/

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object. Can't be a screen.
 * @param parent pointer to the new parent object. (Can't be NULL)
 */
void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent);

/**
 * Move and object to the foreground
 * @param obj pointer to an object
 */
void lv_obj_move_foreground(lv_obj_t * obj);

/**
 * Move and object to the background
 * @param obj pointer to an object
 */
void lv_obj_move_background(lv_obj_t * obj);

/*--------------------
 * Coordinate set
 * ------------------*/

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, lv_coord_t x);

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y);

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h);

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align_origo(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod);

/**
 * Realign the object based on the last `lv_obj_align` parameters.
 * @param obj pointer to an object
 */
void lv_obj_realign(lv_obj_t * obj);

/**
 * Enable the automatic realign of the object when its size has changed based on the last
 * `lv_obj_align` parameters.
 * @param obj pointer to an object
 * @param en true: enable auto realign; false: disable auto realign
 */
void lv_obj_set_auto_realign(lv_obj_t * obj, bool en);

/**
 * Set the size of an extended clickable area
 * @param obj pointer to an object
 * @param left extended clickable are on the left [px]
 * @param right extended clickable are on the right [px]
 * @param top extended clickable are on the top [px]
 * @param bottom extended clickable are on the bottom [px]
 */
void lv_obj_set_ext_click_area(lv_obj_t * obj, lv_coord_t left, lv_coord_t right, lv_coord_t top, lv_coord_t bottom);

/*---------------------
 * Appearance set
 *--------------------*/

/**
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
void lv_obj_set_style(lv_obj_t * obj, const lv_style_t * style);

/**
 * Notify an object about its style is modified
 * @param obj pointer to an object
 */
void lv_obj_refresh_style(lv_obj_t * obj);

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_obj_report_style_mod(lv_style_t * style);

/*-----------------
 * Attribute set
 *----------------*/

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj pointer to an object
 * @param en true: hide the object
 */
void lv_obj_set_hidden(lv_obj_t * obj, bool en);

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void lv_obj_set_click(lv_obj_t * obj, bool en);

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void lv_obj_set_top(lv_obj_t * obj, bool en);

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void lv_obj_set_drag(lv_obj_t * obj, bool en);

/**
 * Set the directions an object can be dragged in
 * @param obj pointer to an object
 * @param drag_dir bitwise OR of allowed drag directions
 */
void lv_obj_set_drag_dir(lv_obj_t * obj, lv_drag_dir_t drag_dir);

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void lv_obj_set_drag_throw(lv_obj_t * obj, bool en);

/**
 * Enable to use parent for drag related operations.
 * If trying to drag the object the parent will be moved instead
 * @param obj pointer to an object
 * @param en true: enable the 'drag parent' for the object
 */
void lv_obj_set_drag_parent(lv_obj_t * obj, bool en);

/**
 * Propagate the events to the parent too
 * @param obj pointer to an object
 * @param en true: enable the event propagation
 */
void lv_obj_set_parent_event(lv_obj_t * obj, bool en);

/**
 * Set the opa scale enable parameter (required to set opa_scale with `lv_obj_set_opa_scale()`)
 * @param obj pointer to an object
 * @param en true: opa scaling is enabled for this object and all children; false: no opa scaling
 */
void lv_obj_set_opa_scale_enable(lv_obj_t * obj, bool en);

/**
 * Set the opa scale of an object
 * @param obj pointer to an object
 * @param opa_scale a factor to scale down opacity [0..255]
 */
void lv_obj_set_opa_scale(lv_obj_t * obj, lv_opa_t opa_scale);

/**
 * Set a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `lv_protect_t`
 */
void lv_obj_set_protect(lv_obj_t * obj, uint8_t prot);

/**
 * Clear a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `lv_protect_t`
 */
void lv_obj_clear_protect(lv_obj_t * obj, uint8_t prot);

/**
 * Set a an event handler function for an object.
 * Used by the user to react on event which happens with the object.
 * @param obj pointer to an object
 * @param event_cb the new event function
 */
void lv_obj_set_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb);

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`.
 * @param data arbitrary data depending on the object type and the event. (Usually `NULL`)
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event
 */
lv_res_t lv_event_send(lv_obj_t * obj, lv_event_t event, const void * data);

/**
 * Call an event function with an object, event, and data.
 * @param event_xcb an event callback function. If `NULL` `LV_RES_OK` will return without any actions.
 *        (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *         the `func_name(object, callback, ...)` convention)
 * @param obj pointer to an object to associate with the event (can be `NULL` to simply call the `event_cb`)
 * @param event an event
 * @param data pointer to a custom data
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event
 */
lv_res_t lv_event_send_func(lv_event_cb_t event_xcb, lv_obj_t * obj, lv_event_t event, const void * data);

/**
 * Get the `data` parameter of the current event
 * @return the `data` parameter
 */
const void * lv_event_get_data(void);

/**
 * Set the a signal function of an object. Used internally by the library.
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param signal_cb the new signal function
 */
void lv_obj_set_signal_cb(lv_obj_t * obj, lv_signal_cb_t signal_cb);

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`.
 */
void lv_signal_send(lv_obj_t * obj, lv_signal_t signal, void * param);

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param design_cb the new design function
 */
void lv_obj_set_design_cb(lv_obj_t * obj, lv_design_cb_t design_cb);

/*----------------
 * Other set
 *--------------*/

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return pointer to the allocated ext
 */
void * lv_obj_allocate_ext_attr(lv_obj_t * obj, uint16_t ext_size);

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void lv_obj_refresh_ext_draw_pad(lv_obj_t * obj);

/*=======================
 * Getter functions
 *======================*/

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_screen(const lv_obj_t * obj);

/**
 * Get the display of an object
 * @param scr pointer to an object
 * @return pointer the object's display
 */
lv_disp_t * lv_obj_get_disp(const lv_obj_t * obj);

/*---------------------
 * Parent/children get
 *--------------------*/

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj'
 */
lv_obj_t * lv_obj_get_parent(const lv_obj_t * obj);

/**
 * Iterate through the children of an object (start from the "youngest, lastly created")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(const lv_obj_t * obj, const lv_obj_t * child);

/**
 * Iterate through the children of an object (start from the "oldest", firstly created)
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child_back(const lv_obj_t * obj, const lv_obj_t * child);

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children(const lv_obj_t * obj);

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children_recursive(const lv_obj_t * obj);

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const lv_obj_t * obj, lv_area_t * cords_p);

/**
 * Reduce area retried by `lv_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_p store the result area here
 */
void lv_obj_get_inner_coords(const lv_obj_t * obj, lv_area_t * coords_p);

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
lv_coord_t lv_obj_get_x(const lv_obj_t * obj);

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
lv_coord_t lv_obj_get_y(const lv_obj_t * obj);

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
lv_coord_t lv_obj_get_width(const lv_obj_t * obj);

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
lv_coord_t lv_obj_get_height(const lv_obj_t * obj);

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container
 */
lv_coord_t lv_obj_get_width_fit(lv_obj_t * obj);

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container
 */
lv_coord_t lv_obj_get_height_fit(lv_obj_t * obj);

/**
 * Get the automatic realign property of the object.
 * @param obj pointer to an object
 * @return  true: auto realign is enabled; false: auto realign is disabled
 */
bool lv_obj_get_auto_realign(lv_obj_t * obj);

/**
 * Get the left padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended left padding
 */
lv_coord_t lv_obj_get_ext_click_pad_left(const lv_obj_t * obj);

/**
 * Get the right padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended right padding
 */
lv_coord_t lv_obj_get_ext_click_pad_right(const lv_obj_t * obj);

/**
 * Get the top padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended top padding
 */
lv_coord_t lv_obj_get_ext_click_pad_top(const lv_obj_t * obj);

/**
 * Get the bottom padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended bottom padding
 */
lv_coord_t lv_obj_get_ext_click_pad_bottom(const lv_obj_t * obj);

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
lv_coord_t lv_obj_get_ext_draw_pad(const lv_obj_t * obj);

/*-----------------
 * Appearance get
 *---------------*/

/**
 * Get the style pointer of an object (if NULL get style of the parent)
 * @param obj pointer to an object
 * @return pointer to a style
 */
const lv_style_t * lv_obj_get_style(const lv_obj_t * obj);

/*-----------------
 * Attribute get
 *----------------*/

/**
 * Get the hidden attribute of an object
 * @param obj pointer to an object
 * @return true: the object is hidden
 */
bool lv_obj_get_hidden(const lv_obj_t * obj);

/**
 * Get the click enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is clickable
 */
bool lv_obj_get_click(const lv_obj_t * obj);

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feature is enabled
 */
bool lv_obj_get_top(const lv_obj_t * obj);

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool lv_obj_get_drag(const lv_obj_t * obj);

/**
 * Get the directions an object can be dragged
 * @param obj pointer to an object
 * @return bitwise OR of allowed directions an object can be dragged in
 */
lv_drag_dir_t lv_obj_get_drag_dir(const lv_obj_t * obj);

/**
 * Get the drag throw enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool lv_obj_get_drag_throw(const lv_obj_t * obj);

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_drag_parent(const lv_obj_t * obj);

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_parent_event(const lv_obj_t * obj);

/**
 * Get the opa scale enable parameter
 * @param obj pointer to an object
 * @return true: opa scaling is enabled for this object and all children; false: no opa scaling
 */
lv_opa_t lv_obj_get_opa_scale_enable(const lv_obj_t * obj);

/**
 * Get the opa scale parameter of an object
 * @param obj pointer to an object
 * @return opa scale [0..255]
 */
lv_opa_t lv_obj_get_opa_scale(const lv_obj_t * obj);

/**
 * Get the protect field of an object
 * @param obj pointer to an object
 * @return protect field ('OR'ed values of `lv_protect_t`)
 */
uint8_t lv_obj_get_protect(const lv_obj_t * obj);

/**
 * Check at least one bit of a given protect bitfield is set
 * @param obj pointer to an object
 * @param prot protect bits to test ('OR'ed values of `lv_protect_t`)
 * @return false: none of the given bits are set, true: at least one bit is set
 */
bool lv_obj_is_protected(const lv_obj_t * obj, uint8_t prot);

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
lv_signal_cb_t lv_obj_get_signal_cb(const lv_obj_t * obj);

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
lv_design_cb_t lv_obj_get_design_cb(const lv_obj_t * obj);

/**
 * Get the event function of an object
 * @param obj pointer to an object
 * @return the event function
 */
lv_event_cb_t lv_obj_get_event_cb(const lv_obj_t * obj);

/*------------------
 * Other get
 *-----------------*/

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * lv_obj_get_ext_attr(const lv_obj_t * obj);

/**
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="lv_btn", buf.type[1]="lv_cont", buf.type[2]="lv_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `lv_obj_type_t` buffer to store the types
 */
void lv_obj_get_type(lv_obj_t * obj, lv_obj_type_t * buf);

#if LV_USE_USER_DATA
/**
 * Get the object's user data
 * @param obj pointer to an object
 * @return user data
 */
lv_obj_user_data_t lv_obj_get_user_data(lv_obj_t * obj);

/**
 * Get a pointer to the object's user data
 * @param obj pointer to an object
 * @return pointer to the user data
 */
lv_obj_user_data_t * lv_obj_get_user_data_ptr(lv_obj_t * obj);

/**
 * Set the object's user data. The data will be copied.
 * @param obj pointer to an object
 * @param data user data
 */
void lv_obj_set_user_data(lv_obj_t * obj, lv_obj_user_data_t data);

#endif

#if LV_USE_GROUP
/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * lv_obj_get_group(const lv_obj_t * obj);

/**
 * Tell whether the object is the focused object of a group or not.
 * @param obj pointer to an object
 * @return true: the object is focused, false: the object is not focused or not in a group
 */
bool lv_obj_is_focused(const lv_obj_t * obj);

#endif

/**********************
 *      MACROS
 **********************/

/**
 * Helps to quickly declare an event callback function.
 * Will be expanded to: `void <name> (lv_obj_t * obj, lv_event_t e)`
 *
 * Examples:
 * static LV_EVENT_CB_DECLARE(my_event1);  //Protoype declaration
 *
 * static LV_EVENT_CB_DECLARE(my_event1)
 * {
 *   if(e == LV_EVENT_CLICKED) {
 *      lv_obj_set_hidden(obj ,true);
 *   }
 * }
 */
#define LV_EVENT_CB_DECLARE(name) void name(lv_obj_t * obj, lv_event_t e)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_H*/
