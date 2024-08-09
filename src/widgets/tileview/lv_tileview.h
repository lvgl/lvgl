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
#include "../../core/lv_obj.h"

#if LV_USE_TILEVIEW

/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_tileview_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_tileview_tile_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a Tileview object
 * @param parent pointer to an object, it will be the parent of the new tileview
 * @return pointer to the created tileview
 */
lv_obj_t * lv_tileview_create(lv_obj_t * parent);

lv_obj_t * lv_tileview_add_tile(lv_obj_t * tv, uint8_t col_id, uint8_t row_id, lv_dir_t dir);

void lv_tileview_set_tile(lv_obj_t * tv, lv_obj_t * tile_obj, lv_anim_enable_t anim_en);
void lv_tileview_set_tile_by_index(lv_obj_t * tv, uint32_t col_id, uint32_t row_id, lv_anim_enable_t anim_en);

lv_obj_t * lv_tileview_get_tile_active(lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TILEVIEW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TILEVIEW_H*/
