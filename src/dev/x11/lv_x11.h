/**
 * @file lv_x11.h
 *
 */

#ifndef LV_X11_H
#define LV_X11_H

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

/** Header of private display driver user data - for internal use only */
typedef struct {
    struct _XDisplay   *  display;  /**< X11 display object     */
    struct _x11_inp_data * inp_data; /**< input user data object */
} _x11_user_hdr_t;

/** optional window close callback function type
 *  @see lv_x11_window_set_close_cb
*/
typedef void(*lv_x11_close_cb)(void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * create and add keyboard, mouse and scrillwheel objects and connect them to x11 display.
 *
 * This is a convenience method handling the typical input initialisation of an X11 window:
 * - create keyboard (@ref lv_x11_keyboard_create)
 * - create mouse (with scrollwheel, @ref lv_x11_mouse_create @ref lv_x11_mousewheel_create)
 *
 * @param[in] disp      the created X11 display object from @ref lv_x11_window_create
 * @param[in] mouse_img optional image description for the mouse cursor (NULL for no/invisible mouse cursor)
 */
void lv_x11_inputs_create(lv_display_t * disp, lv_image_dsc_t const * mouse_img);

/**
 * create the X11 display
 *
 * The minimal initialisation for initializing the X11 display driver with keyboard/mouse support:
 * @code
 * lv_display_t* disp = lv_x11_window_create("My Window Title", window_width, window_width);
 * lv_x11_inputs_create(disp, NULL);
 * @endcode
 * or with mouse cursor icon:
 * @code
 * lv_img_dsc_t mouse_symbol = {.....};
 * lv_display_t* disp = lv_x11_window_create("My Window Title", window_width, window_width);
 * lv_x11_inputs_create(disp, &mouse_symbol);
 * @endcode
 *
 * @param[in] title    title of the created X11 window
 * @param[in] hor_res  horizontal resolution (=width) of the X11 window
 * @param[in] ver_res  vertical resolution (=height) of the X11 window
 * @return             pointer to the display object
 */
lv_display_t * lv_x11_window_create(char const * title, int32_t hor_res, int32_t ver_res);

#endif /* LV_USE_X11 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_X11_H */
