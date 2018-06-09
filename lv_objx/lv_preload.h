/**
 * @file lv_preload.h
 * 
 */


/* TODO Remove these instructions
 * Search an replace: pre loader -> object normal name with lower case (e.g. button, label etc.)
 *                    preload -> object short name with lower case(e.g. btn, label etc)
 *                    PRELOAD -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_PRELOAD_H
#define LV_PRELOAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_PRELOAD != 0

#include "../lv_core/lv_obj.h"
#include "lv_arc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
	LV_PRELOAD_TYPE_SPINNING_ARC,
}lv_preloader_type_t;

/*Data of pre loader*/
typedef struct {
    lv_arc_ext_t arc; /*Ext. of ancestor*/
    /*New data for this type */
    lv_preloader_type_t type;
    uint16_t indic_length;			/*Length of the spinning indicator in degree*/
    uint16_t time;					/*Time of one round*/
}lv_preload_ext_t;


/*Styles*/
typedef enum {
	LV_PRELOAD_STYLE_MAIN,
}lv_preload_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a pre loader objects
 * @param par pointer to an object, it will be the parent of the new pre loader
 * @param copy pointer to a pre loader object, if not NULL then the new object will be copied from it
 * @return pointer to the created pre loader
 */
lv_obj_t * lv_preload_create(lv_obj_t * par, lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/


/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_preload_set_style(lv_obj_t * preload, lv_preload_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
lv_style_t * lv_preload_get_style(lv_obj_t * preload, lv_preload_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_PRELOAD*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_PRELOAD_H*/
