/**
 * @file lv_templ.h
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

/*Style of TEMPL*/
typedef struct
{

}lv_templs_t;


/*Built-in styles of TEMPL*/
typedef enum
{
	LV_TEMPLS_DEF,
}lv_templs_builtin_t;

/*Data of TEMPL*/
typedef struct
{

}lv_templ_ext_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_templ_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_templ_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
lv_templs_t * lv_templs_get(lv_templs_builtin_t style, lv_templs_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
