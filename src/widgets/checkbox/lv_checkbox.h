/**
 * @file lv_checkbox.h
 *
 */

#ifndef LV_CHECKBOX_H
#define LV_CHECKBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_CHECKBOX != 0

/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_checkbox_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a check box object
 * @param parent    pointer to an object, it will be the parent of the new button
 * @return          pointer to the created check box
 */
lv_obj_t * lv_checkbox_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param obj   pointer to a check box
 * @param txt   the text of the check box. NULL to refresh with the current text.
 */
void lv_checkbox_set_text(lv_obj_t * obj, const char * txt);

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param obj   pointer to a check box
 * @param txt   the text of the check box.
 */
void lv_checkbox_set_text_static(lv_obj_t * obj, const char * txt);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param obj   pointer to check box object
 * @return      pointer to the text of the check box
 */
const char * lv_checkbox_get_text(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHECKBOX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHECKBOX_H*/
