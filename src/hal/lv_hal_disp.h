/**
 * @file lv_hal_disp.h
 *
 * @description Display Driver HAL interface header file
 *
 */

#ifndef LV_HAL_DISP_H
#define LV_HAL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "lv_hal.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_ll.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_ll.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE 32 /*Buffer size for invalid areas*/
#endif

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;
struct _lv_disp_t;
struct _lv_disp_drv_t;
struct _lv_theme_t;

/**
 * Structure for holding display buffer information.
 */
typedef struct _lv_disp_draw_buf_t {
    void * buf1; /**< First display buffer.*/
    void * buf2; /**< Second display buffer.*/

    /*Internal, used by the library*/
    void * buf_act;
    uint32_t size; /*In pixel count*/
    /*1: flushing is in progress. (It can't be a bit field because when it's cleared from IRQ Read-Modify-Write issue might occur)*/
    volatile int flushing;
    /*1: It was the last chunk to flush. (It can't be a bit field because when it's cleared from IRQ Read-Modify-Write issue might occur)*/
    volatile int flushing_last;
    volatile uint32_t last_area         : 1; /*1: the last area is being rendered*/
    volatile uint32_t last_part         : 1; /*1: the last part of the current area is being rendered*/
} lv_disp_draw_buf_t;

typedef enum {
    LV_DISP_ROT_NONE = 0,
    LV_DISP_ROT_90,
    LV_DISP_ROT_180,
    LV_DISP_ROT_270
} lv_disp_rot_t;

/**
 * Display Driver structure to be registered by HAL.
 * Only its pointer will be saved in `lv_disp_t` so it should be declared as
 * `static lv_disp_drv_t my_drv` or allocated dynamically.
 */
typedef struct _lv_disp_drv_t {

    lv_coord_t hor_res;         /**< Horizontal resolution.*/
    lv_coord_t ver_res;         /**< Vertical resolution.*/

    lv_coord_t
    physical_hor_res;     /**< Horizontal resolution of the full / physical display. Set to -1 for fullscreen mode.*/
    lv_coord_t
    physical_ver_res;     /**< Vertical resolution of the full / physical display. Set to -1 for fullscreen mode.*/
    lv_coord_t
    offset_x;             /**< Horizontal offset from the full / physical display. Set to 0 for fullscreen mode.*/
    lv_coord_t offset_y;             /**< Vertical offset from the full / physical display. Set to 0 for fullscreen mode.*/

    /** Pointer to a buffer initialized with `lv_disp_draw_buf_init()`.
     * LVGL will use this buffer(s) to draw the screens contents*/
    lv_disp_draw_buf_t * draw_buf;

    uint32_t direct_mode : 1;        /**< 1: Use screen-sized buffers and draw to absolute coordinates*/
    uint32_t full_refresh : 1;       /**< 1: Always make the whole screen redrawn*/
    uint32_t sw_rotate : 1;          /**< 1: use software rotation (slower)*/
    uint32_t antialiasing : 1;       /**< 1: anti-aliasing is enabled on this display.*/
    uint32_t rotated : 2;            /**< 1: turn the display by 90 degree. @warning Does not update coordinates for you!*/
    uint32_t screen_transp : 1;      /**Handle if the screen doesn't have a solid (opa == LV_OPA_COVER) background.
                                       * Use only if required because it's slower.*/

    uint32_t dpi : 10;              /** DPI (dot per inch) of the display. Default value is `LV_DPI_DEF`.*/

    /** MANDATORY: Write the internal buffer (draw_buf) to the display. 'lv_disp_flush_ready()' has to be
     * called when finished*/
    void (*flush_cb)(struct _lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

    /** OPTIONAL: Extend the invalidated areas to match with the display drivers requirements
     * E.g. round `y` to, 8, 16 ..) on a monochrome display*/
    void (*rounder_cb)(struct _lv_disp_drv_t * disp_drv, lv_area_t * area);

    /** OPTIONAL: Set a pixel in a buffer according to the special requirements of the display
     * Can be used for color format not supported in LittelvGL. E.g. 2 bit -> 4 gray scales
     * @note Much slower then drawing with supported color formats.*/
    void (*set_px_cb)(struct _lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                      lv_color_t color, lv_opa_t opa);

    void (*clear_cb)(struct _lv_disp_drv_t * disp_drv, uint8_t * buf, uint32_t size);

    /** OPTIONAL: Called after every refresh cycle to tell the rendering and flushing time + the
     * number of flushed pixels*/
    void (*monitor_cb)(struct _lv_disp_drv_t * disp_drv, uint32_t time, uint32_t px);

    /** OPTIONAL: Called periodically while lvgl waits for operation to be completed.
     * For example flushing or GPU
     * User can execute very simple tasks here or yield the task*/
    void (*wait_cb)(struct _lv_disp_drv_t * disp_drv);

    /** OPTIONAL: Called when lvgl needs any CPU cache that affects rendering to be cleaned*/
    void (*clean_dcache_cb)(struct _lv_disp_drv_t * disp_drv);

    /** OPTIONAL: called when driver parameters are updated */
    void (*drv_update_cb)(struct _lv_disp_drv_t * disp_drv);

    /** OPTIONAL: called when start rendering */
    void (*render_start_cb)(struct _lv_disp_drv_t * disp_drv);

    /** On CHROMA_KEYED images this color will be transparent.
     * `LV_COLOR_CHROMA_KEY` by default. (lv_conf.h)*/
    lv_color_t color_chroma_key;

    lv_draw_ctx_t * draw_ctx;
    void (*draw_ctx_init)(struct _lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx);
    void (*draw_ctx_deinit)(struct _lv_disp_drv_t * disp_drv, lv_draw_ctx_t * draw_ctx);
    size_t draw_ctx_size;

#if LV_USE_USER_DATA
    void * user_data; /**< Custom display driver user data*/
#endif

} lv_disp_drv_t;

/**
 * Display structure.
 * @note `lv_disp_drv_t` should be the first member of the structure.
 */
typedef struct _lv_disp_t {
    /**< Driver to the display*/
    struct _lv_disp_drv_t * driver;

    /**< A timer which periodically checks the dirty areas and refreshes them*/
    lv_timer_t * refr_timer;

    /**< The theme assigned to the screen*/
    struct _lv_theme_t * theme;

    /** Screens of the display*/
    struct _lv_obj_t ** screens;    /**< Array of screen objects.*/
    struct _lv_obj_t * act_scr;     /**< Currently active screen on this display*/
    struct _lv_obj_t * prev_scr;    /**< Previous screen. Used during screen animations*/
    struct _lv_obj_t * scr_to_load; /**< The screen prepared to load in lv_scr_load_anim*/
    struct _lv_obj_t * top_layer;   /**< @see lv_disp_get_layer_top*/
    struct _lv_obj_t * sys_layer;   /**< @see lv_disp_get_layer_sys*/
    uint32_t screen_cnt;
    uint8_t draw_prev_over_act : 1; /**< 1: Draw previous screen over active screen*/
    uint8_t del_prev : 1;           /**< 1: Automatically delete the previous screen when the screen load anim. is ready*/
    uint8_t rendering_in_progress : 1; /**< 1: The current screen rendering is in progress*/

    lv_opa_t bg_opa;                /**<Opacity of the background color or wallpaper*/
    lv_color_t bg_color;            /**< Default display color when screens are transparent*/
    const void * bg_img;            /**< An image source to display as wallpaper*/

    /** Invalidated (marked to redraw) areas*/
    lv_area_t inv_areas[LV_INV_BUF_SIZE];
    uint8_t inv_area_joined[LV_INV_BUF_SIZE];
    uint16_t inv_p;
    int32_t inv_en_cnt;

    /** Double buffer sync areas */
    lv_ll_t sync_areas;

    /*Miscellaneous data*/
    uint32_t last_activity_time;        /**< Last time when there was activity on this display*/
} lv_disp_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a display driver with default values.
 * It is used to have known values in the fields and not junk in memory.
 * After it you can safely set only the fields you need.
 * @param driver pointer to driver variable to initialize
 */
void lv_disp_drv_init(lv_disp_drv_t * driver);

/**
 * Initialize a display buffer
 * @param draw_buf pointer `lv_disp_draw_buf_t` variable to initialize
 * @param buf1 A buffer to be used by LVGL to draw the image.
 *             Always has to specified and can't be NULL.
 *             Can be an array allocated by the user. E.g. `static lv_color_t disp_buf1[1024 * 10]`
 *             Or a memory address e.g. in external SRAM
 * @param buf2 Optionally specify a second buffer to make image rendering and image flushing
 *             (sending to the display) parallel.
 *             In the `disp_drv->flush` you should use DMA or similar hardware to send
 *             the image to the display in the background.
 *             It lets LVGL to render next frame into the other buffer while previous is being
 * sent. Set to `NULL` if unused.
 * @param size_in_px_cnt size of the `buf1` and `buf2` in pixel count.
 */
void lv_disp_draw_buf_init(lv_disp_draw_buf_t * draw_buf, void * buf1, void * buf2, uint32_t size_in_px_cnt);

/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'lv_disp_drv_t' variable. Only its pointer is saved!
 * @return pointer to the new display or NULL on error
 */
lv_disp_t * lv_disp_drv_register(lv_disp_drv_t * driver);

/**
 * Update the driver in run time.
 * @param disp pointer to a display. (return value of `lv_disp_drv_register`)
 * @param new_drv pointer to the new driver
 */
void lv_disp_drv_update(lv_disp_t * disp, lv_disp_drv_t * new_drv);

/**
 * Remove a display
 * @param disp pointer to display
 */
void lv_disp_remove(lv_disp_t * disp);

/**
 * Set a default display. The new screens will be created on it by default.
 * @param disp pointer to a display
 */
void lv_disp_set_default(lv_disp_t * disp);

/**
 * Get the default display
 * @return pointer to the default display
 */
lv_disp_t * lv_disp_get_default(void);

/**
 * Get the horizontal resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal resolution of the display
 */
lv_coord_t lv_disp_get_hor_res(lv_disp_t * disp);

/**
 * Get the vertical resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the vertical resolution of the display
 */
lv_coord_t lv_disp_get_ver_res(lv_disp_t * disp);

/**
 * Get the full / physical horizontal resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the full / physical horizontal resolution of the display
 */
lv_coord_t lv_disp_get_physical_hor_res(lv_disp_t * disp);

/**
 * Get the full / physical vertical resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the full / physical vertical resolution of the display
 */
lv_coord_t lv_disp_get_physical_ver_res(lv_disp_t * disp);

/**
 * Get the horizontal offset from the full / physical display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal offset from the full / physical display
 */
lv_coord_t lv_disp_get_offset_x(lv_disp_t * disp);

/**
 * Get the vertical offset from the full / physical display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal offset from the full / physical display
 */
lv_coord_t lv_disp_get_offset_y(lv_disp_t * disp);

/**
 * Get if anti-aliasing is enabled for a display or not
 * @param disp pointer to a display (NULL to use the default display)
 * @return true: anti-aliasing is enabled; false: disabled
 */
bool lv_disp_get_antialiasing(lv_disp_t * disp);

/**
 * Get the DPI of the display
 * @param disp pointer to a display (NULL to use the default display)
 * @return dpi of the display
 */
lv_coord_t lv_disp_get_dpi(const lv_disp_t * disp);

/**
 * Set the rotation of this display.
 * @param disp pointer to a display (NULL to use the default display)
 * @param rotation rotation angle
 */
void lv_disp_set_rotation(lv_disp_t * disp, lv_disp_rot_t rotation);

/**
 * Get the current rotation of this display.
 * @param disp pointer to a display (NULL to use the default display)
 * @return rotation angle
 */
lv_disp_rot_t lv_disp_get_rotation(lv_disp_t * disp);

//! @cond Doxygen_Suppress

/**
 * Call in the display driver's `flush_cb` function when the flushing is finished
 * @param disp_drv pointer to display driver in `flush_cb` where this function is called
 */
void /* LV_ATTRIBUTE_FLUSH_READY */ lv_disp_flush_ready(lv_disp_drv_t * disp_drv);

/**
 * Tell if it's the last area of the refreshing process.
 * Can be called from `flush_cb` to execute some special display refreshing if needed when all areas area flushed.
 * @param disp_drv pointer to display driver
 * @return true: it's the last area to flush; false: there are other areas too which will be refreshed soon
 */
bool /* LV_ATTRIBUTE_FLUSH_READY */ lv_disp_flush_is_last(lv_disp_drv_t * disp_drv);

//! @endcond

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_get_next(lv_disp_t * disp);

/**
 * Get the internal buffer of a display
 * @param disp pointer to a display
 * @return pointer to the internal buffers
 */
lv_disp_draw_buf_t * lv_disp_get_draw_buf(lv_disp_t * disp);

void lv_disp_drv_use_generic_set_px_cb(lv_disp_drv_t * disp_drv, lv_img_cf_t cf);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
