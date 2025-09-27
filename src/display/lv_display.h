/**
 * @file lv_display.h
 *
 */

#ifndef LV_DISPLAY_H
#define LV_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_event.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_DISPLAY_ROTATION_0 = 0,
    LV_DISPLAY_ROTATION_90,
    LV_DISPLAY_ROTATION_180,
    LV_DISPLAY_ROTATION_270
} lv_display_rotation_t;

typedef enum {
    /**
     * Use the buffer(s) to render the screen is smaller parts.
     * This way the buffers can be smaller then the display to save RAM. At least 1/10 screen size buffer(s) are recommended.
     */
    LV_DISPLAY_RENDER_MODE_PARTIAL,

    /**
     * The buffer(s) has to be screen sized and LVGL will render into the correct location of the buffer.
     * This way the buffer always contain the whole image. Only the changed ares will be updated.
     * With 2 buffers the buffers' content are kept in sync automatically and in flush_cb only address change is required.
     */
    LV_DISPLAY_RENDER_MODE_DIRECT,

    /**
     * Always redraw the whole screen even if only one pixel has been changed.
     * With 2 buffers in flush_cb only an address change is required.
     */
    LV_DISPLAY_RENDER_MODE_FULL,
} lv_display_render_mode_t;

typedef enum {
    LV_SCREEN_LOAD_ANIM_NONE,
    LV_SCREEN_LOAD_ANIM_OVER_LEFT,
    LV_SCREEN_LOAD_ANIM_OVER_RIGHT,
    LV_SCREEN_LOAD_ANIM_OVER_TOP,
    LV_SCREEN_LOAD_ANIM_OVER_BOTTOM,
    LV_SCREEN_LOAD_ANIM_MOVE_LEFT,
    LV_SCREEN_LOAD_ANIM_MOVE_RIGHT,
    LV_SCREEN_LOAD_ANIM_MOVE_TOP,
    LV_SCREEN_LOAD_ANIM_MOVE_BOTTOM,
    LV_SCREEN_LOAD_ANIM_FADE_IN,
    LV_SCREEN_LOAD_ANIM_FADE_ON = LV_SCREEN_LOAD_ANIM_FADE_IN, /*For backward compatibility*/
    LV_SCREEN_LOAD_ANIM_FADE_OUT,
    LV_SCREEN_LOAD_ANIM_OUT_LEFT,
    LV_SCREEN_LOAD_ANIM_OUT_RIGHT,
    LV_SCREEN_LOAD_ANIM_OUT_TOP,
    LV_SCREEN_LOAD_ANIM_OUT_BOTTOM,
} lv_screen_load_anim_t;

typedef void (*lv_display_flush_cb_t)(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
typedef void (*lv_display_flush_wait_cb_t)(lv_display_t * disp);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a new display with the given resolution
 * @param hor_res   horizontal resolution in pixels
 * @param ver_res   vertical resolution in pixels
 * @return          pointer to a display object or `NULL` on error
 */
lv_display_t * lv_display_create(int32_t hor_res, int32_t ver_res);

/**
 * Remove a display
 * @param disp      pointer to display
 */
void lv_display_delete(lv_display_t * disp);

/**
 * Set a default display. The new screens will be created on it by default.
 * @param disp      pointer to a display
 */
void lv_display_set_default(lv_display_t * disp);

/**
 * Get the default display
 * @return          pointer to the default display
 */
lv_display_t * lv_display_get_default(void);

/**
 * Get the next display.
 * @param disp      pointer to the current display. NULL to initialize.
 * @return          the next display or NULL if no more. Gives the first display when the parameter is NULL.
 */
lv_display_t * lv_display_get_next(lv_display_t * disp);

/*---------------------
 * RESOLUTION
 *--------------------*/

/**
 * Sets the resolution of a display. `LV_EVENT_RESOLUTION_CHANGED` event will be sent.
 * Here the native resolution of the device should be set. If the display will be rotated later with
 * `lv_display_set_rotation` LVGL will swap the hor. and ver. resolution automatically.
 * @param disp      pointer to a display
 * @param hor_res   the new horizontal resolution
 * @param ver_res   the new vertical resolution
 */
void lv_display_set_resolution(lv_display_t * disp, int32_t hor_res, int32_t ver_res);

/**
 * It's not mandatory to use the whole display for LVGL, however in some cases physical resolution is important.
 * For example the touchpad still sees whole resolution and the values needs to be converted
 * to the active LVGL display area.
 * @param disp      pointer to a display
 * @param hor_res   the new physical horizontal resolution, or -1 to assume it's the same as the normal hor. res.
 * @param ver_res   the new physical vertical resolution, or -1 to assume it's the same as the normal hor. res.
 */
void lv_display_set_physical_resolution(lv_display_t * disp, int32_t hor_res, int32_t ver_res);

/**
 * If physical resolution is not the same as the normal resolution
 * the offset of the active display area can be set here.
 * @param disp      pointer to a display
 * @param x         X offset
 * @param y         Y offset
 */
void lv_display_set_offset(lv_display_t * disp, int32_t x, int32_t y);

/**
 * Set the rotation of this display. LVGL will swap the horizontal and vertical resolutions internally.
 * @param disp      pointer to a display (NULL to use the default display)
 * @param rotation  `LV_DISPLAY_ROTATION_0/90/180/270`
 */
void lv_display_set_rotation(lv_display_t * disp, lv_display_rotation_t rotation);

/**
 * Use matrix rotation for the display. This function is depended on `LV_DRAW_TRANSFORM_USE_MATRIX`
 * @param disp      pointer to a display (NULL to use the default display)
 * @param enable    true: enable matrix rotation, false: disable
 */
void lv_display_set_matrix_rotation(lv_display_t * disp, bool enable);

/**
 * Set the DPI (dot per inch) of the display.
 * dpi = sqrt(hor_res^2 + ver_res^2) / diagonal"
 * @param disp      pointer to a display
 * @param dpi       the new DPI
 */
void lv_display_set_dpi(lv_display_t * disp, int32_t dpi);

/**
 * Get the horizontal resolution of a display.
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the horizontal resolution of the display.
 */
int32_t lv_display_get_horizontal_resolution(const lv_display_t * disp);

/**
 * Get the vertical resolution of a display
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the vertical resolution of the display
 */
int32_t lv_display_get_vertical_resolution(const lv_display_t * disp);

/**
 * Get the original horizontal resolution of a display without considering rotation
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the horizontal resolution of the display.
 */
int32_t lv_display_get_original_horizontal_resolution(const lv_display_t * disp);

/**
 * Get the original vertical resolution of a display without considering rotation
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the vertical resolution of the display
 */
int32_t lv_display_get_original_vertical_resolution(const lv_display_t * disp);

/**
 * Get the physical horizontal resolution of a display
 * @param disp      pointer to a display (NULL to use the default display)
 * @return the      physical horizontal resolution of the display
 */
int32_t lv_display_get_physical_horizontal_resolution(const lv_display_t * disp);

/**
 * Get the physical vertical resolution of a display
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the physical vertical resolution of the display
 */
int32_t lv_display_get_physical_vertical_resolution(const lv_display_t * disp);

/**
 * Get the horizontal offset from the full / physical display
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the horizontal offset from the physical display
 */
int32_t lv_display_get_offset_x(const lv_display_t * disp);

/**
 * Get the vertical offset from the full / physical display
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the horizontal offset from the physical display
 */
int32_t lv_display_get_offset_y(const lv_display_t * disp);

/**
 * Get the current rotation of this display.
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          the current rotation
 */
lv_display_rotation_t lv_display_get_rotation(lv_display_t * disp);

/**
 * Get if matrix rotation is enabled for a display or not
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          true: matrix rotation is enabled; false: disabled
 */
bool lv_display_get_matrix_rotation(lv_display_t * disp);

/**
 * Get the DPI of the display
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          dpi of the display
 */
int32_t lv_display_get_dpi(const lv_display_t * disp);

/*---------------------
 * BUFFERING
 *--------------------*/

/**
 * Set the buffers for a display, similarly to `lv_display_set_draw_buffers`, but accept the raw buffer pointers.
 * For DIRECT/FULL rending modes, the buffer size must be at least
 * `hor_res * ver_res * lv_color_format_get_size(lv_display_get_color_format(disp))`
 * @param disp              pointer to a display
 * @param buf1              first buffer
 * @param buf2              second buffer (can be `NULL`)
 * @param buf_size          buffer size in byte
 * @param render_mode       LV_DISPLAY_RENDER_MODE_PARTIAL/DIRECT/FULL
 */
void lv_display_set_buffers(lv_display_t * disp, void * buf1, void * buf2, uint32_t buf_size,
                            lv_display_render_mode_t render_mode);

/**
 * Set the frame buffers for a display, similarly to `lv_display_set_buffers`, but allow
 * for a custom stride as required by a display controller.
 * This allows the frame buffers to have a stride alignment different from the rest of
 * the buffers`
 * @param disp              pointer to a display
 * @param buf1              first buffer
 * @param buf2              second buffer (can be `NULL`)
 * @param buf_size          buffer size in byte
 * @param stride            buffer stride in bytes
 * @param render_mode       LV_DISPLAY_RENDER_MODE_PARTIAL/DIRECT/FULL
 */
void lv_display_set_buffers_with_stride(lv_display_t * disp, void * buf1, void * buf2, uint32_t buf_size,
                                        uint32_t stride, lv_display_render_mode_t render_mode);

/**
 * Set the buffers for a display, accept a draw buffer pointer.
 * Normally use `lv_display_set_buffers` is enough for most cases.
 * Use this function when an existing lv_draw_buf_t is available.
 * @param disp              pointer to a display
 * @param buf1              first buffer
 * @param buf2              second buffer (can be `NULL`)
 */
void lv_display_set_draw_buffers(lv_display_t * disp, lv_draw_buf_t * buf1, lv_draw_buf_t * buf2);

/**
 * Set the third draw buffer for a display.
 * @param disp              pointer to a display
 * @param buf3              third buffer
 */
void lv_display_set_3rd_draw_buffer(lv_display_t * disp, lv_draw_buf_t * buf3);

/**
 * Set display render mode
 * @param disp              pointer to a display
 * @param render_mode       LV_DISPLAY_RENDER_MODE_PARTIAL/DIRECT/FULL
 */
void lv_display_set_render_mode(lv_display_t * disp, lv_display_render_mode_t render_mode);

/**
 * Set the flush callback which will be called to copy the rendered image to the display.
 * @param disp      pointer to a display
 * @param flush_cb  the flush callback (`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display)
 */
void lv_display_set_flush_cb(lv_display_t * disp, lv_display_flush_cb_t flush_cb);

/**
 * Set a callback to be used while LVGL is waiting flushing to be finished.
 * It can do any complex logic to wait, including semaphores, mutexes, polling flags, etc.
 * If not set the `disp->flushing` flag is used which can be cleared with `lv_display_flush_ready()`
 * @param disp      pointer to a display
 * @param wait_cb   a callback to call while LVGL is waiting for flush ready.
 *                  If NULL `lv_display_flush_ready()` can be used to signal that flushing is ready.
 */
void lv_display_set_flush_wait_cb(lv_display_t * disp, lv_display_flush_wait_cb_t wait_cb);

/**
 * Set the color format of the display.
 * @param disp              pointer to a display
 * @param color_format      Possible values are
 *                          - LV_COLOR_FORMAT_RGB565
 *                          - LV_COLOR_FORMAT_RGB888
 *                          - LV_COLOR_FORMAT_XRGB888
 *                          - LV_COLOR_FORMAT_ARGB888
 *@note To change the endianness of the rendered image in case of RGB565 format
 *      (i.e. swap the 2 bytes) call `lv_draw_sw_rgb565_swap` in the flush_cb
 */
void lv_display_set_color_format(lv_display_t * disp, lv_color_format_t color_format);

/**
 * Get the color format of the display
 * @param disp              pointer to a display
 * @return                  the color format
 */
lv_color_format_t lv_display_get_color_format(lv_display_t * disp);

/**
 * Set the number of tiles for parallel rendering.
 * @param disp              pointer to a display
 * @param tile_cnt          number of tiles (1 =< tile_cnt < 256)
 */
void lv_display_set_tile_cnt(lv_display_t * disp, uint32_t tile_cnt);

/**
 * Get the number of tiles used for parallel rendering
 * @param disp              pointer to a display
 * @return                  number of tiles
 */
uint32_t lv_display_get_tile_cnt(lv_display_t * disp);

/**
 * Disabling anti-aliasing is not supported since v9. This function will be removed.
 * Enable anti-aliasing for the render engine
 * @param disp      pointer to a display
 * @param en        true/false
 */
void lv_display_set_antialiasing(lv_display_t * disp, bool en);

/**
 * Get if anti-aliasing is enabled for a display or not
 * @param disp      pointer to a display (NULL to use the default display)
 * @return          true/false
 */
bool lv_display_get_antialiasing(lv_display_t * disp);

/**
 * Call from the display driver when the flushing is finished
 * @param disp      pointer to display whose `flush_cb` was called
 */
LV_ATTRIBUTE_FLUSH_READY void lv_display_flush_ready(lv_display_t * disp);

/**
 * Tell if it's the last area of the refreshing process.
 * Can be called from `flush_cb` to execute some special display refreshing if needed when all areas area flushed.
 * @param disp      pointer to display
 * @return          true: it's the last area to flush;
 *                  false: there are other areas too which will be refreshed soon
 */
LV_ATTRIBUTE_FLUSH_READY bool lv_display_flush_is_last(lv_display_t * disp);

bool lv_display_is_double_buffered(lv_display_t * disp);

/*---------------------
 * SCREENS
 *--------------------*/

/**
 * Return a pointer to the active screen on a display
 * @param disp      pointer to display which active screen should be get.
 *                  (NULL to use the default screen)
 * @return          pointer to the active screen object (loaded by 'lv_screen_load()')
 */
lv_obj_t * lv_display_get_screen_active(lv_display_t * disp);

/**
 * Return with a pointer to the previous screen. Only used during screen transitions.
 * @param disp      pointer to display which previous screen should be get.
 *                  (NULL to use the default screen)
 * @return          pointer to the previous screen object or NULL if not used now
 */
lv_obj_t * lv_display_get_screen_prev(lv_display_t * disp);

/**
 * Return the screen that is currently being loaded by the display
 * @param disp      pointer to a display object (NULL to use the default screen)
 * @return          pointer to the screen being loaded or NULL if no screen is currently being loaded
 */
lv_obj_t * lv_display_get_screen_loading(lv_display_t * disp);

/**
 * Return the top layer. The top layer is the same on all screens and it is above the normal screen layer.
 * @param disp      pointer to display which top layer should be get. (NULL to use the default screen)
 * @return          pointer to the top layer object
 */
lv_obj_t * lv_display_get_layer_top(lv_display_t * disp);

/**
 * Return the sys. layer. The system layer is the same on all screen and it is above the normal screen and the top layer.
 * @param disp      pointer to display which sys. layer should be retrieved. (NULL to use the default screen)
 * @return          pointer to the sys layer object
 */
lv_obj_t * lv_display_get_layer_sys(lv_display_t * disp);

/**
 * Return the bottom layer. The bottom layer is the same on all screen and it is under the normal screen layer.
 * It's visible only if the screen is transparent.
 * @param disp      pointer to display (NULL to use the default screen)
 * @return          pointer to the bottom layer object
 */
lv_obj_t * lv_display_get_layer_bottom(lv_display_t * disp);


#if LV_USE_OBJ_NAME

/**
 * Get screen by its name on a display. The name should be set by
 * `lv_obj_set_name()` or `lv_obj_set_name_static()`.
 * @param disp          pointer to a display or NULL to use default display
 * @param screen_name   name of the screen to get
 * @return              pointer to the screen, or NULL if not found.
 */
lv_obj_t * lv_display_get_screen_by_name(const lv_display_t * disp, const char * screen_name);

#endif /*LV_USE_OBJ_NAME*/

/**
 * Load a screen on the default display
 * @param scr       pointer to a screen
 */
void lv_screen_load(struct _lv_obj_t * scr);

/**
 * Switch screen with animation
 * @param scr       pointer to the new screen to load
 * @param anim_type type of the animation from `lv_screen_load_anim_t`, e.g. `LV_SCREEN_LOAD_ANIM_MOVE_LEFT`
 * @param time      time of the animation
 * @param delay     delay before the transition
 * @param auto_del  true: automatically delete the old screen
 */
void lv_screen_load_anim(lv_obj_t * scr, lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay,
                         bool auto_del);

/**
 * Get the active screen of the default display
 * @return          pointer to the active screen
 */
lv_obj_t * lv_screen_active(void);

/**
 * Get the top layer  of the default display
 * @return          pointer to the top layer
 */
lv_obj_t * lv_layer_top(void);

/**
 * Get the system layer  of the default display
 * @return          pointer to the sys layer
 */
lv_obj_t * lv_layer_sys(void);

/**
 * Get the bottom layer  of the default display
 * @return          pointer to the bottom layer
 */
lv_obj_t * lv_layer_bottom(void);

/*---------------------
 * OTHERS
 *--------------------*/

/**
 * Add an event handler to the display
 * @param disp          pointer to a display
 * @param event_cb      an event callback
 * @param filter        event code to react or `LV_EVENT_ALL`
 * @param user_data     optional user_data
 */
void lv_display_add_event_cb(lv_display_t * disp, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

/**
 * Get the number of event attached to a display
 * @param disp          pointer to a display
 * @return              number of events
 */
uint32_t lv_display_get_event_count(lv_display_t * disp);

/**
 * Get an event descriptor for an event
 * @param disp          pointer to a display
 * @param index         the index of the event
 * @return              the event descriptor
 */
lv_event_dsc_t * lv_display_get_event_dsc(lv_display_t * disp, uint32_t index);

/**
 * Remove an event
 * @param disp          pointer to a display
 * @param index         the index of the event to remove
 * @return              true: and event was removed; false: no event was removed
 */
bool lv_display_delete_event(lv_display_t * disp, uint32_t index);

/**
 * Remove an event_cb with user_data
 * @param disp          pointer to a display
 * @param event_cb      the event_cb of the event to remove
 * @param user_data     user_data
 * @return              the count of the event removed
 */
uint32_t lv_display_remove_event_cb_with_user_data(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data);

/**
 * Send an event to a display
 * @param disp          pointer to a display
 * @param code          an event code. LV_EVENT_...
 * @param param         optional param
 * @return              LV_RESULT_OK: disp wasn't deleted in the event.
 */
lv_result_t lv_display_send_event(lv_display_t * disp, lv_event_code_t code, void * param);

/**
 * Get the area to be invalidated. Can be used in `LV_EVENT_INVALIDATE_AREA`
 * @param e     pointer to an event
 * @return      the area to invalidated (can be modified as required)
 */
lv_area_t * lv_event_get_invalidated_area(lv_event_t * e);

/**
 * Set the theme of a display. If there are no user created widgets yet the screens' theme will be updated
 * @param disp      pointer to a display
 * @param th        pointer to a theme
 */
void lv_display_set_theme(lv_display_t * disp, lv_theme_t * th);

/**
 * Get the theme of a display
 * @param disp      pointer to a display
 * @return          the display's theme (can be NULL)
 */
lv_theme_t * lv_display_get_theme(lv_display_t * disp);

/**
 * Get elapsed time since last user activity on a display (e.g. click)
 * @param disp      pointer to a display (NULL to get the overall smallest inactivity)
 * @return          elapsed ticks (milliseconds) since the last activity
 */
uint32_t lv_display_get_inactive_time(const lv_display_t * disp);

/**
 * Manually trigger an activity on a display
 * @param disp      pointer to a display (NULL to use the default display)
 */
void lv_display_trigger_activity(lv_display_t * disp);

/**
 * Temporarily enable and disable the invalidation of the display.
 * @param disp      pointer to a display (NULL to use the default display)
 * @param en        true: enable invalidation; false: invalidation
 */
void lv_display_enable_invalidation(lv_display_t * disp, bool en);

/**
 * Get display invalidation is enabled.
 * @param disp      pointer to a display (NULL to use the default display)
 * @return return   true if invalidation is enabled
 */
bool lv_display_is_invalidation_enabled(lv_display_t * disp);

/**
 * Get a pointer to the screen refresher timer to
 * modify its parameters with `lv_timer_...` functions.
 * @param disp      pointer to a display
 * @return          pointer to the display refresher timer. (NULL on error)
 */
lv_timer_t * lv_display_get_refr_timer(lv_display_t * disp);

/**
 * Delete screen refresher timer
 * @param disp      pointer to a display
 */
void lv_display_delete_refr_timer(lv_display_t * disp);

/**
 * Register vsync event of a display. `LV_EVENT_VSYNC` event will be sent periodically.
 * Please don't use it in display event listeners, as it may cause memory leaks and illegal access issues.
 *
 * @param disp      pointer to a display
 * @param event_cb      an event callback
 * @param user_data     optional user_data
 */
bool lv_display_register_vsync_event(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data);

/**
 * Unregister vsync event of a display. `LV_EVENT_VSYNC` event won't be sent periodically.
 * Please don't use it in display event listeners, as it may cause memory leaks and illegal access issues.
 * @param disp      pointer to a display
 * @param event_cb      an event callback
 * @param user_data     optional user_data
 */
bool lv_display_unregister_vsync_event(lv_display_t * disp, lv_event_cb_t event_cb, void * user_data);

/**
 * Send an vsync event to a display
 * @param disp          pointer to a display
 * @param param         optional param
 * @return              LV_RESULT_OK: disp wasn't deleted in the event.
 */
lv_result_t lv_display_send_vsync_event(lv_display_t * disp, void * param);

void lv_display_set_user_data(lv_display_t * disp, void * user_data);
void lv_display_set_driver_data(lv_display_t * disp, void * driver_data);
void * lv_display_get_user_data(lv_display_t * disp);
void * lv_display_get_driver_data(lv_display_t * disp);
lv_draw_buf_t * lv_display_get_buf_active(lv_display_t * disp);

/**
 * Rotate an area in-place according to the display's rotation
 * @param disp      pointer to a display
 * @param area      pointer to an area to rotate
 */
void lv_display_rotate_area(lv_display_t * disp, lv_area_t * area);

/**
 * Get the size of the draw buffers
 * @param disp      pointer to a display
 * @return          the size of the draw buffer in bytes for valid display, 0 otherwise
 */
uint32_t lv_display_get_draw_buf_size(lv_display_t * disp);

/**
 * Get the size of the invalidated draw buffer. Can be used in the flush callback
 * to get the number of bytes used in the current render buffer.
 * @param disp      pointer to a display
 * @param width     the width of the invalidated area
 * @param height    the height of the invalidated area
 * @return          the size of the invalidated draw buffer in bytes, not accounting for
 *                  any preceding palette information for a valid display, 0 otherwise
 */
uint32_t lv_display_get_invalidated_draw_buf_size(lv_display_t * disp, uint32_t width, uint32_t height);

/**********************
 *      MACROS
 **********************/

/*------------------------------------------------
 * To improve backward compatibility
 * Recommended only if you have one display
 *------------------------------------------------*/

#ifndef LV_HOR_RES
/**
 * The horizontal resolution of the currently active display.
 */
#define LV_HOR_RES lv_display_get_horizontal_resolution(lv_display_get_default())
#endif

#ifndef LV_VER_RES
/**
 * The vertical resolution of the currently active display.
 */
#define LV_VER_RES lv_display_get_vertical_resolution(lv_display_get_default())
#endif

/**
 * See `lv_dpx()` and `lv_display_dpx()`.
 * Same as Android's DIP. (Different name is chosen to avoid mistype between LV_DPI and LV_DIP)
 *
 * - 40 dip is 40 px on a 160 DPI screen (distance = 1/4 inch).
 * - 40 dip is 80 px on a 320 DPI screen (distance still = 1/4 inch).
 *
 * @sa https://stackoverflow.com/questions/2025282/what-is-the-difference-between-px-dip-dp-and-sp
 */
#define LV_DPX_CALC(dpi, n)   ((n) == 0 ? 0 :LV_MAX((( (dpi) * (n) + 80) / 160), 1)) /*+80 for rounding*/
#define LV_DPX(n)   LV_DPX_CALC(lv_display_get_dpi(NULL), n)

/**
 * For default display, computes the number of pixels (a distance or size) as if the
 * display had 160 DPI.  This allows you to specify 1/160-th fractions of an inch to
 * get real distance on the display that will be consistent regardless of its current
 * DPI.  It ensures `lv_dpx(100)`, for example, will have the same physical size
 * regardless to the DPI of the display.
 * @param n     number of 1/160-th-inch units to compute with
 * @return      number of pixels to use to make that distance
 */
int32_t lv_dpx(int32_t n);

/**
 * For specified display, computes the number of pixels (a distance or size) as if the
 * display had 160 DPI.  This allows you to specify 1/160-th fractions of an inch to
 * get real distance on the display that will be consistent regardless of its current
 * DPI.  It ensures `lv_dpx(100)`, for example, will have the same physical size
 * regardless to the DPI of the display.
 * @param disp  pointer to display whose dpi should be considered
 * @param n     number of 1/160-th-inch units to compute with
 * @return      number of pixels to use to make that distance
 */
int32_t lv_display_dpx(const lv_display_t * disp, int32_t n);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DISPLAY_H*/
