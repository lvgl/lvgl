/**
 * @file lv_roller.h
 * 
 */


/*Search an replace: roller -> object normal name with lower case (e.g. button, label etc.)
 * 					 roller -> object short name with lower case(e.g. btn, label etc)
 *                   ROLLER -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_ROLLER_H
#define LV_ROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_ROLLER != 0

#include "../lv_obj/lv_obj.h"
#include "lv_ddlist.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of roller*/
typedef struct {
    lv_ddlist_ext_t ddlist; /*Ext. of ancestor*/
    /*New data for this type */
}lv_roller_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a roller objects
 * @param par pointer to an object, it will be the parent of the new roller
 * @param copy pointer to a roller object, if not NULL then the new object will be copied from it
 * @return pointer to the created roller
 */
lv_obj_t * lv_roller_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the roller
 * @param roller pointer to a roller object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_roller_signal(lv_obj_t * roller, lv_signal_t sign, void * param);


/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_ROLLER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_ROLLER_H*/
