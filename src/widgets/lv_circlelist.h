/**
 * @file lv_circlelist.h
 *
 */

#ifndef LV_CIRCLELIST_H
#define LV_CIRCLELIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../core/lv_obj.h"

#if LV_USE_CIRCLELIST != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_coord_t width;
    lv_coord_t height;
    lv_obj_t* obj;
}lv_circlelist_item_t;

typedef struct {
    lv_obj_t obj;
    uint16_t count; /*Children count.*/
    lv_circlelist_item_t* items;
    lv_point_t press_point;
    lv_circlelist_item_t* focus;
    uint16_t focus_item_index;
    float focus_y; /* Normalized y pos inside of the focused object. */

    lv_coord_t radius;
    lv_point_t center;
    lv_point_t start; /* Left-up corner of the drawing area  */
} lv_circlelist_t;

extern const lv_obj_class_t lv_circlelist_class;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a circle list object
 * @param parent    pointer to an object, it will be the parent of the new button
 * @return          pointer to the created circle list
 */
lv_obj_t* lv_circlelist_create(lv_obj_t* parent);

/*=====================
 * Setter functions
 *====================*/

void lv_circlelist_add(lv_obj_t* obj, lv_obj_t* child, lv_coord_t h);

/*=====================
 * Getter functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CIRCLE_LIST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CIRCLE_LIST_H*/
