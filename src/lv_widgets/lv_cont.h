/**
 * @file lv_cont.h
 *
 */

#ifndef LV_CONT_H
#define LV_CONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_CONT != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Container layout options*/
enum {
    LV_LAYOUT_OFF = 0, /**< No layout */
    LV_LAYOUT_CENTER, /**< Center objects */

    /**
     * COLUMN:
     * - Place the object below each other
     * - Keep `pad_top` space on the top
     * - Keep `pad_inner` space between the objects
     */
    LV_LAYOUT_COLUMN_LEFT,      /**< Column left align*/
    LV_LAYOUT_COLUMN_MID,       /**< Column middle align*/
    LV_LAYOUT_COLUMN_RIGHT,     /**< Column right align*/

    /**
     * ROW:
     * - Place the object next to each other
     * - Keep `pad_left` space on the left
     * - Keep `pad_inner` space between the objects
     * - If the object which applies the layout has `base_dir == LV_BIDI_DIR_RTL`
     *   the row will start from the right applying `pad.right` space
     */
    LV_LAYOUT_ROW_TOP,          /**< Row top align*/
    LV_LAYOUT_ROW_MID,          /**< Row middle align*/
    LV_LAYOUT_ROW_BOTTOM,       /**< Row bottom align*/

    /**
     * PRETTY:
     * - Place the object next to each other
     * - If there is no more space start a new row
     * - Respect `pad_left` and `pad_right` when determining the available space in a row
     * - Keep `pad_inner` space between the objects in the same row
     * - Keep `pad_inner` space between the objects in rows
     * - Divide the remaining horizontal space equally
     */
    LV_LAYOUT_PRETTY_TOP,       /**< Row top align*/
    LV_LAYOUT_PRETTY_MID,       /**< Row middle align*/
    LV_LAYOUT_PRETTY_BOTTOM,    /**< Row bottom align*/

    /**
     * GRID
     * - Place the object next to each other
     * - If there is no more space start a new row
     * - Respect `pad_left` and `pad_right` when determining the available space in a row
     * - Keep `pad_inner` space between the objects in the same row
     * - Keep `pad_inner` space between the objects in rows
     * - Unlike `PRETTY`, `GRID` always keep `pad_inner` space horizontally between objects
     *   so it doesn't divide the remaining horizontal space equally
     */
    LV_LAYOUT_GRID,   /**< Align same-sized object into a grid*/

    _LV_LAYOUT_LAST
};
typedef uint8_t lv_layout_t;

/**
 * How to resize the container around the children.
 */
enum {
    LV_FIT_NONE,  /**< Do not change the size automatically*/
    LV_FIT_TIGHT, /**< Shrink wrap around the children */
    LV_FIT_PARENT, /**< Align the size to the parent's edge*/
    LV_FIT_MAX,  /**< Align the size to the parent's edge first but if there is an object out of it
                     then get larger */
    _LV_FIT_LAST
};
typedef uint8_t lv_fit_t;

typedef struct {
    /*Inherited from 'base_obj' so no inherited ext. */ /*Ext. of ancestor*/
    /*New data for this type */
    lv_layout_t layout : 4;     /*A layout from 'lv_layout_t' enum*/
    lv_fit_t fit_left : 2;   /*A fit type from `lv_fit_t` enum */
    lv_fit_t fit_right : 2;  /*A fit type from `lv_fit_t` enum */
    lv_fit_t fit_top : 2;    /*A fit type from `lv_fit_t` enum */
    lv_fit_t fit_bottom : 2; /*A fit type from `lv_fit_t` enum */
} lv_cont_ext_t;

/*Part of the container*/
enum {
    LV_CONT_PART_MAIN = LV_OBJ_PART_MAIN,
    _LV_CONT_PART_VIRTUAL_LAST = _LV_OBJ_PART_VIRTUAL_LAST,
    _LV_CONT_PART_REAL_LAST = _LV_OBJ_PART_REAL_LAST,
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
lv_obj_t * lv_cont_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'lv_cont_layout_t'
 */
void lv_cont_set_layout(lv_obj_t * cont, lv_layout_t layout);

/**
 * Set the fit policy in all 4 directions separately.
 * It tell how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param left left fit policy from `lv_fit_t`
 * @param right right fit policy from `lv_fit_t`
 * @param top top fit policy from `lv_fit_t`
 * @param bottom bottom fit policy from `lv_fit_t`
 */
void lv_cont_set_fit4(lv_obj_t * cont, lv_fit_t left, lv_fit_t right, lv_fit_t top, lv_fit_t bottom);

/**
 * Set the fit policy horizontally and vertically separately.
 * It tells how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param hor horizontal fit policy from `lv_fit_t`
 * @param ver vertical fit policy from `lv_fit_t`
 */
static inline void lv_cont_set_fit2(lv_obj_t * cont, lv_fit_t hor, lv_fit_t ver)
{
    lv_cont_set_fit4(cont, hor, hor, ver, ver);
}

/**
 * Set the fit policy in all 4 direction at once.
 * It tells how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param fit fit policy from `lv_fit_t`
 */
static inline void lv_cont_set_fit(lv_obj_t * cont, lv_fit_t fit)
{
    lv_cont_set_fit4(cont, fit, fit, fit, fit);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'lv_cont_layout_t'
 */
lv_layout_t lv_cont_get_layout(const lv_obj_t * cont);

/**
 * Get left fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_left(const lv_obj_t * cont);

/**
 * Get right fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_right(const lv_obj_t * cont);

/**
 * Get top fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_top(const lv_obj_t * cont);

/**
 * Get bottom fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_bottom(const lv_obj_t * cont);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CONT*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CONT_H*/
