/**
 * @file lv_nxp_elcdif.h
 * Driver for NXP's ELCD
 */

#ifndef LV_NXP_ELCDIF_DRIVER_H
#define LV_NXP_ELCDIF_DRIVER_H

#include "../../../lvgl.h"

#if LV_USE_NXP_ELCDIF == 1

#include "fsl_elcdif.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    elcdif_rgb_mode_config_t config;
    void * frame_buffer1;
    void * frame_buffer2;
    size_t buf_size;
    lv_display_render_mode_t mode;
    void (init_cb)(void);
    lv_display_flush_cb_t flush_cb;
    lv_display_flush_wait_cb_t wait_cb;
} lv_nxp_display_obj_params_t;

typedef struct {
    void (indev_init_cb)(void);
    lv_indev_type_t indev_type;
    lv_indev_read_cb_t read_cb
} lv_nxp_indev_obj_params_t;

typedef struct {
    lv_nxp_display_obj_params_t * display_params;
    lv_nxp_indev_obj_params_t * indev_params;
} lv_nxp_lcd_obj_params_t;

typedef struct {
    lv_display_t * display;
    lv_indev_t * indev;
} lv_nxp_lcd_object_t;

/**
 * API to create display for NXP processors by default. Sets internal buffers and callbacks and everything accordingly
 * @param config : NXP eLCD config
 * @param frame_buffer1 : handle to buffer 1
 * @param frame_buffer2 : handle to buffer 2
 * @param buf_size : frame buffer size in bytes
 * @param mode : LV_DISPLAY_RENDER_MODE (FULL, PARTIAL, DIRECT)
 * @param init_cb : LCD init callback : HW dependent
 */
lv_display_t * lv_nxp_display_elcdif_create(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                            void * frame_buffer2, size_t buf_size, lv_display_render_mode_t mode,
                                            void (init_cb)(void), lv_display_flush_cb_t flush_cb, lv_display_flush_wait_cb_t wait_cb);


/**
 * API to create a display in direct mode. Does NOT set the callbacks and does not initialize the LCD.
 * This function is pure LVGL
 * @param config : NXP eLCD config object
 * @param frame_buffer1 : handle to buffer 1
 * @param frame_buffer2 : handle to buffer 2
 * @param buf_size : buffer size in bytes
 */
lv_display_t * lv_nxp_display_elcdif_create_direct(elcdif_rgb_mode_config_t config, void * frame_buffer1,
                                                   void * frame_buffer2, size_t buf_size);

/**
* API to create nxp indev
* @param indev_init_cb : Callback function for indev init
* @param indev_type : LV_INDEV_TYPE (POINTER, KEYPAD, etc.)
* @param read_cb : Indev read callback : HW dependant
*/
lv_indev_t * lv_nxp_indev_create(void (indev_init_cb)(void), lv_indev_type_t indev_type, lv_indev_read_cb_t read_cb);

/**
 * API to create a touchpad
 * @param touchpad_init_cb : Handle to the initialization function. HW dependent
 * @param touchpad_read_cb : Handle to data extraction function. HW dependent
 */
lv_indev_t * lv_nxp_touchpad_create(void (touchpad_init_cb)(void), lv_indev_read_cb_t touchpad_read_cb);

/**
 * Convenience API for display creation
 * @param display_params : struct that contains the display parameters
 */
lv_display_t * lv_nxp_display_obj_create(lv_nxp_display_obj_params_t display_params);

/**
 * Convenience API for indev creation
 * @param indev_params : struct that contains the indev parameters
 */
lv_indev_t * lv_nxp_indev_obj_create(lv_nxp_indev_obj_params_t indev_params);

/**
 * Convenience API for display + indev creation.
 * @param lv_nxp_lcd_obj_params_t : struct that contains the lcd parameters (display + touchpad)
 */
lv_nxp_lcd_object_t * lv_nxp_lcd_obj_create(lv_nxp_lcd_obj_params_t lcd_params);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_NXP_ELCDIF*/

#endif /* LV_NXP_ELCDIF_DRIVER_H */
