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
 * Create a tileview object
 * @param parent      pointer to an object, it will be the parent of the new tileview
 * @return            pointer to the created tileview
 */
lv_obj_t * lv_tileview_create(lv_obj_t * parent);

/**
 * Add a tile to the tileview
 * @param tv          pointer to the tileview object
 * @param col_id      column id of the tile
 * @param row_id      row id of the tile
 * @param dir         direction to move to the next tile
 * @return            pointer to the added tile object
 */
lv_obj_t * lv_tileview_add_tile(lv_obj_t * tv, uint8_t col_id, uint8_t row_id, lv_dir_t dir);

/**
 * Set the active tile in the tileview.
 * @param parent      pointer to the tileview object
 * @param tile_obj    pointer to the tile object to be set as active
 * @param anim_en     animation enable flag (LV_ANIM_ON or LV_ANIM_OFF)
 */
void lv_tileview_set_tile(lv_obj_t * tv, lv_obj_t * tile_obj, lv_anim_enable_t anim_en);

/**
 * Set the active tile by index in the tileview
 * @param tv          pointer to the tileview object
 * @param col_id      column id of the tile to be set as active
 * @param row_id      row id of the tile to be set as active
 * @param anim_en     animation enable flag (LV_ANIM_ON or LV_ANIM_OFF)
 */
void lv_tileview_set_tile_by_index(lv_obj_t * tv, uint32_t col_id, uint32_t row_id, lv_anim_enable_t anim_en);

/**
 * Get the currently active tile in the tileview
 * @param obj         pointer to the tileview object
 * @return            pointer to the currently active tile object
 */
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
