/**
 * @file lv_templ.h
 *
 */

/**
 * TODO Remove these instructions
 * Search and replace: templ -> object short name with lower case(e.g. btn, label etc)
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
#include "../lv_conf_internal.h"

#if LV_USE_TEMPL != 0

#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of template*/
typedef struct {
    lv_ANCESTOR_t ancestor; /*The ancestor widget, e.g. lv_slider_t slider*/
    /*New data for this type*/
} lv_templ_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_templ_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a templ object
 * @param parent    pointer to an object, it will be the parent of the new templ
 * @return          pointer to the created bar
 */
lv_obj_t * lv_templ_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEMPL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEMPL_H*/
