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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

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
    uint8_t id;
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
void lv_objmask_add_mask(lv_obj_t * objmask, void * param,  uint8_t id);

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
