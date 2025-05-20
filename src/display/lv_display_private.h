/**
 * @file lv_display_private.h
 *
 */

#ifndef LV_DISPLAY_PRIVATE_H
#define LV_DISPLAY_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../core/lv_obj.h"
#include "../draw/lv_draw.h"
#include "lv_display.h"

#if LV_USE_SYSMON
#include "../others/sysmon/lv_sysmon_private.h"
#endif

/*********************
 *      DEFINES
 *********************/
#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE 32 /**< Buffer size for invalid areas */
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_display_t {

    /*---------------------
     * Resolution
     *--------------------*/

    /** Horizontal resolution.*/
    int32_t hor_res;

    /** Vertical resolution.*/
    int32_t ver_res;

    /** Horizontal resolution of the full / physical display. Set to -1 for fullscreen mode.*/
    int32_t physical_hor_res;

    /** Vertical resolution of the full / physical display. Set to -1 for fullscreen mode.*/
    int32_t physical_ver_res;

    /** Horizontal offset from the full / physical display. Set to 0 for fullscreen mode.*/
    int32_t offset_x;

    /** Vertical offset from the full / physical display. Set to 0 for fullscreen mode.*/
    int32_t offset_y;

    /** DPI (dot per inch) of the display. Default value is `LV_DPI_DEF`.*/
    uint32_t dpi;

    /*---------------------
     * Buffering
     *--------------------*/
    lv_draw_buf_t * buf_1;
    lv_draw_buf_t * buf_2;
    lv_draw_buf_t * buf_3;

    /** Internal, used by the library*/
    lv_draw_buf_t * buf_act;

    /** MANDATORY: Write the internal buffer (draw_buf) to the display. 'lv_display_flush_ready()' has to be
     * called when finished*/
    lv_display_flush_cb_t flush_cb;

    /**
     * Used to wait while flushing is ready.
     * It can do any complex logic to wait, including semaphores, mutexes, polling flags, etc.
     * If not set `flushing` flag is used which can be cleared with `lv_display_flush_ready()` */
    lv_display_flush_wait_cb_t flush_wait_cb;

    /** 1: flushing is in progress. (It can't be a bit field because when it's cleared from IRQ
     * Read-Modify-Write issue might occur) */
    volatile int flushing;

    /** 1: It was the last chunk to flush. (It can't be a bit field because when it's cleared
     * from IRQ Read-Modify-Write issue might occur) */
    volatile int flushing_last;
    volatile uint32_t last_area         : 1; /**< 1: last area is being rendered */
    volatile uint32_t last_part         : 1; /**< 1: last part of the current area is being rendered */

    lv_display_render_mode_t render_mode;
    uint32_t antialiasing : 1;       /**< 1: anti-aliasing is enabled on this display.*/
    uint32_t tile_cnt     : 8;       /**< Divide the display buffer into these number of tiles */
    uint32_t stride_is_auto : 1;     /**< 1: The stride of the buffers was not set explicitly. */


    /** 1: The current screen rendering is in progress*/
    uint32_t rendering_in_progress : 1;

    lv_color_format_t   color_format;

    /** Invalidated (marked to redraw) areas*/
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint32_t inv_p;
    int32_t inv_en_cnt;

    /** Double buffer sync areas (redrawn during last refresh) */
    lv_ll_t sync_areas;

    lv_draw_buf_t _static_buf1; /**< Used when user pass in a raw buffer as display draw buffer */
    lv_draw_buf_t _static_buf2;
    /*---------------------
     * Layer
     *--------------------*/
    lv_layer_t * layer_head;
    void (*layer_init)(lv_display_t * disp, lv_layer_t * layer);
    void (*layer_deinit)(lv_display_t * disp, lv_layer_t * layer);

    /*---------------------
     * Screens
     *--------------------*/

    /** Screens of the display*/
    lv_obj_t ** screens;    /**< Array of screen objects.*/
    lv_obj_t * sys_layer;   /**< @see lv_display_get_layer_sys*/
    lv_obj_t * top_layer;   /**< @see lv_display_get_layer_top*/
    lv_obj_t * act_scr;     /**< Currently active screen on this display*/
    lv_obj_t * bottom_layer;/**< @see lv_display_get_layer_bottom*/
    lv_obj_t * prev_scr;    /**< Previous screen. Used during screen animations*/
    lv_obj_t * scr_to_load; /**< The screen prepared to load in lv_screen_load_anim*/
    uint32_t screen_cnt;
    uint8_t draw_prev_over_act  : 1;/** 1: Draw previous screen over active screen*/
    uint8_t del_prev  : 1;  /** 1: Automatically delete the previous screen when the screen load animation is ready*/

    /*---------------------
     * Others
     *--------------------*/

    void * driver_data; /**< Custom user data*/

    void * user_data; /**< Custom user data*/

    lv_event_list_t event_list;

    uint32_t rotation  : 3; /**< Element of  lv_display_rotation_t*/

    uint32_t matrix_rotation : 1; /**< 1: Use matrix for display rotation*/

    lv_theme_t * theme;     /**< The theme assigned to the screen*/

    /** A timer which periodically checks the dirty areas and refreshes them*/
    lv_timer_t * refr_timer;

    /*Miscellaneous data*/
    uint32_t last_activity_time;        /**< Last time when there was activity on this display*/

    /** The area being refreshed*/
    lv_area_t refreshed_area;
    uint32_t vsync_count;

#if LV_USE_PERF_MONITOR
    lv_obj_t * perf_label;
    lv_sysmon_backend_data_t perf_sysmon_backend;
    lv_sysmon_perf_info_t perf_sysmon_info;
#endif

#if LV_USE_MEM_MONITOR
    lv_obj_t * mem_label;
#endif

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

#endif /*LV_DISPLAY_PRIVATE_H*/
