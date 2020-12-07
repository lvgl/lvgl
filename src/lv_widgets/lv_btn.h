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

#include "../lv_core/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**Styles*/
enum {
    LV_BTN_PART_MAIN = LV_OBJ_PART_MAIN,
};
typedef uint8_t lv_btn_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a button object
 * @param parent pointer to an object, it will be the parent of the new button
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other button to copy.
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * parent, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BUTTON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTN_H*/
