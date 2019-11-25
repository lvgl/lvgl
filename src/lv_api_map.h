/**
 * @file lv_api_map.h
 *
 */

#ifndef LV_API_MAP_H
#define LV_API_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*---------------------
 * V6.0 COMPATIBILITY
 *--------------------*/
#define lv_chart_get_point_cnt lv_chart_get_point_count

static inline void lv_ddlist_set_draw_arrow(lv_obj_t * ddlist, bool en)
{
    if(en) lv_ddlist_set_symbol(ddlist, LV_SYMBOL_DOWN);
    else lv_ddlist_set_symbol(ddlist, NULL);
}


static inline bool lv_ddlist_get_draw_arrow(lv_obj_t * ddlist)
{
    if(lv_ddlist_get_symbol(ddlist)) return true;
    else return false;
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_API_MAP_H*/
