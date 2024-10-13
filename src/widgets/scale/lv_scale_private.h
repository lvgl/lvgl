/**
 * @file lv_scale_private.h
 *
 */

#ifndef LV_SCALE_PRIVATE_H
#define LV_SCALE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_scale.h"

#if LV_USE_SCALE != 0
#include "../../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_scale_section_t {
    lv_style_t * main_style;
    lv_style_t * indicator_style;
    lv_style_t * items_style;
    int32_t minor_range;
    int32_t major_range;
    uint32_t first_tick_idx_in_section;
    uint32_t last_tick_idx_in_section;
    uint32_t first_tick_idx_is_major;
    uint32_t last_tick_idx_is_major;
    int32_t first_tick_in_section_width;
    int32_t last_tick_in_section_width;
    lv_point_t first_tick_in_section;
    lv_point_t last_tick_in_section;
};

struct _lv_scale_t {
    lv_obj_t obj;
    lv_ll_t section_ll;     /**< Linked list for the sections (stores lv_scale_section_t)*/
    const char ** txt_src;
    lv_scale_mode_t mode;
    int32_t range_min;
    int32_t range_max;
    uint32_t total_tick_count   : 15;
    uint32_t major_tick_every   : 15;
    uint32_t label_enabled      : 1;
    uint32_t post_draw          : 1;
    uint32_t draw_ticks_on_top  : 1;
    /* Round scale */
    uint32_t angle_range;
    int32_t rotation;
    /* Private properties */
    int32_t custom_label_cnt;
    int32_t last_tick_width;
    int32_t first_tick_width;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_SCALE != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SCALE_PRIVATE_H*/
