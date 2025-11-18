/**
 * @file lv_list.h
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

#if LV_USE_FLEX == 0
#error "lv_list: lv_flex is required. Enable it in lv_conf.h (LV_USE_FLEX 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_list_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_list_text_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_list_button_class;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a list object
 * @param parent    pointer to an object, it will be the parent of the new list
 * @return          pointer to the created list
 */
lv_obj_t * lv_list_create(lv_obj_t * parent);

/**
 * Add text to a list
 * @param list      pointer to a list, it will be the parent of the new label
 * @param txt       text of the new label
 * @return          pointer to the created label
 */
lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt);

/**
 * Add button to a list
 * @param list      pointer to a list, it will be the parent of the new button
 * @param icon      icon for the button, when NULL it will have no icon
 * @param txt       text of the new button, when NULL no text will be added
 * @return          pointer to the created button
 */
lv_obj_t * lv_list_add_button(lv_obj_t * list, const void * icon, const char * txt);

/**
 * Get text of a given list button
 * @param list      pointer to a list
 * @param btn       pointer to the button
 * @return          text of btn, if btn doesn't have text "" will be returned
 */
const char * lv_list_get_button_text(lv_obj_t * list, lv_obj_t * btn);

/**
 * Set text of a given list button
 * @param list      pointer to a list
 * @param btn       pointer to the button
 * @param txt       pointer to the text
 */
void lv_list_set_button_text(lv_obj_t * list, lv_obj_t * btn, const char * txt);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LIST_H*/
