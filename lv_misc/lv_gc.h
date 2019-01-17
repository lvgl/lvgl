/**
 * @file lv_gc.h
 *
 */

#ifndef LV_GC_H
#define LV_GC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#include <stdint.h>
#include <stdbool.h>
#include "lv_mem.h"
#include "lv_ll.h"

/*********************
 *      DEFINES
 *********************/

#define LV_GC_ROOTS(prefix) \
    prefix lv_ll_t _lv_task_ll;      /*Linked list to store the lv_tasks*/ \
    prefix lv_ll_t _scr_ll;          /*Linked list of screens*/ \
    prefix lv_ll_t _drv_ll;\
    prefix lv_ll_t _file_ll;\
    prefix lv_ll_t _anim_ll;\
    prefix void * _def_scr;\
    prefix void * _act_scr;\
    prefix void * _top_layer;\
    prefix void * _sys_layer;\
    prefix void * _task_act;\
    prefix void * _indev_list;\
    prefix void * _disp_list;\
    
    
#define LV_NO_PREFIX
#define LV_ROOTS LV_GC_ROOTS(LV_NO_PREFIX)

#if LV_ENABLE_GC == 1
#   if LV_MEM_CUSTOM != 1
#       error "GC requires CUSTOM_MEM"
#   endif /* LV_MEM_CUSTOM */    
#else /* LV_ENABLE_GC */
#   define LV_GC_ROOT(x) x
    LV_GC_ROOTS(extern)
#endif /* LV_ENABLE_GC */


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GC_H*/
