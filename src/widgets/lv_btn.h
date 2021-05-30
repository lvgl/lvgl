/**
 * @file lv_btn.h
 *
 */

#ifndef LV_BTN_H
#define LV_BTN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_BTN != 0
#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
}lv_btn_t;

extern const lv_obj_class_t lv_btn_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a button object
 * @param parent    pointer to an object, it will be the parent of the new button
 * @return          pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * parent);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BTN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BTN_H*/
