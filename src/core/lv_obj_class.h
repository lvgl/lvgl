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
#include <stdint.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;
struct _lv_obj_class_t;
struct _lv_event_t;

typedef enum {
    LV_OBJ_CLASS_EDITABLE_INHERIT,      /**< Check the base class. Must have 0 value to let zero initialized class inherit*/
    LV_OBJ_CLASS_EDITABLE_TRUE,
    LV_OBJ_CLASS_EDITABLE_FALSE,
}lv_obj_class_editable_t;

typedef enum {
    LV_OBJ_CLASS_GROUP_DEF_INHERIT,      /**< Check the base class. Must have 0 value to let zero initialized class inherit*/
    LV_OBJ_CLASS_GROUP_DEF_TRUE,
    LV_OBJ_CLASS_GROUP_DEF_FALSE,
}lv_obj_class_group_def_t;

typedef void (*lv_obj_class_event_cb_t)(struct _lv_obj_class_t * class_p, struct _lv_event_t * e);
/**
 * Describe the common methods of every object.
 * Similar to a C++ class.
 */
typedef struct _lv_obj_class_t {
    const struct _lv_obj_class_t * base_class;
    void (*constructor_cb)(const struct _lv_obj_class_t * class_p, struct _lv_obj_t * obj);
    void (*destructor_cb)(const struct _lv_obj_class_t * class_p, struct _lv_obj_t * obj);
#if LV_USE_USER_DATA
    void * user_data;
#endif
    void (*event_cb)(const struct _lv_obj_class_t * class_p, struct _lv_event_t * e);  /**< Widget type specific event function*/
    lv_coord_t width_def;
    lv_coord_t height_def;
    uint32_t editable : 2;             /**< Value from ::lv_obj_class_editable_t*/
    uint32_t group_def : 2;            /**< Value from ::lv_obj_class_group_def_t*/
    uint32_t instance_size : 16;
}lv_obj_class_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an object form a class descriptor
 * @param class_p   pointer to a class
 * @param parent    pointer to an object where the new object should be created
 * @return          pointer to the created object
 */
struct _lv_obj_t * lv_obj_class_create_obj(const struct _lv_obj_class_t * class_p, struct _lv_obj_t * parent);

void lv_obj_class_init_obj(struct _lv_obj_t * obj);

void _lv_obj_destructor(struct _lv_obj_t * obj);

bool lv_obj_is_editable(struct _lv_obj_t * obj);

bool lv_obj_is_group_def(struct _lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_CLASS_H*/
