/**
 * @file lv_layout.h
 *
 */

#ifndef LV_LAYOUT_H
#define LV_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_layout_update_cb_t)(lv_obj_t *, void * user_data);

typedef enum {
    LV_LAYOUT_NONE = 0,

#if LV_USE_FLEX
    LV_LAYOUT_FLEX,
#endif

#if LV_USE_GRID
    LV_LAYOUT_GRID,
#endif

    LV_LAYOUT_LAST
} lv_layout_t;

/**
 * Register a new layout
 * @param cb        the layout update callback
 * @param user_data custom data that will be passed to `cb`
 * @return          the ID of the new layout
 */
uint32_t lv_layout_register(lv_layout_update_cb_t cb, void * user_data);

/**********************
 *      MACROS
 **********************/

#if LV_USE_FLEX
#include "flex/lv_flex.h"
#endif /* LV_USE_FLEX */

#if LV_USE_GRID
#include "grid/lv_grid.h"
#endif /* LV_USE_GRID */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LAYOUT_H*/
