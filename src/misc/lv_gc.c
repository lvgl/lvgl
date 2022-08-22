/**
 * @file lv_gc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if(!defined(LV_ENABLE_GC)) || LV_ENABLE_GC == 0
    LV_ROOTS
#endif /*LV_ENABLE_GC*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_gc_clear_roots(void)
{
#define LV_CLEAR_ROOT(root_type, root_name) lv_memzero(&LV_GC_ROOT(root_name), sizeof(LV_GC_ROOT(root_name)));
    LV_ITERATE_ROOTS(LV_CLEAR_ROOT)
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
