/**
 * @file lv_disp_private.h
 *
 */

#ifndef LV_DISP_PRIVATE_H
#define LV_DISP_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "../draw/lv_draw.h"
#include "lv_disp.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE 32 /*Buffer size for invalid areas*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_disp_t;

struct _lv_disp_t {

    /*---------------------
     * Resolution
     *--------------------*/

    /** Horizontal resolution.*/
    lv_coord_t hor_res;

    /** Vertical resolution.*/
    lv_coord_t ver_res;

    /** Horizontal resolution of the full / physical display. Set to -1 for fullscreen mode.*/
    lv_coord_t physical_hor_res;

    /** Vertical resolution of the full / physical display. Set to -1 for fullscreen mode.*/
    lv_coord_t physical_ver_res;

    /** Horizontal offset from the full / physical display. Set to 0 for fullscreen mode.*/
    lv_coord_t offset_x;

    /** Vertical offset from the full / physical display. Set to 0 for fullscreen mode.*/
    lv_coord_t offset_y;

    uint32_t dpi;              /** DPI (dot per inch) of the display. Default value is `LV_DPI_DEF`.*/

    /*---------------------
     * Buffering
     *--------------------*/

    /** First display buffer.*/
    void * draw_buf_1;

    /** Second display buffer.*/
    void * draw_buf_2;

    /** Internal, used by the library*/
    void * draw_buf_act;

    /** In byte count*/
    uint32_t draw_buf_size;

    /** MANDATORY: Write the internal buffer (draw_buf) to the display. 'lv_disp_flush_ready()' has to be
     * called when finished*/
    lv_disp_flush_cb_t flush_cb;

    /*1: flushing is in progress. (It can't be a bit field because when it's cleared from IRQ Read-Modify-Write issue might occur)*/
    volatile int flushing;

    /*1: It was the last chunk to flush. (It can't be a bit field because when it's cleared from IRQ Read-Modify-Write issue might occur)*/
    volatile int flushing_last;
    volatile uint32_t last_area         : 1; /*1: the last area is being rendered*/
    volatile uint32_t last_part         : 1; /*1: the last part of the current area is being rendered*/

    lv_disp_render_mode_t render_mode;
    uint32_t antialiasing : 1;       /**< 1: anti-aliasing is enabled on this display.*/

    /** 1: The current screen rendering is in progress*/
    uint32_t rendering_in_progress : 1;

    lv_color_format_t   color_format;

    /** Invalidated (marked to redraw) areas*/
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint16_t inv_p;
    int32_t inv_en_cnt;

    /*---------------------
     * Layer
     *--------------------*/
    lv_layer_t * layer_head;
    lv_layer_t * (*layer_init)(struct _lv_disp_t * disp);
    void (*layer_deinit)(struct _lv_disp_t * disp, lv_layer_t * layer);

    /*---------------------
     * Screens
     *--------------------*/

    /** Screens of the display*/
    struct _lv_obj_t ** screens;    /**< Array of screen objects.*/
    struct _lv_obj_t * act_scr;     /**< Currently active screen on this display*/
    struct _lv_obj_t * prev_scr;    /**< Previous screen. Used during screen animations*/
    struct _lv_obj_t * scr_to_load; /**< The screen prepared to load in lv_scr_load_anim*/
    struct _lv_obj_t * bottom_layer;    /**< @see lv_disp_get_layer_bottom*/
    struct _lv_obj_t * top_layer;       /**< @see lv_disp_get_layer_top*/
    struct _lv_obj_t * sys_layer;       /**< @see lv_disp_get_layer_sys*/
    uint32_t screen_cnt;
    uint8_t draw_prev_over_act  : 1;/** 1: Draw previous screen over active screen*/
    uint8_t del_prev  : 1; /** 1: Automatically delete the previous screen when the screen load animation is ready*/

    /*---------------------
     * Others
     *--------------------*/

    void * driver_data; /**< Custom user data*/

    void * user_data; /**< Custom user data*/

    lv_event_list_t event_list;

    uint32_t sw_rotate : 1; /**< 1: use software rotation (slower)*/
    uint32_t rotation  : 2; /**< Element of  @lv_disp_rotation_t*/

    /**< The theme assigned to the screen*/
    struct _lv_theme_t * theme;

    /** A timer which periodically checks the dirty areas and refreshes them*/
    lv_timer_t * refr_timer;

    /*Miscellaneous data*/
    uint32_t last_activity_time;        /**< Last time when there was activity on this display*/

    /** The area being refreshed*/
    lv_area_t refreshed_area;
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

#endif /*LV_DISP_PRIVATE_H*/
