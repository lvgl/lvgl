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
typedef bool (*lv_layout_get_min_size_cb_t)(lv_obj_t *, int32_t * req_size, bool width, void * user_data);

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

/**
 * Set the optional callback which is used to precompute the minimum required size for a layout before it is applied to
 * the children.
 * @param layout_id the ID of the layout
 * @param cb        the callback
 * @return          `true` if the callback was set successfully, `false` if not.
 * @note            `user_data` set with `lv_layout_register()` will be passed to the callback.
 */
bool lv_layout_set_min_size_cb(uint32_t layout_id, lv_layout_get_min_size_cb_t cb);

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
