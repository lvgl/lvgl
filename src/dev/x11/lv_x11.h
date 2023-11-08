/**
 * @file lv_sdl_window.h
 *
 */

#ifndef LV_X11_DISP_H
#define LV_X11_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_X11

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef void(*lv_x11_close_cb)(lv_display_t*);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * create the X11 display
 * @param title    title of the created X11 window
 * @param hor_res  horizontal resolution (=width) of the X11 window
 * @param ver_res  vertical resolution (=height) of the X11 window
 * @return         pointer to the display object
 */
lv_display_t* lv_x11_window_create(char const* title, int32_t hor_res, int32_t ver_res);

/**
 * set optional application callback on X11 window close event
 * @param disp      the created X11 display object from @lv_x11_window_create
 * @param close_cb  callback funtion to be called
 */
void lv_x11_window_set_close_cb(lv_display_t* disp, lv_x11_close_cb close_cb);

/**
 * create the keyboard input object for the X11 display object
 * @param disp  the created X11 display object from @lv_x11_window_create
 * @return      pointer to the keyboard input object
 */
lv_indev_t* lv_x11_keyboard_create(lv_display_t* disp);

/**
 * create the mouse input object for the X11 display object
 * @param disp  the created X11 display object from @lv_x11_window_create
 * @param symb  optional symbol for the mouse (if NULL no symbol is added)
 * @return      pointer to the mouse input object
 */
lv_indev_t* lv_x11_mouse_create(lv_display_t* disp, lv_image_dsc_t const* symb);

/**
 * add the X11 display encoder (=mousewheel) input
 * @param disp  the created X11 display object from @lv_x11_window_create
 * @return      pointer to the mousewheel input object
 */
lv_indev_t* lv_x11_mousewheel_create(lv_display_t* disp);


/**********************
 *      MACROS
 **********************/

#endif /* LV_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_DISP_H */
