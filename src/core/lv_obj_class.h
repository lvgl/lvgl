/**
 * @file struct _lv_obj_tree.h
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
#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;

typedef enum {
    LV_OBJ_CLASS_EDITABLE_INHERIT,      /**< Check the base class. Must have 0 value to let he zero initialized class inherit*/
    LV_OBJ_CLASS_EDITABLE_TRUE,
    LV_OBJ_CLASS_EDITABLE_FALSE,
}lv_obj_class_editable_t;

/**
 * Describe the common methods of every object.
 * Similar to a C++ class.
 */
typedef struct _lv_obj_class_t{
    const struct _lv_obj_class_t * base_class;
    void (*constructor_cb)(struct _lv_obj_t * obj, const struct _lv_obj_t * copy);
    void (*destructor_cb)(struct _lv_obj_t * obj);
    lv_signal_cb_t signal_cb;       /**< Object type specific signal function*/
    lv_draw_cb_t draw_cb;           /**< Object type specific draw function*/
    uint32_t editable :2;           /**< Value from ::lv_obj_class_editable_t*/
    uint32_t instance_size :20;
}lv_obj_class_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an object form a class descriptor
 * @param class_p pointer to  a class
 * @param parent pointer to an object where the new object should be created
 * @param copy pointer to an other object with the same type to copy (DEPRECATED will be removed in v9)
 * @return pointer to the created object
 */
struct _lv_obj_t * lv_obj_create_from_class(const struct _lv_obj_class_t * class_p, struct _lv_obj_t * parent, const struct _lv_obj_t * copy);

void _lv_obj_destruct(struct _lv_obj_t * obj);

lv_res_t lv_obj_signal_base(const lv_obj_class_t * class_p, struct _lv_obj_t * obj, lv_signal_t sign, void * param);

lv_draw_res_t lv_obj_draw_base(const lv_obj_class_t * class_p, struct _lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);

bool lv_obj_is_editable(struct _lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_CLASS_H*/
