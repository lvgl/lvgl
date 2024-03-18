/**
 * @file lv_win.h
 *
 */

#ifndef LV_WIN_H
#define LV_WIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_WIN
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;
} lv_win_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_win_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_win_create(lv_obj_t * parent);

lv_obj_t * lv_win_add_title(lv_obj_t * win, const char * txt);
lv_obj_t * lv_win_add_button(lv_obj_t * win, const void * icon, int32_t btn_w);

lv_obj_t * lv_win_get_header(lv_obj_t * win);
lv_obj_t * lv_win_get_content(lv_obj_t * win);
/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_WIN*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WIN_H*/
