/**
 * @file lv_obj_private.h
 *
 */

#ifndef LV_OBJ_PRIVATE_H
#define LV_OBJ_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Special, rarely used attributes.
 * They are allocated automatically if any elements is set.
 */
struct lv_obj_spec_attr_t {
    lv_obj_t ** children;   /**< Store the pointer of the children in an array.*/
    lv_group_t * group_p;
    lv_event_list_t event_list;

    lv_point_t scroll;              /**< The current X/Y scroll offset*/

    int32_t ext_click_pad;          /**< Extra click padding in all direction*/
    int32_t ext_draw_size;          /**< EXTend the size in every direction for drawing.*/

    uint16_t child_cnt;             /**< Number of children*/
    uint16_t scrollbar_mode : 2;    /**< How to display scrollbars, see `lv_scrollbar_mode_t`*/
    uint16_t scroll_snap_x : 2;     /**< Where to align the snappable children horizontally, see `lv_scroll_snap_t`*/
    uint16_t scroll_snap_y : 2;     /**< Where to align the snappable children vertically*/
    uint16_t scroll_dir : 4;        /**< The allowed scroll direction(s), see `lv_dir_t`*/
    uint16_t layer_type : 2;        /**< Cache the layer type here. Element of @lv_intermediate_layer_type_t */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_PRIVATE_H*/
