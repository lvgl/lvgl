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

#if LV_USE_EXT_DATA
#include "../lvgl_private.h"
#endif

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
struct _lv_obj_spec_attr_t {
    lv_obj_t ** children;           /**< Store the pointer of the children in an array.*/
    lv_group_t * group_p;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_matrix_t * matrix;           /**< The transform matrix*/
#endif
    lv_event_list_t event_list;
#if LV_USE_OBJ_NAME
    const char * name;              /**< Pointer to the name */
#endif
    lv_point_t scroll;              /**< The current X/Y scroll offset*/

    int32_t ext_click_pad;          /**< Extra click padding in all direction*/
    int32_t ext_draw_size;          /**< EXTend the size in every direction for drawing.*/

    uint16_t child_cnt;             /**< Number of children*/
    uint16_t scrollbar_mode : 2;    /**< How to display scrollbars, see `lv_scrollbar_mode_t`*/
    uint16_t scroll_snap_x : 2;     /**< Where to align the snappable children horizontally, see `lv_scroll_snap_t`*/
    uint16_t scroll_snap_y : 2;     /**< Where to align the snappable children vertically*/
    uint16_t scroll_dir : 4;        /**< The allowed scroll direction(s), see `lv_dir_t`*/
    uint16_t layer_type : 2;        /**< Cache the layer type here. Element of lv_intermediate_layer_type_t */
    uint16_t name_static : 1;        /**< 1: `name` was not dynamically allocated */
};

struct _lv_obj_t {
#if LV_USE_EXT_DATA
    lv_ext_data_t ext_data;
#endif
    const lv_obj_class_t * class_p;
    lv_obj_t * parent;
    lv_obj_spec_attr_t * spec_attr;
    lv_obj_style_t * styles;
#if LV_OBJ_STYLE_CACHE
    uint32_t style_main_prop_is_set;
    uint32_t style_other_prop_is_set;
#endif
    void * user_data;
#if LV_USE_OBJ_ID
    void * id;
#endif
    lv_area_t coords;
    lv_obj_flag_t flags;
    uint16_t state;
    uint16_t layout_inv : 1;
    uint16_t readjust_scroll_after_layout : 1;
    uint16_t scr_layout_inv : 1;
    uint16_t skip_trans : 1;
    uint16_t style_cnt  : 6;
    uint16_t h_layout   : 1;
    uint16_t w_layout   : 1;
    uint16_t h_ignore_size : 1; /* ignore this obj when calculating content height of parent */
    uint16_t w_ignore_size : 1; /* ignore this obj when calculating content width of parent */
    uint16_t is_deleting : 1;

    /** The widget is rendered at least once already.
     * It's used to skip initial animations and transitions. */
    uint16_t rendered : 1;
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
