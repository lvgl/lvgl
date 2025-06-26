/**
 * @file lv_obj_class.h
 *
 */

#ifndef LV_OBJ_CLASS_H
#define LV_OBJ_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_area.h"
#include "lv_obj_property.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_OBJ_CLASS_EDITABLE_INHERIT,      /**< Check the base class. Must have 0 value to let zero initialized class inherit*/
    LV_OBJ_CLASS_EDITABLE_TRUE,
    LV_OBJ_CLASS_EDITABLE_FALSE,
} lv_obj_class_editable_t;

typedef enum {
    LV_OBJ_CLASS_GROUP_DEF_INHERIT,      /**< Check the base class. Must have 0 value to let zero initialized class inherit*/
    LV_OBJ_CLASS_GROUP_DEF_TRUE,
    LV_OBJ_CLASS_GROUP_DEF_FALSE,
} lv_obj_class_group_def_t;

typedef enum {
    LV_OBJ_CLASS_THEME_INHERITABLE_FALSE,    /**< Do not inherit theme from base class. */
    LV_OBJ_CLASS_THEME_INHERITABLE_TRUE,
} lv_obj_class_theme_inheritable_t;

typedef void (*lv_obj_class_event_cb_t)(lv_obj_class_t * class_p, lv_event_t * e);
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an object form a class descriptor
 * @param class_p   pointer to a class
 * @param parent    pointer to an object where the new object should be created
 * @return          pointer to the created object
 */
lv_obj_t * lv_obj_class_create_obj(const lv_obj_class_t * class_p, lv_obj_t * parent);

void lv_obj_class_init_obj(lv_obj_t * obj);

bool lv_obj_is_editable(lv_obj_t * obj);

bool lv_obj_is_group_def(lv_obj_t * obj);

#if LV_EXTERNAL_DATA_AND_DESTRUCTOR
/**
 * @brief Associates an array of external data pointers with an LVGL object
 *
 * Attaches multiple user-defined data pointers to an LVGL object along with an optional
 * destructor callback that will be called when the object is deleted. The function:
 * - Safely handles NULL checks for object and input array
 * - Respects LV_EXT_DATA_MAX_NUM limit
 * - Avoids duplicate entries
 * - Preserves existing non-NULL data slots
 *
 * @param obj Target LVGL object (if NULL, function returns early)
 * @param ext_data Array of data pointers to associate (elements can be NULL)
 * @param ext_data_num Number of elements in ext_data array
 * @param destructor Cleanup function called for each non-NULL data pointer during
 *                   object deletion. Receives single data pointer as parameter.
 *                   NULL means no automatic cleanup.
 */
void lv_obj_set_external_data(lv_obj_t * obj, void * ext_data[], int ext_data_num,
                              void (* destructor)(void * ext_data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_CLASS_H*/
