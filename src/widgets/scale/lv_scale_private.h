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
    lv_style_t * main_style;               /**< Style to use for MAIN part(s) of scale
                                            *   when it falls within this section's range */
    lv_style_t * indicator_style;          /**< Style to use for INDICATOR part(s) of scale
                                            *   when it falls within this section's range */
    lv_style_t * items_style;              /**< Style to use for ITEMS part(s) of scale
                                            *   when it falls within this section's range */
    int32_t range_min;                     /**< Scale parts with value >= this value will be drawn using applicable style. */
    int32_t range_max;                     /**< Scale parts with value <= this value will be drawn using applicable style. */
    uint32_t first_tick_idx_in_section;    /**< Internal (set during drawing): Tick index of first tick that falls within
                                            *   this section; LV_SCALE_TICK_IDX_DEFAULT_ID if section contains no ticks. */
    uint32_t last_tick_idx_in_section;     /**< Internal (set during drawing): Tick index of last tick that falls within
                                            *   this section; LV_SCALE_TICK_IDX_DEFAULT_ID if section contains no ticks. */
    int32_t first_tick_in_section_width;   /**< Internal (set during drawing) */
    int32_t last_tick_in_section_width;    /**< Internal (set during drawing) */
    lv_point_t first_tick_in_section;      /**< Internal (set during drawing) */
    lv_point_t last_tick_in_section;       /**< Internal (set during drawing) */
    uint32_t first_tick_idx_is_major : 1;  /**< Internal (set during drawing): true if
                                            * `first_tick_idx_in_section` represents a major tick. */
    uint32_t last_tick_idx_is_major  : 1;  /**< Internal (set during drawing): true if
                                            * `last_tick_idx_in_section` represents a major tick. */
};

struct _lv_scale_t {
    lv_obj_t obj;                      /**< Base Widget part of Scale */
    lv_ll_t section_ll;                /**< Linked list for the sections (stores lv_scale_section_t)*/
    const char ** txt_src;             /**< Optional list of text strings for major ticks
                                        *   when custom labels are provided. */
    lv_scale_mode_t mode;              /**< Orientation and layout of scale. */
    int32_t range_min;                 /**< Scale's minimum value */
    int32_t range_max;                 /**< Scale's maximum value */
    uint32_t total_tick_count   : 15;  /**< Total number of ticks (major and minor) */
    uint32_t major_tick_every   : 15;  /**< Frequency of major ticks to minor ticks */
    uint32_t label_enabled      : 1;   /**< Draw labels for major ticks? */
    uint32_t post_draw          : 1;   /**< false: drawing occurs during LV_EVENT_DRAW_MAIN;
                                        *   true : drawing occurs during LV_EVENT_DRAW_POST. */
    uint32_t draw_ticks_on_top  : 1;   /**< Draw ticks on top of main line? */
    /* Round scale */
    uint32_t angle_range;              /**< Degrees between low end and high end of scale */
    int32_t rotation;                  /**< Clockwise angular offset from 3-o'clock position of low end of scale */
    /* Private properties */
    int32_t custom_label_cnt;          /**< Number of custom labels provided in `txt_src` */
    int32_t last_tick_width;           /**< Width of last tick in pixels */
    int32_t first_tick_width;          /**< Width of first tick in pixels */
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
