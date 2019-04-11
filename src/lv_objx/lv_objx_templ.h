/**
 * @file lv_templ.h
 *
 */

/* TODO Remove these instructions
 * Search an replace: template -> object normal name with lower case (e.g. button, label etc.)
 *                    templ -> object short name with lower case(e.g. btn, label etc)
 *                    TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_TEMPL_H
#define LV_TEMPL_H

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

#if LV_USE_TEMPL != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of template*/
typedef struct
{
    lv_ANCESTOR_ext_t ANCESTOR; /*Ext. of ancestor*/
    /*New data for this type */
} lv_templ_ext_t;

/*Styles*/
enum {
    LV_TEMPL_STYLE_X,
    LV_TEMPL_STYLE_Y,
};
typedef uint8_t lv_templ_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a template objects
 * @param par pointer to an object, it will be the parent of the new template
 * @param copy pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
lv_obj_t * lv_templ_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a template.
 * @param templ pointer to template object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_templ_set_style(lv_obj_t * templ, lv_templ_style_t type, const lv_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a template.
 * @param templ pointer to template object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_templ_get_style(const lv_obj_t * templ, lv_templ_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEMPL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/
