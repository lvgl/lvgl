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
#include "../core/lv_obj.h"

#if LV_USE_LIST

#if LV_USE_FLEX == 0
#error "lv_list: lv_flex is required. Enable it in lv_conf.h (LV_USE_FLEX 1)"
#endif

/**
 * @deprecated The `lv_list` widget is deprecated and kept only for backward
 * compatibility. A list is just a flex container with a column flow, so build
 * one directly from `lv_obj` + a `LV_FLEX_FLOW_COLUMN` layout instead. See the
 * `lv_example_flex_list` example for a starting point.
 */
#define LV_LIST_DEPRECATED_MSG \
    "lv_list is deprecated; build a list from a flex column instead. See the lv_example_flex_list example."

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
 * @deprecated Use a flex container with `LV_FLEX_FLOW_COLUMN` instead. See `lv_example_flex_list`.
 */
LV_DEPRECATED(LV_LIST_DEPRECATED_MSG)
lv_obj_t * lv_list_create(lv_obj_t * parent);

/**
 * Add text to a list
 * @param list      pointer to a list, it will be the parent of the new label
 * @param txt       text of the new label
 * @return          pointer to the created label
 * @deprecated Add a full-width `lv_label` to a flex container instead. See `lv_example_flex_list`.
 */
lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt);

/**
 * Add button to a list
 * @param list      pointer to a list, it will be the parent of the new button
 * @param icon      icon for the button, when NULL it will have no icon
 * @param txt       text of the new button, when NULL no text will be added
 * @return          pointer to the created button
 * @deprecated Add a full-width `lv_button` to a flex container instead. See `lv_example_flex_list`.
 */
lv_obj_t * lv_list_add_button(lv_obj_t * list, const void * icon, const char * txt);

/**
 * Get text of a given list button
 * @param list      pointer to a list
 * @param btn       pointer to the button
 * @return          text of btn, if btn doesn't have text "" will be returned
 * @deprecated The `lv_list` widget is deprecated. See `lv_example_flex_list`.
 */
const char * lv_list_get_button_text(lv_obj_t * list, lv_obj_t * btn);

/**
 * Set text of a given list button
 * @param list      pointer to a list
 * @param btn       pointer to the button
 * @param txt       pointer to the text
 * @deprecated The `lv_list` widget is deprecated. See `lv_example_flex_list`.
 */
LV_DEPRECATED(LV_LIST_DEPRECATED_MSG)
void lv_list_set_button_text(lv_obj_t * list, lv_obj_t * btn, const char * txt);

#if LV_USE_TRANSLATION

/**
 * Add translation tag text to a list
 * @param list      pointer to a list, it will be the parent of the new label
 * @param tag       translation tag of the new label
 * @return          pointer to the created label
 * @deprecated The `lv_list` widget is deprecated. See `lv_example_flex_list`.
 */
lv_obj_t * lv_list_add_translation_tag(lv_obj_t * list, const char * tag);

/**
 * Add translation tag button to a list
 * @param list      pointer to a list, it will be the parent of the new button
 * @param icon      icon for the button, when NULL it will have no icon
 * @param tag       translation tag of the new button, when NULL no translation tag will be added
 * @return          pointer to the created button
 * @deprecated The `lv_list` widget is deprecated. See `lv_example_flex_list`.
 */
lv_obj_t * lv_list_add_button_translation_tag(lv_obj_t * list, const void * icon, const char * tag);

/**
 * Set translation tag text of a given list button
 * @param list      pointer to a list
 * @param btn       pointer to the button
 * @param tag       pointer to the translation tag
 * @deprecated The `lv_list` widget is deprecated. See `lv_example_flex_list`.
 */
void lv_list_set_button_translation_tag(lv_obj_t * list, lv_obj_t * btn, const char * tag);

#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LIST_H*/
