/**
 * @file lv_win.h
 *
 */

#ifndef LV_LIST_H
#define LV_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_LIST

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

extern const lv_obj_class_t lv_list_class;
extern const lv_obj_class_t lv_list_text_class;
extern const lv_obj_class_t lv_list_btn_class;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_list_create(lv_obj_t * parent);

lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt);

lv_obj_t * lv_list_add_btn(lv_obj_t * list, const void * icon, const char * txt);

const char * lv_list_get_btn_text(lv_obj_t * list, lv_obj_t * btn);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LIST_H*/
