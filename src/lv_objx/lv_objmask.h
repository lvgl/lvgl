/**
 * @file lv_objmask.h
 *
 */

#ifndef LV_OBJMASK_H
#define LV_OBJMASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_OBJMASK != 0

#include "../lv_core/lv_obj.h"
#include "../lv_objx/lv_cont.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void * param;
}lv_objmask_mask_t;

/*Data of object mask*/
typedef struct
{
    lv_cont_ext_t cont; /*Ext. of ancestor*/
    /*New data for this type */
    lv_ll_t mask_ll;    /*Store the created masks*/

} lv_objmask_ext_t;

/*Styles*/
enum {
    LV_OBJMASK_STYLE_BG,
};
typedef uint8_t lv_objmask_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a object mask objects
 * @param par pointer to an object, it will be the parent of the new object mask
 * @param copy pointer to a object mask object, if not NULL then the new object will be copied from it
 * @return pointer to the created object mask
 */
lv_obj_t * lv_objmask_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a mask
 * @param objmask pointer to an Object mask object
 * @param param an initialized mask parameter
 * @return pointer to the added mask
 */
lv_objmask_mask_t * lv_objmask_add_mask(lv_obj_t * objmask, void * param);

/**
 * Update an already created mask
 * @param objmask pointer to an Object mask object
 * @param mask pointer to created mask (returned by `lv_objmask_add_mask`)
 * @param param an initialized mask parameter (initialized by `lv_draw_mask_line/angle/.../_init`)
 */
void lv_objmask_update_mask(lv_obj_t * objmask, lv_objmask_mask_t * mask, void * param);

/**
 * Remove a mask
 * @param objmask pointer to an Object mask object
 * @param mask pointer to created mask (returned by `lv_objmask_add_mask`)
 * If `NULL` passed all masks will be deleted.
 */
void lv_objmask_remove_mask(lv_obj_t * objmask, lv_objmask_mask_t * mask);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the style of a object mask
 * @param objmask pointer to a container object
 * @param type which style should be set (can be only `LV_CONT_STYLE_MAIN`)
 * @param style pointer to the new style
 */
static inline void lv_objmask_set_style(lv_obj_t * objmask, lv_cont_style_t type, const lv_style_t * style)
{
    lv_cont_set_style(objmask, type, style);
}

/*=====================
 * Getter functions
 *====================*/
/**
 * Get the style of an object mask
 * @param objmask pointer to a container object
 * @param type which style should be get (can be only `LV_CONT_STYLE_MAIN`)
 * @return pointer to the container's style
 */
static inline const lv_style_t * lv_objmask_get_style(const lv_obj_t * objmask, lv_cont_style_t type)
{
    return lv_cont_get_style(objmask, type);
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OBJMASK*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJMASK_H*/
