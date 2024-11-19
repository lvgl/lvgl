/**
 * @file lv_dropdown_private.h
 *
 */

#ifndef LV_DROPDOWN_PRIVATE_H
#define LV_DROPDOWN_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_dropdown.h"

#if LV_USE_DROPDOWN != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_dropdown_t {
    lv_obj_t obj;
    lv_obj_t * list;                /**< The dropped down list*/
    const char * text;              /**< Text to display on the dropdown's button*/
    const void * symbol;            /**< Arrow or other icon when the drop-down list is closed*/
    char * options;                 /**< Options in a '\n' separated list*/
    uint32_t option_cnt;            /**< Number of options*/
    uint32_t sel_opt_id;            /**< Index of the currently selected option*/
    uint32_t sel_opt_id_orig;       /**< Store the original index on focus*/
    uint32_t pr_opt_id;             /**< Index of the currently pressed option*/
    uint8_t dir               : 4;  /**< Direction in which the list should open*/
    uint8_t static_txt        : 1;  /**< 1: Only a pointer is saved in `options`*/
    uint8_t selected_highlight: 1;  /**< 1: Make the selected option highlighted in the list*/
};

struct _lv_dropdown_list_t {
    lv_obj_t obj;
    lv_obj_t * dropdown;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DROPDOWN != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DROPDOWN_PRIVATE_H*/
