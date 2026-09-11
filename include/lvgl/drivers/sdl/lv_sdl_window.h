/**
 * @file lv_sdl_window.h
 *
 */

/**
 * Modified by NXP in 2025
 */

#ifndef LV_SDL_WINDOW_H
#define LV_SDL_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct SDL_Window;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_display_t * lv_sdl_window_create(int32_t hor_res, int32_t ver_res);

/**
 * Allow or forbid resizing the window
 * @param disp      pointer to an SDL display
 * @param value     true: resizeable, false: fixed size
 */
void lv_sdl_window_set_resizeable(lv_display_t * disp, bool value);

/**
 * Set the size of the window
 * @param disp      pointer to an SDL display
 * @param hor_res   the new width in pixels
 * @param ver_res   the new height in pixels
 */
void lv_sdl_window_set_size(lv_display_t * disp, int32_t hor_res, int32_t ver_res);

/**
 * Set the zoom factor of the window
 * @param disp      pointer to an SDL display
 * @param zoom      the new zoom factor
 */
void lv_sdl_window_set_zoom(lv_display_t * disp, float zoom);

/**
 * Get the zoom factor of the window
 * @param disp      pointer to an SDL display
 * @return          the current zoom factor
 */
float lv_sdl_window_get_zoom(lv_display_t * disp);

/**
 * Set the title of the window
 * @param disp      pointer to an SDL display
 * @param title     the new title
 */
void lv_sdl_window_set_title(lv_display_t * disp, const char * title);

/**
 * Set the icon of the window
 * @param disp      pointer to an SDL display
 * @param icon      pointer to the ARGB8888 pixels of the icon
 * @param width     the icon width in pixels
 * @param height    the icon height in pixels
 */
void lv_sdl_window_set_icon(lv_display_t * disp, void * icon, int32_t width, int32_t height);

/**
 * Get the SDL renderer of the window
 * @param disp      pointer to an SDL display
 * @return          pointer to the SDL_Renderer, or NULL on error
 */
void * lv_sdl_window_get_renderer(lv_display_t * disp);

void lv_sdl_quit(void);

/**
 * Get the SDL window
 * @param disp      pointer to an SDL display
 * @return          pointer to the SDL_Window, or NULL on error
 */
struct SDL_Window * lv_sdl_window_get_window(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/


#endif /* LV_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_WINDOW_H */
