/**
 * @file lv_layout.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_layout_private.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define layout_cnt LV_GLOBAL_DEFAULT()->layout_count
#define layout_list_def LV_GLOBAL_DEFAULT()->layout_list
#include "../core/lv_obj_style_internal.h"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_layout_init(void)
{
    /*Malloc a list for the built in layouts*/
    layout_list_def = lv_malloc(layout_cnt * sizeof(lv_layout_dsc_t));

#if LV_USE_FLEX
    lv_flex_init();
#endif

#if LV_USE_GRID
    lv_grid_init();
#endif
}

void lv_layout_deinit(void)
{
    lv_free(layout_list_def);
}

uint32_t lv_layout_create(lv_layout_callbacks_t callbacks, void * user_data)
{

    layout_list_def = lv_realloc(layout_list_def, (layout_cnt + 1) * sizeof(lv_layout_dsc_t));
    LV_ASSERT_MALLOC(layout_list_def);

    layout_list_def[layout_cnt].callbacks = callbacks;
    layout_list_def[layout_cnt].user_data = user_data;
    return layout_cnt++;
}

uint32_t lv_layout_register(lv_layout_update_position_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return 0);

    LV_LOG_DEPRECATED("`lv_layout_register` is deprecated and replaced by `lv_layout_create`.");
    lv_layout_callbacks_t cbs = {.update_positions_cb  = cb, .update_sizes_cb = NULL};
    return lv_layout_create(cbs, user_data);
}

void lv_layout_update_children_positions(lv_obj_t * obj)
{
    LV_ASSERT(obj != NULL);
    lv_layout_t layout_id = lv_obj_get_style_layout_internal(obj, LV_PART_MAIN);
    if(layout_id > 0 && layout_id < layout_cnt) {
        void  * user_data = layout_list_def[layout_id].user_data;
        lv_layout_update_position_cb_t cb = layout_list_def[layout_id].callbacks.update_positions_cb;
        if(cb) cb(obj, user_data);
    }
}

void lv_layout_update_children_sizes(lv_obj_t * obj, int32_t iteration)
{
    LV_ASSERT(obj != NULL);
    lv_layout_t layout_id = lv_obj_get_style_layout_internal(obj, LV_PART_MAIN);
    if(layout_id > 0 && layout_id < layout_cnt) {
        void  * user_data = layout_list_def[layout_id].user_data;
        lv_layout_update_sizes_cb_t cb = layout_list_def[layout_id].callbacks.update_sizes_cb;
        if(cb) cb(obj, iteration, user_data);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
