/**
 * @file lv_tileview.h
 *
 */

#ifndef LV_TILEVIEW_H
#define LV_TILEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_core/lv_obj.h"

#if LV_USE_TILEVIEW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_obj_t obj;
}lv_tileview_t;

typedef struct {
    lv_obj_t obj;
    lv_dir_t dir;
}lv_tileview_tile_t;

extern const lv_obj_class_t lv_tileview_class;
extern const lv_obj_class_t lv_tileview_tile_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a tileview objects
 * @param par pointer to an object, it will be the parent of the new tileview
 * @param copy pointer to a tileview object, if not NULL then the new object will be copied from it
 * @return pointer to the created tileview
 */
lv_obj_t * lv_tileview_create(lv_obj_t * parent);

lv_obj_t * lv_tileview_add_tile(lv_obj_t * tv, uint8_t row_id, uint8_t col_id, lv_dir_t dir);



/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TILEVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TILEVIEW_H*/
