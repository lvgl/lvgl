/**
 * @file lv_templ.h
 * 
 */


/*Search an replace: template -> object normal name with lower case (e.g. button, label etc.)
 * 					 templ -> object short name with lower case(e.g. btn, label etc)
 *                   TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_TEMPL_H
#define LV_TEMPL_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TEMPL != 0

#include "../lv_obj/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of template*/
typedef struct
{
	/*Style of ancestor*/
	/*New style element for this type */
}lv_templs_t;

/*Built-in styles of template*/
typedef enum
{
	LV_TEMPLS_DEF,
}lv_templs_builtin_t;

/*Data of template*/
typedef struct
{
	/*Ext. of ancestor*/
	/*New data for this type */
}lv_templ_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_templ_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_templ_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
lv_templs_t * lv_templs_get(lv_templs_builtin_t style, lv_templs_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
